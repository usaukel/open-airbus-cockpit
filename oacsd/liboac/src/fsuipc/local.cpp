/*
 * This file is part of Open Airbus Cockpit
 * Copyright (C) 2012, 2013 Alvaro Polo
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

#include <sstream>

#include <Windows.h>

#include <boost/format.hpp>
#include <fsuipc/fsuipc_user.h>

#include <liboac/fsuipc/local.h>

namespace oac { namespace fsuipc {

namespace {

const char* fsuipc_error_msg[] =
{
   "no error",
   "attempt to Open when already Open",
   "cannot link to FSUIPC or WideClient",
   "failed to register common message with Windows",
   "failed to create Atom for mapping filename",
   "failed to create a file mapping object",
   "failed to open a view to the file map",
   "incorrect version of FSUIPC, or not FSUIPC",
   "sim is not version requested",
   "call cannot execute, link not Open",
   "call cannot execute, no requests accumulated",
   "IPC timed out all retries",
   "IPC sendmessage failed all retries",
   "IPC request contains bad data",
   "maybe running on WideClient, but FS not running "
      "on Server, or wrong FSUIPC",
   "read or write request cannot be added, memory for Process is full",
};

const char*
get_result_message(DWORD result)
{
   return (result < sizeof(fsuipc_error_msg) / sizeof(const char*))
         ? fsuipc_error_msg[result] : nullptr;
}

std::string
io_error_message(const std::string& action, DWORD result)
{
   return str(boost::format("IO error while %s: %s") %
      action.c_str() % get_result_message(result));
}

class local_fsuipc_handler
{
public:

   typedef std::shared_ptr<local_fsuipc_handler> ptr_type;

   inline static void init()
   throw (fsuipc_error)
   {
      if (!_singleton)
         _singleton = local_fsuipc_handler::ptr_type(
               new local_fsuipc_handler());
   }

   inline static void reset()
   {
      _singleton.reset();
   }

   inline ~local_fsuipc_handler()
   { FSUIPC_Close(); }

private:

   static local_fsuipc_handler::ptr_type _singleton;
   BYTE _buffer[LOCAL_FSUIPC_BUFFER_SIZE];

   local_fsuipc_handler()
   throw (fsuipc_error)
   {
      DWORD result;
      if (!FSUIPC_Open2(SIM_ANY, &result, _buffer, LOCAL_FSUIPC_BUFFER_SIZE))
         OAC_THROW_EXCEPTION(fsuipc_error(result, get_result_message(result)));
   }

};

typedef local_fsuipc_handler::ptr_type local_fsuipc_handler_ptr;

local_fsuipc_handler_ptr local_fsuipc_handler::_singleton;

} // anonymous namespace

local_fsuipc::local_fsuipc()
   : linear_stream_buffer_base<local_fsuipc>(this)
{
   local_fsuipc_handler::init();
}

void
local_fsuipc::read(void* dst, std::uint32_t offset, std::size_t length) const
throw (buffer::index_out_of_bounds, io_exception)
{
   DWORD error;
   if (FSUIPC_Read(offset, length, dst, &error))
      if (FSUIPC_Process(&error))
         return;
   OAC_THROW_EXCEPTION(fsuipc_error(error, get_result_message(error)));
}

void
local_fsuipc::write(const void* src, std::uint32_t offset, std::size_t length)
throw (buffer::index_out_of_bounds, io_exception)
{
   DWORD error;
   if (FSUIPC_Write(offset, length, (void*) src, &error))
      if (FSUIPC_Process(&error))
         return;
   OAC_THROW_EXCEPTION(fsuipc_error(error, get_result_message(error)));
}



std::uint32_t local_user_adapter::_instance_count(0);

std::uint8_t local_user_adapter::_buffer[LOCAL_FSUIPC_BUFFER_SIZE];

local_user_adapter::local_user_adapter()
throw (fsuipc_error)
 : logger_component("local_user_adapter")
{
   if (_instance_count == 0)
   {
      // No previous instance, let's open FSUIPC
      log_info("Opening local channel to FSUIPC");
      DWORD error;
      if (!FSUIPC_Open2(
             SIM_ANY,
             &error,
             _buffer,
             LOCAL_FSUIPC_BUFFER_SIZE))
      {
         OAC_THROW_EXCEPTION(fsuipc_error(error, get_result_message(error)));
      }
      log_info("Local channel to FSUIPC successfully open");
   }
   _instance_count++;
}

local_user_adapter::local_user_adapter(
      const local_user_adapter& adapter)
 : logger_component("local_user_adapter")
{
   _instance_count++;
}

local_user_adapter::~local_user_adapter()
{
   _instance_count--;
   if (_instance_count == 0)
   {
      log_info("Closing local channel to FSUIPC");
      // Surprisingly this function has no return code. It's like nothing
      // could go wrong while closing a connection... It's like some functions
      // in FSUIPC deserve to be a third-class citizens... Bad code!
      FSUIPC_Close();
      log_info("Local channel to FSUIPC successfully closed");
   }
}

void
local_user_adapter::read(
      valued_offset& valued_offset)
throw (fsuipc_error)
{
   DWORD error;
   if (!FSUIPC_Read(
            valued_offset.address,
            valued_offset.length,
            &valued_offset.value,
            &error))
   {
      OAC_THROW_EXCEPTION(fsuipc_error(error, get_result_message(error)));
   }
}

void
local_user_adapter::write(
      const valued_offset& valued_offset)
throw (fsuipc_error)
{
   DWORD error;
   if (!FSUIPC_Write(
            valued_offset.address,
            valued_offset.length,
            (void*) &(valued_offset.value),
            &error))
   {
      OAC_THROW_EXCEPTION(fsuipc_error(error, get_result_message(error)));
   }
}

void
local_user_adapter::process()
throw (fsuipc_error)
{
   DWORD error;
   if (!FSUIPC_Process(&error))
   {
      OAC_THROW_EXCEPTION(fsuipc_error(error, get_result_message(error)));
   }
}

}} // namespace oac::fsuipc
