/*
 * This file is part of Open Airbus Cockpit
 * Copyright (C) 2012 Alvaro Polo
 *
 * Open Airbus Cockpit is free software: you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Open Airbus Cockpit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Open Airbus Cockpit.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Boost/format.hpp>

#include "logging.h"
#include "simconn.h"

namespace oac {

namespace {

void CALLBACK DispatchMessage(
      SIMCONNECT_RECV* pData, DWORD cbData, void* pContext)
{
   try 
   {
      auto receiver = static_cast<SimConnectClient*>(pContext);
      receiver->onMessage(pData, cbData);
   } catch (std::exception& ex) {
      Log(WARN, ex.what());
   }
}

}; // anonymous namespace

SimConnectClient::DataDefinition&
SimConnectClient::DataDefinition::add(
      const DataName& name, 
      const DataUnits& units, 
      SIMCONNECT_DATATYPE data_type)
throw (InvalidInputException)
{ 
   auto result = SimConnect_AddToDataDefinition(
         _handle, _id, name.c_str(), units.c_str(), data_type); 
   if (result != S_OK)
      throw InvalidInputException(boost::format(
            "cannot add data definition for variable %s in %s units")
            % name % units);
   return *this;
}

void
SimConnectClient::DataPullRequest::submit()
throw (InvalidInputException)
{
   auto result = SimConnect_RequestDataOnSimObject(
         _handle, _id, _data_def, _object, _period, 
         _flags, _origin, _interval, _limit);
   if (result != S_OK)
      throw InvalidInputException(boost::format(
            "cannot pull-request data definition %d on user object") 
            % _data_def);
}

void
SimConnectClient::DataPushRequest::submit(void* data)
{
   auto result = SimConnect_SetDataOnSimObject(
         _handle, _data_def, _object, _flags, _count, _element_size, data);
   if (result != S_OK)
      throw InvalidInputException(boost::format(
            "cannot push request data definition %d on user object") 
            % _data_def);
}

const SimConnectClient::EventName SimConnectClient::SYSTEM_EVENT_1SEC(
      "1sec");
const SimConnectClient::EventName SimConnectClient::SYSTEM_EVENT_4SEC(
      "4sec");
const SimConnectClient::EventName SimConnectClient::SYSTEM_EVENT_6HZ(
      "6hz");
const SimConnectClient::EventName SimConnectClient::SYSTEM_EVENT_FRAME(
      "frame");

SimConnectClient::SimConnectClient(const std::string& name)
throw (ConnectionException) :
   _name(name)
{
   if (SimConnect_Open(&_handle, name.c_str(), NULL, 0, 0, 0) != S_OK)
      throw ConnectionException(
            "cannot connect to SimConnect: connection error");
   if (SimConnect_CallDispatch(_handle, DispatchMessage, this) != S_OK)
      throw ConnectionException(
            "cannot connect to SimConnect: callback register failed");      
}

SimConnectClient::~SimConnectClient()
{
   if (SimConnect_Close(_handle) != S_OK)
      Log(WARN, str(boost::format(
            "something failed while disconnecting %s from SimConnect")
            % _name));
}

void
SimConnectClient::dispatchMessage()
{ SimConnect_CallDispatch(_handle, DispatchMessage, this); }

void
SimConnectClient::onMessage(SIMCONNECT_RECV* msg, DWORD msg_len)
{
   try 
   {
      auto msg_id = msg->dwID;
      auto callback = _msg_receivers[msg_id];
      if (callback)
         callback->sendMessage(msg);
      else
         Log(WARN, str(boost::format(
               "cannot deliver message with ID %d: no callback registered") %
               msg_id));
   }
   catch (const std::exception& ex)
   {
      Log(WARN, str(boost::format("cannot deliver SimConnect message: %s") %
            ex.what()));
   }
}

void 
SimConnectClient::registerOnNullCallback(
      const std::function<void(const SIMCONNECT_RECV& msg)>& callback)
{
   this->registerCallback<SIMCONNECT_RECV>(callback, SIMCONNECT_RECV_ID_NULL);
}

void 
SimConnectClient::registerOnExceptionCallback(
         const std::function<void(
               const SIMCONNECT_RECV_EXCEPTION& msg)>& callback)
{
   this->registerCallback<SIMCONNECT_RECV_EXCEPTION>(
         callback, SIMCONNECT_RECV_ID_EXCEPTION);
}

void 
SimConnectClient::registerOnOpenCallback(
      const std::function<void(const SIMCONNECT_RECV_OPEN& msg)>& callback)
{
   this->registerCallback<SIMCONNECT_RECV_OPEN>(
      callback, SIMCONNECT_RECV_ID_OPEN);
}

void 
SimConnectClient::registerOnQuitCallback(
      const std::function<void(const SIMCONNECT_RECV& msg)>& callback)
{
   this->registerCallback<SIMCONNECT_RECV>(
      callback, SIMCONNECT_RECV_ID_QUIT);
}

void 
SimConnectClient::registerOnEventCallback(
      const std::function<void(const SIMCONNECT_RECV_EVENT& msg)>& callback)
{
   this->registerCallback<SIMCONNECT_RECV_EVENT>(
      callback, SIMCONNECT_RECV_ID_EVENT);
}

void 
SimConnectClient::registerOnEventObjectAddRemoveCallback(
      const std::function<void(
            const SIMCONNECT_RECV_EVENT_OBJECT_ADDREMOVE& msg)>& callback)
{
   this->registerCallback<SIMCONNECT_RECV_EVENT_OBJECT_ADDREMOVE>(
      callback, SIMCONNECT_RECV_ID_EVENT_OBJECT_ADDREMOVE);
}

void 
SimConnectClient::registerOnEventFilenameCallback(
      const std::function<void(
            const SIMCONNECT_RECV_EVENT_FILENAME& msg)>& callback)
{
   this->registerCallback<SIMCONNECT_RECV_EVENT_FILENAME>(
      callback, SIMCONNECT_RECV_ID_EVENT_FILENAME);
}

void 
SimConnectClient::registerOnEventFrameCallback(
      const std::function<void(
            const SIMCONNECT_RECV_EVENT_FRAME& msg)>& callback)
{
   this->registerCallback<SIMCONNECT_RECV_EVENT_FRAME>(
      callback, SIMCONNECT_RECV_ID_EVENT_FRAME);
}

void 
SimConnectClient::registerOnSimObjectDataCallback(
      const std::function<void(
            const SIMCONNECT_RECV_SIMOBJECT_DATA& msg)>& callback)
{
   this->registerCallback<SIMCONNECT_RECV_SIMOBJECT_DATA>(
      callback, SIMCONNECT_RECV_ID_SIMOBJECT_DATA);
}

void 
SimConnectClient::registerOnSimObjectDataByTypeCallback(
      const std::function<void(
            const SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE& msg)>& callback)
{
   this->registerCallback<SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE>(
      callback, SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE);
}

void 
SimConnectClient::registerOnWeatherObservationCallback(
      const std::function<void(
            const SIMCONNECT_RECV_WEATHER_OBSERVATION& msg)>& callback)
{
   this->registerCallback<SIMCONNECT_RECV_WEATHER_OBSERVATION>(
      callback, SIMCONNECT_RECV_ID_CLOUD_STATE);
}

void 
SimConnectClient::registerOnCloudStateCallback(
      const std::function<void(
            const SIMCONNECT_RECV_CLOUD_STATE& msg)>& callback)
{
   this->registerCallback<SIMCONNECT_RECV_CLOUD_STATE>(
      callback, SIMCONNECT_RECV_ID_WEATHER_OBSERVATION);
}

void 
SimConnectClient::registerOnAssignedObjectIDCallback(
      const std::function<void(
            const SIMCONNECT_RECV_ASSIGNED_OBJECT_ID& msg)>& callback)
{
   this->registerCallback<SIMCONNECT_RECV_ASSIGNED_OBJECT_ID>(
      callback, SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID);
}

void 
SimConnectClient::registerOnReservedKeyCallback(
      const std::function<void(
            const SIMCONNECT_RECV_RESERVED_KEY & msg)>& callback)
{
   this->registerCallback<SIMCONNECT_RECV_RESERVED_KEY>(
      callback, SIMCONNECT_RECV_ID_RESERVED_KEY);
}

void 
SimConnectClient::registerOnCustomActionCallback(
      const std::function<void(
            const SIMCONNECT_RECV_CUSTOM_ACTION& msg)>& callback)
{
   this->registerCallback<SIMCONNECT_RECV_CUSTOM_ACTION>(
      callback, SIMCONNECT_RECV_ID_CUSTOM_ACTION);
}

void 
SimConnectClient::registerOnSystemStateCallback(
      const std::function<void(
            const SIMCONNECT_RECV_SYSTEM_STATE& msg)>& callback)
{
   this->registerCallback<SIMCONNECT_RECV_SYSTEM_STATE>(
      callback, SIMCONNECT_RECV_ID_SYSTEM_STATE);
}

void 
SimConnectClient::registerOnClientDataCallback(
      const std::function<void(
            const SIMCONNECT_RECV_CLIENT_DATA& msg)>& callback)
{
   this->registerCallback<SIMCONNECT_RECV_CLIENT_DATA>(
      callback, SIMCONNECT_RECV_ID_CLIENT_DATA);
}

void
SimConnectClient::subscribeToSystemEvent(
      const EventName& event_name,
      SIMCONNECT_CLIENT_EVENT_ID event_id)
throw (InvalidInputException)
{
   auto result = SimConnect_SubscribeToSystemEvent(
         _handle, event_id, event_name.c_str());
   if (result != S_OK)
      throw InvalidInputException(boost::format(
            "cannot subscribe to system event %s") % event_name);
}

SimConnectClient::DataDefinition
SimConnectClient::newDataDefinition()
{
   static SIMCONNECT_DATA_DEFINITION_ID next_id = 0xffff0000;
   return DataDefinition(*this, next_id++);
}

SimConnectClient::DataPullRequest 
SimConnectClient::newDataPullRequest(const DataDefinition& data_def)
{
   static SIMCONNECT_DATA_REQUEST_ID next_id = 0xffff0000;
   return DataPullRequest(*this, data_def, next_id++);
}

SimConnectClient::DataPushRequest 
SimConnectClient::newDataPushRequest(const DataDefinition& data_def)
{ return DataPushRequest(*this, data_def); }

}; // namespace oac