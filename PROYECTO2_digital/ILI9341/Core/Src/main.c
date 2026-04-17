/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ili9341.h"
#include "Bitmaps.h"
#include "fatfs_sd.h"
#include "string.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
SPI_HandleTypeDef hspi1;
FATFS fs;
FATFS *pfs;
FIL fil;
FRESULT fres;
DWORD fre_clust;
uint32_t totalSpace, freeSpace;
char buffer[100];
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
extern uint16_t STAGE2[];
extern uint16_t STAGE1[];
extern uint16_t INICIO[];
extern uint16_t STAGE3[];
//Declaramos variable que lleva el control de
volatile uint8_t estado = 0;
uint8_t rxByte;

volatile uint8_t rxState = 0;      // 0 = esperando control, 1 = esperando accion
volatile char rxControl = 0;
volatile char rxAction = 0;
volatile uint8_t comandoListo = 0;
volatile uint8_t stage1 = 0;
volatile uint8_t stage2 = 0;
volatile uint8_t stage3 = 0;
uint8_t dato_disparo = 'A';
uint8_t dato_inicio = 'S';
uint8_t flag_inicio = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
//funcion para poder leer imagen de la SD
FRESULT LCD_DrawImageFromSD(const char *filename,
                            uint16_t x,
                            uint16_t y,
                            uint16_t width,
                            uint16_t height);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void transmit_uart(char *string){
	uint8_t len = strlen(string);
	HAL_UART_Transmit(&huart2, (uint8_t*) string, len, 200);
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
  MX_USART3_UART_Init();
  MX_SPI1_Init();
  MX_FATFS_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  	HAL_UART_Receive_IT(&huart3, &rxByte, 1);
  	//codigo para poder leer la microSD
  	/*HAL_Delay(500);
  	//montamos el sistema
  	fres = f_mount(&fs, "/", 0);
  	if(fres == FR_OK){
  		transmit_uart("Micro SD card is mounted successfully\n");
  	}
  	else if(fres != FR_OK){
  		transmit_uart("Micro SD card's mount error\n");
  	}
  	//abrimos archivo ya sea en lectura o escritura o en ambos y append
  	fres = f_open(&fil, "prueba.txt", FA_OPEN_APPEND | FA_READ | FA_WRITE);
  	if(fres == FR_OK){
  		transmit_uart("File opened for reading\n");
  	}
  	else if(fres != FR_OK){
  		transmit_uart("File was not opened for reading\n");
  	}

  	//escribir en el archivo abierto
  	for(uint8_t i = 0; i < 5; i++){
  		f_puts("Hola mundo digital 2\n", &fil);
  	}

  	//cerramo archivo abierto
  	fres = f_close(&fil);
  	if (fres == FR_OK){
  		transmit_uart("The file is closed\n");
  	}
  	else if(fres != FR_OK){
  		transmit_uart("The was not file closed\n");
  	}

  	//leemos archivo
  	fres = f_open(&fil, "prueba.txt", FA_READ);
  	if(fres == FR_OK){
  	  		transmit_uart("File opened for reading\n");
  	  	}
  	  	else if(fres != FR_OK){
  	  		transmit_uart("File was not opened for reading\n");
  	  	}
  	//leemos el archivo
  	while (f_gets(buffer, sizeof(buffer),&fil)){
  		char mRd[100];
  		sprintf(mRd, "%s", buffer);
  		transmit_uart(mRd);
  	}
  	fres = f_close(&fil);
  	  	if (fres == FR_OK){
  	  		transmit_uart("The file is closed\n");
  	  	}
  	  	else if(fres != FR_OK){
  	  		transmit_uart("The was not file closed\n");
  	  	}
  	  	f_mount(NULL, "", 1);
  	  	if(fres == FR_OK){
  	  		transmit_uart("The microSD is unmounted\n");
  	  	}
  	  	else if(fres != FR_OK){
  	  		transmit_uart("The microSD was not unmounted\n");
  	  	}*/
  	//cerramos el archivo

	LCD_Init();
	HAL_Delay(100);

	if (LCD_DrawImageFromSD("0:/STAGE3.bin", 0, 0, 320, 240) == FR_OK) {
	    transmit_uart("Fondo cargado desde SD\r\n");
	} else {
	    transmit_uart("Error cargando fondo desde SD\r\n");
	}
	//LCD_Clear(0x00);
	//FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
	//FillRect(0, 0, 319, 239, 0x0DFE);
	//FillRect(0, 0, 319, 239, 0x4c9d);

	//LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);

	//---- Desplegamos la pantalla de inicio del juego ----//
	//LCD_Bitmap(0, 0, 320, 240, INICIO);

	//LCD_Bitmap(100, 100, 24, 30, sonic);

	//LCD_Bitmap(0, 0, 320, 240, fondoPinguin);
	//LCD_BitmapTransparent(100, 100, 49, 36, dkong, 0x4b7e);
	//LCD_BitmapTransparent(50, 60,32,32,megaman,0x03ba);
	//LCD_Bitmap(132, 100, 32, 32, megaman);



// 	FillRect(0, 0, 319, 206, 0x079f);
//	LCD_Print("Hola Mundo", 20, 150, 2, 0x001F, 0x055b);
//
//
//for (int x = 0; x < 319; x++) {
//			LCD_Bitmap(x, 116, 15, 15, tile);
//			LCD_Bitmap(x, 68, 15, 15, tile);
//			LCD_Bitmap(x, 207, 15, 15, tile);
//			LCD_Bitmap(x, 223, 15, 15, tile);
//			x += 14;
//		}

	//LCD_BitmapTransparent(100, 116-36, 40, 40, TANK, 0x079f);
	//LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset);

	//LCD_Sprite(100, 116-29, 40, 40, TANK, 1, 0, 0, 0);


	//  LCD_Print("Hola Mundo", 20, 100, 1, 0x001F, 0xCAB9);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	  while (1)
	  {
		  //---- logica iniciar musica de fondo inicio ----//
		  if(flag_inicio == 0){
			  flag_inicio = 1;
			  //HAL_UART_Transmit(&huart3, &dato_inicio, 1, 100);
		  }
		  //---- fin de logica ----//

		  //---- logica para saber de que control viene la info ----//
	      if (comandoListo)
	      {
	          comandoListo = 0;

	          if (rxControl == '1')
	          {
	              switch (rxAction)
	              {
	                  case 'A':
	                      if (estado == 0){
	                    	  estado = 1;
	                    	  HAL_UART_Transmit(&huart3, &dato_disparo, 1, 100);
	                      }
	                      break;

	                  case 'B':
	                	  if(estado == 1){
	                		  estado = 2;
	                		  HAL_UART_Transmit(&huart3, &dato_disparo, 1, 100);
	                	  }
	                      break;

	                  case 'U':
	                      break;

	                  case 'D':
	                      break;

	                  case 'L':
	                      break;

	                  case 'R':
	                      break;
	              }
	          }
	          else if (rxControl == '2')
	          {
	              switch (rxAction)
	              {
	                  case 'A':
	                      break;

	                  case 'B':
	                      break;

	                  case 'L':
	                      break;

	                  case 'R':
	                      break;

	                  case 'U':
	                      break;

	                  case 'D':
	                      break;
	              }
	          }
	      }
	      //---- Fin de logica para detectar controles ----//

	      //---- Logica para poder ver en que estado estamos ----//
	      switch (estado)
	      {
	          case 1:
	              if (stage1 == 0)
	              {
	                  stage1 = 1;
	                  LCD_Bitmap(0, 0, 320, 240, STAGE1);
	              }
	              break;

	          case 2:
	        	  if(stage2 == 0){
	        	  	    stage2 = 1;
	        	  	    stage1 = 0;
	        	  	    LCD_Bitmap(0, 0, 320, 240, STAGE2);
	        	  	}
	              break;
	          case 3:
	        	  if(stage3 == 0){
	        		  stage2 = 0;
	        		  stage1 = 0;
	        		  stage3 = 1;
	        		  //LCD_Bitmap(0, 0, 320, 240, STAGE3);
	        	  }
	        	  break;
	          default:
	              break;
	      }
	      //---- fin de logica para estados ----//
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  }
  /* USER CODE END 3 */
}

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
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
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
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  huart2.Init.BaudRate = 9600;
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
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LCD_RST_Pin|LCD_D1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LCD_RD_Pin|LCD_WR_Pin|LCD_RS_Pin|LCD_D7_Pin
                          |LCD_D0_Pin|LCD_D2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin|LCD_D6_Pin|LCD_D3_Pin|LCD_D5_Pin
                          |LCD_D4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SD_SS_GPIO_Port, SD_SS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : LCD_RST_Pin LCD_D1_Pin */
  GPIO_InitStruct.Pin = LCD_RST_Pin|LCD_D1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_RD_Pin LCD_WR_Pin LCD_RS_Pin LCD_D7_Pin
                           LCD_D0_Pin LCD_D2_Pin */
  GPIO_InitStruct.Pin = LCD_RD_Pin|LCD_WR_Pin|LCD_RS_Pin|LCD_D7_Pin
                          |LCD_D0_Pin|LCD_D2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_CS_Pin LCD_D6_Pin LCD_D3_Pin LCD_D5_Pin
                           LCD_D4_Pin */
  GPIO_InitStruct.Pin = LCD_CS_Pin|LCD_D6_Pin|LCD_D3_Pin|LCD_D5_Pin
                          |LCD_D4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : SD_SS_Pin */
  GPIO_InitStruct.Pin = SD_SS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(SD_SS_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)
    {
        if (rxState == 0)
        {
            // Esperando número de control
            if (rxByte == '1' || rxByte == '2')
            {
                rxControl = rxByte;
                rxState = 1;
            }
        }
        else if (rxState == 1)
        {
            // Esperando acción
            if (rxByte == 'A' || rxByte == 'B' || rxByte == 'L' || rxByte == 'R' ||
                rxByte == 'U' || rxByte == 'D')
            {
                rxAction = rxByte;
                comandoListo = 1;
            }

            // Después de recibir acción, volver a esperar control
            rxState = 0;
        }

        HAL_UART_Receive_IT(&huart3, &rxByte, 1);
    }
}

