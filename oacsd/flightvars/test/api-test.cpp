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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You Must have received a copy of the GNU General Public License
 * along with Open Airbus Cockpit. If not, see <http://www.gnu.org/licenses/>.
 */

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>

#include "api.h"

using namespace oac;
using namespace oac::fv;

BOOST_AUTO_TEST_SUITE(VariableId)

BOOST_AUTO_TEST_CASE(MustMakeVarIdFromObjects)
{
   variable_group grp("my_group/foobar");
   variable_name name("my_var/millibars");
   variable_id id(grp, name);
   BOOST_CHECK_EQUAL("my_group/foobar", id.group);
   BOOST_CHECK_EQUAL("my_var/millibars", id.name);
}

BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(VariableValue)

BOOST_AUTO_TEST_CASE(MustMatchCreationAndExtractionTypeForBool)
{
   auto val = variable_value::from_bool(true);
   BOOST_CHECK_EQUAL(variable_type::BOOLEAN, val.get_type());
   BOOST_CHECK_EQUAL(true, val.as_bool());
}

BOOST_AUTO_TEST_CASE(MustMatchCreationAndExtractionTypeForByte)
{
   auto val = variable_value::from_byte(65);
   BOOST_CHECK_EQUAL(variable_type::BYTE, val.get_type());
   BOOST_CHECK_EQUAL(65, val.as_byte());
}

BOOST_AUTO_TEST_CASE(MustMatchCreationAndExtractionTypeForWord)
{
   auto val = variable_value::from_word(1200);
   BOOST_CHECK_EQUAL(variable_type::WORD, val.get_type());
   BOOST_CHECK_EQUAL(1200, val.as_word());
}

BOOST_AUTO_TEST_CASE(MustMatchCreationAndExtractionTypeForDoubleWord)
{
   auto val = variable_value::from_dword(1200);
   BOOST_CHECK_EQUAL(variable_type::DWORD, val.get_type());
   BOOST_CHECK_EQUAL(1200, val.as_dword());
}

BOOST_AUTO_TEST_CASE(MustMatchCreationAndExtractionTypeForFloat)
{
   auto val = variable_value::from_float(3.1416f);
   BOOST_CHECK_EQUAL(variable_type::FLOAT, val.get_type());
   BOOST_CHECK_CLOSE(3.1416f, val.as_float(), 0.0001f);
}

BOOST_AUTO_TEST_CASE(MustFailOnExtractionOfWrongType)
{
   auto val = variable_value::from_dword(1200);
   BOOST_CHECK_THROW(val.as_bool(), variable_value::invalid_type_error);
}

BOOST_AUTO_TEST_CASE(MustConvertToStringForBool)
{
   auto val1 = variable_value::from_bool(true);
   auto val2 = variable_value::from_bool(false);
   BOOST_CHECK_EQUAL("true(bool)", val1.to_string());
   BOOST_CHECK_EQUAL("false(bool)", val2.to_string());
}

BOOST_AUTO_TEST_CASE(MustConvertToStringForByte)
{
   auto val = variable_value::from_byte(32);
   BOOST_CHECK_EQUAL("32(byte)", val.to_string());
}

BOOST_AUTO_TEST_CASE(MustConvertToStringForWord)
{
   auto val = variable_value::from_word(1001);
   BOOST_CHECK_EQUAL("1001(word)", val.to_string());
}

BOOST_AUTO_TEST_CASE(MustConvertToStringForDoubleWord)
{
   auto val = variable_value::from_dword(1001);
   BOOST_CHECK_EQUAL("1001(dword)", val.to_string());
}

BOOST_AUTO_TEST_CASE(MustConvertToStringForFloat)
{
   auto val = variable_value::from_float(3.1415f);
   BOOST_CHECK_EQUAL("3.141500(float)", val.to_string());
}

BOOST_AUTO_TEST_SUITE_END()
