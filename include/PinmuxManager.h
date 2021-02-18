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
#include <set>
#include <string>
#include <vector>
#include <peripheral_io.h>
#include "Logger.h"

struct Pin {
    bool in_use;
    bool gpio;
    pin_mux_callbacks callbacks;
};

class PinMuxManager {
public:
    friend class PinMuxManagerTest;
    ~PinMuxManager();

    // Get the singleton.
    static PinMuxManager* GetPinMuxManager();

    // The following Register* functions as used to configure
    // the pin mux configuation. These are called by the during
    // BSP setup, using the HAL.
    bool RegisterPin(std::string name, bool gpio, pin_mux_callbacks callbacks);
    bool RegisterPinGroup(std::string name, std::set<std::string> pins);
    bool RegisterSource(std::string name, std::set<std::string> groups);
    // Helper that registers a source with a default group of the same name.
    bool RegisterSimpleSource(std::string name, std::set<std::string> pins);

    // Called by the device managers, e.g SpiManager.
    // These are used to configure the pin muxing at runtime.
    bool SetSource(std::string name, std::string group);
    bool SetSimpleSource(std::string name);
    void ReleaseSource(std::string name, std::string group);

    // Used by the GpioManager to enable Gpio pins.
    bool SetGpio(std::string name);
    void ReleaseGpio(std::string name);
    bool SetGpioDirection(std::string name, bool output);

private:
    // Private constuctor as this class is accessed via a singleton.
    PinMuxManager();

    std::map<std::string, Pin> pins_;
    std::map<std::string, std::set<std::string>> groups_;
    std::map<std::string, std::set<std::string>> sources_;

};