//logica para poder leer imagenes desde la SD y poder enviarlos a la pantalla
FRESULT LCD_DrawImageFromSD(const char *filename,
                            uint16_t x,
                            uint16_t y,
                            uint16_t width,
                            uint16_t height)
{
    FRESULT fres_local;
    UINT bytesRead;
    char msg[100];
    uint16_t lineBuffer[320];

    if (width > 320) {
        transmit_uart("Error: width > 320\r\n");
        return FR_INVALID_PARAMETER;
    }

    fres_local = f_mount(&fs, "/", 1);
    if (fres_local != FR_OK) {
        sprintf(msg, "Mount error: %d\r\n", fres_local);
        transmit_uart(msg);
        return fres_local;
    }

    fres_local = f_open(&fil, filename, FA_READ);
    if (fres_local != FR_OK) {
        sprintf(msg, "Open error: %d\r\n", fres_local);
        transmit_uart(msg);
        f_mount(NULL, "/", 1);
        return fres_local;
    }

    HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin, GPIO_PIN_RESET);   // CS low
    SetWindows(x, y, x + width - 1, y + height - 1);
    HAL_GPIO_WritePin(GPIOA, LCD_RS_Pin, GPIO_PIN_SET);     // RS high

    for (uint16_t row = 0; row < height; row++) {

        fres_local = f_read(&fil, lineBuffer, width * 2, &bytesRead);
        if (fres_local != FR_OK) {
            sprintf(msg, "Read error row %d: %d\r\n", row, fres_local);
            transmit_uart(msg);
            f_close(&fil);
            HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin, GPIO_PIN_SET);
            f_mount(NULL, "/", 1);
            return fres_local;
        }

        if (bytesRead != width * 2) {
            sprintf(msg, "Partial read row %d\r\n", row);
            transmit_uart(msg);
            f_close(&fil);
            HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin, GPIO_PIN_SET);
            f_mount(NULL, "/", 1);
            return FR_INT_ERR;
        }

        // AQUI va la inversion de bytes
        for (uint16_t i = 0; i < width; i++) {
            lineBuffer[i] = (lineBuffer[i] >> 8) | (lineBuffer[i] << 8);
        }

        for (uint16_t col = 0; col < width; col++) {
            LCD_DATA(lineBuffer[col] >> 8);
            LCD_DATA(lineBuffer[col] & 0xFF);
        }
    }

    HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin, GPIO_PIN_SET);     // CS high
    f_close(&fil);
    f_mount(NULL, "/", 1);

    transmit_uart("Image drawn successfully\r\n");
    return FR_OK;
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
	while (1) {
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
