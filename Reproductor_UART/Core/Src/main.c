/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
    uint32_t freq;
    uint32_t duration;
} Note_t;

typedef enum
{
    MODE_STOP = 0,
    MODE_PWM  = 1,
    MODE_DAC  = 2
} PlayerMode_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494

#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_A5  880
#define NOTE_B5  988

#define REST              0
#define GAP_MS 5
#define DAC_SAMPLES       32
#define TIMER6_CLK_HZ     84000000UL   // con tu configuración actual
#define PWM_REPEATS       2
#define DAC_REPEATS       1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac;
DMA_HandleTypeDef hdma_dac1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t rxByte;
volatile PlayerMode_t currentMode = MODE_STOP;
volatile uint8_t showMenu = 1;
volatile uint8_t dacRunning = 0;

/* Melodía PWM: la tuya (Mario aprox) */
const Note_t melody_pwm[] =
{
    {NOTE_E5, 180}, {NOTE_E5, 180}, {REST, 80},  {NOTE_E5, 180},
    {REST, 80},     {NOTE_C5, 180}, {NOTE_E5, 180}, {NOTE_G5, 320},
    {REST, 200},

    {NOTE_G4, 180}, {REST, 200}, {NOTE_C5, 180}, {REST, 80},
    {NOTE_G4, 180}, {REST, 80},  {NOTE_E4, 180}, {REST, 80},

    {NOTE_A4, 180}, {NOTE_B4, 180}, {NOTE_A4, 180}, {NOTE_G4, 180},
    {NOTE_E5, 180}, {NOTE_G5, 180}, {NOTE_A5, 220}, {REST, 120},

    {NOTE_F5, 180}, {NOTE_G5, 180}, {REST, 80},  {NOTE_E5, 180},
    {REST, 80},     {NOTE_C5, 180}, {NOTE_D5, 180}, {NOTE_B4, 220},
    {REST, 200}
};

const uint32_t melody_pwm_len = sizeof(melody_pwm) / sizeof(melody_pwm[0]);

/* Melodía DAC: distinta */
const Note_t melody_dac[] =
{
    {NOTE_E4, 250}, {NOTE_E4, 250}, {NOTE_F4, 250}, {NOTE_G4, 250},
    {NOTE_G4, 250}, {NOTE_F4, 250}, {NOTE_E4, 250}, {NOTE_D4, 250},
    {NOTE_C4, 250}, {NOTE_C4, 250}, {NOTE_D4, 250}, {NOTE_E4, 250},
    {NOTE_E4, 375}, {NOTE_D4, 125}, {NOTE_D4, 500},

    {NOTE_E4, 250}, {NOTE_E4, 250}, {NOTE_F4, 250}, {NOTE_G4, 250},
    {NOTE_G4, 250}, {NOTE_F4, 250}, {NOTE_E4, 250}, {NOTE_D4, 250},
    {NOTE_C4, 250}, {NOTE_C4, 250}, {NOTE_D4, 250}, {NOTE_E4, 250},
    {NOTE_D4, 375}, {NOTE_C4, 125}, {NOTE_C4, 500}
};

const uint32_t melody_dac_len = sizeof(melody_dac) / sizeof(melody_dac[0]);

/* Tabla seno de 32 muestras para el DAC */
const uint16_t sineLUT[DAC_SAMPLES] =
{
    2048,2399,2737,3048,3321,3545,3711,3813,
    3848,3813,3711,3545,3321,3048,2737,2399,
    2048,1697,1359,1048,775,551,385,283,
    248,283,385,551,775,1048,1359,1697
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_DAC_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE BEGIN 0 */
void SendText(char *txt)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)txt, strlen(txt), 1000);
}

void PrintMenu(void)
{
    char menu[] =
        "\r\n========== REPRODUCTOR ==========\r\n"
        "1 -> Reproducir melodia PWM\r\n"
        "2 -> Reproducir melodia DAC\r\n"
        "0 -> Detener audio\r\n"
        "m -> Mostrar menu otra vez\r\n"
        "================================\r\n";
    SendText(menu);
}

uint8_t WaitMsInterruptible(uint32_t time_ms, PlayerMode_t expectedMode)
{
    uint32_t elapsed = 0;

    while (elapsed < time_ms)
    {
        if (currentMode != expectedMode)
        {
            return 0;
        }

        HAL_Delay(5);
        elapsed += 5;
    }

    return 1;
}

/* ================= PWM ================= */

void PWM_Stop(void)
{
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
}

