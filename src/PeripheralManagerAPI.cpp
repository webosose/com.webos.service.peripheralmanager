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

PeripheralManagerService::~PeripheralManagerService() {}

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
            std::vector<std::string> gpios;
            peripheral_manager_client->ListGpio(&gpios);
            pbnjson::JValue users_payload = pbnjson::JArray();
            for (std::string user : gpios) {
                users_payload << user;
            }
            for(int ii = 0; ii < gpios.size(); ii++)
            {
                AppLogError() << "mylog...." << gpios[ii] << "\n";
            }
            subscription = parsed["subscribe"].asBool();
            response_json =
                    pbnjson::JObject{{"returnValue", true},
                {"subscribed", subscription},
                {"gpioList", users_payload}};

            request.respond(response_json.stringify().c_str());

        }
    }
    return true;
}

bool PeripheralManagerService::OpenGpio(LSMessage &ls_message) {
    bool ret = false;
    LS::Message request(&ls_message);
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
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ "property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        else {
            try {
                ret = peripheral_manager_client->OpenGpio(pin);
                response_json =
                        pbnjson::JObject{{"returnValue", ret}};
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


bool PeripheralManagerService::ReleaseGpio(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    bool ret = false;
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
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ "property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        else {
            try {
                ret = peripheral_manager_client->ReleaseGpio(pin);

                response_json =
                        pbnjson::JObject{{"returnValue", ret}};
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

bool PeripheralManagerService::SetGpioDirection(LSMessage &ls_message) {
    AppLogError() << __func__ << ":" << __LINE__  ;
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
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ "property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }
        else {
            if(dir == "in") direction = 0;
            if(dir == "outHigh") direction = 1;
            if(dir == "outLow") direction = 2;
            try {
                ret = peripheral_manager_client->SetGpioDirection(pin, direction);
                response_json =
                        pbnjson::JObject{{"returnValue", true}};
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
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ "property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }else {
            if(val == "high") value = true;
            else if(val == "low") value = false;
            try {
                ret = peripheral_manager_client->SetGpioValue(pin, value);
                response_json =
                        pbnjson::JObject{{"returnValue", true}};
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
                response_json = pbnjson::JObject{{"returnValue", false},{"errorText", temp+ "property not allowed"}};
            }
        }
        if(extra_property == true)
        {
            request.respond(response_json.stringify().c_str());
            return true;
        }else {
            try {
                ret = peripheral_manager_client->GetGpioValue(pin, &value);
                AppLogError() << __func__ << ":" << __LINE__ << " value = " << value ;
                std::string val = value ? "high" : "low";
                response_json =
                        pbnjson::JObject{{"returnValue", true},
                    {"subscribed", subscription},
                    {"value", val}};
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
bool PeripheralManagerService::GetGpioPollingFd(LSMessage &ls_message) {
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
        subscription = parsed["subscribe"].asBool();
        response_json =
                pbnjson::JObject{{"returnValue", true}};

        request.respond(response_json.stringify().c_str());

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
        subscription = parsed["subscribe"].asBool();
        std::vector<std::string> devices;
        try {
            peripheral_manager_client->ListUartDevices(&devices);
            pbnjson::JValue device_list = pbnjson::JArray();
            for (std::string device : devices) {
                device_list << device;
            }
            response_json =
                    pbnjson::JObject{{"returnValue", true},
                {"subscribed", subscription},
                {"uartList", device_list}};
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
        subscription = parsed["subscribe"].asBool();
        const std::string deviceName = parsed["deviceName"].asString();
        try {
            peripheral_manager_client->OpenUartDevice(deviceName);
            response_json =
                    pbnjson::JObject{{"returnValue", true}};
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
    } else {
        subscription = parsed["subscribe"].asBool();
        const std::string deviceName = parsed["deviceName"].asString();
        try {
            peripheral_manager_client->ReleaseUartDevice(deviceName);

            response_json =
                    pbnjson::JObject{{"returnValue", true}};
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
    } else {
        subscription = parsed["subscribe"].asBool();
        const std::string deviceName = parsed["deviceName"].asString();
        int32_t baudrate = parsed["baudrate"].asNumber<int>();
        try {
            ret = peripheral_manager_client->SetUartDeviceBaudrate(deviceName, baudrate);
            response_json =
                    pbnjson::JObject{{"returnValue", ret}};
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
    } else {
        subscription = parsed["subscribe"].asBool();
        const std::string deviceName = parsed["deviceName"].asString();
        const std::vector<uint8_t> data {1};
        int bytes_written = 0;
        try {
            ret = peripheral_manager_client->UartDeviceWrite(deviceName, data, &bytes_written);
            response_json =
                    pbnjson::JObject{{"returnValue", ret},
                {"bytes_written", bytes_written}};
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
    } else {
        subscription = parsed["subscribe"].asBool();
        const std::string deviceName = parsed["deviceName"].asString();
        int size = parsed["size"].asNumber<int>();
        std::vector<uint8_t> data;
        data.resize(9);
        int bytes_read = 0;
        try {
            ret = peripheral_manager_client->UartDeviceRead(deviceName, &data, data.size(), &bytes_read);
            pbnjson::JValue data_array = pbnjson::JArray();
//            bytes_read = (data.size() > bytes_read) ? bytes_read :  data.size();
            for(int i = 0; i < bytes_read; i++) {
                data_array << data[i];

            }
            response_json =
                    pbnjson::JObject{{"returnValue", ret},
                {"bytes_read", bytes_read},
                {"data", data_array}};
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
    return true;
}
bool PeripheralManagerService::getBaudrate(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    bool subscription = false;
    pbnjson::JValue response_json;
    pbnjson::JValue baudrate_array = pbnjson::JArray();
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        subscription = parsed["subscribe"].asBool();
        response_json =
                pbnjson::JObject{{"returnValue", true},
            {"baudrate", baudrate_array},
            {"subscribed", subscription}};

        request.respond(response_json.stringify().c_str());

    }
    return true;
}
bool PeripheralManagerService::getDirection(LSMessage &ls_message) {
    LS::Message request(&ls_message);
    bool subscription = false;
    pbnjson::JValue response_json;
    pbnjson::JValue direction_array = pbnjson::JArray();
    pbnjson::JValue parsed = pbnjson::JDomParser::fromString(request.getPayload());
    if (parsed.isError()) {
        response_json =
                pbnjson::JObject{{"returnValue", false}, {"errorText", "Failed to parse params"}, {"errorCode", 1}};
        request.respond(response_json.stringify().c_str());
        return false;
    } else {
        subscription = parsed["subscribe"].asBool();
        response_json =
                pbnjson::JObject{{"returnValue", true},
            {"direction", direction_array},
            {"subscribed", subscription}};

        request.respond(response_json.stringify().c_str());

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
    static const LSMethod root_table[] = {
            {"getPollingFd", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::GetGpioPollingFd>,
             static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
             {nullptr, nullptr}};
    luna_handle->registerCategory("/", root_table, nullptr, nullptr);
    luna_handle->setCategoryData("/", this);
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
	     {"getPollingFd", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::GetGpioPollingFd>,
             static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},
	     {"getBaudrate", &LS::Handle::methodWraper<PeripheralManagerService, &PeripheralManagerService::getBaudrate>,
             static_cast<LSMethodFlags>(LUNA_METHOD_FLAG_VALIDATE_IN)},

{nullptr, nullptr}};
    luna_handle->registerCategory("/uart", uart, nullptr, nullptr);
    luna_handle->setCategoryData("/uart", this);
}
