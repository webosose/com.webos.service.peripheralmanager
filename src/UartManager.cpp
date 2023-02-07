// Copyright (c) 2021-2023 LG Electronics, Inc.
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

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <algorithm>
#include <regex>

#include "UartManager.h"
#include "UartDriverSysfs.h"

std::unique_ptr<UartManager> g_uart_manager;

UartManager* UartManager::GetManager() {
    if (!g_uart_manager) {
        g_uart_manager.reset(new UartManager);
    }
    return g_uart_manager.get();
}

void UartManager::ResetManager() {
    g_uart_manager.reset();
}

UartManager::UartManager() {}

UartManager::~UartManager() {}

bool UartManager::RegisterDriver(
        std::unique_ptr<UartDriverInfoBase> driver_info) {
    std::string key = driver_info->Compat();
    driver_infos_[key] = std::move(driver_info);
    return true;
}

bool UartManager::RegisterUartDevice(const std::string& name,
        const std::string& uart_device_name) {
    if (uart_devices_.count(name)) {
        AppLogError() << "Uart device " << name << " is already registered";
        return false;
    }
    uart_devices_[name].name = name;
    uart_devices_[name].path = uart_device_name;
    return true;
}

bool UartManager::SetPinMux(const std::string& name,
        const std::string& pin_mux) {
    auto bus_it = uart_devices_.find(name);
    if (bus_it == uart_devices_.end()) {
        AppLogError() << "Uart device " << name << " is not registered";
        return false;
    }

    bus_it->second.mux = pin_mux;
    return true;
}

std::vector<std::string> UartManager::GetDevicesList() {

    std::vector<std::string> list;
    FILE *fp;
    char result[1024];

    std::string command = "ls /dev/ttyACM* /dev/ttyUSB*";
    fp = popen(command.c_str(), "r");

    if (fp == NULL) {
        AppLogError() << "failed to open port";
        return list;
    }

    memset(result, 0, sizeof(result));

    while (fgets(result, sizeof(result), fp) != NULL) {
        std::string regex_result;
        try {
            std::regex newlines_re("\n+");
            regex_result = std::regex_replace(result, newlines_re, "");
        } catch (const std::regex_error &e) {
            AppLogError() << "regex_error caught:" << e.what();
            pclose(fp);
            return list;
        }

        auto device = strstr(const_cast<char*>(regex_result.c_str()), "ttyACM");
        if (device != NULL) {
            RegisterUartDevice(device, regex_result.c_str());
            SetPinMux(device, device);
        }

        device = strstr(const_cast<char*>(regex_result.c_str()), "ttyUSB");
        if (device != NULL) {
            RegisterUartDevice(device, regex_result.c_str());
            SetPinMux(device, device);
        }

        memset(result, 0, sizeof(result));
    }
    pclose(fp);

    for (const auto& it : uart_devices_) {
        list.push_back(it.first);
    }
    return list;
}

bool UartManager::HasUartDevice(const std::string& name) {
    return uart_devices_.count(name);
}

std::unique_ptr<UartDevice> UartManager::OpenUartDevice(
        const std::string& name,
        bool canonical) {
    // Get the Bus from the BSP.
    auto bus_it = uart_devices_.find(name);
    if (bus_it == uart_devices_.end()) {
        return nullptr;
    }

    // Check its not already in use
    if (bus_it->second.driver_) {
        return nullptr;
    }

    // Find a driver.
    // Currently there is only hardcoded support for UARTSYSFS
    auto driver_info_it = driver_infos_.find(UartDriverSysfs::Compat());

    // Fail if there is no driver.
    if (driver_info_it == driver_infos_.end()) {
        return nullptr;
    }

    std::unique_ptr<UartDriverInterface> driver(driver_info_it->second->Probe());
    if (!driver->Init(bus_it->second.path, canonical)) {
        AppLogError() << __func__ << ":" << __LINE__ ;
        return nullptr;
    }

  // Set Pin muxing.
    if (!bus_it->second.mux.empty()) {
        PinMuxManager::GetPinMuxManager()->SetSource(bus_it->second.mux,
                bus_it->second.mux);
    }

    bus_it->second.driver_ = std::move(driver);
    return std::unique_ptr<UartDevice>(new UartDevice(&(bus_it->second)));
}
