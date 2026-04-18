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
#define CHUNK_ROWS 20
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
static uint8_t blockBuffer[320 * CHUNK_ROWS * 2];
volatile uint8_t inicio = 0;
volatile uint16_t x_rojo = 0;
volatile uint16_t y_rojo = 23;
volatile uint16_t x_celeste = 320-14;
volatile uint16_t y_celeste = 240-13;

//varibles para poder hacer logica de colisiones
#define STAGE_W         320
#define STAGE_H         240
#define TANK_W          14
#define TANK_H          13
#define MOVE_STEP       2
#define COLOR_CAMINO    0x0000
#define STAGE1_FILE     "0:/STAGE1.bin"

typedef enum {
    DIR_UP = 0,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} TankDir;

volatile TankDir dir_rojo = DIR_RIGHT;
volatile TankDir dir_celeste = DIR_LEFT;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
//funcion para poder leer imagen de la SD
FRESULT LCD_DrawImageFromSD_Fast(const char *filename,
                                 uint16_t x,
                                 uint16_t y,
                                 uint16_t width,
                                 uint16_t height);

//prototipos para poder hacer colisiones
uint16_t LeerPixelStage1(uint16_t x, uint16_t y);
uint8_t PuntoTransitable(uint16_t x, uint16_t y);
uint8_t PuedeMoverTanqueRojo(uint16_t newX, uint16_t newY, TankDir dir);
FRESULT LCD_DrawRegionFromSD(const char *filename,
                             uint16_t srcX, uint16_t srcY,
                             uint16_t width, uint16_t height,
                             uint16_t dstX, uint16_t dstY);
