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

#include "I2cDriverI2cdev.h"
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <string>
#include <memory.h>

const char kI2cDevPath[] = "/dev/i2c-";

I2cDriverI2cDev::I2cDriverI2cDev(CharDeviceFactory* char_device_factory)
: fd_(-1), char_device_factory_(char_device_factory) {}

I2cDriverI2cDev::~I2cDriverI2cDev() {
    if (fd_ >= 0 && char_interface_ != nullptr) {
        char_interface_->Close(fd_);
    }
}

bool I2cDriverI2cDev::Init(uint32_t bus_id, uint32_t address) {
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

    std::string path = kI2cDevPath + std::to_string(bus_id);

    int fd = char_interface_->Open(path.c_str(), O_RDWR);
    if (fd < 0) {
        return false;
    }
    uintptr_t tmp_addr = address;
    if (char_interface_->Ioctl(fd, I2C_SLAVE, reinterpret_cast<void*>(tmp_addr)) < 0) {
        AppLogError() << "Failed to set I2C slave";
        char_interface_->Close(fd);
        return false;
    }

    fd_ = fd;
    return true;
}

int32_t I2cDriverI2cDev::Read(void* data, uint32_t size, uint32_t* bytes_read) {
    *bytes_read = char_interface_->Read(fd_, data, size);
    return *bytes_read == size ? 0 : EIO;
}

int32_t I2cDriverI2cDev::ReadRegByte(uint8_t reg, uint8_t* val) {
    union i2c_smbus_data read_data;
    struct i2c_smbus_ioctl_data smbus_args {
        .read_write = I2C_SMBUS_READ, .command = reg, .size = I2C_SMBUS_BYTE_DATA,
                .data = &read_data,
    };

    if (char_interface_->Ioctl(fd_, I2C_SMBUS, &smbus_args) < 0) {
        AppLogError() << "Failed I2C_SMBUS";
        return EIO;
    }
    *val = read_data.byte;
    return 0;
}


int32_t I2cDriverI2cDev::ReadRegWord(uint8_t reg, uint16_t* val) {
    union i2c_smbus_data read_data;
    struct i2c_smbus_ioctl_data smbus_args {
        .read_write = I2C_SMBUS_READ, .command = reg,  .size = I2C_SMBUS_WORD_DATA,
                .data = &read_data,
    };

    if (char_interface_->Ioctl(fd_, I2C_SMBUS, &smbus_args) < 0) {
        AppLogError() << "Failed I2C_SMBUS";
        return EIO;
    }

    *val = read_data.word;
    return 0;
}

int32_t I2cDriverI2cDev::ReadRegBuffer(uint8_t reg,
        uint8_t* data,
        uint32_t size,
        uint32_t* bytes_read) {
    *bytes_read = 0;
    if (size > I2C_SMBUS_BLOCK_MAX) {
        AppLogError() << "Can't read more than 32 bytes at a time.";
        return EINVAL;
    }

    union i2c_smbus_data read_data;
    read_data.block[0] = size;

    struct i2c_smbus_ioctl_data smbus_args;
    smbus_args.command = reg;
    smbus_args.read_write = I2C_SMBUS_READ;
    smbus_args.size = I2C_SMBUS_I2C_BLOCK_DATA;
    smbus_args.data = &read_data;

    if (char_interface_->Ioctl(fd_, I2C_SMBUS, &smbus_args) < 0) {
        AppLogError() << "Failed I2C_SMBUS";
        return EIO;
    }

    memcpy(data, &read_data.block[1], size);
    for(int i = 0; i < size; i++) {
        AppLogError() << __func__ << ":" << __LINE__ << " read_data.block[" << i << "] = " << read_data.block[1+i] ;
    }

    *bytes_read = size;
    return 0;
}

int32_t I2cDriverI2cDev::Write(const void* data,
        uint32_t size,
        uint32_t* bytes_written) {
    *bytes_written = char_interface_->Write(fd_, data, size);
    return *bytes_written == size ? 0 : EIO;
}

int32_t I2cDriverI2cDev::WriteRegByte(uint8_t reg, uint8_t val) {
    union i2c_smbus_data write_data;
    write_data.byte = val;
    struct i2c_smbus_ioctl_data smbus_args;
    smbus_args.command = reg;
    smbus_args.read_write = I2C_SMBUS_WRITE;
    smbus_args.size = I2C_SMBUS_BYTE_DATA;
    smbus_args.data = &write_data;

    if (char_interface_->Ioctl(fd_, I2C_SMBUS, &smbus_args) < 0) {
        return EIO;
    }
    return 0;
}

int32_t I2cDriverI2cDev::WriteRegWord(uint8_t reg, uint16_t val) {
    union i2c_smbus_data write_data;
    write_data.word = val;
    struct i2c_smbus_ioctl_data smbus_args;
    smbus_args.command = reg;
    smbus_args.read_write = I2C_SMBUS_WRITE;
    smbus_args.size = I2C_SMBUS_WORD_DATA;
    smbus_args.data = &write_data;

    if (char_interface_->Ioctl(fd_, I2C_SMBUS, &smbus_args) < 0) {
        return EIO;
    }
    return 0;
}

int32_t I2cDriverI2cDev::WriteRegBuffer(uint8_t reg,
        const uint8_t* data,
        uint32_t size,
        uint32_t* bytes_written) {
    *bytes_written = 0;
    if (size > I2C_SMBUS_BLOCK_MAX) {
        AppLogError() << "Can't write more than 32 bytes at a time.";
        return EINVAL;
    }

    union i2c_smbus_data write_data;
    write_data.block[0] = size;
    memcpy(&write_data.block[1], data, size);

    struct i2c_smbus_ioctl_data smbus_args;
    smbus_args.command = reg;
    smbus_args.read_write = I2C_SMBUS_WRITE;
    smbus_args.size = I2C_SMBUS_I2C_BLOCK_DATA;
    smbus_args.data = &write_data;

    if (char_interface_->Ioctl(fd_, I2C_SMBUS, &smbus_args) < 0) {
        AppLogError() << "Failed I2C_SMBUS";
        return EIO;
    }

    *bytes_written = size;

    return 0;
}
int  I2cDriverI2cDev::GetPollingFd(int * fd) {
    *fd = fd_;
    return fd_;
}

