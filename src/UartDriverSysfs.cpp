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

#include "UartDriverSysfs.h"

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "CharDevice.h"


UartDriverSysfs::UartDriverSysfs(CharDeviceFactory* factory)
: char_device_factory_(factory){}

UartDriverSysfs::~UartDriverSysfs() {}

bool UartDriverSysfs::Init(const std::string& name, bool canonical) {
    path_ = name;
    int fd = -1;

    // Get a char device. If char_device_factory_ is set
    // then this is a unittest and the char device is provided
    // by the test. Otherwise create a normal CharDevice.
    if (!char_device_factory_) {
        char_interface_.reset(new CharDevice());
    } else {
        char_interface_ = char_device_factory_->NewCharDevice();
    }

    // Open as non-blocking as we don't want peripheral_manager to block on a
    // single client read.
    if(canonical) {
        fd = char_interface_->Open(path_.c_str(), O_RDWR );
    }
    else {
        fd = char_interface_->Open(path_.c_str(), O_RDWR | O_NONBLOCK);
    }
    if (fd < 0) {
        AppLogError() << "Failed to open " << path_;
        return false;
    }

    // Configure the device in raw mode.
    struct termios config;
    if (char_interface_->Ioctl(fd, TCGETS, &config)) {
        AppLogError() << "Failed to read the tty config";
        close(fd);
        return false;
    }
    if(canonical) {
        config.c_cflag = CS8 | CLOCAL | CREAD;
        config.c_lflag |= ICANON;
        config.c_iflag = IGNPAR;
        config.c_oflag = 0;
        config.c_lflag |= ICANON;
        config.c_cc[VMIN] = 0;
        config.c_cc[VTIME] = 0;
    }
    else {
        cfmakeraw(&config);
    }

    if (char_interface_->Ioctl(fd, TCSETSF, &config)) {
        AppLogError() << "Failed to configure the UART device as Raw.";
        close(fd);
        return false;
    }

    fd_ = fd;
    return true;
}

int UartDriverSysfs::SetBaudrate(uint32_t baudrate) {
    speed_t s;
    switch (baudrate) {
    case 0:
        s = B0;
        break;

    case 50:
        s = B50;
        break;

    case 75:
        s = B75;
        break;

    case 110:
        s = B110;
        break;

    case 134:
        s = B134;
        break;

    case 150:
        s = B150;
        break;

    case 200:
        s = B200;
        break;

    case 300:
        s = B300;
        break;

    case 600:
        s = B600;
        break;

    case 1200:
        s = B1200;
        break;

    case 1800:
        s = B1800;
        break;

    case 2400:
        s = B2400;
        break;

    case 4800:
        s = B4800;
        break;

    case 9600:
        s = B9600;
        break;

    case 19200:
        s = B19200;
        break;

    case 38400:
        s = B38400;
        break;

    case 57600:
        s = B57600;
        break;

    case 115200:
        s = B115200;
        break;

    case 230400:
        s = B230400;
        break;
    default:
        return EINVAL;
    }

    struct termios config;

    if (char_interface_->Ioctl(fd_, TCGETS, &config) != 0 ||
            cfsetspeed(&config, s) != 0 ||
            char_interface_->Ioctl(fd_, TCSETS, &config) != 0) {
        AppLogError() << "Failed to set the UART baurate to " << baudrate;
        return EIO;
    }

    return 0;
}

int UartDriverSysfs::Write(const std::vector<uint8_t>& data,
        uint32_t* bytes_written) {
    errno = 0;
    int ret = char_interface_->Write(fd_, data.data(), data.size());

    if (ret == -1) {
        AppLogError() << "Failed to write to UART device";
        *bytes_written = 0;
        return EIO;
    }

    *bytes_written = ret;
    return 0;
}

int UartDriverSysfs::Read(std::vector<uint8_t>* data,
        uint32_t size,
        uint32_t* bytes_read) {
    errno = 0;
    data->resize(size);

    int ret = char_interface_->Read(fd_, data->data(), size);

    if (ret == -1) {
        *bytes_read = 0;
        data->resize(0);
        if (errno == EAGAIN) {
            return EAGAIN;
        }
        AppLogError() << "Failed to read from UART device";
        return EIO;
    }

    *bytes_read = ret;
    data->resize(ret);
    return 0;
}
int  UartDriverSysfs::GetuPollingFd(int * fd) {
    *fd = fd_;
    return fd_;
}
uint32_t UartDriverSysfs::getBaudrate(uint32_t* baudrate) {
    struct termios config;
    tcgetattr( fd_, &config);
    speed_t speed;
    speed = cfgetospeed( &config);
    switch(speed){
    case B0:
        speed = 0;
        break;

    case B50:
        speed = 50;
        break;

    case B75:
        speed = 75;
        break;

    case B110:
        speed = 110;
        break;

    case B134:
        speed = 134;
        break;

    case B150:
        speed = 150;
        break;

    case B200:
        speed = 200;
        break;

    case B300:
        speed = 300;
        break;

    case B600:
        speed = 600;
        break;

    case B1200:
        speed = 1200;
        break;

    case B1800:
        speed = 1800;
        break;

    case B2400:
        speed = 2400;
        break;

    case B4800:
        speed = 4800;
        break;

    case B9600:
        speed = 9600;
        break;

    case B19200:
        speed = 19200;
        break;

    case B38400:
        speed = 38400;
        break;

    case B57600:
        speed = 57600;
        break;

    case B115200:
        speed = 115200;
        break;

    case B230400:
        speed = 230400;
        break;
    }
    *baudrate = speed;
    return speed;
}

