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

#include <stdint.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "UartDriver.h"
#include "Logger.h"
#include "PinmuxManager.h"

struct UartSysfs {
    std::string name;
    std::string path;
    std::string mux;
    std::unique_ptr<UartDriverInterface> driver_;
};

class UartDevice {
public:
    explicit UartDevice(UartSysfs* uart_device) : uart_device_(uart_device) {}
    ~UartDevice() {
        if (!uart_device_->mux.empty()) {
            PinMuxManager::GetPinMuxManager()->ReleaseSource(uart_device_->mux,uart_device_->mux);
        }
        uart_device_->driver_.reset();
    }

    int SetBaudrate(uint32_t baudrate) {
        return uart_device_->driver_->SetBaudrate(baudrate);
    }
    uint32_t getBaudrate(uint32_t* baudrate){
        return uart_device_->driver_->getBaudrate(baudrate);
    }

    int Write(const std::vector<uint8_t>& data, uint32_t* bytes_written) {
        return uart_device_->driver_->Write(data, bytes_written);
    }

    int Read(std::vector<uint8_t>* data, uint32_t size, uint32_t* bytes_read) {
        return uart_device_->driver_->Read(data, size, bytes_read);
    }
    bool GetuPollingFd(int* fd) {
        return uart_device_->driver_->GetuPollingFd(fd);
    }

private:
    UartSysfs* uart_device_;
};

class UartManager {
public:
    ~UartManager();

    // Get the singleton.
    static UartManager* GetManager();
    static void ResetManager();

    // Used by the BSP to tell PMan of an sysfs uart_device.
    bool RegisterUartDevice(const std::string& name, const std::string& path);
    bool SetPinMux(const std::string& name, const std::string& mux);

    std::vector<std::string> GetDevicesList();
    bool HasUartDevice(const std::string& name);

    bool RegisterDriver(std::unique_ptr<UartDriverInfoBase> driver_info);

    std::unique_ptr<UartDevice> OpenUartDevice(const std::string& name, bool canonical = false);

private:
    UartManager();

    std::map<std::string, std::unique_ptr<UartDriverInfoBase>> driver_infos_;
    std::map<std::string, UartSysfs> uart_devices_;

};
