// Copyright (c) 2021 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <config.h>
#include <glib.h>
#include <luna-service2++/handle.hpp>
#include <memory>
#include <unordered_map>
#include <list>
#include "Logger.h"
#include "PeripheralManagerClient.h"


class PeripheralManagerService {
public:
    PeripheralManagerService(LS::Handle *ls_handle);
    virtual ~PeripheralManagerService();

    // run function should block
    void run();
    void registerMethodsToLsHub();

    // some sort of stop should be there, so that it can be stopped
    // may be this should be static
    void stop();

    // Actual CallBack Methods
    bool ListGpio(LSMessage &ls_message);
    bool OpenGpio(LSMessage &ls_message);
    bool ReleaseGpio(LSMessage &ls_message);
    bool SetGpioDirection(LSMessage &ls_message);
    bool SetGpioValue(LSMessage &ls_message);
    bool GetGpioValue(LSMessage &ls_message);
    bool GetGpioPollingFd(LSMessage &ls_message);
    bool ListUartDevices(LSMessage &ls_message);
    bool OpenUartDevice(LSMessage &ls_message);
    bool ReleaseUartDevice(LSMessage &ls_message);
    bool SetUartDeviceBaudrate(LSMessage &ls_message);
    bool UartDeviceWrite(LSMessage &ls_message);
    bool UartDeviceRead(LSMessage &ls_message);
    bool getBaudrate(LSMessage &ls_message);
    bool getDirection(LSMessage &ls_message);
    bool GetuartPollingFd(LSMessage &ls_message);
    bool ListI2cBuses(LSMessage &ls_message);
    bool OpenI2cDevice(LSMessage &ls_message);
    bool ReleaseI2cDevice(LSMessage &ls_message);
    bool I2cRead(LSMessage &ls_message);
    bool I2cReadRegByte(LSMessage &ls_message);
    bool I2cReadRegWord(LSMessage &ls_message);

    void subscribeLoraReceive();
    static bool receiveCallback(LSHandle *sh, LSMessage *reply, void *ctx);
private:
    using MainLoopT = std::unique_ptr<GMainLoop, void (*)(GMainLoop *)>;
    MainLoopT main_loop_ptr;
    std::list<LS::Call> callObjects;
    PeripheralManagerClient *peripheral_manager_client;
    LS::Handle *luna_handle;
    std::list<LS::Message> getTimeClients;
};
