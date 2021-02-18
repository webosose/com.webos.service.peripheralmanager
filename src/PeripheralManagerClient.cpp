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

Status PeripheralManagerClient::ListGpio(std::vector<DevicesPinInfo>& gpioStat) {
    std::vector<std::string> gpios;
    gpios = GpioManager::GetGpioManager()->GetGpioPins();
    DevicesPinInfo gpioPinInfo;
    for(auto name:gpios)
    {
        if(gpios_.count(name))
        {
            gpioPinInfo.status = "used";
        }
        else
        {
            gpioPinInfo.status = "available";
        }
        gpioPinInfo.name = name;
        gpioStat.push_back(gpioPinInfo);
    }
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
    if (!GpioManager::GetGpioManager()->HasGpio(name)) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kENODEV);
        return false;
    }
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
bool PeripheralManagerClient::getDirection(const std::string& name,
        std::string& direction) {
    if (!gpios_.count(name)) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    if (gpios_.find(name)->second->getDirection(direction)) {
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
int PeripheralManagerClient::GetGpioPollingFd(
        const std::string& name,
        int* fd) {
    if (!gpios_.count(name)){
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    else (gpios_.find(name)->second->GetPollingFd(fd));
    {
        return *fd;
    }
    return 0;
}


Status PeripheralManagerClient::ListSpiBuses(std::vector<std::string>* buses) {
    *buses = SpiManager::GetSpiManager()->GetSpiDevBuses();
    return;
}

Status PeripheralManagerClient::OpenSpiDevice(const std::string& name) {
    if (!SpiManager::GetSpiManager()->HasSpiDevBus(name)){
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kENODEV);
    }

    std::unique_ptr<SpiDevice> device =
            SpiManager::GetSpiManager()->OpenSpiDevice(name);

    if (!device) {
        AppLogError()  << "Failed to open device " << name;
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEBUSY);
    }

    spi_devices_.emplace(name, std::move(device));
    return;
}

Status PeripheralManagerClient::ReleaseSpiDevice(const std::string& name) {
    if (!spi_devices_.count(name)) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    spi_devices_.erase(name);
    return;
}

Status PeripheralManagerClient::SpiDeviceWriteByte(const std::string& name,
        int8_t byte) {
    if (!spi_devices_.count(name)){
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    if (spi_devices_.find(name)->second->WriteByte(byte)) {
        return;
    }
    throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
    return;
}

Status PeripheralManagerClient::SpiDeviceWriteBuffer(
        const std::string& name,
        const std::vector<uint8_t>& buffer) {
    if (!spi_devices_.count(name)) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }
    if (spi_devices_.find(name)->second->WriteBuffer(buffer.data(),
            buffer.size())) {
        return;
    }

    throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
    return;
}

Status PeripheralManagerClient::SpiDeviceTransfer(
        const std::string& name,
        std::vector<uint8_t>& tx_data,
        std::vector<uint8_t>* rx_data,
        int size) {
    if (!spi_devices_.count(name)) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    if (!rx_data) {
        if (spi_devices_.find(name)->second->Transfer(tx_data.data(), nullptr,
                size)) {
            return;
        }
    } else {
        if (spi_devices_.find(name)->second->Transfer(tx_data.data(),
                (*rx_data).data(), size)) {
            return;
        }
    }

    throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
}

Status PeripheralManagerClient::SpiDeviceSetMode(const std::string& name,
        int mode) {
    if (!spi_devices_.count(name)) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    if (spi_devices_.find(name)->second->SetMode(SpiMode(mode))) {
        return;
    }

    throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
    return;
}

Status PeripheralManagerClient::SpiDeviceSetFrequency(const std::string& name,
        int frequency_hz) {
    if (!spi_devices_.count(name)) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    if (frequency_hz > 0 &&
            spi_devices_.find(name)->second->SetFrequency(frequency_hz)) {
        return;
    }

    throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
}

Status PeripheralManagerClient::SpiDeviceSetBitJustification(
        const std::string& name,
        bool lsb_first) {
    if (!spi_devices_.count(name)) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    if (spi_devices_.find(name)->second->SetBitJustification(lsb_first)) {
        return;
    }

    throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
    return;
}

Status PeripheralManagerClient::SpiDeviceSetBitsPerWord(const std::string& name,
        int nbits) {
    if (!spi_devices_.count(name)) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    if (spi_devices_.find(name)->second->SetBitsPerWord(nbits)) {
        return;
    }

    throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
    return;
}

Status PeripheralManagerClient::SpiDeviceSetDelay(const std::string& name,
        int delay_usecs) {
    if (!spi_devices_.count(name)) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    // |delay_usecs| must be positive and fit in an unsigned 16 bit.
    if (delay_usecs < 0 || delay_usecs > INT16_MAX) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEINVAL);
    }

    if (spi_devices_.find(name)->second->SetDelay(
            static_cast<uint16_t>(delay_usecs))) {
        return;
    }

    throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
    return;
}

