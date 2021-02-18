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
#include <string>
#include "Logger.h"
#include "Constants.h"


class I2cDriverInterface {
public:
    I2cDriverInterface() {}
    virtual ~I2cDriverInterface() {}
    virtual bool Init(uint32_t bus_id, uint32_t address) = 0;

    // Returns 0 on success, errno on errors.
    virtual int32_t Read(void* data, uint32_t size, uint32_t* bytes_read) = 0;
    virtual int32_t ReadRegByte(uint8_t reg, uint8_t* val) = 0;
    virtual int32_t ReadRegWord(uint8_t reg, uint16_t* val) = 0;
    virtual int32_t ReadRegBuffer(uint8_t reg,
            uint8_t* data,
            uint32_t size,
            uint32_t* bytes_read) = 0;

    virtual int32_t Write(const void* data,
            uint32_t size,
            uint32_t* bytes_written) = 0;
    virtual int32_t WriteRegByte(uint8_t reg, uint8_t val) = 0;
    virtual int32_t WriteRegWord(uint8_t reg, uint16_t val) = 0;
    virtual int32_t WriteRegBuffer(uint8_t reg,
            const uint8_t* data,
            uint32_t size,
            uint32_t* bytes_written) = 0;
    virtual int GetPollingFd(int* fd) = 0;
};

class I2cDriverInfoBase {
public:
    I2cDriverInfoBase() {}
    virtual ~I2cDriverInfoBase() {}

    virtual std::string Compat() = 0;
    virtual std::unique_ptr<I2cDriverInterface> Probe() = 0;
};

template <class T, class PARAM>
class I2cDriverInfo : public I2cDriverInfoBase {
public:
    explicit I2cDriverInfo(PARAM param) : param_(param) {}
    ~I2cDriverInfo() override {}

    std::string Compat() override { return T::Compat(); }

    std::unique_ptr<I2cDriverInterface> Probe() override {
        return std::unique_ptr<I2cDriverInterface>(new T(param_));
    }

private:
    PARAM param_;
};
