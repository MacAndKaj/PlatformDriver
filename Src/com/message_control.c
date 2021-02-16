/**
  * Copyright (c) 2021 M. Kajdak. All rights reserved.
  *
  ******************************************************************************
  * @file           : message_control.c
  * @brief          : Header for <source file name> file.
  *                   This file contains <what does the file contains>
  ******************************************************************************
  */

#include <com/message_control.h>

#include <com/interface/consts.h>
#include <com/interface/messages_size_map.h>
#include <com/interface/ser_des/deserializers/PlatformSetMotorSpeed.h>
#include <com/interface/ser_des/serializers/PlatformSetMotorSpeed.h>

#include <stddef.h>

int validateCtrlData(MessageControl* messageControl, const uint8_t data[HEADER_SIZE])
{
    if (data[0] != FRAME_CTRL_DATA) return 1;

    return 0;
}

uint16_t getMessageSize(uint8_t id)
{
    Node* idAndSize = find(id);
    if (idAndSize != NULL)
    {
        return idAndSize->size;
    }
    return 0;
}

char* serialize(void* resp, uint8_t id)
{
    switch (id)
    {
        case PLATFORM_SET_MOTOR_SPEED_RESP_ID:
            return serialize_PlatformSetMotorSpeedResp((PlatformSetMotorSpeedResp*) resp);
    }
    return NULL;
}

void* deserialize(char* data, uint8_t id)
{
    switch (id)
    {
        case PLATFORM_SET_MOTOR_SPEED_REQ_ID:
            return deserialize_PlatformSetMotorSpeedReq(data);
    }
    return NULL;
}

void processSubscriptions(MessageControl* messageControl)
{
    while (!isEmpty(messageControl->storage))
    {
        Message message = popMessage(messageControl->storage);
        if (subscriptionExists(messageControl->subscriptionContainer, message.messageId))
        {
            //TODO: handle subscription
        }
    }

}
