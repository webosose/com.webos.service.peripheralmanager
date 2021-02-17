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

#include "SpiManager.h"

#include "PinmuxManager.h"


std::unique_ptr<SpiManager> g_spi_manager;

SpiManager::SpiManager() {}

SpiManager::~SpiManager() {}

// static
SpiManager* SpiManager::GetSpiManager() {
    if (!g_spi_manager) {
        g_spi_manager.reset(new SpiManager());
    }
    return g_spi_manager.get();
}

// static
void SpiManager::ResetSpiManager() {
    g_spi_manager.reset();
}

bool SpiManager::RegisterSpiDevBus(const std::string& name,
        uint32_t bus,
        uint32_t cs) {
    if (spidev_buses_.count(name))
        return false;
    spidev_buses_.emplace(name, SpiDevBus(bus, cs));
    return true;
}

std::vector<std::string> SpiManager::GetSpiDevBuses() {
    std::vector<std::string> buses;
    for (auto& i : spidev_buses_)
        buses.push_back(i.first);
    return buses;
}

bool SpiManager::HasSpiDevBus(const std::string& name) {
    return spidev_buses_.count(name);
}

bool SpiManager::RegisterDriver(
        std::unique_ptr<SpiDriverInfoBase> driver_info) {
    std::string key = driver_info->Compat();
    driver_infos_[key] = std::move(driver_info);
    return true;
}

bool SpiManager::SetPinMux(const std::string& name, const std::string& mux) {
    auto bus_it = spidev_buses_.find(name);
    if (bus_it == spidev_buses_.end()) {
        return false;
    }
    bus_it->second.mux = mux;
    bus_it->second.mux_group = mux;
    return true;
}

bool SpiManager::SetPinMuxWithGroup(const std::string& name,
        const std::string& mux,
        const std::string& group) {
    auto bus_it = spidev_buses_.find(name);
    if (bus_it == spidev_buses_.end())
        return false;
    bus_it->second.mux = mux;
    bus_it->second.mux_group = group;
    return true;
}

std::unique_ptr<SpiDevice> SpiManager::OpenSpiDevice(const std::string& name) {
    // Get the Bus from the BSP.
    auto bus_it = spidev_buses_.find(name);
    if (bus_it == spidev_buses_.end()) {
        return nullptr;
    }

    // Check its not alread in use
    if (bus_it->second.driver_) {
        return nullptr;
    }

    // Find a driver.
    // Currently there is only hardcoded support for SPIDEV
    auto driver_info_it = driver_infos_.find("SPIDEV");

    // Fail if there is no driver.
    if (driver_info_it == driver_infos_.end()) {
        return nullptr;
    }

    std::unique_ptr<SpiDriverInterface> driver(driver_info_it->second->Probe());

    if (!driver->Init(bus_it->second.bus, bus_it->second.cs)) {
        return nullptr;
    }

    // Set Pin muxing.
    if (!bus_it->second.mux.empty()) {
        PinMuxManager::GetPinMuxManager()->SetSource(bus_it->second.mux,
                bus_it->second.mux_group);
    }

    bus_it->second.driver_ = std::move(driver);

    return std::unique_ptr<SpiDevice>(new SpiDevice(&(bus_it->second)));
}