Status PeripheralManagerClient::ListI2cBuses(std::vector<std::string>* buses) {
    *buses = I2cManager::GetI2cManager()->GetI2cDevBuses();
    return;
}

Status PeripheralManagerClient::OpenI2cDevice(const std::string& name,
        int32_t address) {
    if (!I2cManager::GetI2cManager()->HasI2cDevBus(name)) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kENODEV);
    }

    std::unique_ptr<I2cDevice> device =
            I2cManager::GetI2cManager()->OpenI2cDevice(name, address);
    if (!device) {
        AppLogError()  << "Failed to open device " << name;
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEBUSY);
    }
    std::pair<std::string, uint32_t> i2c_dev(name, address);
    i2c_devices_.emplace(i2c_dev, std::move(device));
    return;
}

Status PeripheralManagerClient::ReleaseI2cDevice(const std::string& name,
        int32_t address) {
    if (!i2c_devices_.count({name, address})) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    i2c_devices_.erase({name, address});
    return;
}

Status PeripheralManagerClient::I2cRead(const std::string& name,
        int32_t address,
        std::vector<uint8_t>* data,
        int32_t size,
        int32_t* bytes_read) {
    if (!i2c_devices_.count({name, address})) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    if (size < 0) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEINVAL);
    }
    data->resize(size);

    uint32_t* nread = reinterpret_cast<uint32_t*>(bytes_read);
    int32_t ret = i2c_devices_.find({name, address})
                                            ->second->Read(data->data(), data->size(), nread);
    data->resize(*nread);

    if (ret == 0) {
        return;
    }
    else {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
    }
}

Status PeripheralManagerClient::I2cReadRegByte(const std::string& name,
        int32_t address,
        int32_t reg,
        int32_t* val) {
    if (!i2c_devices_.count({name, address})) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    uint8_t tmp_val = 0;
    if (i2c_devices_.find({name, address})->second->ReadRegByte(reg, &tmp_val) ==
            0) {
        *val = tmp_val;
        return;
    }

    throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
}

Status PeripheralManagerClient::I2cReadRegWord(const std::string& name,
        int32_t address,
        int32_t reg,
        int32_t* val) {
    if (!i2c_devices_.count({name, address})) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    uint16_t tmp_val = 0;
    if (i2c_devices_.find({name, address})->second->ReadRegWord(reg, &tmp_val) == 0) {
        *val = tmp_val;
        return;
    }

    throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
}

Status PeripheralManagerClient::I2cReadRegBuffer(const std::string& name,
        int32_t address,
        int32_t reg,
        std::vector<uint8_t>* data,
        int32_t size,
        int32_t* bytes_read) {
    if (!i2c_devices_.count({name, address})) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    if (size < 0) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEINVAL);
    }
    data->resize(size);

    uint32_t* nread = reinterpret_cast<uint32_t*>(bytes_read);
    int32_t ret =
            i2c_devices_.find({name, address})
            ->second->ReadRegBuffer(reg, data->data(), data->size(), nread);

    data->resize(*nread);
    if (ret) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
    }
}

