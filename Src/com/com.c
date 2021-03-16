/**
  * Copyright (c) 2021 M. Kajdak. All rights reserved.
  *
  ******************************************************************************
  * @file           : com.c
  * @brief          : Header for <source file name> file.
  *                   This file contains <what does the file contains>
  ******************************************************************************
  */

#include <com/com.h>

#include <com/message_control.h>
#include <com/interface/consts.h>
#include <main/log.h>

#include <malloc.h>
#include <memory.h>

/*
 * STRUCTURES FUNCTIONS DECLARATIONS BEGIN
 */
void newRxData(RxBuffer *rxBuffer);

void noNewRxData(RxBuffer *rxBuffer);

void startUartRx(RxBuffer *rxBuffer, uint16_t N);

uint8_t isNewData(RxBuffer *rxBuffer);

enum DataTypes nextReadType(RxBuffer *rxBuffer);

void scheduleNextRead(CommunicationContext *communicationContext, enum DataTypes dataType);

void clearBuffer(RxBuffer *rxBuffer);

enum DataTypes processFrameCtrlData(CommunicationContext* communicationContext);

void processUserData(CommunicationContext* communicationContext);

/*
 *      STRUCTURES FUNCTIONS DECLARATIONS END
 */

/*
 *  PUBLIC BEGIN
 */

void initCom(CommunicationContext *communicationContext)
{
    communicationContext->rxBuffer.data = NULL;
    communicationContext->rxBuffer.nextRead = FrameCtrlData;

    noNewRxData(&communicationContext->rxBuffer);
    startUartRx(&communicationContext->rxBuffer, HEADER_SIZE);

    communicationContext->msgControl.storage = createMessageStorage();
    communicationContext->msgControl.subscriptionContainer = createSubscriptionContainer();
}

void workCom(CommunicationContext *communicationContext)
{
    if (!isNewData(&communicationContext->rxBuffer))
    {
        return;
    }

    switch (nextReadType(&communicationContext->rxBuffer))
    {
        case FrameCtrlData:
        {
            scheduleNextRead(communicationContext, processFrameCtrlData(communicationContext));
            break;
        }
        case UserData:
        {
            processUserData(communicationContext);
            scheduleNextRead(communicationContext, FrameCtrlData);
            break;
        }
        default:
        {
            printf("Incorrect data type: %d\n", nextReadType(&communicationContext->rxBuffer));
            scheduleNextRead(communicationContext, FrameCtrlData);
        }
    }

    uint16_t nextReadDataSize = HEADER_SIZE;
    if (nextReadType(&communicationContext->rxBuffer) == UserData)
    {
        nextReadDataSize = getMessageSize(communicationContext->msgControl.nextReadMessageId);
    }

    clearBuffer(&communicationContext->rxBuffer);
    startUartRx(&communicationContext->rxBuffer, nextReadDataSize);

    processSubscriptions(&communicationContext->msgControl);
}

void comReceiveCallback(UART_HandleTypeDef *huart, CommunicationContext *communicationContext)
{
    if (huart->Instance != USART2)
    {
        return;
    }
    newRxData(&communicationContext->rxBuffer);
}

uint16_t subscribe(CommunicationContext* communicationContext, uint8_t id, MessageHandler messageHandler)
{
    return addSubscriptionForMessage(&communicationContext->msgControl, id, messageHandler);
}

void unsubscribe(CommunicationContext* communicationContext, uint16_t subscriptionId)
{
    removeSubscriptionWithId(&communicationContext->msgControl, subscriptionId);
}


/*
 *  PUBLIC END
 */


/*
 *  PRIVATE BEGIN
 */

void newRxData(RxBuffer *rxBuffer)
{
    rxBuffer->newData = 1;
}

void startUartRx(RxBuffer *rxBuffer, uint16_t N)
{
    rxBuffer->data = (uint8_t *) malloc(N * sizeof(uint8_t));
    HAL_StatusTypeDef status = HAL_UART_Receive_DMA(&huart2, rxBuffer->data, N);

    if (status != HAL_OK)
    {
        printf("%d\n", status);
    }
}

void noNewRxData(RxBuffer *rxBuffer)
{
    rxBuffer->newData = 0;
}

uint8_t isNewData(RxBuffer *rxBuffer)
{
    return rxBuffer->newData;
}

enum DataTypes nextReadType(RxBuffer *rxBuffer)
{
    return rxBuffer->nextRead;
}

void scheduleNextRead(CommunicationContext *communicationContext, enum DataTypes dataType)
{
    communicationContext->rxBuffer.nextRead = dataType;

    if (dataType == UserData)
    {
        communicationContext->msgControl.nextReadMessageId = communicationContext->rxBuffer.data[1];
        return;
    }
    communicationContext->msgControl.nextReadMessageId = WAITING_FOR_ID;
}

void clearBuffer(RxBuffer *rxBuffer)
{
    if (rxBuffer->data == NULL) return;

    free(rxBuffer->data);
    rxBuffer->data = NULL;
    noNewRxData(rxBuffer);
}


enum DataTypes processFrameCtrlData(CommunicationContext* communicationContext)
{
    int status = validateCtrlData(&communicationContext->msgControl, communicationContext->rxBuffer.data);

    if (status != 0)
    {
        return FrameCtrlData;
    }
    return UserData;
}

void processUserData(CommunicationContext* communicationContext)
{
    void* msg = deserialize((char*)communicationContext->rxBuffer.data,communicationContext->msgControl.nextReadMessageId);

    if (msg == NULL)
    {
        printf("Unknown message\n");
        return;
    }

    Message temp;
    memcpy(&temp, msg, getMessageSize(communicationContext->msgControl.nextReadMessageId));
    free(msg);

    addMessage(communicationContext->msgControl.storage, &temp);
}

/*
 *  PRIVATE END
 */
