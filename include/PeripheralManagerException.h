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

#include <exception>
#include <map>
#include <string>

enum PeripheralManagerErrors
{
    kInvalidJSON = -1009,     // INVALID_JSON
    kInvalidParameters,       // INVALID_PARAMETERS
    kKindNotCreated,          // STORAGE_KIND_NOT_CREATED
    kProfileAlreadyExists,    // PROFILE_ALREADY_EXISTS
    kProfileDoesNotExist,     // PROFILE_DOES_NOT_EXIST
    kServiceAlreadyRunning,   // SERVICE_ALREADY_RUNNING
    kInternalError,           // INTERNAL_ERROR
    kIntitializeError,        // INITIALIZE_ERROR
    kUnknownError,            // UNKNOWN_ERROR
    kActivateInHostError,     // ACTIVATE_IN_HOST_ERROR
    kInHostError,             // CANNOT_RUN_IN_HOST_ERROR
    kActiveInOtherSessionError,// CANNOT_REMOVE_USER_OF_OTHER_SESSION
    kEBUSY,
    kENODEV,
    kEPERM,
    kEREMOTEIO,
    kEINVAL,
    kNoError = 0,             // NO_ERROR
};

extern const std::map<PeripheralManagerErrors, std::string> error_text;
class PeripheralManagerException {
public:
    PeripheralManagerException();
    explicit PeripheralManagerException(const std::string& error_message, const PeripheralManagerErrors error_code = PeripheralManagerErrors::kUnknownError);
    virtual ~PeripheralManagerException();

    virtual const char* what() const throw();
    PeripheralManagerErrors getErrorCode() { return error_code;}

private:
    PeripheralManagerErrors error_code;
    std::string error_message;
    std::string user_error_message;
    void setErrorMsg();
};
