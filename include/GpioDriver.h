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
#include "Constants.h"
#include "Logger.h"


// This interface must be implemented by all
// Gpio drivers.
class GpioDriverInterface {
public:
    GpioDriverInterface() {}
    virtual ~GpioDriverInterface() {}

    virtual bool Init(uint32_t index) = 0;

    virtual bool SetValue(bool val) = 0;
    virtual bool GetValue(bool* val) = 0;
    virtual bool SetDirection(GpioDirection direction) = 0;
    virtual int  GetPollingFd(int * fd) = 0;
    virtual bool getDirection(std::string& direction) =0;
};

// The following is driver boilerplate.
class GpioDriverInfoBase {
public:
    GpioDriverInfoBase() {}
    virtual ~GpioDriverInfoBase() {}

    virtual std::string Compat() = 0;
    virtual std::unique_ptr<GpioDriverInterface> Probe() = 0;
};

template <class T, class PARAM>
class GpioDriverInfo : public GpioDriverInfoBase {
public:
    explicit GpioDriverInfo(PARAM param) : param_(param) {}
    ~GpioDriverInfo() override {}

    std::string Compat() override { return T::Compat(); }

    std::unique_ptr<GpioDriverInterface> Probe() override {
        return std::unique_ptr<GpioDriverInterface>(new T(param_));
    }

private:
    PARAM param_;
};