const uint16_t *SpriteTanqueRojo(TankDir dir);
void DibujarTanqueRojo(void);
void MoverTanqueRojo(TankDir dir);
uint8_t PuedeMoverTanqueCeleste(uint16_t newX, uint16_t newY, TankDir dir);
const uint16_t *SpriteTanqueCeleste(TankDir dir);
void DibujarTanqueCeleste(void);
void MoverTanqueCeleste(TankDir dir);
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

	fres = f_mount(&fs, "/", 1);
	if (fres == FR_OK) {
	    transmit_uart("SD montada correctamente\r\n");
	} else {
	    char msg[50];
	    sprintf(msg, "Mount error: %d\r\n", fres);
	    transmit_uart(msg);
	}

	if (LCD_DrawImageFromSD_Fast("0:/INICIO.bin", 0, 0, 320, 240) == FR_OK) {
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

	//LCD_Sprite(100, 116-29, 14, 13, TANK_DERECHA_ROJO, 1, 0, 0, 0);


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
	                	  if (stage1) MoverTanqueRojo(DIR_UP);
	                      break;

	                  case 'D':
	                	  if (stage1) MoverTanqueRojo(DIR_DOWN);
	                      break;

	                  case 'L':
	                	  if (stage1) MoverTanqueRojo(DIR_LEFT);
	                      break;

	                  case 'R':
	                	  if (stage1) MoverTanqueRojo(DIR_RIGHT);
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
	                      if (stage1) MoverTanqueCeleste(DIR_LEFT);
	                      break;

	                  case 'R':
	                      if (stage1) MoverTanqueCeleste(DIR_RIGHT);
	                      break;

	                  case 'U':
	                      if (stage1) MoverTanqueCeleste(DIR_UP);
	                      break;

	                  case 'D':
	                      if (stage1) MoverTanqueCeleste(DIR_DOWN);
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
	            	  if (stage1 == 0)
	            	      {
	            		  //dibujar sprite de tanque rojo
	            	          stage2 = 0;
	            	          stage1 = 1;
	            	          stage3 = 0;

	            	          x_rojo = 33;
	            	          y_rojo = 45;
	            	          dir_rojo = DIR_RIGHT;

	            	          LCD_DrawImageFromSD_Fast("0:/STAGE1.bin", 0, 0, 320, 240);
	            	          DibujarTanqueRojo();

	            	      //dibujar sprite de tanque celeste
	            	          x_celeste = STAGE_W - TANK_W;
	            	          y_celeste = STAGE_H - TANK_H;
	            	          dir_celeste = DIR_LEFT;
	            	          DibujarTanqueCeleste();
	            	      }

	              }
	              break;

	          case 2:
	        	  if(stage2 == 0){
	        	  	    stage2 = 1;
	        	  	    stage1 = 0;
	        	  	    stage3 = 0;
	        	  	    LCD_DrawImageFromSD_Fast("0:/STAGE2.bin", 0, 0, 320, 240);
	        	  	    //DIBUJAMOS TANQUE ROJO
	        	  	    x_rojo = 31;
	        	  	  	y_rojo = 200;
	        	  	  	DibujarTanqueRojo();

	        	  	  	x_celeste = 195 - TANK_W;
	        	  	  	y_celeste = 69 - TANK_H;
	        	  	  	dir_celeste = DIR_LEFT;
	        	  	  	DibujarTanqueCeleste();

	        	  	}
	              break;
	          case 3:
	        	  if(stage3 == 0){
	        		  stage2 = 0;
	        		  stage1 = 0;
	        		  stage3 = 1;
	        		  LCD_DrawImageFromSD_Fast("0:/STAGE3.bin", 0, 0, 320, 240);
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
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
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
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
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
FRESULT LCD_DrawImageFromSD_Fast(const char *filename,
                                 uint16_t x,
                                 uint16_t y,
                                 uint16_t width,
                                 uint16_t height)
{
    FRESULT fres_local;
    UINT bytesRead;
    char msg[100];


    if (width > 320) {
        transmit_uart("Error: width > 320\r\n");
        return FR_INVALID_PARAMETER;
    }

    fres_local = f_open(&fil, filename, FA_READ);
    if (fres_local != FR_OK) {
        sprintf(msg, "Open error: %d\r\n", fres_local);
        transmit_uart(msg);
        return fres_local;
    }

    HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin, GPIO_PIN_RESET);
    SetWindows(x, y, x + width - 1, y + height - 1);
    HAL_GPIO_WritePin(GPIOA, LCD_RS_Pin, GPIO_PIN_SET);

    uint16_t rowsRemaining = height;
    while (rowsRemaining > 0) {

        uint16_t rowsToRead = (rowsRemaining > CHUNK_ROWS) ? CHUNK_ROWS : rowsRemaining;
        uint32_t bytesToRead = width * rowsToRead * 2;

        fres_local = f_read(&fil, blockBuffer, bytesToRead, &bytesRead);
        if (fres_local != FR_OK) {
            sprintf(msg, "Read error: %d\r\n", fres_local);
            transmit_uart(msg);
            f_close(&fil);
            HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin, GPIO_PIN_SET);
            return fres_local;
        }

        if (bytesRead != bytesToRead) {
            transmit_uart("Partial read\r\n");
            f_close(&fil);
            HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin, GPIO_PIN_SET);
            return FR_INT_ERR;
        }

        for (uint32_t i = 0; i < bytesToRead; i += 2) {
            uint8_t hi = blockBuffer[i];
            uint8_t lo = blockBuffer[i + 1];

            // Si los colores salen mal, intercambia:
            // uint8_t temp = hi; hi = lo; lo = temp;

            LCD_DATA(hi);
            LCD_DATA(lo);
        }

        rowsRemaining -= rowsToRead;
    }

    HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin, GPIO_PIN_SET);
    f_close(&fil);

    transmit_uart("Image drawn successfully\r\n");
    return FR_OK;
}

//---- Funciones para lograr colisiones ----//
uint16_t LeerPixelStage1(uint16_t x, uint16_t y)
{
    UINT br;
    uint8_t pix[2];
    DWORD offset;

    if (x >= STAGE_W || y >= STAGE_H) return 0xFFFF;

    offset = ((DWORD)y * STAGE_W + x) * 2;

    if (f_open(&fil, STAGE1_FILE, FA_READ) != FR_OK) return 0xFFFF;
    if (f_lseek(&fil, offset) != FR_OK) {
        f_close(&fil);
        return 0xFFFF;
    }
    if (f_read(&fil, pix, 2, &br) != FR_OK || br != 2) {
        f_close(&fil);
        return 0xFFFF;
    }

    f_close(&fil);
    return ((uint16_t)pix[0] << 8) | pix[1];
}