void Buzzer_SetFrequency(uint32_t freq_hz)
{
    if (freq_hz == 0)
    {
        PWM_Stop();
        return;
    }

    uint32_t arr = (1000000UL / freq_hz) - 1UL;

    __HAL_TIM_SET_AUTORELOAD(&htim2, arr);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, (arr + 1UL) / 2UL);
    __HAL_TIM_SET_COUNTER(&htim2, 0);
}

void PlaySongPWM(const Note_t *song, uint32_t length, uint8_t repeats)
{
    for (uint8_t r = 0; r < repeats; r++)
    {
        for (uint32_t i = 0; i < length; i++)
        {
            if (currentMode != MODE_PWM)
            {
                PWM_Stop();
                return;
            }

            Buzzer_SetFrequency(song[i].freq);

            if (!WaitMsInterruptible(song[i].duration, MODE_PWM))
            {
                PWM_Stop();
                return;
            }

            PWM_Stop();

            if (!WaitMsInterruptible(GAP_MS, MODE_PWM))
            {
                PWM_Stop();
                return;
            }
        }
    }

    PWM_Stop();
}

/* ================= DAC ================= */

void DAC_Stop(void)
{
    if (dacRunning)
    {
        HAL_TIM_Base_Stop(&htim6);
        HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
        dacRunning = 0;
    }
}

void DAC_SetFrequency(uint32_t freq_hz)
{
    if (freq_hz == 0)
    {
        DAC_Stop();
        return;
    }

    uint32_t sample_rate = freq_hz * DAC_SAMPLES;
    uint32_t arr = (TIMER6_CLK_HZ / sample_rate) - 1UL;

    if (arr < 1)
    {
        arr = 1;
    }

    if (arr > 0xFFFF)
    {
        arr = 0xFFFF;
    }

    __HAL_TIM_SET_PRESCALER(&htim6, 0);
    __HAL_TIM_SET_AUTORELOAD(&htim6, arr);
    __HAL_TIM_SET_COUNTER(&htim6, 0);

    if (!dacRunning)
    {
        HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)sineLUT, DAC_SAMPLES, DAC_ALIGN_12B_R);
        HAL_TIM_Base_Start(&htim6);
        dacRunning = 1;
    }
}

void PlaySongDAC(const Note_t *song, uint32_t length, uint8_t repeats)
{
    for (uint8_t r = 0; r < repeats; r++)
    {
        for (uint32_t i = 0; i < length; i++)
        {
            if (currentMode != MODE_DAC)
            {
                DAC_Stop();
                return;
            }

            DAC_SetFrequency(song[i].freq);

            if (!WaitMsInterruptible(song[i].duration, MODE_DAC))
            {
                DAC_Stop();
                return;
            }

            DAC_Stop();

            if (!WaitMsInterruptible(GAP_MS, MODE_DAC))
            {
                DAC_Stop();
                return;
            }
        }
    }

    DAC_Stop();
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  MX_DAC_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_UART_Receive_IT(&huart2, &rxByte, 1);
  PrintMenu();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (showMenu)
	        {
	            PrintMenu();
	            showMenu = 0;
	        }

	        switch (currentMode)
	        {
	            case MODE_PWM:
	                PlaySongPWM(melody_pwm, melody_pwm_len, PWM_REPEATS);
	                if (currentMode == MODE_PWM)
	                {
	                    currentMode = MODE_STOP;
	                    showMenu = 1;
	                }
	                break;

	            case MODE_DAC:
	                PlaySongDAC(melody_dac, melody_dac_len, DAC_REPEATS);
	                if (currentMode == MODE_DAC)
	                {
	                    currentMode = MODE_STOP;
	                    showMenu = 1;
	                }
	                break;

	            case MODE_STOP:
	            default:
	                PWM_Stop();
	                DAC_Stop();
	                HAL_Delay(10);
	                break;
	        }
	    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief DAC Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC_Init(void)
{

  /* USER CODE BEGIN DAC_Init 0 */

  /* USER CODE END DAC_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC_Init 1 */

  /* USER CODE END DAC_Init 1 */

  /** DAC Initialization
  */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT1 config
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC_Init 2 */

  /* USER CODE END DAC_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 83;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 100-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 50;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 0;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65535;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        switch (rxByte)
        {
            case '1':
                currentMode = MODE_PWM;
                SendText("\r\nPWM seleccionado\r\n");
                break;

            case '2':
                currentMode = MODE_DAC;
                SendText("\r\nDAC seleccionado\r\n");
                break;

            case '0':
                currentMode = MODE_STOP;
                SendText("\r\nAudio detenido\r\n");
                break;

            case 'm':
            case 'M':
                showMenu = 1;
                break;

            default:
                SendText("\r\nComando invalido\r\n");
                showMenu = 1;
                break;
        }

        HAL_UART_Receive_IT(&huart2, &rxByte, 1);
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
