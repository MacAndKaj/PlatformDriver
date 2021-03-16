/**
  * Copyright (c) 2020 M. Kajdak. All rights reserved.
  *
  ******************************************************************************
  * @file           : init.c
  * @brief          : Sources for init.h file.
  *                   This file contains definitions of init funcitons.
  ******************************************************************************
  */

#include <com/com.h>
#include <main/boolean.h>
#include <main/defs.h>
#include <main/init.h>
#include <main/log.h>
#include <platform/motor_control.h>
#include <platform/motor_info.h>
#include <platform/pid.h>

#include <tim.h>

#include <stdio.h>
#include <com/interface/message_ids.h>

double speedLeft = 0;
double speedRight = 0;
CommunicationContext communicationContext;


int init_Modules()
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

double transformSpeed(int8_t speedInt, uint8_t speedFl)
{
    if (speedInt < 0)
    {
        return speedInt - (speedFl * 0.01);
    }
    return speedInt + (speedFl * 0.01);
}

void toggleSpeed(Message* message)
{
    PlatformSetMotorSpeedReq req = message->platformSetMotorSpeedReq;
    if (req.motor == 0)
    {
        speedLeft = transformSpeed(req.speedI, req.speedF);
    }
    else
    {
        speedRight = transformSpeed(req.speedI, req.speedF);
    }
}

void onRun()
{
    double error = 0.;
    PID pid = {.kP = 4000.,
        .kI = 150.,
        .kD = 30.};

    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    HAL_Delay(1000);
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);


    initCom(&communicationContext);
    subscribe(&communicationContext, PLATFORM_SET_MOTOR_SPEED_REQ_ID, toggleSpeed);

    while (TRUE)
    {
        workCom(&communicationContext);
        if (isSpeedUpdateFlagSet(&leftMotorHandle))
        {
            disableSpeedUpdateFlag(&leftMotorHandle);
            disableSpeedUpdateFlag(&rightMotorHandle);

//            printf("speed: %f| pwm: %ld | error: %f \n", getSpeed(&leftMotorHandle), getPwmDuty(&leftControllerHandle), error);
            printf("%f|%ld|data: %f\r\n", getSpeed(&leftMotorHandle), getPwmDuty(&leftControllerHandle), speedLeft);

//            error = speed - getSpeed(&leftMotorHandle);

//            setLeftPwm(evaluate(&pid, error, speedUpdateTime));
        }
    }
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
        case UserButton_Pin:
            toggleSpeed();
            break;
    }
}

// Callbacks with 1kHz frequency
void onPeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM17)
    {
        updateSpeed(&rightMotorHandle, speedUpdateTime);
        updateSpeed(&leftMotorHandle, speedUpdateTime);
        enableSpeedUpdateFlag(&rightMotorHandle);
        enableSpeedUpdateFlag(&leftMotorHandle);
    }
}

void onRxCpltCallback(UART_HandleTypeDef *huart)
{
    comReceiveCallback(huart, &communicationContext);
}
