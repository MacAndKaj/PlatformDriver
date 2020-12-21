/**
  * Copyright (c) 2020 M. Kajdak. All rights reserved.
  *
  ******************************************************************************
  * @file           : log.c
  * @brief          : Header for <source file name> file.
  *                   This file contains <what does the file contains>
  ******************************************************************************
  */
#include <platform/log.h>

#include <usart.h>

#include <errno.h>
#include <sys/unistd.h>
#include <stdio.h>


UartBuffer rxBuffer = {
    .newData = 0,
    .rxData = {0}
};

int _write(int file, char *ptr, int len)
{
    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO))
    {
        errno = EBADF;
        return -1;
    }

    // arbitrary timeout 1000
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart2, (uint8_t*)ptr, len, 100);

    return (status == HAL_OK ? len : 0);
}

void newRxData()
{
    rxBuffer.newData = 1;
}

void startUartRx()
{
    HAL_StatusTypeDef status = HAL_UART_Receive_DMA(&huart2, rxBuffer.rxData, 2);

    if (status != HAL_OK)
    {
        printf("%d\n", status);
    }
}

void noNewRxData()
{
    rxBuffer.newData = 0;
}

uint8_t isNewData()
{
    return rxBuffer.newData;
}
