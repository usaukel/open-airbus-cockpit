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

#include <cstdio>
#include <ctime>

#include "logging.h"

namespace oac {

namespace {

const char* LEVEL_STR[] =
{
   "INFO", "WARN", "FAIL"
};

std::string GetTime()
{
   auto t = time(nullptr);
   char time_buf[26];
   struct tm lt;
   localtime_s(&lt, &t);
   asctime_s(time_buf, &lt);
   time_buf[24] = '\0';
   return time_buf;
}

} // anonymous namespace


ptr<Logger> Logger::_main;

void
Logger::log(LogLevel level, const std::string& msg)
{
   if (_level <= level)
   {
      auto line = str(boost::format("[%s] %s : %s\n") %
                      LEVEL_STR[level] % GetTime() % msg);
      _output->write_as<std::string>(line);
      _output->flush();
   }
}

void
Log(LogLevel level, const std::string& msg)
{
   auto main = Logger::main();
   if (main)
      main->log(level, msg);
}

void
Log(LogLevel level, const boost::format& fmt)
{ Log(level, str(fmt)); }

} // namespace oac
