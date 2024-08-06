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

#include "PeripheralManager.h"
#include "GpioDriverSysfs.h"
#include "GpioManager.h"
#include "I2cDriverI2cdev.h"
#include "I2cManager.h"
#include "PinmuxManager.h"
#include "SpiDriverSpidev.h"
#include "UartDriverSysfs.h"
#include "UartManager.h"
#include "peripheral_io.h"
#include "Logger.h"

int register_devices(const peripheral_io_module_t* dev,
        const peripheral_registration_cb_t* callbacks) {
    const char* gpio1 = "gpio1";
    const char* gpio2 = "gpio2";
    const char* gpio3 = "gpio3";
    const char* gpio4 = "gpio4";
    const char* gpio5 = "gpio5";
    const char* gpio6 = "gpio6";
    const char* gpio7 = "gpio7";
    const char* gpio8 = "gpio8";
    const char* gpio9 = "gpio9";
    const char* gpio10 = "gpio10";
    const char* gpio11 = "gpio11";
    const char* gpio12 = "gpio12";
    const char* gpio13 = "gpio13";
    const char* gpio14 = "gpio14";
    const char* gpio15 = "gpio15";
    const char* gpio16 = "gpio16";
    const char* gpio17 = "gpio17";
    const char* gpio18 = "gpio18";
    const char* gpio19 = "gpio19";
    const char* gpio20 = "gpio20";
    const char* gpio21 = "gpio21";
    const char* gpio22 = "gpio22";
    const char* gpio23 = "gpio23";
    const char* gpio24 = "gpio24";
    const char* gpio25 = "gpio25";

    const char* UART1 = "UART1";
    const char* UART2 = "UART2";
    const char* I2C1 = "I2C1";
    const char* I2C4 = "I2C4";
    const char* SPI00 = "SPI00";

    const char* uart0 [] = {gpio14, gpio15};
    const char* i2c1 [] = {gpio2, gpio3};
    const char* i2c4 [] = {gpio6, gpio7};
    const char* spi00 [] = {gpio7, gpio8, gpio9, gpio10, gpio11};

    if (callbacks) {
        if(callbacks->register_gpio_sysfs) {
            callbacks->register_gpio_sysfs(gpio4,4);
            callbacks->register_gpio_sysfs(gpio5,5);
            callbacks->register_gpio_sysfs(gpio6,6);
            callbacks->register_gpio_sysfs(gpio12,12);
            callbacks->register_gpio_sysfs(gpio13,13);
            callbacks->register_gpio_sysfs(gpio16,16);
            callbacks->register_gpio_sysfs(gpio17,17);
            callbacks->register_gpio_sysfs(gpio18,18);
            callbacks->register_gpio_sysfs(gpio19,19);
            callbacks->register_gpio_sysfs(gpio20,20);
            callbacks->register_gpio_sysfs(gpio21,21);
            callbacks->register_gpio_sysfs(gpio22,22);
            callbacks->register_gpio_sysfs(gpio23,23);
            callbacks->register_gpio_sysfs(gpio24,24);
            callbacks->register_gpio_sysfs(gpio25,25);
        }
        if(callbacks->register_i2c_dev_bus) {
            callbacks->register_i2c_dev_bus(I2C1, 1);
            callbacks->register_i2c_dev_bus(I2C4, 4);
        }
        if(callbacks->register_spi_dev_bus) {
            callbacks->register_spi_dev_bus(SPI00, 0 ,0);
        }
        if(callbacks->register_pin) {
            callbacks->register_pin(gpio1, true, pin_mux_callbacks());
            callbacks->register_pin(gpio2, true, pin_mux_callbacks());
            callbacks->register_pin(gpio3, true, pin_mux_callbacks());
            callbacks->register_pin(gpio7, true, pin_mux_callbacks());
            callbacks->register_pin(gpio8, true, pin_mux_callbacks());
            callbacks->register_pin(gpio9, true, pin_mux_callbacks());
            callbacks->register_pin(gpio10, true, pin_mux_callbacks());
            callbacks->register_pin(gpio11, true, pin_mux_callbacks());
            callbacks->register_pin(gpio14, true, pin_mux_callbacks());
            callbacks->register_pin(gpio15, true, pin_mux_callbacks());
        }
        if(callbacks->set_gpio_pin_mux) {
            callbacks->set_gpio_pin_mux(gpio1, gpio1);
        }
        if(callbacks->register_simple_source) {
            callbacks->register_simple_source(UART1, uart0, 2);
        }
        if(callbacks->register_simple_source) {
            callbacks->register_simple_source(I2C1, i2c1, 2);
            callbacks->register_simple_source(I2C4, i2c4, 2);
        }
        if(callbacks->register_simple_source) {
            callbacks->register_simple_source(SPI00, spi00, 5);
        }
        if(callbacks->set_i2c_pin_mux) {
            callbacks->set_i2c_pin_mux(I2C1,I2C1);
        }
        if(callbacks->set_spi_pin_mux) {
            callbacks->set_spi_pin_mux(SPI00,SPI00);
        }
    }
    return 0;
}
