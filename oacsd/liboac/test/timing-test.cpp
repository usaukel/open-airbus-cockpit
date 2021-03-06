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

#include <liboac/timing.h>

using namespace oac;

BOOST_AUTO_TEST_SUITE(DummyTickObserver)

BOOST_AUTO_TEST_CASE(MustInvokeHandlersOnTick)
{
   dummy_tick_observer obs;
   int num1 = 0, num2 = 0;
   obs.register_handler([&num1]() { num1++; });
   obs.register_handler([&num2]() { num2--; });

   for (int i = 1; i < 64; i++)
   {
      obs.tick();

      BOOST_CHECK_EQUAL(i, num1);
      BOOST_CHECK_EQUAL(-i, num2);
   }
}

BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(AsioTickObserverAdapter)

BOOST_AUTO_TEST_CASE(MustInvokeHandlersOnTick)
{
   auto io_srv = std::make_shared<boost::asio::io_service>();
   asio_tick_observer_adapter<dummy_tick_observer> obs(io_srv);
   int num1 = 0, num2 = 0;
   obs.register_handler([&num1]() { num1++; });
   obs.register_handler([&num2]() { num2--; });

   for (int i = 1; i < 64; i++)
   {
      obs.delegate().tick();
      int runs = 0;
      while (runs < 2) { runs += io_srv->run_one(); }
      BOOST_CHECK_EQUAL(i, num1);
      BOOST_CHECK_EQUAL(-i, num2);
   }
}

BOOST_AUTO_TEST_SUITE_END()
