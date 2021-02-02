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

#include <poll.h>
#include <stdlib.h>

#include <memory>


// Used to wrap the interface to a char device file.
// This can then be stubbed out and used in unittests.
class CharDeviceInterface {
 public:
  CharDeviceInterface(){};
  virtual ~CharDeviceInterface(){};

  virtual int Open(const char* pathname, int flags) = 0;
  virtual int Close(int fd) = 0;
  virtual int Ioctl(int fd, int request, void* argp) = 0;
  virtual ssize_t Read(int fd, void* buf, size_t count) = 0;
  virtual ssize_t Write(int fd, const void* buf, size_t count) = 0;
  virtual int Poll(struct pollfd* fds, nfds_t nfds, int timeout) = 0;
};

class CharDevice : public CharDeviceInterface {
 public:
  CharDevice();
  ~CharDevice() override;

  int Open(const char* pathname, int flags) override;
  int Close(int fd) override;
  int Ioctl(int fd, int request, void* argp) override;
  ssize_t Read(int fd, void* buf, size_t count) override;
  ssize_t Write(int fd, const void* buf, size_t count) override;
  int Poll(struct pollfd* fds, nfds_t nfds, int timeout) override;
};

class CharDeviceFactory {
 public:
  CharDeviceFactory() {}
  virtual ~CharDeviceFactory() {}

  virtual std::unique_ptr<CharDeviceInterface> NewCharDevice() = 0;
};


