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

#include "GpioDriverSysfs.h"
#include "GpioManager.h"
#include "PeripheralManager.h"


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
  if (!GpioManager::GetGpioManager()->RegisterDriver(
          std::unique_ptr<GpioDriverInfoBase>(
              new GpioDriverInfo<GpioDriverSysfs, void*>(nullptr)))) {
    AppLogError() << "Failed to load driver: GpioDriverSysfs";
    return false;
  }
  return true;
}


bool PeripheralManager::InitHal() {
#if 0
  const hw_module_t* module = nullptr;
  if (hw_get_module(PERIPHERAL_IO_HARDWARE_MODULE_ID, &module) != 0) {
    LOG(ERROR) << "Failed to load HAL" << module;
    return true;
  }

  const peripheral_io_module_t* peripheral_module =
      reinterpret_cast<const peripheral_io_module_t*>(module);

  struct peripheral_registration_cb_t callbacks = {
      // Gpio
      .register_gpio_sysfs = RegisterGpioSysfs,
      .set_gpio_pin_mux = SetGpioPinMux,

      // Spi
      .register_spi_dev_bus = RegisterSpiDevBus,
      .set_spi_pin_mux = SetSpiPinMux,

      // Led
      .register_led_sysfs = RegisterLedSysfs,

      // Uart
      .register_uart_bus = RegisterUartBus,
      .set_uart_pin_mux = SetUartPinMux,

      // I2c
      .register_i2c_dev_bus = RegisterI2cDevBus,
      .set_i2c_pin_mux = SetI2cPinMux,

      // Pin Mux
      .register_pin = RegisterPin,
      .register_pin_group = RegisterPinGroup,
      .register_source = RegisterSource,
      .register_simple_source = RegisterSimpleSource,
  };

  peripheral_module->register_devices(peripheral_module, &callbacks);
#endif
  return true;
}
