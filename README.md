Summary
-------
Peripheralmanager service

Description
-----------
Peripheral manager is a service to provide APIs to monitor sensors and control actuators connected to I/O peripherals using industry standard protocols.
The service manages, parallel request from multiple applications to access peripherals and control the hardware. Currently UART ,GPIO, SPI and I2C will be supported.
The APIs provied are protocol specific and hardware independent, so any custom hardware can be connected without any code change required in the middleware.

How to Build on Linux
---------------------

## Dependencies

Below are the tools and libraries (and their minimum versions) required to build sample program:

* cmake (version required by cmake-modules-webos)
* gcc
* glib-2.0
* make
* cmake-modules-webos

## Building

    $ cd build-webos
    $ source oe-init-build-env
    $ bitbake com.webos.service.peripheralmanager

Copyright and License Information
=================================
Unless otherwise specified, all content, including all source code files and
documentation files in this repository are:

Copyright (c) 2020 LG Electronics, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

SPDX-License-Identifier: Apache-2.0

// Author(s)    : Sanjay Poptani
// Email ID.    : sanjay.poptani@lge.com
