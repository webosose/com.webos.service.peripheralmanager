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

#include "GpioManager.h"
#include "PinmuxManager.h"

std::unique_ptr<GpioManager> g_gpio_manager;

GpioManager::GpioManager() {}

GpioManager::~GpioManager() {}

// static
GpioManager* GpioManager::GetGpioManager() {
    if (!g_gpio_manager) {
        g_gpio_manager.reset(new GpioManager());
    }
    return g_gpio_manager.get();
}

void GpioManager::ResetGpioManager() {
    g_gpio_manager.reset();
}

bool GpioManager::RegisterGpioSysfs(const std::string& name, uint32_t index) {
    if (sysfs_pins_.count(name))
        return false;
    sysfs_pins_[name].index = index;
    return true;
}

bool GpioManager::SetPinMux(const std::string& name, const std::string& mux) {
    if (!sysfs_pins_.count(name))
        return false;
    sysfs_pins_[name].mux = mux;
    return true;
}

std::vector<std::string> GpioManager::GetGpioPins() {
    std::vector<std::string> pins;
    for (auto& i : sysfs_pins_)
        pins.push_back(i.first);
    return pins;
}

bool GpioManager::HasGpio(const std::string& pin_name) {
    return sysfs_pins_.count(pin_name);
}

bool GpioManager::RegisterDriver(
        std::unique_ptr<GpioDriverInfoBase> driver_info) {
    std::string key = driver_info->Compat();
    AppLogError() << "key = " << key ;
    driver_infos_[key] = std::move(driver_info);
    return true;
}

std::unique_ptr<GpioPin> GpioManager::OpenGpioPin(const std::string& name) {
    // Get the Pin  from the BSP.
    auto pin_it = sysfs_pins_.find(name);
    if (pin_it == sysfs_pins_.end()) {
        AppLogError() << "GpioManager: Pin not found. " << name;
        return nullptr;
    }

    // Check its not alread in use
    if (pin_it->second.driver_) {
        AppLogError() << "GpioManager: Pin in use. " << name;
        return nullptr;
    }

    // Find a driver.
    // Currently there is only hardcoded support for GPIOSYSFS
    auto driver_info_it = driver_infos_.find("GPIOSYSFS");

    // Fail if there is no driver.
    if (driver_info_it == driver_infos_.end()) {
        AppLogError() << "GpioManager: Failed to find driver " << name;;
        return nullptr;
    }

    std::unique_ptr<GpioDriverInterface> driver(driver_info_it->second->Probe());

    // Set pin mux
    if (!pin_it->second.mux.empty()) {
        PinMuxManager::GetPinMuxManager()->SetGpio(pin_it->second.mux);
    }
    AppLogError() << "pin_it->second.index = " << pin_it->second.index;
    if (!driver->Init(pin_it->second.index)) {
        AppLogError() << "GpioManager: Failed to init driver " << name; ;
        return nullptr;
    }
    pin_it->second.driver_ = std::move(driver);
    return std::unique_ptr<GpioPin>(new GpioPin(&(pin_it->second)));
}
