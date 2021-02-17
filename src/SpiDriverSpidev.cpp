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

#include "SpiDriverSpidev.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <string>
#include "Logger.h"
#include <cstring>

const char kSpiDevPath[] = "/dev/spidev";

SpiDriverSpiDev::SpiDriverSpiDev(CharDeviceFactory* char_device_factory)
: fd_(-1), char_device_factory_(char_device_factory) {}

SpiDriverSpiDev::~SpiDriverSpiDev() {
    if (fd_ >= 0 && char_interface_ != nullptr) {
        char_interface_->Close(fd_);
    }
}

bool SpiDriverSpiDev::Init(uint32_t bus_id, uint32_t cs) {
    if (fd_ >= 0) {
        return false;
    }

    // Get a char device. If char_device_factory_ is set
    // then this is a unittest and the char device is provided
    // by the test. Otherwise create a normal CharDevice.
    if (!char_device_factory_) {
        char_interface_.reset(new CharDevice());
    } else {
        char_interface_ = char_device_factory_->NewCharDevice();
    }


    std::string path =
            kSpiDevPath + std::to_string(bus_id) + "." + std::to_string(cs);
    int fd = char_interface_->Open(path.c_str(), O_RDWR);
    if (fd < 0) {
        return false;
    }

    fd_ = fd;
    // Set the default speed to the max
    uint32_t max_freq = 0;
    if (!GetMaxFrequency(&max_freq)) {
        char_interface_->Close(fd_);
        fd_ = -1;
        return false;
    }

    speed_hz_ = max_freq;

    // Default to 0 microseconds delay between transfers.
    delay_usecs_ = 0;

    // Default to 8 bits per word.
    bits_per_word_ = 8;

    return true;
}

bool SpiDriverSpiDev::Transfer(const void* tx_data, void* rx_data, size_t len) {
    struct spi_ioc_transfer msg;
    memset(&msg, 0, sizeof(msg));

    msg.tx_buf = (unsigned long)tx_data;
    msg.rx_buf = (unsigned long)rx_data;
    msg.speed_hz = speed_hz_;
    msg.bits_per_word = bits_per_word_;
    msg.delay_usecs = delay_usecs_;
    msg.len = len;
    if (char_interface_->Ioctl(fd_, SPI_IOC_MESSAGE(1), &msg) < 0) {
        AppLogError()  << "SPI Transfer IOCTL Failed";
        return false;
    }
    return true;
}

bool SpiDriverSpiDev::SetFrequency(uint32_t speed_hz) {
    if (fd_ < 0)
        return false;
    // Get the max speed and ensure speed_hz is less than it.
    uint32_t max_speed = 0;
    if (!GetMaxFrequency(&max_speed))
        return false;
    if (speed_hz > max_speed)
        speed_hz = max_speed;

    speed_hz_ = speed_hz;
    return true;
}

bool SpiDriverSpiDev::GetMaxFrequency(uint32_t* max_freq) {
    if (char_interface_->Ioctl(fd_, SPI_IOC_RD_MAX_SPEED_HZ, max_freq) < 0) {
        AppLogError()  << "Failed to get spi max freq";
        return false;
    }
    return true;
}

bool SpiDriverSpiDev::SetMode(SpiMode mode) {
    uint8_t k_mode = 0;
    switch (mode) {
    case kMode0:
        k_mode = SPI_MODE_0;
        break;
    case kMode1:
        k_mode = SPI_MODE_1;
        break;
    case kMode2:
        k_mode = SPI_MODE_2;
        break;
    case kMode3:
        k_mode = SPI_MODE_3;
        break;
    }

    if (char_interface_->Ioctl(fd_, SPI_IOC_WR_MODE, &k_mode) < 0) {
        AppLogError()  << "Failed to set mode";
        return false;
    }

    return true;
}

bool SpiDriverSpiDev::SetBitJustification(bool lsb_first) {
    uint8_t k_lsb_first = lsb_first;
    if (char_interface_->Ioctl(fd_, SPI_IOC_WR_LSB_FIRST, &k_lsb_first) < 0) {
        AppLogError()  << "Failed to set bit justifcation";
        return false;
    }
    return true;
}

bool SpiDriverSpiDev::SetBitsPerWord(uint8_t bits_per_word) {
    if (char_interface_->Ioctl(fd_, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word) <
            0) {
        AppLogError()  << "Failed to set bits per word";
        return false;
    }
    bits_per_word_ = bits_per_word;
    return true;
}

bool SpiDriverSpiDev::SetDelay(uint16_t delay_usecs) {
    delay_usecs_ = delay_usecs;
    return true;
}

