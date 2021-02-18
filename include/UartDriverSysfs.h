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
#include "CharDevice.h"
#include "UartDriver.h"
#include "Logger.h"

class UartDriverSysfs : public UartDriverInterface {
public:
    explicit UartDriverSysfs(CharDeviceFactory* factory);
    ~UartDriverSysfs();

    static std::string Compat() { return "UARTSYSFS"; }

    bool Init(const std::string& name) override;

    int SetBaudrate(uint32_t baudrate) override;
    uint32_t getBaudrate(uint32_t* baudrate) override;

    int Write(const std::vector<uint8_t>& data, uint32_t* bytes_written) override;

    int Read(std::vector<uint8_t>* data,
            uint32_t size,
            uint32_t* bytes_read) override;
    int  GetuPollingFd(int * fd) override;

private:
    int fd_;
    std::string path_;

    CharDeviceFactory* char_device_factory_;
    std::unique_ptr<CharDeviceInterface> char_interface_;

};
