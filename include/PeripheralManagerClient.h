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

#include <map>
#include <memory>
#include <string>
#include "Logger.h"
#include <list>
#include <vector>
#include <bits/stdc++.h>
#include "GpioManager.h"


using namespace std;

typedef void Status;

class PeripheralManagerClient {
 public:
  PeripheralManagerClient();
  ~PeripheralManagerClient();

  // Gpio functions.
  Status ListGpio(std::vector<std::string>* gpios) ;

  bool OpenGpio(const std::string& name) ;

  bool  ReleaseGpio(const std::string& name) ;

  bool  SetGpioDirection(const std::string& name,
                          int direction) ;

  bool  SetGpioValue(const std::string& name, bool value) ;

  bool  GetGpioValue(const std::string& name, bool* value) ;
  Status GetGpioPollingFd(const std::string& name,
                          void* fd) ;


 private:
  std::map<std::string, std::unique_ptr<GpioPin>> gpios_;
};

class test {
public:
    map<int, int> mp;
    void add(void);
};
