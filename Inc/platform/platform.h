/**
  * Copyright (c) 2021 M. Kajdak. All rights reserved.
  *
  ******************************************************************************
  * @file           : platform.h
  * @brief          : Header for <source file name> file.
  *                   This file contains <what does the file contains>
  ******************************************************************************
  */
#ifndef MOTORDRIVER_PLATFORM_H
#define MOTORDRIVER_PLATFORM_H

typedef struct
{
    double error;
    PID pid;
} PlatformContext;

void initPlatform(PlatformContext* platformContext);
void evalPlatform(PlatformContext* platformContext);


#endif //MOTORDRIVER_PLATFORM_H
