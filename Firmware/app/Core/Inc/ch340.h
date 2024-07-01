#ifndef CH340_H
#define CH340_H

#include "usbd_def.h"
#include "usbd_cdc.h"

#define CMD_R  0x95
#define CMD_W  0x9A
#define CMD_C1 0xA1
#define CMD_C2 0xA4
#define CMD_C3 0x5F

void CH340_Requset_Handle(USBD_HandleTypeDef *pdev, USBD_CDC_HandleTypeDef *hcdc, USBD_SetupReqTypedef *req);

#endif
