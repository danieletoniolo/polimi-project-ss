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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

// Sensor Identification Enum
typedef enum {
  SENSOR_NONE = 0,
  SENSOR_LIS2DE,
  SENSOR_LIS2DW
} SensorType_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// I2C Addresses (8-bit shifted)
#define LIS2DE_ADDR       (0x28 << 1)
#define LIS2DW_ADDR       (0x18 << 1)

// LIS2DE Registers
#define LIS2DE_CTRL_REG1  0x20
#define LIS2DE_CTRL_REG4  0x23
#define LIS2DE_OUT_X      (0x28 | 0x80)

// LIS2DW Registers
#define LIS2DW_CTRL1      0x20
#define LIS2DW_CTRL2      0x21
#define LIS2DW_CTRL6      0x25
#define LIS2DW_OUT_X_L    0x28

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_rx;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */

// Active Sensor
SensorType_t active_sensor = SENSOR_NONE;

// Sensor reading variables
uint8_t cfg_reg;
uint8_t raw_data[6];
int8_t x_raw, y_raw, z_raw;
float acc_x, acc_y, acc_z;

// UART transmission buffer and length
char tx_buffer[100];
int length;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  if (HAL_I2C_IsDeviceReady(&hi2c1, LIS2DE_ADDR, 2, 100) == HAL_OK) {

    // Select active sensor
    active_sensor = SENSOR_LIS2DE;

    // Initialize LIS2DE (1 Hz, Normal mode, +/- 2g)
    cfg_reg = 0x17; // CTRL_REG1
    HAL_I2C_Mem_Write(&hi2c1, LIS2DE_ADDR, LIS2DE_CTRL_REG1, I2C_MEMADD_SIZE_8BIT, &cfg_reg, 1, 100);
    cfg_reg = 0x00; // CTRL_REG4
    HAL_I2C_Mem_Write(&hi2c1, LIS2DE_ADDR, LIS2DE_CTRL_REG4, I2C_MEMADD_SIZE_8BIT, &cfg_reg, 1, 100);

    length = snprintf(tx_buffer, sizeof(tx_buffer), "LIS2DE detected and configured.\r\n");
    HAL_UART_Transmit_DMA(&huart2, (uint8_t*)tx_buffer, length);

  } else if (HAL_I2C_IsDeviceReady(&hi2c1, LIS2DW_ADDR, 2, 100) == HAL_OK) {

    // Select active sensor
    active_sensor = SENSOR_LIS2DW;

    // Initialize LIS2DW (12.5 Hz, High-Performance mode, BDU on, IF_INC on, +/- 2g)
    cfg_reg = 0x14; // CTRL1
    HAL_I2C_Mem_Write(&hi2c1, LIS2DW_ADDR, LIS2DW_CTRL1, I2C_MEMADD_SIZE_8BIT, &cfg_reg, 1, 100);
    cfg_reg = 0x0C; // CTRL2
    HAL_I2C_Mem_Write(&hi2c1, LIS2DW_ADDR, LIS2DW_CTRL2, I2C_MEMADD_SIZE_8BIT, &cfg_reg, 1, 100);
    cfg_reg = 0x00; // CTRL6
    HAL_I2C_Mem_Write(&hi2c1, LIS2DW_ADDR, LIS2DW_CTRL6, I2C_MEMADD_SIZE_8BIT, &cfg_reg, 1, 100);

    length = snprintf(tx_buffer, sizeof(tx_buffer), "LIS2DW detected and configured.\r\n");
    HAL_UART_Transmit_DMA(&huart2, (uint8_t*)tx_buffer, length);

  } else {

    length = snprintf(tx_buffer, sizeof(tx_buffer), "Error: No Accelerometer Detected.\r\n");
    HAL_UART_Transmit_DMA(&huart2, (uint8_t*)tx_buffer, length);
  }

  // Start the timer in interrupt mode
  HAL_TIM_Base_Start_IT(&htim2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

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
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 8399;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9999;
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
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

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
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

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

// This function is automatically called when a Timer Period elapses
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2) {

    if (active_sensor == SENSOR_LIS2DE) {

      // Start I2C DMA Read for LIS2DE
      HAL_I2C_Mem_Read_DMA(&hi2c1, LIS2DE_ADDR, LIS2DE_OUT_X, I2C_MEMADD_SIZE_8BIT, raw_data, 6);

    } else if (active_sensor == SENSOR_LIS2DW) {

      // Start I2C DMA Read for LIS2DW
      HAL_I2C_Mem_Read_DMA(&hi2c1, LIS2DW_ADDR, LIS2DW_OUT_X_L, I2C_MEMADD_SIZE_8BIT, raw_data, 6);
    }
  }
}

// This function is automatically called when the I2C DMA read finishes
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  if (hi2c->Instance == I2C1) {

    if (active_sensor == SENSOR_LIS2DE) {

      // 8-bit extraction
      int8_t x_raw = (int8_t)raw_data[1];
      int8_t y_raw = (int8_t)raw_data[3];
      int8_t z_raw = (int8_t)raw_data[5];

      // 15.6 mg/digit sensitivity
      acc_x = x_raw * 0.0156f;
      acc_y = y_raw * 0.0156f;
      acc_z = z_raw * 0.0156f;

    } else if (active_sensor == SENSOR_LIS2DW) {

      // 16-bit extraction (14-bit left justified)
      int16_t x_raw = (int16_t)((raw_data[1] << 8) | raw_data[0]);
      int16_t y_raw = (int16_t)((raw_data[3] << 8) | raw_data[2]);
      int16_t z_raw = (int16_t)((raw_data[5] << 8) | raw_data[4]);

      // 0.061 mg/digit sensitivity
      acc_x = x_raw * 0.000061f;
      acc_y = y_raw * 0.000061f;
      acc_z = z_raw * 0.000061f;
    }

    // Send read data via UART DMA
    if (active_sensor != SENSOR_NONE) {
      length = snprintf(tx_buffer, sizeof(tx_buffer), "X: %+.2f g\r\nY: %+.2f g\r\nZ: %+.2f g\r\n\r\n", acc_x, acc_y, acc_z);
      HAL_UART_Transmit_DMA(&huart2, (uint8_t*)tx_buffer, length);
    }
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
