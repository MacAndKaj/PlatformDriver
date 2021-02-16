/**
  * Copyright (c) 2021 M. Kajdak. All rights reserved.
  *
  ******************************************************************************
  * @file           : com.h
  * @brief          : Header for <source file name> file.
  *                   This file contains <what does the file contains>
  ******************************************************************************
  */
#ifndef MOTORDRIVER_COM_H
#define MOTORDRIVER_COM_H

#include <com/message_control.h>
#include <com/interface/consts.h>

#include <usart.h>


struct RxBuffer
{
    uint8_t newData;
    uint8_t *data;
    enum DataTypes nextRead;
};
typedef struct RxBuffer RxBuffer;

struct CommunicationContext
{
    RxBuffer rxBuffer;
    MessageControl msgControl;
};
typedef struct CommunicationContext CommunicationContext;

void initCom(CommunicationContext* communicationContext);
void workCom(CommunicationContext* communicationContext);

uint16_t subscribe(uint8_t id, void *(Message));
void unsubscribe(uint16_t subscriptionId);

void comReceiveCallback(UART_HandleTypeDef *huart, CommunicationContext* communicationContext);

#endif //MOTORDRIVER_COM_H
