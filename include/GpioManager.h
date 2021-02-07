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
// Author(s)    : Sanjay Poptani
// Email ID.    : sanjay.poptani@lge.com

#pragma once

#include <stdint.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "GpioDriver.h"
#include "Logger.h"

struct GpioPinSysfs {
  uint32_t index;
  std::string mux;
  std::unique_ptr<GpioDriverInterface> driver_;
};

class GpioPin {
 public:
  // TODO(leecam): pins should have a generic device
  // reference, not a sysfs one.
  explicit GpioPin(GpioPinSysfs* pin) : pin_(pin) {}
  ~GpioPin() {
    if (!pin_->mux.empty()) {
    }
    pin_->driver_.reset();
  }

  bool SetValue(bool val) { return pin_->driver_->SetValue(val); }

  bool GetValue(bool* val) { return pin_->driver_->GetValue(val); }

  bool SetDirection(GpioDirection direction) {
    if (!pin_->mux.empty()) {
      bool dir = true;
      if (direction == kDirectionIn)
        dir = false;
    }
    return pin_->driver_->SetDirection(direction);
  }
  bool getDirection(std::string&  direction) { return pin_->driver_->getDirection(direction);}
  bool GetPollingFd(void* fd) {
    return pin_->driver_->GetPollingFd(fd);
  }

 private:
  GpioPinSysfs* pin_;
};

class GpioManager {
 public:
  friend class GpioManagerTest;
  ~GpioManager();

  // Get the singleton.
  static GpioManager* GetGpioManager();

  // Delete the GpioManager (used for test);
  static void ResetGpioManager();

  // Used by the BSP to tell PMan of an GPIO Pin.
  bool RegisterGpioSysfs(const std::string& name, uint32_t index);
  bool SetPinMux(const std::string& name, const std::string& mux);

  // Query for available pins.
  std::vector<std::string> GetGpioPins();
  bool HasGpio(const std::string& pin_name);

  bool RegisterDriver(std::unique_ptr<GpioDriverInfoBase> driver_info);

  std::unique_ptr<GpioPin> OpenGpioPin(const std::string& name);

 private:
  GpioManager();

  std::map<std::string, std::unique_ptr<GpioDriverInfoBase>> driver_infos_;
  std::map<std::string, GpioPinSysfs> sysfs_pins_;

};
