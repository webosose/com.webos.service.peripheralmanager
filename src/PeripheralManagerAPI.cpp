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

#include "Logger.h"
#include <unistd.h>
#include <stdio.h>
#include <regex>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "PeripheralManagerAPI.h"
#include "PeripheralManagerException.h"

PeripheralManagerService::PeripheralManagerService(LS::Handle *ls_handle)
: main_loop_ptr(g_main_loop_new(nullptr, false), g_main_loop_unref),
  luna_handle(ls_handle)
{
    peripheral_manager_client = new PeripheralManagerClient ;
    luna_handle->attachToLoop(main_loop_ptr.get());
}

PeripheralManagerService::~PeripheralManagerService() {
    delete peripheral_manager_client;
}

void PeripheralManagerService::run() {
    // attach to main loop and start running
    g_main_loop_run(main_loop_ptr.get());
}

void PeripheralManagerService::stop() { g_main_loop_quit(main_loop_ptr.get()); }

bool PeripheralManagerService::receiveCallback(LSHandle *sh, LSMessage *pMessage, void *pCtx)
{
    return true;
}


bool PeripheralManagerService::ListGpio(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    bool subscription = false;
    pbnjson::JValue response_json;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "subscribe")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+" property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        else {
            try {
            std::vector<DevicesPinInfo> gpios;
            peripheral_manager_client->ListGpio(gpios);
            pbnjson::JValue gpioList = pbnjson::JArray();
            for (auto gpio : gpios) {
                pbnjson::JValue gpioJson  = pbnjson::JObject{{"pin", gpio.name},{"status", gpio.status}};
                gpioList << gpioJson;
            }
            subscription = parsed["subscribe"].asBool();
            response_json =
                    pbnjson::JObject{
                {"returnValue", true},
                {"subscribed", subscription},
                {"gpioList", gpioList}};
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
    }
    return true;
}

