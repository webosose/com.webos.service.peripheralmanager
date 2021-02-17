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

#include <map>
#include <memory>
#include <string>
#include "Logger.h"
#include <list>
#include <vector>
#include <bits/stdc++.h>
#include "GpioManager.h"
#include "I2cManager.h"
#include "SpiManager.h"
#include "UartManager.h"

using namespace std;

typedef void Status;

struct DevicesPinInfo {
    std::string name;
    std::string status;
};
class PeripheralManagerClient {
public:
    PeripheralManagerClient();
    ~PeripheralManagerClient();

    // Gpio functions.
    Status ListGpio(std::vector<DevicesPinInfo>& gpios) ;

    bool OpenGpio(const std::string& name) ;

    bool  ReleaseGpio(const std::string& name) ;

    bool  SetGpioDirection(const std::string& name,
            int direction) ;

    bool  SetGpioValue(const std::string& name, bool value) ;

    bool  GetGpioValue(const std::string& name, bool* value) ;
    int  GetGpioPollingFd(const std::string& name,
            int* fd) ;
    bool  getDirection(const std::string& name,
            std::string& direction) ;
    // Spi functions.
    Status ListSpiBuses(std::vector<std::string>* buses) ;

    Status OpenSpiDevice(const std::string& name) ;

    Status ReleaseSpiDevice(const std::string& name) ;

    Status SpiDeviceWriteByte(const std::string& name,
            int8_t byte) ;

    Status SpiDeviceWriteBuffer(
            const std::string& name,
            const std::vector<uint8_t>& buffer) ;

    Status SpiDeviceTransfer(
            const std::string& name,
            std::vector<uint8_t>& tx_data,
            std::vector<uint8_t>* rx_data,
            int size);

    Status SpiDeviceSetMode(const std::string& name, int mode) ;

    Status SpiDeviceSetFrequency(const std::string& name,
            int frequency_hz) ;

    Status SpiDeviceSetBitJustification(const std::string& name,
            bool lsb_first) ;

    Status SpiDeviceSetBitsPerWord(const std::string& name,
            int nbits) ;

    Status SpiDeviceSetDelay(const std::string& name,
            int delay_usecs) ;
    // I2c functions.
    Status ListI2cBuses(std::vector<std::string>* buses) ;

    Status OpenI2cDevice(const std::string& name,
            int32_t address) ;

    Status ReleaseI2cDevice(const std::string& name,
            int32_t address) ;

    Status I2cRead(const std::string& name,
            int32_t address,
            std::vector<uint8_t>* data,
            int32_t size,
            int32_t* bytes_read) ;

    Status I2cReadRegByte(const std::string& name,
            int32_t address,
            int32_t reg,
            int32_t* val) ;

    Status I2cReadRegWord(const std::string& name,
            int32_t address,
            int32_t reg,
            int32_t* val) ;

    Status I2cReadRegBuffer(const std::string& name,
            int32_t address,
            int32_t reg,
            std::vector<uint8_t>* data,
            int32_t size,
            int32_t* bytes_read) ;

    Status I2cWrite(const std::string& name,
            int32_t address,
            const std::vector<uint8_t>& data,
            int32_t* bytes_written) ;

    Status I2cWriteRegByte(const std::string& name,
            int32_t address,
            int32_t reg,
            int8_t val) ;

    Status I2cWriteRegWord(const std::string& name,
            int32_t address,
            int32_t reg,
            int32_t val) ;

    Status I2cWriteRegBuffer(const std::string& name,
            int32_t address,
            int32_t reg,
            const std::vector<uint8_t>& data,
            int32_t* bytes_written) ;

    // Uart functions.
    Status ListUartDevices(std::vector<DevicesPinInfo>& devices);

    Status OpenUartDevice(const std::string& name);

    bool ReleaseUartDevice(const std::string& name);

    bool SetUartDeviceBaudrate(const std::string& name,
            int32_t baudrate);

    bool UartDeviceWrite(const std::string& name,
            const std::vector<uint8_t>& data,
            int* bytes_written);

    bool UartDeviceRead(const std::string& name,
            std::vector<uint8_t>* data,
            int size,
            int* bytes_read);
    int32_t getBaudrate(const std::string& name,
            uint32_t* baudrate);
    int  GetuartPollingFd(const std::string& name,
            int* fd) ;

private:
    std::map<std::string, std::unique_ptr<GpioPin>> gpios_;
    std::map<std::pair<std::string, uint32_t>, std::unique_ptr<I2cDevice>>
    i2c_devices_;
    std::map<std::string, std::unique_ptr<SpiDevice>> spi_devices_;
    std::map<std::string, std::unique_ptr<UartDevice>> uart_devices_;
};

class test {
public:
    map<int, int> mp;
    void add(void);
};
