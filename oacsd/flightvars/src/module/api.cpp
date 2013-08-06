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
 * along with Open Airbus Cockpit. If not, see <http://www.gnu.org/licenses/>.
 */

#include "api.h"

namespace oac { namespace fv {

namespace {

template <typename T>
ptr<linear_buffer> make_buffer_of(const T& value)
{
   auto buff = new linear_buffer(sizeof(T));
   buffer::write_as(*buff, 0, value);
   return buff;
}

} // anonymous namespace

variable_value
variable_value::from_bool(bool value)
{ return variable_value(VAR_BOOLEAN, make_buffer_of<bool>(value)); }

variable_value
variable_value::from_byte(std::uint8_t value)
{ return variable_value(VAR_BYTE, make_buffer_of(value)); }

variable_value
variable_value::from_word(std::uint16_t value)
{ return variable_value(VAR_WORD, make_buffer_of(value)); }

variable_value
variable_value::from_dword(std::uint32_t value)
{ return variable_value(VAR_DWORD, make_buffer_of(value)); }

variable_value
variable_value::from_float(float value)
{ return variable_value(VAR_FLOAT, make_buffer_of(value)); }

bool
variable_value::as_bool() const
throw (invalid_type_error)
{
   checkType(VAR_BOOLEAN);
   return buffer::read_as<bool>(*_buffer, 0);
}

std::uint8_t
variable_value::as_byte() const
throw (invalid_type_error)
{
   checkType(VAR_BYTE);
   return buffer::read_as<std::uint8_t>(*_buffer, 0);
}

std::uint16_t
variable_value::as_word() const
throw (invalid_type_error)
{
   checkType(VAR_WORD);
   return buffer::read_as<std::uint16_t>(*_buffer, 0);
}

std::uint32_t
variable_value::as_dword() const
throw (invalid_type_error)
{
   checkType(VAR_DWORD);
   return buffer::read_as<std::uint32_t>(*_buffer, 0);
}

float
variable_value::as_float() const
throw (invalid_type_error)
{
   checkType(VAR_FLOAT);
   return buffer::read_as<float>(*_buffer, 0);
}

std::string
variable_value::to_string() const
{
   switch (_type)
   {
      case VAR_BOOLEAN: return as_bool() ? "true(bool)" : "false(bool)";
      case VAR_BYTE: return str(boost::format("%d(byte)") % int(as_byte()));
      case VAR_WORD: return str(boost::format("%d(word)") % as_word());
      case VAR_DWORD: return str(boost::format("%d(dword)") % as_dword());
      case VAR_FLOAT: return str(boost::format("%f(float)") % as_float());
      default: BOOST_THROW_EXCEPTION(illegal_state_error()); // never reached
   }
}

void
variable_value::checkType(const variable_type& type) const
throw (invalid_type_error)
{
   if (_type != type)
      BOOST_THROW_EXCEPTION(invalid_type_error() <<
            expected_type_info(type) << actual_type_info(_type));
}


}} // namespace oac::fv