bool PeripheralManagerService::OpenGpio(LSMessage &ls_message) {
    bool ret = false;
    LS::Message request(&ls_message);
    pbnjson::JValue response_json;
    std::string pin ;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string pin = parsed["pin"].asString();
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "pin")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("pin"))
        {
            try {
                ret = peripheral_manager_client->OpenGpio(pin);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "pin is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}


bool PeripheralManagerService::ReleaseGpio(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    bool ret = false;
    std::string pin;
    pbnjson::JValue response_json;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string pin = parsed["pin"].asString();
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "pin")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("pin"))
        {
            try {
                ret = peripheral_manager_client->ReleaseGpio(pin);

                response_json =
                        pbnjson::JObject{
                    {"returnValue", true}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "pin is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}

bool PeripheralManagerService::SetGpioDirection(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    bool ret = false;
    int direction =0;
    pbnjson::JValue response_json;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string pin = parsed["pin"].asString();
        std::string dir = parsed["direction"].asString();
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "pin" || ii.first.asString() == "direction" )
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if(parsed.hasKey("pin") &&  parsed.hasKey("direction"))
        {
            if(dir == "in") direction = 0;
            else if(dir == "outHigh") direction = 1;
            else if(dir == "outLow") direction = 2;
            else {
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", dir+ " value not allowed"}};
                request.respond(response_json.stringify().c_str());
                return true;
            }
            try {
                ret = peripheral_manager_client->SetGpioDirection(pin, direction);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "pin/direction  is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}
bool PeripheralManagerService::SetGpioValue(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    bool ret = false;
    bool value = false;
    pbnjson::JValue response_json;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string pin = parsed["pin"].asString();
        std::string val = parsed["value"].asString();
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "pin" || ii.first.asString() == "value" )
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("pin") && parsed.hasKey("value"))
        {
            if(val == "high") value = true;
            else if(val == "low") value = false;
            else {
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", val+ " value not allowed"}};
                request.respond(response_json.stringify().c_str());
                return true;
            }
            try {
                ret = peripheral_manager_client->SetGpioValue(pin, value);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "pin/value is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}

bool PeripheralManagerService::GetGpioValue(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    bool subscription = false;
    bool ret = false;
    bool value = false;
    pbnjson::JValue response_json;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string pin = parsed["pin"].asString();
        subscription = parsed["subscribe"].asBool();
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "pin")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("pin"))
        {
            try {
                ret = peripheral_manager_client->GetGpioValue(pin, &value);
                std::string val = value ? "high" : "low";
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true},
                    {"subscribed", subscription},
                    {"value", val}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else{
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "pin is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}
bool PeripheralManagerService::GetGpioPollingFd(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    int fd = 0 ;
    int ret;
    pbnjson::JValue response_json;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    }
    else {
        std::string pin = parsed["id"].asString();
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "id")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("id"))
        {
            try {
                ret = peripheral_manager_client->GetGpioPollingFd(pin, &fd);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true},
                    {"fd", fd}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "id is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}
bool PeripheralManagerService::ListUartDevices(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    bool subscription = false;
    pbnjson::JValue response_json;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "subscribe")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+" property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }else {
            subscription = parsed["subscribe"].asBool();
            std::vector<DevicesPinInfo> devices;
            try {
                peripheral_manager_client->ListUartDevices(devices);
                pbnjson::JValue device_list = pbnjson::JArray();
                for (auto device : devices) {
                    pbnjson::JValue uartJson  = pbnjson::JObject{{"name", device.name},{"status", device.status}};
                    device_list << uartJson;
                }
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true},
                    {"subscribed", subscription},
                    {"uartList", device_list}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
    }
    return true;
}

bool PeripheralManagerService::OpenUartDevice(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    bool subscription = false;
    pbnjson::JValue response_json;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "interfaceId")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("interfaceId"))
        {
            const std::string interfaceId = parsed["interfaceId"].asString();
            try {
                peripheral_manager_client->OpenUartDevice(interfaceId);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "interfaceId is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}


bool PeripheralManagerService::ReleaseUartDevice(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    bool subscription = false;
    pbnjson::JValue response_json;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    }else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "interfaceId")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("interfaceId"))
        {
            const std::string interfaceId = parsed["interfaceId"].asString();
            try {
                peripheral_manager_client->ReleaseUartDevice(interfaceId);

                response_json =
                        pbnjson::JObject{
                    {"returnValue", true}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "interfaceId is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}

bool PeripheralManagerService::SetUartDeviceBaudrate(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    bool subscription = false;
    pbnjson::JValue response_json;
    bool ret = false;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    }else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "baudrate" || ii.first.asString() == "interfaceId")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("interfaceId") && parsed.hasKey("baudrate"))
        {
            const std::string interfaceId = parsed["interfaceId"].asString();
            int32_t baudrate = parsed["baudrate"].asNumber<int>();
            try {
                ret = peripheral_manager_client->SetUartDeviceBaudrate(interfaceId, baudrate);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "interfaceId/baudrate is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}

bool PeripheralManagerService::UartDeviceWrite(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    bool subscription = false;
    pbnjson::JValue response_json;
    bool ret = false;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    }else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "data" || ii.first.asString() == "interfaceId" || ii.first.asString() == "dataType" || ii.first.asString() == "size")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("interfaceId") && parsed.hasKey("data") && parsed.hasKey("dataType") && parsed.hasKey("size"))
        {
            const std::string interfaceId = parsed["interfaceId"].asString();

            pbnjson::JValue jsonData = parsed["data"];
            std::string dataType = parsed["dataType"].asString();
            int size = parsed["size"].asNumber<int>();
            int jsonDataSize = jsonData.arraySize();
            std::vector<uint8_t> data;
            for (int i = 0; i < jsonDataSize; i++) {
                uint8_t dataTemp = jsonData[i].asNumber<int>();
                data.push_back(dataTemp);
            }

            int bytes_written = 0;

            try {
                ret = peripheral_manager_client->UartDeviceWrite(interfaceId, data, &bytes_written);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true},
                    {"size", bytes_written}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "interfaceId/data/dataType/size is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}

bool PeripheralManagerService::UartDeviceRead(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    bool subscription = false;
    pbnjson::JValue response_json;
    bool ret = false;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    }else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "size" || ii.first.asString() == "interfaceId")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("interfaceId") && parsed.hasKey("size"))
        {
            const std::string interfaceId = parsed["interfaceId"].asString();
            int size = parsed["size"].asNumber<int>();
            std::vector<uint8_t> data;
            data.resize(size);
            std::string datatype;
            int bytes_read = 0;
            try {
                ret = peripheral_manager_client->UartDeviceRead(interfaceId, &data, data.size(), &bytes_read);
                pbnjson::JValue data_array = pbnjson::JArray();
                for(int i = 0; i < bytes_read; i++) {
                    data_array << data[i];

                }
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true},
                    {"bytes_read", bytes_read},
                    {"data", data_array},
                    {"dataType", datatype}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "interfaceId/size is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}
bool PeripheralManagerService::getBaudrate(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    uint32_t baudrate = 0;
    int ret;
    pbnjson::JValue response_json;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    }else {
        std::string interfaceId = parsed["interfaceId"].asString();
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "interfaceId" )
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("interfaceId"))
        {
            try {
            ret = peripheral_manager_client->getBaudrate(interfaceId, &baudrate);
            response_json =
                    pbnjson::JObject{
                {"returnValue", true},
                {"baudrate", (int)baudrate}
            };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else{
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "interfaceId is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}
bool PeripheralManagerService::getDirection(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    bool subscription = false;
    bool ret;
    std::string direction;
    pbnjson::JValue response_json;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string pin = parsed["pin"].asString();
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "pin")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("pin"))
        {
            try {
                ret = peripheral_manager_client->getDirection(pin, direction);

                response_json =
                        pbnjson::JObject{
                    {"returnValue", true},
                    {"direction", direction}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "pin is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}

bool PeripheralManagerService::GetuartPollingFd(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    int fd  ;
    pbnjson::JValue response_json;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    }
    else {
        std::string id = parsed["id"].asString();
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "id")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("id"))
        {
            try {
                peripheral_manager_client->GetuartPollingFd(id, &fd);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true},
                    {"fd", fd}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "id is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}

bool PeripheralManagerService::ListI2cBuses(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    pbnjson::JValue response_json;
    bool ret = false;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "subscribe")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+" property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        else{
            try {
                std::vector<std::string> buses;
                peripheral_manager_client->ListI2cBuses(&buses);
                pbnjson::JValue device_list = pbnjson::JArray();
                for (std::string device : buses) {
                    device_list << device;
                }
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true},
                    {"i2cBusList", device_list}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
    }
    return true;
}

bool PeripheralManagerService::OpenI2cDevice(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    pbnjson::JValue response_json;
    bool ret = false;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "name" || ii.first.asString() == "address")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+" property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if(parsed.hasKey("name") && parsed.hasKey("address"))
        {
            try {
                std::string name = parsed["name"].asString();
                int32_t address = parsed["address"].asNumber<int>();
                peripheral_manager_client->OpenI2cDevice(name, address);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "name/address is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}

bool PeripheralManagerService::ReleaseI2cDevice(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    pbnjson::JValue response_json;
    bool ret = false;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "name" || ii.first.asString() == "address")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+" property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if(parsed.hasKey("name") && parsed.hasKey("address"))
        {
            try {
                std::string name = parsed["name"].asString();
                int32_t address = parsed["address"].asNumber<int>();
                peripheral_manager_client->ReleaseI2cDevice(name, address);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "name/address is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }

    }
    return true;
}
bool PeripheralManagerService::I2cRead(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    pbnjson::JValue response_json;
    bool ret = false;
    pbnjson::JValue data_array = pbnjson::JArray();
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "name" || ii.first.asString() == "address" || ii.first.asString() == "size" )
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("name") && parsed.hasKey("address"))
        {
            try {
                const std::string name = parsed["name"].asString();
                int address = parsed["address"].asNumber<int>();
                std::vector<uint8_t> data;
                int size = parsed["size"].asNumber<int>();
                size =  size ? size : 8;
                data.resize(size);
                int bytes_read = 0;
                peripheral_manager_client->I2cRead(name, address, &data, size, &bytes_read);
                for(int i = 0; i < bytes_read; i++) {
                    data_array << data[i];
                }
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true},
                    {"data", data_array},
                    {"size", bytes_read}

                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "name/address  is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }

    }
    return true;
}

bool PeripheralManagerService::I2cReadRegByte(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    pbnjson::JValue response_json;
    bool ret = false;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "name" || ii.first.asString() == "address" || ii.first.asString() == "reg" )
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("name") && parsed.hasKey("address") && parsed.hasKey("reg"))
        {
            try {
                const std::string name = parsed["name"].asString();
                int32_t address =  parsed["address"].asNumber<int>();
                int32_t reg =  parsed["reg"].asNumber<int>();
                int32_t val =  0;
                peripheral_manager_client->I2cReadRegByte(name, address, reg, &val);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true},
                    {"data", val}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "name/address/reg  is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}

bool PeripheralManagerService::I2cReadRegWord(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    pbnjson::JValue response_json;
    bool ret = false;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "name" || ii.first.asString() == "address" || ii.first.asString() == "reg" )
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("name") && parsed.hasKey("address") && parsed.hasKey("reg"))
        {
            try {
                const std::string name = parsed["name"].asString();
                int32_t address = parsed["address"].asNumber<int>();
                int32_t reg = parsed["reg"].asNumber<int>();
                int32_t val = 0;
                peripheral_manager_client->I2cReadRegWord(name, address, reg, &val);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true},
                    {"data", val}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else{
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "name/address/reg  is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}

bool PeripheralManagerService::I2cReadRegBuffer(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    pbnjson::JValue response_json;
    pbnjson::JValue data_array = pbnjson::JArray();
    bool ret = false;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "name" || ii.first.asString() == "address" || ii.first.asString() == "reg" || ii.first.asString() == "size" )
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("name") && parsed.hasKey("address") && parsed.hasKey("reg"))
        {
            try {
                const std::string name = parsed["name"].asString();
                int32_t address = parsed["address"].asNumber<int>();
                int32_t reg = parsed["reg"].asNumber<int>();
                std::vector<uint8_t> data;
                int32_t size = parsed["size"].asNumber<int>();
                size = size ? size : 8;
                int32_t bytes_read = 0;
                peripheral_manager_client->I2cReadRegBuffer(name, address, reg, &data, size, &bytes_read);
                for (int i = 0; i < bytes_read ; i++)
                {
                    data_array <<  data[i];
                }
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true},
                    {"size", bytes_read},
                    {"data", data_array}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "name/address/reg  is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }

    }
    return true;
}
bool PeripheralManagerService::I2cWrite(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    pbnjson::JValue response_json;
    bool ret = false;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "name" || ii.first.asString() == "address" || ii.first.asString() == "data" )
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("name") && parsed.hasKey("address") && parsed.hasKey("data"))
        {
            try {
                const std::string name = parsed["name"].asString();
                int32_t address  = parsed["address"].asNumber<int>();
                pbnjson::JValue jsonData = parsed["data"];
                int jsonDataSize = jsonData.arraySize();
                std::vector<uint8_t> data;
                for (int i = 0; i < jsonDataSize; i++) {
                    uint8_t dataTemp = jsonData[i].asNumber<int>();
                    data.push_back(dataTemp);
                }

                int bytes_written = 0;
                peripheral_manager_client->I2cWrite( name, address, data, &bytes_written);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true},
                    {"size", bytes_written}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "name/address/data  is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }

    }
    return true;
}

bool PeripheralManagerService::I2cWriteRegByte(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    pbnjson::JValue response_json;
    bool ret = false;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "name" || ii.first.asString() == "address" || ii.first.asString() == "reg" || ii.first.asString() == "data" )
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("name") && parsed.hasKey("address") && parsed.hasKey("data") && parsed.hasKey("reg"))
        {
            try {
                const std::string name = parsed["name"].asString();
                int32_t address = parsed["address"].asNumber<int>();
                int32_t reg = parsed["reg"].asNumber<int>();
                int8_t data = parsed["data"].asNumber<int>();
                peripheral_manager_client->I2cWriteRegByte( name, address, reg, data);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "name/address/reg/data  is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }

    }
    return true;
}

bool PeripheralManagerService::I2cWriteRegWord(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    pbnjson::JValue response_json;
    bool ret = false;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "name" || ii.first.asString() == "address" || ii.first.asString() == "reg" || ii.first.asString() == "data" )
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("name") && parsed.hasKey("address") && parsed.hasKey("reg") && parsed.hasKey("data"))
        {
            try {
                const std::string name = parsed["name"].asString();
                int32_t address = parsed["address"].asNumber<int>();
                int32_t reg = parsed["reg"].asNumber<int>();
                int32_t data = parsed["data"].asNumber<int>();
                peripheral_manager_client->I2cWriteRegWord(name, address, reg, data);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else{
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "name/address/data/reg  is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}


bool PeripheralManagerService::I2cWriteRegBuffer(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    pbnjson::JValue response_json;
    bool ret = false;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());

        return false;
    } else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "name" || ii.first.asString() == "address" || ii.first.asString() == "reg"  || ii.first.asString() == "data" )
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("name") && parsed.hasKey("address") && parsed.hasKey("reg") && parsed.hasKey("data"))
        {
            try {
                int32_t bytes_written = 0;
                const std::string name = parsed["name"].asString();
                int32_t address = parsed["address"].asNumber<int>();
                int32_t reg = parsed["reg"].asNumber<int>();
                pbnjson::JValue jsonData = parsed["data"];
                int jsonDataSize = jsonData.arraySize();
                std::vector<uint8_t> data;
                for (int i = 0; i < jsonDataSize; i++) {
                    uint8_t dataTemp = jsonData[i].asNumber<int>();
                    data.push_back(dataTemp);
                }

                peripheral_manager_client->I2cWriteRegBuffer(name, address, reg, data, &bytes_written);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true},
                    {"size" , bytes_written}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "name/address/data/reg  is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}

bool PeripheralManagerService::ListSpiBuses(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    bool subscription = false;
    pbnjson::JValue response_json;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "subscribe")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+" property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }else {
            subscription = parsed["subscribe"].asBool();
            std::vector<std::string> buses;
            try {
                peripheral_manager_client->ListSpiBuses(&buses);
                pbnjson::JValue bus_list = pbnjson::JArray();
                for (std::string device : buses) {
                    bus_list << device;
                }
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true},
                    {"list", bus_list}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
    }
    return true;
}
bool PeripheralManagerService::OpenSpiDevice(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    pbnjson::JValue response_json;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "name")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("name"))
        {
            const std::string name = parsed["name"].asString();
            try {
                peripheral_manager_client->OpenSpiDevice(name);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "name is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}
bool PeripheralManagerService::ReleaseSpiDevice(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    pbnjson::JValue response_json;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    }else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "name")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("name"))
        {
            const std::string name = parsed["name"].asString();
            try {
                peripheral_manager_client->ReleaseSpiDevice(name);

                response_json =
                        pbnjson::JObject{
                    {"returnValue", true}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "name is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}
bool PeripheralManagerService::SpiDeviceSetMode(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    pbnjson::JValue response_json;
    bool ret = false;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    }else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "mode" || ii.first.asString() == "name")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("name") && parsed.hasKey("mode"))
        {
            const std::string name = parsed["name"].asString();
            int mode = parsed["mode"].asNumber<int>();
            try {
                peripheral_manager_client->SpiDeviceSetMode(name, mode);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "name/mode is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}
bool PeripheralManagerService::SpiDeviceSetFrequency(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    pbnjson::JValue response_json;
    bool ret = false;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    }else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "frequency" || ii.first.asString() == "name")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("name") && parsed.hasKey("frequency"))
        {
            const std::string name = parsed["name"].asString();
            long int frequency = parsed["frequency"].asNumber<int>();
            try {
                peripheral_manager_client->SpiDeviceSetFrequency(name, frequency);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "name/frequency is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}
bool PeripheralManagerService::SpiDeviceSetBitJustification(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    pbnjson::JValue response_json;
    bool ret = false;
    bool lsb_first;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    }else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "name" || ii.first.asString() == "lsb_first")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("name") && parsed.hasKey("lsb_first"))
        {
            const std::string name = parsed["name"].asString();
            bool lsb_first = parsed["lsb_first"].asBool();
            try {
                peripheral_manager_client->SpiDeviceSetBitJustification(name, lsb_first);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "name/lsb_first is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}
bool PeripheralManagerService::SpiDeviceSetBitsPerWord(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    pbnjson::JValue response_json;
    bool ret = false;
    int nbits;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    }else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "nbits" || ii.first.asString() == "name")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("name") && parsed.hasKey("nbits"))
        {
            const std::string name = parsed["name"].asString();
            int nbits = parsed["nbits"].asNumber<int>();
            try {
                peripheral_manager_client->SpiDeviceSetBitsPerWord(name, nbits);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "name/nbits is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}
bool PeripheralManagerService::SpiDeviceTransfer(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    pbnjson::JValue response_json;
    bool ret = false;
    int size;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    }else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "size" || ii.first.asString() == "name" || ii.first.asString() == "data" )
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("name") && parsed.hasKey("size"))
        {
            const std::string name = parsed["name"].asString();
            int size = parsed["size"].asNumber<int>();
            pbnjson::JValue jsonData = parsed["data"];
            int jsonDataSize = jsonData.arraySize();
            std::vector<uint8_t> data;
            for (int i = 0; i < jsonDataSize; i++) {
                uint8_t dataTemp = jsonData[i].asNumber<int>();
                data.push_back(dataTemp);
            }

            std::vector<uint8_t> recv_data;
            recv_data.resize(size);
            try {
                peripheral_manager_client->SpiDeviceTransfer(name,data,&recv_data,size);

                int tx_size = data.size();
                int rx_size = recv_data.size();
                pbnjson::JValue data_array = pbnjson::JArray();
                for (int i = 0; i < recv_data.size() ; i++)
                {
                    data_array <<  data[i];
                }

                response_json =
                        pbnjson::JObject{
                    {"returnValue" , true},
                    {"tx_size" , tx_size},
                    {"rx_size" , rx_size},
                    {"rx_data" , data_array}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "name/size is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}

bool PeripheralManagerService::SpiDeviceWriteByte(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    bool subscription = false;
    pbnjson::JValue response_json;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "name" || ii.first.asString() == "data")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+" property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("name") && parsed.hasKey("data"))
        {
            std::string name = parsed["name"].asString();
            int8_t data = parsed["data"].asNumber<int>();
            try {
                peripheral_manager_client->SpiDeviceWriteByte(name,data);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "name/data  is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }

    }
    return true;
}
bool PeripheralManagerService::SpiDeviceWriteBuffer(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    pbnjson::JValue response_json;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "name" || ii.first.asString() == "data")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+" property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("name") && parsed.hasKey("data"))
        {
            std::string name = parsed["name"].asString();

            pbnjson::JValue jsonData = parsed["data"];
            int jsonDataSize = jsonData.arraySize();
            std::vector<uint8_t> data;
            for (int i = 0; i < jsonDataSize; i++) {
                uint8_t dataTemp = jsonData[i].asNumber<int>();
                data.push_back(dataTemp);
            }

            try {
                peripheral_manager_client->SpiDeviceWriteBuffer(name,data);
                int size  = data.size();
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true},
                    {"size", size}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "name/buffer  is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}
bool PeripheralManagerService::SpiDeviceSetDelay(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    bool subscription = false;
    pbnjson::JValue response_json;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "name" || ii.first.asString() == "delay_usecs")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+" property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("name") && parsed.hasKey("delay_usecs"))
        {
            std::string name = parsed["name"].asString();
            int delay_usecs = parsed["delay_usecs"].asNumber<int>();
            try {
                peripheral_manager_client->SpiDeviceSetDelay(name,delay_usecs);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());
        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "name/delay_usecs  is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}
bool PeripheralManagerService::Geti2cPollingFd(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    int fd  ;
    pbnjson::JValue response_json;
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    }
    else {
        std::string temp;
        bool extra_property = false;
        for(auto ii:parsed)
        {
            if(ii.first.asString() == "name" || ii.first.asString() == "address")
            {
                continue;
            }
            else
            {
                extra_property = true;
                temp = ii.first.asString();
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ " property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        if (parsed.hasKey("name") && parsed.hasKey("address"))
        {
            try {
                std::string name = parsed["name"].asString();
                int32_t address = parsed["address"].asNumber<int>();
                peripheral_manager_client->Geti2cPollingFd(name, address, &fd);
                response_json =
                        pbnjson::JObject{
                    {"returnValue", true},
                    {"fd", fd}
                };
            }
            catch (LS::Error &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", err.what()}};
            } catch (PeripheralManagerException &err) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorCode", err.getErrorCode()}, {"errorText", error_text.at(err.getErrorCode())}};
            } catch (...) {
                response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "Unknown Error"}};
            }
            request.respond(response_json.stringify().c_str());

        }
        else {
            response_json = pbnjson::JObject{{"returnValue", false}, {"errorText", "name/address is missing"}};
            request.respond(response_json.stringify().c_str());
            return true;
        }
    }
    return true;
}

// Private Methods
void PeripheralManagerService::registerMethodsToLsHub() {
    static const LSMethod gpio[] = {
        {"list", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::ListGpio>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"open", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::OpenGpio>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"close", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::ReleaseGpio>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"setDirection", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::SetGpioDirection>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"setValue", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::SetGpioValue>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"getValue", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::GetGpioValue>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"getPollingFd", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::GetGpioPollingFd>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"getDirection", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::getDirection>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {nullptr, nullptr}};

    luna_handle->registerCategory("/gpio", gpio, nullptr, nullptr);
    luna_handle->setCategoryData("/gpio", this);

    static const LSMethod uart[] = {
        {"list", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::ListUartDevices>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"open", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::OpenUartDevice>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"close", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::ReleaseUartDevice>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"setBaudrate", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::SetUartDeviceBaudrate>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"write", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::UartDeviceWrite>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"read", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::UartDeviceRead>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"getPollingFd", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::GetuartPollingFd>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"getBaudrate", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::getBaudrate>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {nullptr, nullptr}};

    luna_handle->registerCategory("/uart", uart, nullptr, nullptr);
    luna_handle->setCategoryData("/uart", this);

    static const LSMethod i2c[] = {
        {"list", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::ListI2cBuses>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"open", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::OpenI2cDevice>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"close", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::ReleaseI2cDevice>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"read", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::I2cRead>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"readRegByte", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::I2cReadRegByte>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"readRegWord", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::I2cReadRegWord>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"readRegBuffer", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::I2cReadRegBuffer>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"write", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::I2cWrite>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"writeRegByte", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::I2cWriteRegByte>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"writeRegWord", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::I2cWriteRegWord>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"writeRegBuffer", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::I2cWriteRegBuffer>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"getPollingFd", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::Geti2cPollingFd>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {nullptr, nullptr}};

    luna_handle->registerCategory("/i2c", i2c, nullptr, nullptr);
    luna_handle->setCategoryData("/i2c", this);

        static const LSMethod spi[] = {
        {"list", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::ListSpiBuses>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"open", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::OpenSpiDevice>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"close", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::ReleaseSpiDevice>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"setMode", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::SpiDeviceSetMode>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"setFrequency", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::SpiDeviceSetFrequency>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"setBitJustification", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::SpiDeviceSetBitJustification>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"setBitsPerWord", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::SpiDeviceSetBitsPerWord>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"transfer", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::SpiDeviceTransfer>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"writeByte", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::SpiDeviceWriteByte>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"writeBuffer", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::SpiDeviceWriteBuffer>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {"setDelay", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::SpiDeviceSetDelay>,
        static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
        {nullptr, nullptr}};

    luna_handle->registerCategory("/spi", spi, nullptr, nullptr);
    luna_handle->setCategoryData("/spi", this);
}
