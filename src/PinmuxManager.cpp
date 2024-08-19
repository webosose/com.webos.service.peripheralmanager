// Copyright (c) 2024 LG Electronics, Inc.
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

#include "PinmuxManager.h"
#include "Logger.h"
std::unique_ptr<PinMuxManager> g_pin_mux_manager;

PinMuxManager::PinMuxManager() {}

PinMuxManager::~PinMuxManager() {}

// static
PinMuxManager* PinMuxManager::GetPinMuxManager() {
    if (!g_pin_mux_manager) {
        g_pin_mux_manager.reset(new PinMuxManager());
    }
    return g_pin_mux_manager.get();
}

bool PinMuxManager::RegisterPin(std::string name,
        bool gpio,
        pin_mux_callbacks callbacks) {
    if (pins_.count(name))
        return false;
    pins_[name] = {false, gpio, callbacks};
    return true;
}

bool PinMuxManager::RegisterPinGroup(std::string name,
        std::set<std::string> pins) {
    if (groups_.count(name)) {
        return false;
    }

    // Check each pin is valid.
    for (auto& p : pins) {
        if (!pins_.count(p)) {
            return false;
        }
    }
    groups_[name] = std::move(pins);
    return true;
}

bool PinMuxManager::RegisterSource(std::string name,
        std::set<std::string> groups) {
    if (sources_.count(name)) {
        return false;
    }

    // Check each group is valid.
    for (auto& g : groups) {
        if (!groups_.count(g)) {
            return false;
        }
    }
    sources_[name] = std::move(groups);
    return true;
}

bool PinMuxManager::RegisterSimpleSource(std::string name,
        std::set<std::string> pins) {
    if (!RegisterPinGroup(name, std::move(pins))) {
        return false;
    }
    if (!RegisterSource(name, {name})) {
        // TODO(leecam): Unregister Group
        return false;
    }
    return true;
}

bool PinMuxManager::SetSource(std::string name, std::string group) {
    if (!sources_.count(name))
        return false;

    if (!sources_[name].count(group))
        return false;

    // Check to make sure each pin is not in use.
    for (auto& p : groups_[group]) {
        if (pins_[p].in_use) {
            return false;
        }
    }

    // Enable each Pin
    for (auto& p : groups_[group]) {
        pins_[p].in_use = true;

        // TODO(leecam): Handle failures here!!!
        if (pins_[p].callbacks.mux_cb)
            pins_[p].callbacks.mux_cb(p.c_str(), name.c_str());
    }

    return true;
}

void PinMuxManager::ReleaseSource(std::string name, std::string group) {
    if (!sources_.count(name)) {
        return;
    }
    if (!sources_[name].count(group)) {
        return;
    }
    for (auto& p : groups_[group]) {
        if (!pins_[p].in_use) {
            AppLogWarning() << "Releasig Pin " << p << " that is not in use.";
        }
        pins_[p].in_use = false;
    }
}

bool PinMuxManager::SetSimpleSource(std::string name) {
    return SetSource(name, name);
}

bool PinMuxManager::SetGpio(std::string name) {
    if (!pins_.count(name)) {
        AppLogWarning() << "PinMuxManager: Pin not found " << name;
        return false;
    }
    if (pins_[name].in_use) {
        AppLogWarning() << "PinMuxManager: Pin in use " << name;
        return false;
    }
    if (pins_[name].callbacks.mux_cb) {
        if (!pins_[name].callbacks.mux_cb(name.c_str(), nullptr))
            return false;
    }
    pins_[name].in_use = true;
    return true;
}

void PinMuxManager::ReleaseGpio(std::string name) {
    if (!pins_.count(name))
        return;
    pins_[name].in_use = false;
}

bool PinMuxManager::SetGpioDirection(std::string name, bool output) {
    if (!pins_.count(name)) {
        AppLogWarning() << "SetGpioDirection pin not found " << name;
        return false;
    }
    // Check if not in use - this should maybe be a DCHECK as
    // it shoudn't be able to happen.
    if (!pins_[name].in_use) {
        AppLogWarning() << "SetGpioDirection not in use " << name;
        return false;
    }
    if (pins_[name].callbacks.direction_cb)
        return pins_[name].callbacks.direction_cb(name.c_str(), output);

    return true;
}