uint8_t PuntoTransitable(uint16_t x, uint16_t y)
{
    uint16_t color = LeerPixelStage1(x, y);
    return (color == COLOR_CAMINO);
}

uint8_t PuedeMoverTanqueRojo(uint16_t newX, uint16_t newY, TankDir dir)
{
    uint16_t left, right, top, bottom, midX, midY;

    if (newX > (STAGE_W - TANK_W)) return 0;
    if (newY > (STAGE_H - TANK_H)) return 0;

    left   = newX;
    right  = newX + TANK_W - 1;
    top    = newY;
    bottom = newY + TANK_H - 1;
    midX   = newX + (TANK_W / 2);
    midY   = newY + (TANK_H / 2);

    switch (dir)
    {
        case DIR_RIGHT:
            if (!PuntoTransitable(right, top)) return 0;
            if (!PuntoTransitable(right, midY)) return 0;
            if (!PuntoTransitable(right, bottom)) return 0;
            break;

        case DIR_LEFT:
            if (!PuntoTransitable(left, top)) return 0;
            if (!PuntoTransitable(left, midY)) return 0;
            if (!PuntoTransitable(left, bottom)) return 0;
            break;

        case DIR_UP:
            if (!PuntoTransitable(left, top)) return 0;
            if (!PuntoTransitable(midX, top)) return 0;
            if (!PuntoTransitable(right, top)) return 0;
            break;

        case DIR_DOWN:
            if (!PuntoTransitable(left, bottom)) return 0;
            if (!PuntoTransitable(midX, bottom)) return 0;
            if (!PuntoTransitable(right, bottom)) return 0;
            break;
    }

    return 1;
}

FRESULT LCD_DrawRegionFromSD(const char *filename,
                             uint16_t srcX, uint16_t srcY,
                             uint16_t width, uint16_t height,
                             uint16_t dstX, uint16_t dstY)
{
    FRESULT fr;
    UINT br;
    uint32_t rowBytes = width * 2;

    fr = f_open(&fil, filename, FA_READ);
    if (fr != FR_OK) return fr;

    HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin, GPIO_PIN_RESET);
    SetWindows(dstX, dstY, dstX + width - 1, dstY + height - 1);
    HAL_GPIO_WritePin(GPIOA, LCD_RS_Pin, GPIO_PIN_SET);

    for (uint16_t row = 0; row < height; row++)
    {
        DWORD offset = (((DWORD)(srcY + row) * STAGE_W) + srcX) * 2;

        fr = f_lseek(&fil, offset);
        if (fr != FR_OK) {
            HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin, GPIO_PIN_SET);
            f_close(&fil);
            return fr;
        }

        fr = f_read(&fil, blockBuffer, rowBytes, &br);
        if (fr != FR_OK || br != rowBytes) {
            HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin, GPIO_PIN_SET);
            f_close(&fil);
            return FR_INT_ERR;
        }

        for (uint32_t i = 0; i < rowBytes; i += 2)
        {
            LCD_DATA(blockBuffer[i]);
            LCD_DATA(blockBuffer[i + 1]);
        }
    }

    HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin, GPIO_PIN_SET);
    f_close(&fil);
    return FR_OK;
}

const uint16_t *SpriteTanqueRojo(TankDir dir)
{
    switch (dir)
    {
        case DIR_UP:    return TANK_ARRIBA_ROJO;
        case DIR_DOWN:  return TANK_ABAJO_ROJO;
        case DIR_LEFT:  return TANK_IZQUIERDA_ROJO;
        case DIR_RIGHT: return TANK_DERECHA_ROJO;
        default:        return TANK_DERECHA_ROJO;
    }
}

