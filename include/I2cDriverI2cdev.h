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
#include <memory>
#include "Logger.h"
#include "CharDevice.h"
#include "I2cDriver.h"

class I2cDriverI2cDev : public I2cDriverInterface {
public:
    explicit I2cDriverI2cDev(CharDeviceFactory* char_device_factory);
    ~I2cDriverI2cDev();

    static std::string Compat() { return "I2CDEV"; }

    bool Init(uint32_t bus_id, uint32_t address) override;

    int32_t Read(void* data, uint32_t size, uint32_t* bytes_read) override;
    int32_t ReadRegByte(uint8_t reg, uint8_t* val) override;
    int32_t ReadRegWord(uint8_t reg, uint16_t* val) override;
    int32_t ReadRegBuffer(uint8_t reg,
            uint8_t* data,
            uint32_t size,
            uint32_t* bytes_read) override;

    int32_t Write(const void* data,
            uint32_t size,
            uint32_t* bytes_written) override;
    int32_t WriteRegByte(uint8_t reg, uint8_t val) override;
    int32_t WriteRegWord(uint8_t reg, uint16_t val) override;
    int32_t WriteRegBuffer(uint8_t reg,
            const uint8_t* data,
            uint32_t size,
            uint32_t* bytes_written) override;
    int  GetPollingFd(int * fd) override;

private:
    int fd_;

    // Used for unit testing and is null in production.
    // Ownership is in the test and outlives this class.
    CharDeviceFactory* char_device_factory_;
    std::unique_ptr<CharDeviceInterface> char_interface_;
};
