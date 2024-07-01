#include "ch340.h"


uint16_t ch341_state = 0xdeff;
uint16_t ch341_2C2C = 0x0B0B;
uint16_t ch341_2518 = 0x00C3;
uint16_t ch341_CMD_C3 = 0x0030;
uint16_t ch341_CRTSCTS = 0x0000;   // 0x0101 = ON, 0x0000 = 0FF
static uint8_t zero[2] = { 0, 0 };

void CH340_Requset_Handle(USBD_HandleTypeDef *pdev,
		USBD_CDC_HandleTypeDef *hcdc, USBD_SetupReqTypedef *req) {
	uint16_t wValue = req->wValue;

	switch (req->bRequest) {
	case CMD_R:
		switch (wValue) {
		// Poll Status
		case 0x0706:
			USBD_CtlSendData(pdev, (uint8_t*) &ch341_state, req->wLength);
			break;
		case 0x2518:
			USBD_CtlSendData(pdev, (uint8_t*) &ch341_2518, req->wLength);
			break;
		case 0x2C2C:
			USBD_CtlSendData(pdev, (uint8_t*) &ch341_2C2C, req->wLength);
			break;
		default:
			break;
		}
		break;
	// Start?
	case CMD_C1:
		switch (wValue) {
		// Initialize
		case 0x0000:
//			ch341_state = 0xdeff;
//			ch341_2C2C = 0x0B0B;
			break;
		// Open port, configure baud, stop bits ...
		case 0xC39C:
//			ch341_2C2C = 0x8888;
			break;
		case 0x0F2C:
			switch(req->wIndex){
			case 0x0007:
				break;
			}
			break;
		default:
			break;
		}
		USBD_CtlSendData(pdev, (uint8_t*) &zero, req->wLength);
		break;
	// Shutdown?
	case CMD_C2:
		USBD_CtlSendData(pdev, (uint8_t*) &ch341_state, req->wLength);
		ch341_state = 0xdeff;
		break;
	case CMD_C3:
		USBD_CtlSendData(pdev, (uint8_t*) &ch341_CMD_C3, req->wLength);
		break;
	case CMD_W:
		switch (wValue) {
		case 0x1312:
//			if(req->wIndex)
//				ch341_2C2C = 0x0707;
			break;
		// Set CRTSCTS
		case 0x2727:
			asm("NOP");
			break;
		default:
			break;
		}
	default:
		USBD_CtlSendData(pdev, (uint8_t*) &zero, req->wLength);
		break;
	}
	return;
}
