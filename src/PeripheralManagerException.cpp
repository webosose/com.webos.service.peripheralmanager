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

#include <sstream>
#include <string>
#include "PeripheralManagerException.h"

const std::map<PeripheralManagerErrors, std::string> error_text {
    {PeripheralManagerErrors::kInvalidJSON, "Invalid JSON Object"},
    {PeripheralManagerErrors::kInvalidParameters, "Invalid Parameters"},
    {PeripheralManagerErrors::kKindNotCreated, "DB kind not created"},
    {PeripheralManagerErrors::kProfileAlreadyExists, "Profile exist"},
    {PeripheralManagerErrors::kProfileDoesNotExist, "Profile does not exist"},
    {PeripheralManagerErrors::kServiceAlreadyRunning, "Service already running"},
    {PeripheralManagerErrors::kInternalError, "Internal Error"},
    {PeripheralManagerErrors::kIntitializeError, "Service initialization error"},
    {PeripheralManagerErrors::kUnknownError, "Unknown Error"},
    {PeripheralManagerErrors::kActivateInHostError, "Cannot activate in host"},
    {PeripheralManagerErrors::kInHostError, "Cannot run in host"},
    {PeripheralManagerErrors::kActiveInOtherSessionError, "Can not remove User active in other session"},
    {PeripheralManagerErrors::kEBUSY, "EBUSY"},
    {PeripheralManagerErrors::kENODEV, "kENODEV"},
    {PeripheralManagerErrors::kEPERM, "kEPERM"},
    {PeripheralManagerErrors::kEREMOTEIO, "kEREMOTEIO"},
    {PeripheralManagerErrors::kEINVAL, "kEINVAL"},
    {PeripheralManagerErrors::kNoError, "No Error"},
};


PeripheralManagerException::PeripheralManagerException() : error_code(PeripheralManagerErrors::kUnknownError) { setErrorMsg(); }

PeripheralManagerException::PeripheralManagerException(const std::string& err_msg, const PeripheralManagerErrors err_code) : user_error_message(err_msg),
                                                                                                           error_code(err_code)
{
    setErrorMsg();
}

PeripheralManagerException::~PeripheralManagerException() {}

const char* PeripheralManagerException::what() const throw() { return error_message.c_str(); }


void PeripheralManagerException::setErrorMsg() {
    std::stringstream ss;
    ss << "Code: " << static_cast<int>(error_code) << " , Message: ";
    if (!user_error_message.empty()) ss << user_error_message;
    error_message = ss.str();
}