Status PeripheralManagerClient::I2cWrite(const std::string& name,
        int32_t address,
        const std::vector<uint8_t>& data,
        int32_t* bytes_written) {
    if (!i2c_devices_.count({name, address})) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    int ret = i2c_devices_.find({name, address})
                                        ->second->Write(data.data(),
                                                data.size(),
                                                reinterpret_cast<uint32_t*>(bytes_written));

    if (ret) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
    }
}

Status PeripheralManagerClient::I2cWriteRegByte(const std::string& name,
        int32_t address,
        int32_t reg,
        int8_t val) {
    if (!i2c_devices_.count({name, address})) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    if (i2c_devices_.find({name, address})->second->WriteRegByte(reg, val) == 0) {
        return;
    }
    throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
}

Status PeripheralManagerClient::I2cWriteRegWord(const std::string& name,
        int32_t address,
        int32_t reg,
        int32_t val) {
    if (!i2c_devices_.count({name, address})) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }
    if (i2c_devices_.find({name, address})->second->WriteRegWord(reg, val) == 0) {
        return;
    }

    throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
}

Status PeripheralManagerClient::I2cWriteRegBuffer(
        const std::string& name,
        int32_t address,
        int32_t reg,
        const std::vector<uint8_t>& data,
        int32_t* bytes_written) {
    if (!i2c_devices_.count({name, address})) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    int32_t ret =
            i2c_devices_.find({name, address})
            ->second->WriteRegBuffer(reg,
                    data.data(),
                    data.size(),
                    reinterpret_cast<uint32_t*>(bytes_written));

    if (ret) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
    }
}

Status PeripheralManagerClient::ListUartDevices(
        std::vector<DevicesPinInfo>& uartStat) {
    std::vector<std::string> devices;
    devices = UartManager::GetManager()->GetDevicesList();
    DevicesPinInfo uartPinInfo;
    for(auto name:devices)
    {
        if(uart_devices_.count(name))
            uartPinInfo.status = "used";
        else
            uartPinInfo.status = "available";

        uartPinInfo.name = name;
        uartStat.push_back(uartPinInfo);
        AppLogError() << " GPIO Pins Used" << uartPinInfo.name << ":" << uartPinInfo.status;
    }
    return;
}

Status PeripheralManagerClient::OpenUartDevice(const std::string& name) {
    if (!UartManager::GetManager()->HasUartDevice(name)) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kENODEV);
    }

    auto uart_device = UartManager::GetManager()->OpenUartDevice(name);
    if (!uart_device) {
        AppLogError() << "Failed to open UART device " << name;
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEBUSY);
    }
    uart_devices_.emplace(name, std::move(uart_device));
    return;
}

bool PeripheralManagerClient::ReleaseUartDevice(const std::string& name) {
    return uart_devices_.erase(name) ? true
            : throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
}

bool PeripheralManagerClient::SetUartDeviceBaudrate(const std::string& name,
        int32_t baudrate) {
    auto uart_device = uart_devices_.find(name);
    if (uart_device == uart_devices_.end()) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    if (baudrate < 0) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEINVAL);
    }

    int ret = uart_device->second->SetBaudrate(static_cast<uint32_t>(baudrate));

    return (ret) ? false : true;
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

    return (ret) ? false : true;
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

    return (ret) ? false : true;
}
int PeripheralManagerClient::GetuartPollingFd(
        const std::string& name,
        int* fd) {
    auto uart_device = uart_devices_.find(name);
    if (uart_device == uart_devices_.end()) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    else (uart_devices_.find(name)->second->GetuPollingFd(fd));
    {
        return *fd;
    }

}
int32_t  PeripheralManagerClient::getBaudrate(const std::string& name,
        uint32_t* baudrate) {
    auto uart_device = uart_devices_.find(name);
    if (uart_device == uart_devices_.end()) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    int32_t ret = uart_device->second->getBaudrate(baudrate);

    return ret;
}
int PeripheralManagerClient::Geti2cPollingFd(
        const std::string& name,
        int32_t address,
        int* fd) {
    if (!i2c_devices_.count({name, address})) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kENODEV);
    }
    if (!I2cManager::GetI2cManager()->HasI2cDevBus(name)) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kENODEV);
    }
    int ret = i2c_devices_.find({name, address})->second->GetPollingFd(fd);

    return *fd;
}