void DibujarTanqueRojo(void)
{
    LCD_Sprite(x_rojo, y_rojo, TANK_W, TANK_H, SpriteTanqueRojo(dir_rojo), 1, 0, 0, 0);
}
void MoverTanqueRojo(TankDir dir)
{
    int16_t newX = x_rojo;
    int16_t newY = y_rojo;

    switch (dir)
    {
        case DIR_UP:    newY -= MOVE_STEP; break;
        case DIR_DOWN:  newY += MOVE_STEP; break;
        case DIR_LEFT:  newX -= MOVE_STEP; break;
        case DIR_RIGHT: newX += MOVE_STEP; break;
    }

    if (newX < 0 || newY < 0) return;
    if (!PuedeMoverTanqueRojo((uint16_t)newX, (uint16_t)newY, dir)) return;

    // Restaurar fondo donde estaba el tanque
    LCD_DrawRegionFromSD(STAGE1_FILE, x_rojo, y_rojo, TANK_W, TANK_H, x_rojo, y_rojo);

    // Actualizar posición y dirección
    x_rojo = (uint16_t)newX;
    y_rojo = (uint16_t)newY;
    dir_rojo = dir;

    // Dibujar tanque en nueva posición
    DibujarTanqueRojo();
}
uint8_t PuedeMoverTanqueCeleste(uint16_t newX, uint16_t newY, TankDir dir)
{
    uint16_t left, right, top, bottom, midX, midY;

    if (newX > (STAGE_W - TANK_W)) return 0;
    if (newY > (STAGE_H - TANK_H)) return 0;

    left   = newX;
    right  = newX + TANK_W - 1;
    top    = newY;
    bottom = newY + TANK_H - 1;
    midX   = newX + (TANK_W / 2);
    midY   = newY + (TANK_H / 2);

    switch (dir)
    {
        case DIR_RIGHT:
            if (!PuntoTransitable(right, top)) return 0;
            if (!PuntoTransitable(right, midY)) return 0;
            if (!PuntoTransitable(right, bottom)) return 0;
            break;

        case DIR_LEFT:
            if (!PuntoTransitable(left, top)) return 0;
            if (!PuntoTransitable(left, midY)) return 0;
            if (!PuntoTransitable(left, bottom)) return 0;
            break;

        case DIR_UP:
            if (!PuntoTransitable(left, top)) return 0;
            if (!PuntoTransitable(midX, top)) return 0;
            if (!PuntoTransitable(right, top)) return 0;
            break;

        case DIR_DOWN:
            if (!PuntoTransitable(left, bottom)) return 0;
            if (!PuntoTransitable(midX, bottom)) return 0;
            if (!PuntoTransitable(right, bottom)) return 0;
            break;
    }

    return 1;
}

const uint16_t *SpriteTanqueCeleste(TankDir dir)
{
    switch (dir)
    {
        case DIR_UP:    return TANK_ARRIBA_CELESTE;
        case DIR_DOWN:  return TANK_ABAJO_CELESTE;
        case DIR_LEFT:  return TANK_IZQUIERDA_CELESTE;
        case DIR_RIGHT: return TANK_DERECHA_CELESTE;
        default:        return TANK_IZQUIERDA_CELESTE;
    }
}

void DibujarTanqueCeleste(void)
{
    LCD_Sprite(x_celeste, y_celeste, TANK_W, TANK_H, SpriteTanqueCeleste(dir_celeste), 1, 0, 0, 0);
}

void MoverTanqueCeleste(TankDir dir)
{
    int16_t newX = x_celeste;
    int16_t newY = y_celeste;

    switch (dir)
    {
        case DIR_UP:    newY -= MOVE_STEP; break;
        case DIR_DOWN:  newY += MOVE_STEP; break;
        case DIR_LEFT:  newX -= MOVE_STEP; break;
        case DIR_RIGHT: newX += MOVE_STEP; break;
    }

    if (newX < 0 || newY < 0) return;
    if (!PuedeMoverTanqueCeleste((uint16_t)newX, (uint16_t)newY, dir)) return;

    LCD_DrawRegionFromSD(STAGE1_FILE, x_celeste, y_celeste, TANK_W, TANK_H, x_celeste, y_celeste);

    x_celeste = (uint16_t)newX;
    y_celeste = (uint16_t)newY;
    dir_celeste = dir;

    DibujarTanqueCeleste();
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
