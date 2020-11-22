/**
  * Copyright (c) 2020 M. Kajdak. All rights reserved.
  *
  ******************************************************************************
  * @file           : init.c
  * @brief          : Sources for init.h file.
  *                   This file contains definitions of init funcitons.
  ******************************************************************************
  */

#include <controller.h>
#include <init.h>
#include <log.h>
#include <motor.h>
#include <tim.h>

#include <stdio.h>
#include <defs.h>

int init_Peripheries()
{
    HAL_StatusTypeDef state;
    state = HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_3, &leftControllerHandle.pwmDuty, ONE_WORD);
    if (state != HAL_OK)
    {
        return 1;
    }

    state = HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, &rightControllerHandle.pwmDuty, ONE_WORD);
    if (state != HAL_OK)
    {
        return 1;
    }

    state = HAL_TIM_Base_Start_IT(&htim17);
    if (state != HAL_OK)
    {
        return 1;
    }

    return 0;
}

int init_MotorDriver()
{
    initializeLeftMotor();
    initializeRightMotor();
    setLeftDirection(FORWARD);
    setRightDirection(FORWARD);
    return 0;
}


void onRun()
{
    printf("L-speed: %f| pulses: %ld \r\n", getSpeed(&leftMotorHandle), getPulses(&leftMotorHandle));
    printf("R-speed: %f| pulses: %ld \r\n", getSpeed(&rightMotorHandle), getPulses(&rightMotorHandle));
    HAL_Delay(1000);
}

void onExtInterrupt(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin)
    {
        case LeftMotorEncoderB_Pin:
            updateLeftMotorParameters();
            break;
        case RightMotorEncoderB_Pin:
            updateRightMotorParameters();
            break;
    }
}

void onPeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM17)
    {
        updateSpeed(&rightMotorHandle, speedUpdateTime);
        updateSpeed(&leftMotorHandle, speedUpdateTime);
    }
}