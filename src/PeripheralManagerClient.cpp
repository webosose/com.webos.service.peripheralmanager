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


#include <iostream>
#include <map>
#include <iostream>
#include <vector>
#include <memory>
#include <cstdio>
#include <fstream>
#include <cassert>
#include <functional>
#include <string>

#include <bits/stdc++.h>

#include "PeripheralManagerClient.h"
#include "PeripheralManagerException.h"
#define ROW 4
#define COL 5

PeripheralManagerClient::PeripheralManagerClient() {}
PeripheralManagerClient::~PeripheralManagerClient() {}

Status PeripheralManagerClient::ListGpio(std::vector<std::string>* gpios) {
    *gpios = GpioManager::GetGpioManager()->GetGpioPins();
}



void test::add(void) {
    mp.emplace(3, 40);
}

bool PeripheralManagerClient::OpenGpio(const std::string& name) {
    if (!GpioManager::GetGpioManager()->HasGpio(name)) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kENODEV);

        return false;
    }
        auto gpio = GpioManager::GetGpioManager()->OpenGpioPin(name);
    if (!gpio) {
            AppLogError() << "Failed to open GPIO " << name;
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEBUSY);
        return false;
    }

    gpios_.emplace(name, std::move(gpio));

    return true;
}


bool  PeripheralManagerClient::ReleaseGpio(const std::string& name) {
    gpios_.erase(name);
    return true;
}

bool PeripheralManagerClient::SetGpioDirection(const std::string& name,
        int direction) {
    if (!gpios_.count(name)) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
        return false;
    }

    if (gpios_.find(name)->second->SetDirection(GpioDirection(direction))) {
        return true;
    }
    throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
    return false;
}

bool PeripheralManagerClient::SetGpioValue(const std::string& name,
        bool value) {
    if (!gpios_.count(name)) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    if (gpios_.find(name)->second->SetValue(value)) {
        return true;
    }
    throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
    return false;
}

bool PeripheralManagerClient::GetGpioValue(const std::string& name,
        bool* value) {
    if (!gpios_.count(name)) {
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    if (gpios_.find(name)->second->GetValue(value)) {
        return true;
    }
    throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEREMOTEIO);
    return false;
}
Status PeripheralManagerClient::GetGpioPollingFd(
        const std::string& name,
        void* fd) {
    if (!gpios_.count(name)){
        throw PeripheralManagerException(std::string(" "), PeripheralManagerErrors::kEPERM);
    }

    if (gpios_.find(name)->second->GetPollingFd(fd)) {
        //    return Status::ok();
    }

    //  return Status::fromServiceSpecificError(EREMOTEIO);
}
