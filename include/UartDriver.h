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
#include <vector>


class UartDriverInterface {
public:
    virtual ~UartDriverInterface() {}

    virtual bool Init(const std::string& name, bool canonical = false) = 0;

    virtual int SetBaudrate(uint32_t baudrate) = 0;
    virtual uint32_t getBaudrate(uint32_t* baudrate) = 0;

    virtual int Write(const std::vector<uint8_t>& data,
            uint32_t* bytes_written) = 0;

    virtual int Read(std::vector<uint8_t>* data,
            uint32_t size,
            uint32_t* bytes_read) = 0;
    virtual int  GetuPollingFd(int * fd) = 0;
};

class UartDriverInfoBase {
public:
    virtual ~UartDriverInfoBase() {}

    virtual std::string Compat() = 0;
    virtual std::unique_ptr<UartDriverInterface> Probe() = 0;
};

template <class T, class PARAM>
class UartDriverInfo : public UartDriverInfoBase {
public:
    explicit UartDriverInfo(PARAM param) : param_(param) {}
    ~UartDriverInfo() override {}

    std::string Compat() override { return T::Compat(); }

    std::unique_ptr<UartDriverInterface> Probe() override {
        return std::unique_ptr<UartDriverInterface>(new T(param_));
    }

private:
    PARAM param_;
};
