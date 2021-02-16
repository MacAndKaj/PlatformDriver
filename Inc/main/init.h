/**
  * Copyright (c) 2020 M. Kajdak. All rights reserved.
  *
  ******************************************************************************
  * @file           : hal_config.h
  * @brief          : Header for <source file name> file.
  *                   This file contains general functions for MotorDriver.
  ******************************************************************************
  */
#ifndef MOTORDRIVER_PLATFORM_INIT_H
#define MOTORDRIVER_PLATFORM_INIT_H

#include <gpio.h>

#define ONE_WORD 1
#define SPEED_UPDATE_FREQ 100.0

static double speedUpdateTime = 1./SPEED_UPDATE_FREQ;

/// Function initializing STM32 peripheries used in project.
/// \return [int]: 0 if not error, 1 - otherwise
int init_Modules();

/// Function initializing Motor Driver configuration.
/// \return [int]: 0 if not error, 1 - otherwise
int init_MotorDriver();

void onRun();
void onExtInterrupt(uint16_t GPIO_Pin);
void onPeriodElapsedCallback(TIM_HandleTypeDef *htim);
void onRxCpltCallback(UART_HandleTypeDef *huart);

#endif //MOTORDRIVER_PLATFORM_INIT_H
