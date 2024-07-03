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
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef struct PortSettings {
	uint8_t defaultState :1;
	uint8_t groupControl :1;
	uint8_t polarity :1;
	uint8_t reserved :5;
} __attribute__((packed)) PortSettings_t;

typedef struct DeviceSettings {
	PortSettings_t usb1;
	PortSettings_t usb2;
	PortSettings_t usb3;
	PortSettings_t relay;
} __attribute__((packed)) DeviceSettings_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern TIM_HandleTypeDef htim3;
uint8_t cdcRxBuf[64] = { 0 };
uint8_t rxLen = 0;
uint8_t portCtrlState = 0;
uint8_t portCtrlFlag = 0x00;

__attribute__((section(".persist_data"))) DeviceSettings_t Settings = {
		.usb1 =		{ .defaultState = 1, .groupControl = 1, .polarity = 0 },
		.usb2 = 	{ .defaultState = 1, .groupControl = 1, .polarity = 0 },
		.usb3 = 	{ .defaultState = 1, .groupControl = 1, .polarity = 0 },
		.relay = 	{ .defaultState = 0, .groupControl = 1, .polarity = 1 },
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Initialize_Ports();
void Enable_All_Ports();
void Disable_All_Ports();

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
  MX_USB_DEVICE_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim3);
  Initialize_Ports();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	while (1) {


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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void Initialize_Ports()
{
	portCtrlFlag = 0xF1;
	portCtrlState = 0x01;
}

void Enable_All_Ports() {

	portCtrlFlag = 0x11;
	portCtrlState = 0x01;
}

void Disable_All_Ports() {

	portCtrlFlag = 0x01;
	portCtrlState = 0x00;
}

void Port_Ctrl_Handler() {
	static uint8_t cooldown = 0;

	if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) && (cooldown == 0)) {
		if (portCtrlState) {
			Disable_All_Ports();
		} else {
			Enable_All_Ports();
		}
		cooldown = 10;
	}

	switch (portCtrlFlag) {
	case 0x01:
		if(Settings.usb1.groupControl){
			HAL_GPIO_WritePin(GPIOA, USB1_EN_PIN, Settings.usb1.polarity & 0x1);
		}
		portCtrlFlag += 1;
		break;
	case 0x02:
		if(Settings.usb2.groupControl){
			HAL_GPIO_WritePin(GPIOA, USB2_EN_PIN, Settings.usb2.polarity & 0x1);
		}
		portCtrlFlag += 1;
		break;
	case 0x03:
		if(Settings.usb3.groupControl){
			HAL_GPIO_WritePin(GPIOA, USB3_EN_PIN, Settings.usb3.polarity & 0x1);
		}
		portCtrlFlag += 1;
		break;
	case 0x04:
		if(Settings.relay.groupControl){
			HAL_GPIO_WritePin(GPIOA, RELAY_EN_PIN, Settings.relay.polarity & 0x1);
		}
		portCtrlFlag = 0x00;
		break;
	case 0x11:
		if(Settings.usb1.groupControl){
			HAL_GPIO_WritePin(GPIOA, USB1_EN_PIN, ~Settings.usb1.polarity & 0x1);
		}
		portCtrlFlag += 1;
		break;
	case 0x12:
		if(Settings.usb2.groupControl){
			HAL_GPIO_WritePin(GPIOA, USB2_EN_PIN, ~Settings.usb2.polarity & 0x1);
		}
		portCtrlFlag += 1;
		break;
	case 0x13:
		if(Settings.usb3.groupControl){
			HAL_GPIO_WritePin(GPIOA, USB3_EN_PIN, ~Settings.usb3.polarity & 0x1);
		}
		portCtrlFlag += 1;
		break;
	case 0x14:
		if(Settings.relay.groupControl){
			HAL_GPIO_WritePin(GPIOA, RELAY_EN_PIN, ~Settings.relay.polarity & 0x1);
		}
		portCtrlFlag = 0x00;
		break;
	case 0xF1:
		HAL_GPIO_WritePin(GPIOA, USB1_EN_PIN, Settings.usb1.defaultState);
		portCtrlFlag += 1;
		break;
	case 0xF2:
		HAL_GPIO_WritePin(GPIOA, USB2_EN_PIN, Settings.usb2.defaultState);
		portCtrlFlag += 1;
		break;
	case 0xF3:
		HAL_GPIO_WritePin(GPIOA, USB3_EN_PIN, Settings.usb3.defaultState);
		portCtrlFlag += 1;
		break;
	case 0xF4:
		HAL_GPIO_WritePin(GPIOA, RELAY_EN_PIN, Settings.relay.defaultState);
		portCtrlFlag = 0x00;
		break;
	default:
		portCtrlFlag = 0x00;
		break;
	}

	cooldown -= cooldown ? 1 : 0;
}

void CDC_Process_Rx()
{
	uint32_t command = 0xFFFFFFFF;

	command = *((uint32_t*) cdcRxBuf);

	switch(command){
	case 0xA20101A0:
		Disable_All_Ports();
		break;
	case 0xA10001A0:
		Enable_All_Ports();
		break;
	default:
		break;
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
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
