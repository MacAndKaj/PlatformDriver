/**
  * Copyright (c) 2020 M. Kajdak. All rights reserved.
  *
  ******************************************************************************
  * @file           : pid.h
  * @brief          : Header for pid.c file.
  *                   This file contains PID structure definition and
  *                   functions declarations.
  ******************************************************************************
  */
#ifndef MOTORDRIVER_PLATFORM_PID_H
#define MOTORDRIVER_PLATFORM_PID_H

typedef struct pid
{
    const double kP;      // P factor(proportional)
    const double kI;      // I factor(integral)
    const double kD;      // D factor(derivative)

    double lastError;    // Last value of error used to compute new value of derivative.
    double vI;           // Variable to keep integral of error
} PID;

///Evaluate PID TODO: describe
/// \param pidHandle
/// \param error
/// \return
double evaluate(PID* pidHandle, double error, double dt);

double getKP(const PID* pidHandle);
double getKI(const PID* pidHandle);
double getKD(const PID* pidHandle);
double getLastError(const PID* pidHandle);
void setLastError(PID* pidHandle, double val);
double getIntegral(const PID* pidHandle);
void setIntegral(PID* pidHandle, double val);
void resetPid(PID* pidHandle);

#endif //MOTORDRIVER_PLATFORM_PID_H
