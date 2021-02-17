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

#include "CharDevice.h"
#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

CharDevice::CharDevice() {}
CharDevice::~CharDevice() {}

int CharDevice::Open(const char* pathname, int flags) {
    return open(pathname, flags);
}

int CharDevice::Close(int fd) {
    return close(fd);
}

int CharDevice::Ioctl(int fd, int request, void* argp) {
    return ioctl(fd, request, argp);
}

ssize_t CharDevice::Read(int fd, void* buf, size_t count) {
    return read(fd, buf, count);
}

ssize_t CharDevice::Write(int fd, const void* buf, size_t count) {
    return write(fd, buf, count);
}

int CharDevice::Poll(struct pollfd* fds, nfds_t nfds, int timeout) {
    return poll(fds, nfds, timeout);
}

