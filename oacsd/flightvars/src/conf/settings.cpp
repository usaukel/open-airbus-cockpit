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

#include <liboac/exception.h>

#include "conf/settings.h"

namespace oac { namespace fv { namespace conf {

const boost::filesystem::path
domain_settings::DEFAULT_FSUIPC_OFFSETS_EXPORT_FILE
{ "C:\\ProgramData\\OACSD\\Exports-FSUIPC_Offsets.json" };

const mqtt_broker_runner_id
flightvars_settings::DEFAULT_MQTT_BROKER_RUNNER
{ mqtt_broker_runner_id::NONE };

const mqtt_client_id
flightvars_settings::DEFAULT_MQTT_CLIENT
{ mqtt_client_id::DEFAULT };

const log_level
flightvars_settings::DEFAULT_LOG_LEVEL
{ log_level::WARN };

const boost::filesystem::path
flightvars_settings::DEFAULT_LOG_FILE
{ "C:\\ProgramData\\OACSD\\FlightVars.log" };

}}} // namespace oac::fv::conf
