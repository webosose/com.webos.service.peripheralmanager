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

#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "GpioDriverSysfs.h"

namespace {

// Path to sysfd gpio.
const char kSysfsGpioPathPrefix[] = "/sys/class/gpio/gpio";

// Path to export file.
const char kSysfsGpioExportPath[] = "/sys/class/gpio/export";

// Direction filename.
const char kDirection[] = "direction";

// Value filename.
const char kValue[] = "value";

// Active low filename.
const char kActiveLowFile[] = "active_low";

// Edge filename.
const char kEdge[] = "edge";

// Edge values.
const char kEdgeNoneValue[] = "none";
const char kEdgeRisingValue[] = "rising";
const char kEdgeFallingValue[] = "falling";
const char kEdgeBothValue[] = "both";

// Direction values
const char kDirHigh[] = "high";
const char kDirLow[] = "low";
const char kDirIn[] = "in";

// Value values.
const char kValueHigh[] = "1";
const char kValueLow[] = "0";

}  // namespace

GpioDriverSysfs::GpioDriverSysfs(void* arg) : fd_(-1) {}

GpioDriverSysfs::~GpioDriverSysfs() {
  if (fd_ >= 0) {
    close(fd_);
  }
}

bool GpioDriverSysfs::Init(uint32_t index) {
  if (!ExportGpio(index)) {
      AppLogError() <<  "GpioDriverSysfs: Failed to export " << index;
    return false;
  }
  std::string path = kSysfsGpioPathPrefix + std::to_string(index);

  int fd = open(path.c_str(), O_RDONLY);
  if (fd < 0) {
      AppLogError()  << "Failed to open " << path;
    return false;
  }

  fd_ = fd;
  return true;
}

bool GpioDriverSysfs::SetValue(bool val) {
  bool success = val ? Enable() : Disable();
  if (!success) {
      AppLogError() << __func__ << ":" << __LINE__  << "Failed to set the value of the GPIO. Is it configured as "
                               << "output?";
  }
  return success;
}

bool GpioDriverSysfs::GetValue(bool* val) {
  std::string read_val;
  if (!ReadFromFile(kValue, &read_val))
    return false;
  if (read_val.size() < 1) {
    return false;
  }
  // Remove any whitespace.
  read_val = read_val[0];
  if (read_val == kValueHigh)
    *val = true;
  else
    *val = false;
  return true;
}

bool GpioDriverSysfs::SetDirection(GpioDirection direction) {
  switch (direction) {
    case kDirectionIn:
      return WriteToFile(kDirection, kDirIn);
    case kDirectionOutInitiallyHigh:
      return WriteToFile(kDirection, kDirHigh);
    case kDirectionOutInitiallyLow:
      return WriteToFile(kDirection, kDirLow);
  }
  return false;
}
bool GpioDriverSysfs::GetPollingFd(void * fd) {
  int f = openat(fd_, kValue, O_RDWR);
  if (f < 0)
    return false;

  return true;
}

bool GpioDriverSysfs::Enable() {
  return WriteToFile(kValue, kValueHigh);
}

bool GpioDriverSysfs::Disable() {
  return WriteToFile(kValue, kValueLow);
}

bool GpioDriverSysfs::WriteToFile(const std::string& file,
                                  const std::string& value) {
  int fd = openat(fd_, file.c_str(), O_RDWR);
  if (fd < 0)
    return false;

  ssize_t bytes = write(fd, value.c_str(), value.size());
  close(fd);
  if (bytes < 0)
    return false;
  if ((size_t)bytes != value.size())
    return false;
  return true;
}

bool GpioDriverSysfs::ReadFromFile(const std::string& file,
                                   std::string* value) {
  int fd = openat(fd_, file.c_str(), O_RDONLY);
  if (fd < 0)
    return false;
  char tmp_buf[16] = "";
  ssize_t bytes = read(fd, tmp_buf, sizeof(tmp_buf));
  close(fd);
  if (bytes < 0)
    return false;
  value->assign(tmp_buf, bytes);
  return true;
}

bool GpioDriverSysfs::ExportGpio(uint32_t index) {
  // Check if the pin is already exported.
  // It will fail otherwise.
  std::string path = kSysfsGpioPathPrefix + std::to_string(index);
  struct stat stat_buf;
  if (!stat(path.c_str(), &stat_buf)) {
    return true;
  }
  int fd = open(kSysfsGpioExportPath, O_WRONLY);
  if (fd < 0) {
      AppLogError() <<  "Failed to open " << kSysfsGpioExportPath;
    return false;
  }
  std::string value = std::to_string(index);
  ssize_t bytes = write(fd, value.c_str(), value.size());
  close(fd);
  if (bytes < 0) {
      AppLogError()  << "Failed to write " << bytes;
    return false;
  }
  if ((size_t)bytes != value.size()) {
    return false;
  }
  return true;
}
