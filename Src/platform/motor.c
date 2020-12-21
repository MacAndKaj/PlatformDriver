/**
  * Copyright (c) 2020 M. Kajdak. All rights reserved.
  *
  ******************************************************************************
  * @file           : motor.c
  * @brief          : Sources for motor.h file.
  ******************************************************************************
  */
#include <platform/boolean.h>
#include <platform/defs.h>
#include <platform/motor.h>

#include <stm32f0xx_hal.h>

#include <math.h>

/**
  ******************************************************************************
  *
  * Static variables definitions
  *
  *******************************************************************************
  */
GPIO_PinState leftMotorEncoderA_PinLastState;
GPIO_PinState rightMotorEncoderA_PinLastState;
MotorHandle leftMotorHandle;
MotorHandle rightMotorHandle;
/**
  ******************************************************************************
  *
  * General functions begin
  *
  *******************************************************************************
  */
void setForward(MotorHandle *handle)
{
    handle->direction = FORWARD;
}

int isForward(const MotorHandle *handle)
{
    return handle->direction == FORWARD;
}

void setBackward(MotorHandle *handle)
{
    handle->direction = BACKWARD;
}

void setPulses(MotorHandle *handle, int32_t newPulses)
{
    handle->pulses = newPulses;
}

int32_t getPulses(const MotorHandle *handle)
{
    return handle->pulses;
}

void setSpeed(MotorHandle *handle, double newSpeed)
{
    handle->speed = newSpeed;
}

double getSpeed(const MotorHandle *handle)
{
    return handle->speed;
}

void enableSpeedUpdateFlag(MotorHandle *handle)
{
    handle->updateFlag = TRUE;
}

void disableSpeedUpdateFlag(MotorHandle *handle)
{
    handle->updateFlag = FALSE;
}

uint8_t isSpeedUpdateFlagSet(MotorHandle *handle)
{
    return handle->updateFlag;
}

void updateSpeed(MotorHandle *handle, double t)
{
    if (t == 0.) return;

    double rounds = getPulses(handle) / PULSES_PER_ROUND;
    setPulses(handle, 0);

    setSpeed(handle, 2.*M_PI*rounds / t);
}

void Motor_Init(MotorHandle *handle)
{
    setForward(handle);
    setPulses(handle, 0);
    setSpeed(handle, 0);
    disableSpeedUpdateFlag(handle);
}

/**
  ******************************************************************************
  *
  * General functions begin
  *
  *******************************************************************************
  */

void initializeLeftMotor()
{
    Motor_Init(&leftMotorHandle);
    leftMotorEncoderA_PinLastState = HAL_GPIO_ReadPin(LeftMotorEncoderA_GPIO_Port, LeftMotorEncoderA_Pin);
}

void initializeRightMotor()
{
    Motor_Init(&rightMotorHandle);
    rightMotorEncoderA_PinLastState = HAL_GPIO_ReadPin(RightMotorEncoderA_GPIO_Port, RightMotorEncoderA_Pin);
}

void updateLeftMotorParameters()
{
    GPIO_PinState state = HAL_GPIO_ReadPin(LeftMotorEncoderA_GPIO_Port, LeftMotorEncoderA_Pin);
    if ((leftMotorEncoderA_PinLastState == GPIO_PIN_RESET) && state == GPIO_PIN_SET)
    {
        GPIO_PinState val = HAL_GPIO_ReadPin(LeftMotorEncoderB_GPIO_Port, LeftMotorEncoderB_Pin);
        if (val == GPIO_PIN_RESET && isForward(&leftMotorHandle))
        {
            setBackward(&leftMotorHandle);
        }
        else if (val == GPIO_PIN_SET && !isForward(&leftMotorHandle))
        {
            setForward(&leftMotorHandle);
        }
    }
    leftMotorEncoderA_PinLastState = state;

    int32_t pulses = getPulses(&leftMotorHandle);
    if (isForward(&leftMotorHandle))
    {
        setPulses(&leftMotorHandle, pulses + 1);
    }
    else
    {
        setPulses(&leftMotorHandle, pulses - 1);
    }
}

void updateRightMotorParameters()
{
    GPIO_PinState state = HAL_GPIO_ReadPin(RightMotorEncoderA_GPIO_Port, RightMotorEncoderA_Pin);
    if ((rightMotorEncoderA_PinLastState == GPIO_PIN_RESET) && state == GPIO_PIN_SET)
    {
        GPIO_PinState val = HAL_GPIO_ReadPin(RightMotorEncoderB_GPIO_Port, RightMotorEncoderB_Pin);
        if (val == GPIO_PIN_SET && isForward(&rightMotorHandle))
        {
            setBackward(&rightMotorHandle);
        }
        else if (val == GPIO_PIN_RESET && !isForward(&rightMotorHandle))
        {
            setForward(&rightMotorHandle);
        }
    }
    rightMotorEncoderA_PinLastState = state;

    int32_t pulses = getPulses(&rightMotorHandle);
    if (isForward(&rightMotorHandle))
    {
        setPulses(&rightMotorHandle, pulses + 1);
    }
    else
    {
        setPulses(&rightMotorHandle, pulses - 1);
    }
}
