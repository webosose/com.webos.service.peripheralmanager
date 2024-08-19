// Copyright (c) 2024 LG Electronics, Inc.
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
#include "PeripheralManager.h"
#include "GpioDriverSysfs.h"
#include "GpioManager.h"
#include "I2cDriverI2cdev.h"
#include "I2cManager.h"

#include "PinmuxManager.h"
#include "SpiDriverSpidev.h"
#include "UartDriverSysfs.h"
#include "UartManager.h"
#include "peripheral_io.h"


// Gpio callbacks
static int RegisterGpioSysfs(const char* name, uint32_t index) {
    return GpioManager::GetGpioManager()->RegisterGpioSysfs(name, index);
}

static int SetGpioPinMux(const char* name, const char* source) {
    return GpioManager::GetGpioManager()->SetPinMux(name, source);
}

// Spi callbacks
static int RegisterSpiDevBus(const char* name, uint32_t bus, uint32_t cs) {
    return SpiManager::GetSpiManager()->RegisterSpiDevBus(name, bus, cs);
}

static int SetSpiPinMux(const char* name, const char* source) {
    return SpiManager::GetSpiManager()->SetPinMux(name, source);
}
// Uart callbacks
static int RegisterUartBus(const char* name, const char* dev_name) {
    return UartManager::GetManager()->RegisterUartDevice(name, dev_name);
}

static int SetUartPinMux(const char* name, const char* source) {
    return UartManager::GetManager()->SetPinMux(name, source);
}

// I2c callbacks
static int RegisterI2cDevBus(const char* name, uint32_t bus) {
    return I2cManager::GetI2cManager()->RegisterI2cDevBus(name, bus);
}

static int SetI2cPinMux(const char* name, const char* source) {
    return I2cManager::GetI2cManager()->SetPinMux(name, source);
}

// Pin Mux callbacks.
static int RegisterPin(const char* name,
        int gpio,
        pin_mux_callbacks callbacks) {
    return PinMuxManager::GetPinMuxManager()->RegisterPin(name, gpio, callbacks);
}

static int RegisterPinGroup(const char* name, char** pins, size_t nr_pins) {
    std::set<std::string> pins_set;
    for (size_t i = 0; i < nr_pins; i++)
        pins_set.emplace(pins[i]);
    return PinMuxManager::GetPinMuxManager()->RegisterPinGroup(name, std::move(pins_set));
}

static int RegisterSource(const char* name, char** groups, size_t nr_groups) {
    std::set<std::string> groups_set;
    for (size_t i = 0; i < nr_groups; i++)
        groups_set.emplace(groups[i]);
    return PinMuxManager::GetPinMuxManager()->RegisterSource(name, std::move(groups_set));
}

static int RegisterSimpleSource(const char* name,
        const char** pins,
        size_t nr_pins) {
    std::set<std::string> pins_set;
    for (size_t i = 0; i < nr_pins; i++)
        pins_set.emplace(pins[i]);
    return PinMuxManager::GetPinMuxManager()->RegisterSimpleSource(name,
            std::move(pins_set));
}
PeripheralManager::PeripheralManager() {}

PeripheralManager::~PeripheralManager() = default;

bool PeripheralManager::Init() {
    if (!RegisterDrivers()) {
        return false;
    }

    if (!InitHal()) {
        return false;
    }

    return true;
}

bool PeripheralManager::RegisterDrivers() {
    if (!SpiManager::GetSpiManager()->RegisterDriver(
            std::unique_ptr<SpiDriverInfoBase>(
                    new SpiDriverInfo<SpiDriverSpiDev, CharDeviceFactory*>(
                            nullptr)))) {
        AppLogError() << "Failed to load driver: SpiDriverSpiDev";
        return false;
    }
    if (!I2cManager::GetI2cManager()->RegisterDriver(
            std::unique_ptr<I2cDriverInfoBase>(
                    new I2cDriverInfo<I2cDriverI2cDev, CharDeviceFactory*>(
                            nullptr)))) {
        AppLogError() << "Failed to load driver: I2cDriverI2cDev";
        return false;
    }
    if (!GpioManager::GetGpioManager()->RegisterDriver(
            std::unique_ptr<GpioDriverInfoBase>(
                    new GpioDriverInfo<GpioDriverSysfs, void*>(nullptr)))) {
        AppLogError() << "Failed to load driver: GpioDriverSysfs";
        return false;
    }
    if (!UartManager::GetManager()->RegisterDriver(
            std::unique_ptr<UartDriverInfoBase>(
                    new UartDriverInfo<UartDriverSysfs, CharDeviceFactory*>(
                            nullptr)))) {
        AppLogError() << "Failed to load driver: UartDriverSysfs";
        return false;
    }
    return true;
}


bool PeripheralManager::InitHal() {
    peripheral_io_module_t peripheral_module;
    peripheral_module.register_devices = &register_devices;

    struct peripheral_registration_cb_t callbacks = {
            // Pin Mux
            .register_pin = RegisterPin,
            .register_pin_group = RegisterPinGroup,
            .register_source = RegisterSource,
            .register_simple_source = RegisterSimpleSource,

            // Gpio
            .register_gpio_sysfs = RegisterGpioSysfs,
            .set_gpio_pin_mux = SetGpioPinMux,

            // Spi
            .register_spi_dev_bus = RegisterSpiDevBus,
            .set_spi_pin_mux = SetSpiPinMux,

            // Uart
            .register_uart_bus = RegisterUartBus,
            .set_uart_pin_mux = SetUartPinMux,

            // I2c
            .register_i2c_dev_bus = RegisterI2cDevBus,
            .set_i2c_pin_mux = SetI2cPinMux,

    };

    peripheral_module.register_devices(&peripheral_module, &callbacks);
    return true;
}
