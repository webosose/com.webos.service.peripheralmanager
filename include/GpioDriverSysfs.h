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
#include "GpioDriver.h"
#include "Logger.h"

class GpioDriverSysfs : public GpioDriverInterface {
public:
    explicit GpioDriverSysfs(void* arg);
    ~GpioDriverSysfs();

    static std::string Compat() { return "GPIOSYSFS"; }

    bool Init(uint32_t index) override;

    // Gpio Driver interface.
    bool SetValue(bool val) override;
    bool GetValue(bool* val) override;
    bool SetDirection(GpioDirection direction) override;
    bool getDirection(std::string& direction) override;
    int  GetPollingFd(int * fd) override;

private:
    bool Enable();
    bool Disable();
    bool ExportGpio(uint32_t index);
    bool WriteToFile(const std::string& file, const std::string& value);
    bool ReadFromFile(const std::string& file, std::string* value);
    bool ReadFromFileDirection(const std::string& file, std::string* value);

    int fd_;

};
