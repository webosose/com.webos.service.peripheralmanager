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
#include <string>
#include <vector>
#include "Constants.h"
#include "PinmuxManager.h"
#include "SpiDriver.h"
#include "Logger.h"

struct SpiDevBus {
    SpiDevBus(uint32_t b, uint32_t c) : bus(b), cs(c) {}
    uint32_t bus;
    uint32_t cs;
    std::string mux;
    std::string mux_group;
    std::unique_ptr<SpiDriverInterface> driver_;
};

class SpiDevice {
public:
    explicit SpiDevice(SpiDevBus* bus) : bus_(bus) {}
    ~SpiDevice() {
        if (!bus_->mux.empty()) {
            PinMuxManager::GetPinMuxManager()->ReleaseSource(bus_->mux,
                    bus_->mux_group);
        }
        bus_->driver_.reset();
    }

    bool WriteByte(uint8_t val) {
        return Transfer(&val, nullptr, sizeof(val));
    }

    bool WriteBuffer(const void* data, size_t len) {
        return Transfer(data, nullptr, len);
    }

    bool Transfer(const void* tx_data, void* rx_data, size_t len) {
        return bus_->driver_->Transfer(tx_data, rx_data, len);
    }

    bool SetFrequency(uint32_t speed_hz) {
        return bus_->driver_->SetFrequency(speed_hz);
    }

    bool SetMode(SpiMode mode) { return bus_->driver_->SetMode(mode); }

    bool SetBitJustification(bool lsb_first) {
        return bus_->driver_->SetBitJustification(lsb_first);
    }

    bool SetBitsPerWord(uint8_t bits_per_word) {
        return bus_->driver_->SetBitsPerWord(bits_per_word);
    }

    bool SetDelay(uint16_t delay_usecs) {
        return bus_->driver_->SetDelay(delay_usecs);
    }

private:
    SpiDevBus* bus_;
};

class SpiManager {
public:
    friend class SpiManagerTest;
    ~SpiManager();

    // Get the singleton.
    static SpiManager* GetSpiManager();

    // Reset the Spi manager.
    static void ResetSpiManager();

    // Used by the BSP to tell PMan of an SPI bus
    // that can be used by a PMan client.
    // Will return false if the bus has already been registered.
    bool RegisterSpiDevBus(const std::string& name, uint32_t bus, uint32_t cs);

    std::vector<std::string> GetSpiDevBuses();
    bool HasSpiDevBus(const std::string& name);

    bool SetPinMux(const std::string& name, const std::string& mux);
    bool SetPinMuxWithGroup(const std::string& name,
            const std::string& mux,
            const std::string& group);

    bool RegisterDriver(std::unique_ptr<SpiDriverInfoBase> driver_info);

    std::unique_ptr<SpiDevice> OpenSpiDevice(const std::string& name);

private:
    SpiManager();

    std::map<std::string, std::unique_ptr<SpiDriverInfoBase>> driver_infos_;
    std::map<std::string, SpiDevBus> spidev_buses_;

};

