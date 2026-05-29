/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body for SHT31 to UART Reading (Fully Validated)
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h> // Required for sprintf to format the text
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SHT31_I2C_ADDR         (0x44 << 1) // I2C address 0x88
#define SHT31_MEAS_HIGH_REP    0x2C06      // Measurement command
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
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
  uint8_t cmd[2];
  uint8_t rx_data[6];
  float temperature = 0.0f;
  float humidity = 0.0f;
  char uart_buf[100];
  int msg_len;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init(); // Initializes the virtual COM port link to your PC

  /* USER CODE BEGIN 2 */
  // Split the 16-bit command into two 8-bit bytes for transmission
  cmd[0] = (SHT31_MEAS_HIGH_REP >> 8) & 0xFF;
  cmd[1] = SHT31_MEAS_HIGH_REP & 0xFF;

  // Print a startup message to the console to confirm UART is working
  msg_len = sprintf(uart_buf, "--- SHT31 Temperature & Humidity Sensor System Started ---\r\n");
  HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, msg_len, HAL_MAX_DELAY);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // 1. Transmit measurement command to the SHT31 via I2C
    if (HAL_I2C_Master_Transmit(&hi2c1, SHT31_I2C_ADDR, cmd, 2, HAL_MAX_DELAY) == HAL_OK)
    {
        // 2. Wait for internal ADC conversion inside the sensor
        HAL_Delay(20);

        // 3. Read back 6 bytes of raw environmental data
        if (HAL_I2C_Master_Receive(&hi2c1, SHT31_I2C_ADDR, rx_data, 6, HAL_MAX_DELAY) == HAL_OK)
        {
            // 4. Extract raw bits and compute physical temperature
            uint16_t raw_temp = (rx_data[0] << 8) | rx_data[1];
            temperature = -45.0f + 175.0f * ((float)raw_temp / 65535.0f);

            // 5. Extract raw bits and compute physical relative humidity
            uint16_t raw_hum = (rx_data[3] << 8) | rx_data[4];
            humidity = 100.0f * ((float)raw_hum / 65535.0f);

            // 6. Format data into a readable string frame
            msg_len = sprintf(uart_buf, "Temp: %.2f °C | Humidity: %.2f %%\r\n", temperature, humidity);

            // 7. Transmit text string over the virtual USB serial port (USART2)
            HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, msg_len, HAL_MAX_DELAY);
        }
        else
        {
            // Error handling if sensor doesn't respond during read phase
            msg_len = sprintf(uart_buf, "Error: Failed to receive data from sensor.\r\n");
            HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, msg_len, HAL_MAX_DELAY);
        }
    }
    else
    {
        // Error handling if sensor doesn't acknowledge the initial request
        msg_len = sprintf(uart_buf, "Error: Failed to transmit command to sensor.\r\n");
        HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, msg_len, HAL_MAX_DELAY);
    }

    // Delay execution for 2 seconds before taking the next reading
    HAL_Delay(2000);
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

  // FIXED: Corrected spelling to VOLTAGE instead of VOLVESTATE
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
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
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10D19CE4;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE(); // Enabled for USART2 Pins (PA2/PA3)
  __HAL_RCC_GPIOB_CLK_ENABLE(); // Enabled for I2C1 Pins (PB8/PB9)
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif /* USE_FULL_ASSERT */
