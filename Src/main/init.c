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
        printf("Left Motor PWM start failed\r\n");
        return 1;
    }

    state = HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, &rightControllerHandle.pwmDuty, ONE_WORD);
    if (state != HAL_OK)
    {
        printf("Right Motor PWM start failed\r\n");
        return 1;
    }

    state = HAL_TIM_Base_Start_IT(&htim17);
    if (state != HAL_OK)
    {
        printf("Timer start failed\r\n");
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
    PlatformSetMotorSpeedReq req = message->msg.platformSetMotorSpeedReq;
    if (req.motor == 0)
    {
        speedLeft = transformSpeed(req.speedI, req.speedF);
        printf("New left speed: %f\r\n", speedLeft);
    }
    else
    {
        speedRight = transformSpeed(req.speedI, req.speedF);
        printf("New right speed: %f\r\n", speedRight);
    }
}

void onRun()
{
    double errorLeft = 0.;
    PID pidControllerLeft = {.kP = 4000.,
        .kI = 150.,
        .kD = 30.};

    double errorRight = 0.;
    PID pidControllerRight = {.kP = 4000.,
        .kI = 150.,
        .kD = 30.};

    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    HAL_Delay(300);
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

    initCom(&communicationContext);
    subscribe(&communicationContext, PLATFORM_SET_MOTOR_SPEED_REQ_ID, toggleSpeed);

    while (TRUE)
    {
        workCom(&communicationContext);
        if (isSpeedUpdateFlagSet(&leftMotorHandle))
        {
            disableSpeedUpdateFlag(&leftMotorHandle);

            errorLeft = speedLeft - getSpeed(&leftMotorHandle);

            setLeftPwm(evaluate(&pidControllerLeft, errorLeft, speedUpdateTime));
        }

        if (isSpeedUpdateFlagSet(&rightMotorHandle))
        {
            disableSpeedUpdateFlag(&rightMotorHandle);

            errorRight = speedRight - getSpeed(&rightMotorHandle);

            setRightPwm(evaluate(&pidControllerRight, errorRight, speedUpdateTime));
        }
    }
}

void onExtInterrupt(uint16_t GPIO_Pin)
{
    PlatformSetMotorSpeedReq req = {
        .motor = 0,
        .speedI = 5,
        .speedF = 0
    };
    Message msg = {
        .messageId = PLATFORM_SET_MOTOR_SPEED_REQ_ID,
        .msg.platformSetMotorSpeedReq = req
    };

    switch (GPIO_Pin)
    {
        case LeftMotorEncoderB_Pin:
            updateLeftMotorParameters();
            break;
        case RightMotorEncoderB_Pin:
            updateRightMotorParameters();
            break;
        case UserButton_Pin:
            toggleSpeed(&msg);
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
