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
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "usbd_cdc_if.h"
#include <stdio.h>
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

static uint32_t s_lastHeartbeatTick = 0;
static uint32_t s_heartbeatCounter = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

static uint8_t USB_SendString(const char *text);
static void USB_ProcessCommand(char *cmdLine);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static uint8_t USB_SendString(const char *text)
{
  uint16_t len = (uint16_t)strlen(text);
  uint32_t startTick = HAL_GetTick();

  while (CDC_Transmit_FS((uint8_t *)text, len) == USBD_BUSY)
  {
    if ((HAL_GetTick() - startTick) > 100)
    {
      return USBD_BUSY;
    }
    HAL_Delay(1);
  }

  return USBD_OK;
}

static void USB_ProcessCommand(char *cmdLine)
{
  size_t len = strlen(cmdLine);
  const char *echoText;
  char txBuf[160];

  while (len > 0 && (cmdLine[len - 1] == '\r' || cmdLine[len - 1] == '\n'))
  {
    cmdLine[len - 1] = '\0';
    len--;
  }

  if (len == 0)
  {
    return;
  }

  if (strcmp(cmdLine, "help") == 0)
  {
    USB_SendString("Commands: help, ping, status, dice, echo <text>\r\n");
  }
  else if (strcmp(cmdLine, "ping") == 0)
  {
    USB_SendString("pong\r\n");
  }
  else if (strcmp(cmdLine, "status") == 0)
  {
    int n = snprintf(txBuf, sizeof(txBuf),
                     "Uptime: %lu ms, Heartbeat: %lu\r\n",
                     HAL_GetTick(), s_heartbeatCounter);
    if (n > 0)
    {
      USB_SendString(txBuf);
    }
  }
  else if (strcmp(cmdLine, "dice") == 0)
  {
    uint32_t diceValue = (HAL_GetTick() % 6UL) + 1UL;
    int n = snprintf(txBuf, sizeof(txBuf),
                     "Dice: %lu\r\n", diceValue);
    if (n > 0)
    {
      USB_SendString(txBuf);
    }
  }
  else if (strncmp(cmdLine, "echo ", 5) == 0)
  {
    echoText = &cmdLine[5];
    if (*echoText == '\0')
    {
      USB_SendString("Usage: echo <text>\r\n");
    }
    else
    {
      int n = snprintf(txBuf, sizeof(txBuf), "Echo: %s\r\n", echoText);
      if (n > 0)
      {
        USB_SendString(txBuf);
      }
    }
  }
  else
  {
    int n = snprintf(txBuf, sizeof(txBuf), "Unknown command: %s\r\n", cmdLine);
    if (n > 0)
    {
      USB_SendString(txBuf);
    }
  }
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
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  HAL_Delay(1200);
  USB_SendString("\r\n===== STM32 USB CDC Demo =====\r\n");
  USB_SendString("Type 'help' and press Enter.\r\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if ((HAL_GetTick() - s_lastHeartbeatTick) >= 1000)
    {
      char heartbeatMsg[64];
      s_lastHeartbeatTick = HAL_GetTick();
      s_heartbeatCounter++;

      if ((s_heartbeatCounter % 5UL) == 0UL)
      {
        int n = snprintf(heartbeatMsg, sizeof(heartbeatMsg),
                         "Heartbeat %lu\r\n", s_heartbeatCounter);
        if (n > 0)
        {
          USB_SendString(heartbeatMsg);
        }
      }
    }

    if (g_usbRxFlag != 0U)
    {
      uint32_t rxLen;
      char cmdBuf[APP_RX_DATA_SIZE + 1U];

      __disable_irq();
      rxLen = g_usbRxLength;
      if (rxLen > APP_RX_DATA_SIZE)
      {
        rxLen = APP_RX_DATA_SIZE;
      }
      memcpy(cmdBuf, g_usbRxBuffer, rxLen);
      g_usbRxFlag = 0U;
      g_usbRxLength = 0U;
      __enable_irq();

      cmdBuf[rxLen] = '\0';
      USB_ProcessCommand(cmdBuf);
    }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
