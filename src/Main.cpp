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


#include <config.h>
#include <glib.h>
#include <iostream>
#include <luna-service2++/handle.hpp>
#include "Logger.h"
#include <luna-service2/lunaservice.hpp>
#include "PeripheralManagerAPI.h"
#include <time.h>
#include <sys/time.h>
#include "PeripheralManager.h"

int main(int argc, char* argv[]) {
    try {
        LS::Handle luna_handle(SERVICE_NAME);
        PeripheralManagerService luna_service(&luna_handle);
        PeripheralManager pm;
        pm.Init();
        luna_service.registerMethodsToLsHub();
        luna_service.run();
    } catch (...) {
        AppLogError() << "some Issue in Application "
                << "\n";
    }
    return 0;
}
