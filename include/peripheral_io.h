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

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#pragma once

typedef enum {
  GPIO_DIRECTION_IN,      /** Input */
  GPIO_DIRECTION_OUT_LOW, /** Output, initially low */
  GPIO_DIRECTION_OUT_HIGH /** Output, initially high */
} gpio_direction_t;

/**
 * Callback to enable a given pinmux source on a specific pin.
 *
 * Args:
 *  pin: Name of the pin.
 *  source: Name of the pinmux source to enable. When NULL, setup the pinmux
 *          to enable GPIO on this pin.
 *
 * Returns:
 *  0 on success, errno on error.
 */
typedef int (*pin_mux_cb)(const char* pin, const char* source);

/**
 * Callback to set the direction of a given GPIO.
 *
 * Args:
 *  pin: Name of the pin to configure.
 *  dir: Direction to set. One of gpio_direction_t.
 *
 * Returns:
 *  0 on success, errno on error.
 */
typedef int (*pin_mux_direction_cb)(const char* pin, int dir);

/**
 * Callbacks used by peripheral manager to configure given pins.
 */
typedef struct pin_mux_callbacks {
  pin_mux_cb mux_cb;
  pin_mux_direction_cb direction_cb;
} pin_mux_callbacks;

/**
 * Callbacks into peripheral manager.
 *
 * Those callbacks must be used in register_devices to declare the available
 * interfaces to peripheral manager.
 *
 * All peripherals are referred to by their friendly name.
 * The friendly name is a string, used to refer to a given peripheral (ex: gpio
 * name, spi bus name) that is simple to understand, well documented for the
 * board and unambiguous.
 *
 * Before coming up with a new naming scheme, consider:
 * - Using an existing naming scheme: if the board provides an arduino pinout, a
 *   raspberry Pi pinout or a 96 boards pinout, use it.
 * - Using the names written on the board next to the physical ports if any.
 * - Using the documented name when widely available.
 *   Referring to the same pin or peripheral by two different names in the
 *   documentation and in peripheral manager would be confusing to the user.
 *
 * If coming up with a new name, use only simple characters (ideally, A-Z, 0-9,
 * -, _).
 */
typedef struct peripheral_registration_cb_t {
  /**
   * Register a pin.
   *
   * Args:
   *  name: Friendly name (see definition above) of the pin.
   *  callbacks: Callbacks used to set up pinmuxes.
   *
   * Returns:
   *  0 on success, errno on error.
   */
  int (*register_pin)(const char* name, int gpio, pin_mux_callbacks callbacks);

  /**
   * Register a pin group.
   *
   * Args:
   *  name: Name of the group.
   *  pins: List of pin names that are part of the group.
   *  nr_pins: Size of |pins|.
   *
   * Returns:
   *  0 on success, errno on error.
   */
  int (*register_pin_group)(const char* name, char** pins, size_t nr_pins);

  /**
   * Register a pinmux source.
   *
   * Args:
   *  name: Name of the pinmux source.
   *  groups: List of possible pinmux groups this source can come up on.
   *  nr_groups: Size of |groups|.
   *
   * Returns:
   *  0 on success, errno on error.
   */
  int (*register_source)(const char* name, char** groups, size_t nr_groups);

  /**
   * Register a simple source, mapped to a single pin group.
   *
   * This convenience function replaces the two calls to register_pin_group and
   * register_source in most cases.
   *
   * Args:
   *  name: Name of the pinmux source.
   *  pins: List of pins this source comes up on.
   *  nr_pins: Size of |pins|.
   *
   * Returns:
   *  0 on success, errno on error.
   */
  int (*register_simple_source)(const char* name,
                                const char** pins,
                                size_t nr_pins);

  /**
   * Register a sysfs backed GPIO.
   *
   * Args:
   *  name: Friendly name of the GPIO.
   *  index: Index of the gpio in sysfs.
   *
   * Returns:
   *  0 on success, errno on error.
   */
  int (*register_gpio_sysfs)(const char* name, uint32_t index);

  /**
   * Set the pinmux for a given GPIO.
   *
   * Args:
   *  name: Friendly name of the pin.
   *  source: Name of the pinmux source.
   *
   * Returns:
   *  0 on success, errno on error.
   */
  int (*set_gpio_pin_mux)(const char* name, const char* source);

  /**
   * Register a SPI device.
   *
   * Args:
   *  name: Friendly name of the device.
   *  bus: Bus number.
   *  cs: Chip select.
   *
   * Returns:
   *  0 on success, errno on error.
   */
  int (*register_spi_dev_bus)(const char* name, uint32_t bus, uint32_t cs);

  /**
   * Set the pinmux for a given SPI device.
   *
   * Args:
   *  name: Friendly name of the device.
   *  source: Name of the pinmuxing source.
   *
   * Returns:
   *  0 on success, errno on error.
   */
  int (*set_spi_pin_mux)(const char* name, const char* source);

  /**
   * Register a sysfs-backed LED.
   *
   * Args:
   *  name: Friendly name of the LED.
   *  sysfs_name: Name of the device in sysfs.
   *
   * Returns:
   *  0 on success, errno on error.
   */
  int (*register_led_sysfs)(const char* name, const char* sysfs_name);

  /**
   * Register a UART bus.
   *
   * Args:
   *  name: Friendly name of the bus.
   *  dev_name: Name of the device in sysfs.
   *
   * Returns:
   *  0 on success, errno on error.
   */
  int (*register_uart_bus)(const char* name, const char* dev_name);

  /**
   * Set the pinmux for a given UART bus.
   *
   * Args:
   *  name: Friendly name of the UART bus.
   *  source: Name of the pinmuxing source.
   *
   * Returns:
   *  0 on success, errno on error.
   */
  int (*set_uart_pin_mux)(const char* name, const char* source);

  /**
   * Register an I2C bus.
   *
   * Args:
   *  name: Friendly name of the I2C device.
   *  bus: I2C bus number.
   *
   * Returns:
   *  0 on success, errno on error.
   */
  int (*register_i2c_dev_bus)(const char* name, uint32_t bus);

  /**
   * Set the pinmux for a given I2C bus.
   *
   * Args:
   *  name: Friendly name of the I2C bus.
   *  source: Name of the pinmuxing source.
   *
   * Returns:
   *  0 on success, errno on error.
   */
  int (*set_i2c_pin_mux)(const char* name, const char* source);

} peripheral_registration_cb_t;

typedef struct peripheral_io_module_t peripheral_io_module_t;

struct peripheral_io_module_t {

  /**
   * Register all available devices with the peripheral manager.
   *
   * Arguments:
   *  dev: Pointer to the peripheral_io module.
   *  callbacks: Callbacks into peripheral manager to register devices.
   *
   * Returns:
   *  0 on success, errno on error.
   */
  int (*register_devices)(const peripheral_io_module_t* dev,
                          const peripheral_registration_cb_t* callbacks);
};

