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

#include "Logger.h"
#include <stdint.h>
#include <memory>
#include "CharDevice.h"
#include "SpiDriver.h"

class SpiDriverSpiDev : public SpiDriverInterface {
public:
    explicit SpiDriverSpiDev(CharDeviceFactory* char_device_factory);
    ~SpiDriverSpiDev();

    static std::string Compat() { return "SPIDEV"; }

    bool Init(uint32_t bus_id, uint32_t cs) override;
    bool Transfer(const void* tx_data, void* rx_data, size_t len) override;
    bool SetFrequency(uint32_t speed_hz) override;
    bool SetMode(SpiMode mode) override;
    bool SetBitJustification(bool lsb_first) override;
    bool SetBitsPerWord(uint8_t bits_per_word) override;
    bool SetDelay(uint16_t delay_usecs) override;

private:
    bool GetMaxFrequency(uint32_t* max_freq);

    int fd_;
    uint32_t bits_per_word_;
    uint32_t speed_hz_;
    uint16_t delay_usecs_;
    std::unique_ptr<CharDeviceInterface> char_interface_;

    // Used for unit testing and is null in production.
    // Ownership is in the test and outlives this class.
    CharDeviceFactory* char_device_factory_;

};

