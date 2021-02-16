/**
  * Copyright (c) 2020 M. Kajdak. All rights reserved.
  *
  ******************************************************************************
  * @file           : PlatformSetMotorSpeed.h
  * @brief          : Header for <source file name> file.
  *                   This file contains <what does the file contains>
  ******************************************************************************
  */
#ifndef MOTORDRIVER_COM_INTERFACE_DEFS_PLATFORMSETMOTORSPEED_H
#define MOTORDRIVER_COM_INTERFACE_DEFS_PLATFORMSETMOTORSPEED_H

#include <com/interface/consts.h>

#include <stdint.h>

struct PlatformSetMotorSpeedReq
{
    uint8_t motor;
};
typedef struct PlatformSetMotorSpeedReq PlatformSetMotorSpeedReq;

struct PlatformSetMotorSpeedResp
{
    uint8_t motor;
    int8_t speedI;
    uint8_t speedF;
    enum Status status;
};
typedef struct PlatformSetMotorSpeedResp PlatformSetMotorSpeedResp;

#endif //MOTORDRIVER_COM_INTERFACE_DEFS_PLATFORMSETMOTORSPEED_H
