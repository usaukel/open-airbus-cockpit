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

#ifndef OAC_NETWORK_TYPES_H
#define OAC_NETWORK_TYPES_H

#include <cstdint>
#include <functional>
#include <string>

#include <liboac/exception.h>
#include <liboac/io.h>

namespace oac { namespace network {

/**
 * A TCP protocol port.
 */
typedef std::uint16_t tcp_port;

/**
 * A hostname.
 */
typedef std::string hostname;

/**
 * A function to handle errors.
 */
typedef std::function<void(const io_exception&)> error_handler;

}} // namespace oac::network

#endif
