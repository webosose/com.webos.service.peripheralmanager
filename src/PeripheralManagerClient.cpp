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


#include <iostream>
#include <map>
#include <iostream>
#include <vector>
#include <memory>
#include <cstdio>
#include <fstream>
#include <cassert>
#include <functional>
#include <string>

#include <bits/stdc++.h>

#include "PeripheralManagerClient.h"
#include "PeripheralManagerException.h"
#define ROW 4
#define COL 5

PeripheralManagerClient::PeripheralManagerClient() {}
PeripheralManagerClient::~PeripheralManagerClient() {}

Status PeripheralManagerClient::ListGpio(std::vector<std::string>* gpios) {
    *gpios = GpioManager::GetGpioManager()->GetGpioPins();
}



void test::add(void) {
    mp.emplace(3, 40);
}

bool PeripheralManagerClient::OpenGpio(const std::string& name) {
    if (!GpioManager::GetGpioManager()->HasGpio(name)) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kENODEV);

        return false;
    }
        auto gpio = GpioManager::GetGpioManager()->OpenGpioPin(name);
    if (!gpio) {
            AppLogError() << "Failed to open GPIO " << name;
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEBUSY);
        return false;
    }

    gpios_.emplace(name, std::move(gpio));

    return true;
}


bool  PeripheralManagerClient::ReleaseGpio(const std::string& name) {
    gpios_.erase(name);
    return true;
}

bool PeripheralManagerClient::SetGpioDirection(const std::string& name,
        int direction) {
    if (!gpios_.count(name)) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
        return false;
    }

    if (gpios_.find(name)->second->SetDirection(GpioDirection(direction))) {
        return true;
    }
    throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
    return false;
}

bool PeripheralManagerClient::SetGpioValue(const std::string& name,
        bool value) {
    if (!gpios_.count(name)) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    if (gpios_.find(name)->second->SetValue(value)) {
        return true;
    }
    throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
    return false;
}

bool PeripheralManagerClient::GetGpioValue(const std::string& name,
        bool* value) {
    if (!gpios_.count(name)) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    if (gpios_.find(name)->second->GetValue(value)) {
        return true;
    }
    throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
    return false;
}
Status PeripheralManagerClient::GetGpioPollingFd(
        const std::string& name,
        void* fd) {
    if (!gpios_.count(name)){
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    if (gpios_.find(name)->second->GetPollingFd(fd)) {
    }

}
Status PeripheralManagerClient::ListUartDevices(
    std::vector<std::string>* devices) {
  *devices = UartManager::GetManager()->GetDevicesList();
}

Status PeripheralManagerClient::OpenUartDevice(const std::string& name) {
    AppLogError() << __func__ << ":" << __LINE__ ;
  if (!UartManager::GetManager()->HasUartDevice(name)) {
      AppLogError() << __func__ << ":" << __LINE__ ;
      throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kENODEV);
  }

  auto uart_device = UartManager::GetManager()->OpenUartDevice(name);
  if (!uart_device) {
      AppLogError() << __func__ << ":" << __LINE__ ;
      AppLogError() << "Failed to open UART device " << name;
      throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEBUSY);
  }
  AppLogError() << __func__ << ":" << __LINE__ ;
  uart_devices_.emplace(name, std::move(uart_device));
}
bool PeripheralManagerClient::ReleaseUartDevice(const std::string& name) {
  return uart_devices_.erase(name) ? true
                                   : throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
}

bool PeripheralManagerClient::SetUartDeviceBaudrate(const std::string& name,
                                                      int32_t baudrate) {
    AppLogError() << __func__ << ":" << __LINE__ ;
  auto uart_device = uart_devices_.find(name);
  if (uart_device == uart_devices_.end()) {
      AppLogError() << __func__ << ":" << __LINE__ ;
      throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
  }

  if (baudrate < 0) {
      AppLogError() << __func__ << ":" << __LINE__ ;
      throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEINVAL);
  }

  int ret = uart_device->second->SetBaudrate(static_cast<uint32_t>(baudrate));

  if (ret) {
      AppLogError() << __func__ << ":" << __LINE__ ;
      return false;
  }
  AppLogError() << __func__ << ":" << __LINE__ ;
  return true;
}

bool PeripheralManagerClient::UartDeviceWrite(
    const std::string& name,
    const std::vector<uint8_t>& data,
    int32_t* bytes_written) {
  auto uart_device = uart_devices_.find(name);
  if (uart_device == uart_devices_.end()) {
      throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
  }

  int ret = uart_device->second->Write(
      data, reinterpret_cast<uint32_t*>(bytes_written));

  if (ret) {
      return false;
  }

  return true;
}

bool PeripheralManagerClient::UartDeviceRead(const std::string& name,
                                               std::vector<uint8_t>* data,
                                               int size,
                                               int* bytes_read) {
  auto uart_device = uart_devices_.find(name);
  if (uart_device == uart_devices_.end()) {
      throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
  }

  int ret = uart_device->second->Read(
      data, size, reinterpret_cast<uint32_t*>(bytes_read));

  if (ret) {
      return false;
  }

  return true;
}
