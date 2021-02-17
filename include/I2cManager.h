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
#include "Logger.h"

#include "I2cDriver.h"
#include "Constants.h"
#include "PinmuxManager.h"


struct I2cDevBus {
    explicit I2cDevBus(uint32_t b) : bus(b) {}
    uint32_t bus;
    std::string mux;
    std::string mux_group;
    std::map<uint32_t, std::unique_ptr<I2cDriverInterface>> driver_;
};

class I2cDevice {
public:
    I2cDevice(I2cDevBus* bus, uint32_t address) : bus_(bus), address_(address) {}
    ~I2cDevice() {
        if (!bus_->mux.empty()) {
            PinMuxManager::GetPinMuxManager()->ReleaseSource(bus_->mux,
                    bus_->mux_group);
        }
        bus_->driver_.erase(address_);
    }

    int32_t Read(void* data, uint32_t size, uint32_t* bytes_read) {
        return bus_->driver_[address_]->Read(data, size, bytes_read);
    }

    int32_t ReadRegByte(uint8_t reg, uint8_t* val) {
        return bus_->driver_[address_]->ReadRegByte(reg, val);
    }

    int32_t ReadRegWord(uint8_t reg, uint16_t* val) {
        return bus_->driver_[address_]->ReadRegWord(reg, val);
    }

    int32_t ReadRegBuffer(uint8_t reg,
            uint8_t* data,
            uint32_t size,
            uint32_t* bytes_read) {
        return bus_->driver_[address_]->ReadRegBuffer(reg, data, size, bytes_read);
    }

    int32_t Write(const void* data, uint32_t size, uint32_t* bytes_written) {
        return bus_->driver_[address_]->Write(data, size, bytes_written);
    }

    int32_t WriteRegByte(uint8_t reg, uint8_t val) {
        return bus_->driver_[address_]->WriteRegByte(reg, val);
    }

    int32_t WriteRegWord(uint8_t reg, uint16_t val) {
        return bus_->driver_[address_]->WriteRegWord(reg, val);
    }

    int32_t WriteRegBuffer(uint8_t reg,
            const uint8_t* data,
            uint32_t size,
            uint32_t* bytes_written) {
        return bus_->driver_[address_]->WriteRegBuffer(
                reg, data, size, bytes_written);
    }

private:
    I2cDevBus* bus_;
    uint32_t address_;
};

class I2cManager {
public:
    friend class I2cManagerTest;
    ~I2cManager();

    // Get the singleton.
    static I2cManager* GetI2cManager();

    // Resets the global I2c manager (used for testing).
    static void ResetI2cManager();

    bool RegisterI2cDevBus(const std::string& name, uint32_t bus);

    std::vector<std::string> GetI2cDevBuses();
    bool HasI2cDevBus(const std::string& name);

    bool SetPinMux(const std::string& name, const std::string& mux);
    bool SetPinMuxWithGroup(const std::string& name,
            const std::string& mux,
            const std::string& group);

    bool RegisterDriver(std::unique_ptr<I2cDriverInfoBase> driver_info);

    std::unique_ptr<I2cDevice> OpenI2cDevice(const std::string& name,
            uint32_t address);

private:
    I2cManager();

    std::map<std::string, std::unique_ptr<I2cDriverInfoBase>> driver_infos_;
    std::map<std::string, I2cDevBus> i2cdev_buses_;

};
