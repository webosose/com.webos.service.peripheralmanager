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
#include "Logger.h"

#include <memory>
#include <string>

#include "Constants.h"

class SpiDriverInterface {
public:
    SpiDriverInterface() {}
    virtual ~SpiDriverInterface() {}

    virtual bool Init(uint32_t bus_id, uint32_t cs) = 0;
    virtual bool Transfer(const void* tx_data, void* rx_data, size_t len) = 0;
    virtual bool SetFrequency(uint32_t speed_hz) = 0;
    virtual bool SetMode(SpiMode mode) = 0;
    virtual bool SetBitJustification(bool lsb_first) = 0;
    virtual bool SetBitsPerWord(uint8_t bits_per_word) = 0;
    virtual bool SetDelay(uint16_t delay_usecs) = 0;
};

class SpiDriverInfoBase {
public:
    SpiDriverInfoBase() {}
    virtual ~SpiDriverInfoBase() {}

    virtual std::string Compat() = 0;
    virtual std::unique_ptr<SpiDriverInterface> Probe() = 0;
};

template <class T, class PARAM>
class SpiDriverInfo : public SpiDriverInfoBase {
public:
    explicit SpiDriverInfo(PARAM param) : param_(param) {}
    ~SpiDriverInfo() override {}

    std::string Compat() override { return T::Compat(); }

    std::unique_ptr<SpiDriverInterface> Probe() override {
        return std::unique_ptr<SpiDriverInterface>(new T(param_));
    }

private:
    PARAM param_;
};

