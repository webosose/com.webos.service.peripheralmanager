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

#include "I2cManager.h"
#include "PinmuxManager.h"

std::unique_ptr<I2cManager> g_i2c_manager;

I2cManager::I2cManager() {}

I2cManager::~I2cManager() {}

// static
I2cManager* I2cManager::GetI2cManager() {
    if (!g_i2c_manager) {
        g_i2c_manager.reset(new I2cManager());
    }
    return g_i2c_manager.get();
}

// static
void I2cManager::ResetI2cManager() {
    g_i2c_manager.reset();
}

bool I2cManager::RegisterI2cDevBus(const std::string& name,
        uint32_t bus) {
    if (i2cdev_buses_.count(name))
        return false;
    i2cdev_buses_.emplace(name, I2cDevBus(bus));
    return true;
}

std::vector<std::string> I2cManager::GetI2cDevBuses() {
    std::vector<std::string> buses;
    for (auto& i : i2cdev_buses_)
        buses.push_back(i.first);
    return buses;
}

bool I2cManager::HasI2cDevBus(const std::string& name) {
    return i2cdev_buses_.count(name);
}

bool I2cManager::RegisterDriver(
        std::unique_ptr<I2cDriverInfoBase> driver_info) {
    std::string key = driver_info->Compat();
    driver_infos_[key] = std::move(driver_info);
    return true;
}

bool I2cManager::SetPinMux(const std::string& name, const std::string& mux) {
    auto bus_it = i2cdev_buses_.find(name);
    if (bus_it == i2cdev_buses_.end())
        return false;
    bus_it->second.mux = mux;
    bus_it->second.mux_group = mux;
    return true;
}

bool I2cManager::SetPinMuxWithGroup(const std::string& name,
        const std::string& mux,
        const std::string& group) {
    auto bus_it = i2cdev_buses_.find(name);
    if (bus_it == i2cdev_buses_.end())
        return false;
    bus_it->second.mux = mux;
    bus_it->second.mux_group = group;
    return true;
}

std::unique_ptr<I2cDevice> I2cManager::OpenI2cDevice(const std::string& name, uint32_t address) {
    // Get the Bus from the BSP.
    auto bus_it = i2cdev_buses_.find(name);
    if (bus_it == i2cdev_buses_.end()) {
        return nullptr;
    }
    // Check its not already in use
    if (bus_it->second.driver_.count(address)) {
        return nullptr;
    }
    // Find a driver.
    // Currently there is only hardcoded support for I2CDEV
    auto driver_info_it = driver_infos_.find("I2CDEV");
    // Fail if there is no driver.
    if (driver_info_it == driver_infos_.end()) {
        return nullptr;
    }
    std::unique_ptr<I2cDriverInterface> driver(driver_info_it->second->Probe());
    if (!driver->Init(bus_it->second.bus, address)) {
        return nullptr;
    }

    // Set Pin muxing.
    if (!bus_it->second.mux.empty()) {
        PinMuxManager::GetPinMuxManager()->SetSource(bus_it->second.mux,
                bus_it->second.mux_group);
    }

    bus_it->second.driver_[address] = std::move(driver);
    return std::unique_ptr<I2cDevice>(new I2cDevice(&(bus_it->second), address));
}
