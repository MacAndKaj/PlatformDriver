/**
  * Copyright (c) 2020 M. Kajdak. All rights reserved.
  *
  ******************************************************************************
  * @file           : log.h
  * @brief          : Header for <source file name> file.
  *                   This file contains <what does the file contains>
  ******************************************************************************
  */
#ifndef MOTORDRIVER_PLATFORM_LOG_H
#define MOTORDRIVER_PLATFORM_LOG_H

/*
 *  Overriden system call _write(int file, char *ptr, int len)
 */
int _write(int file, char *ptr, int len);

#endif //MOTORDRIVER_PLATFORM_LOG_H
