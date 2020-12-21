/**
  * Copyright (c) 2020 M. Kajdak. All rights reserved.
  *
  ******************************************************************************
  * @file           : init.c
  * @brief          : Sources for init.h file.
  *                   This file contains definitions of init funcitons.
  ******************************************************************************
  */

#include <platform/boolean.h>
#include <platform/motor_control.h>
#include <platform/defs.h>
#include <platform/init.h>
#include <platform/log.h>
#include <platform/motor_info.h>
#include <platform/pid.h>

#include <tim.h>

#include <stdio.h>

double speed = 0;


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

    noNewRxData();
    startUartRx();

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

void toggleSpeed()
{
    if (speed == 0)
    {
        setPwmDuty(&leftControllerHandle, PWM_PERIOD);
//        speed = 5;
    }
    else
    {
        setPwmDuty(&leftControllerHandle, 0);
//        speed = 0;
    }
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
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

    int8_t speedInt;

    while (TRUE)
    {
        if (isSpeedUpdateFlagSet(&leftMotorHandle))
        {
            disableSpeedUpdateFlag(&leftMotorHandle);
            disableSpeedUpdateFlag(&rightMotorHandle);

//            printf("speed: %f| pwm: %ld | error: %f \n", getSpeed(&leftMotorHandle), getPwmDuty(&leftControllerHandle), error);
            printf("%f|%ld|data: %f\r\n", getSpeed(&leftMotorHandle), getPwmDuty(&leftControllerHandle), speed);

//            error = speed - getSpeed(&leftMotorHandle);

//            setLeftPwm(evaluate(&pid, error, speedUpdateTime));
        }

        if (isNewData())
        {
            speedInt = rxBuffer.rxData[0];
            if (speedInt < 0)
            {
                speed = speedInt - (rxBuffer.rxData[1] * 0.01);
            }
            else
            {
                speed = speedInt + (rxBuffer.rxData[1] * 0.01);
            }
            noNewRxData();
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
    if (huart->Instance == USART2)
    {
        newRxData();
        startUartRx();
    }
}
