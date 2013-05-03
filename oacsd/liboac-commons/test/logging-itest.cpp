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
 * You should have received a copy of the GNU General Public License
 * along with Open Airbus Cockpit. If not, see <http://www.gnu.org/licenses/>.
 */

#include <string>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>

#include "buffer.h"
#include "logging.h"

using namespace oac;

std::pair<ptr<buffer>, ptr<logger>> Initlogger()
{
   ptr<buffer> buff = new fixed_buffer(1024);
   ptr<logger> log = new logger(
         log_level::INFO, new buffer_output_stream(buff));
   return std::pair<ptr<buffer>, ptr<logger>>(buff, log);
}

BOOST_AUTO_TEST_CASE(ShouldInitlogger)
{
   auto log = Initlogger();
   logger::set_main(log.second);
   BOOST_CHECK_EQUAL(log.second, logger::get_main());
}

BOOST_AUTO_TEST_CASE(ShouldWriteInlogger)
{
   auto log = Initlogger();
   log.second->log(log_level::INFO, "ABCD");
   reader reader(new buffer_input_stream(log.first));
   auto line = reader.readLine();
   BOOST_CHECK(line.find("[INFO]") != std::string::npos);
   BOOST_CHECK(line.find("ABCD") != std::string::npos);
}
