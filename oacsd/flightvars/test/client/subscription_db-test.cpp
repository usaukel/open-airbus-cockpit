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

#include <cstdlib>

#include <liboac/filesystem.h>

#include <flightvars/client.h>
#include <flightvars/protocol.h>
#include <flightvars/subscription.h>

using namespace oac;
using namespace oac::fv;
using namespace oac::fv::client;

BOOST_AUTO_TEST_SUITE(ClientSubscriptionDbTest)

auto null_handler = [](const variable_id&, const variable_value&){};

struct var_receptor
{
   variable_group received_var_group;
   variable_name received_var_name;
   std::uint32_t received_value;

   var_receptor()
      : received_var_group("nogroup"),
        received_var_name("noname"),
        received_value(0)
   {}

   void operator()(
         const variable_id& id,
         const variable_value& val)
   {
      BOOST_CHECK_EQUAL(variable_type::DWORD, val.get_type());
      received_var_group = id.group;
      received_var_name = id.name;
      received_value = val.as_dword();
   }
};

auto default_handler = [](const variable_id& id, const variable_value& val)
{
   BOOST_CHECK_EQUAL("foobar", id.group);
   BOOST_CHECK_EQUAL("datum", id.name);
   BOOST_CHECK_EQUAL(112233, val.as_dword());
};

BOOST_AUTO_TEST_CASE(MustCreateEntry)
{
   subscription_db db;
   variable_id var_id("foobar", "datum");
   auto master_subs = make_subscription_id();
   var_receptor receptor;

   BOOST_CHECK(!db.entry_defined(var_id));

   auto virtual_subs = db.create_entry(var_id, master_subs, std::ref(receptor));

   BOOST_CHECK(master_subs != virtual_subs);
   BOOST_CHECK(db.entry_defined(var_id));
   BOOST_CHECK_NO_THROW(db.get_master_subscription_id(var_id));
   BOOST_CHECK_NO_THROW(db.invoke_handlers(
         master_subs,
         variable_value::from_dword(112233)));
   BOOST_CHECK_EQUAL("foobar", receptor.received_var_group);
   BOOST_CHECK_EQUAL("datum", receptor.received_var_name);
   BOOST_CHECK_EQUAL(112233, receptor.received_value);
}

BOOST_AUTO_TEST_CASE(MustThrowOnCreateEntryWithDupVariable)
{
   subscription_db db;
   variable_id var_id("foobar", "datum");

   auto virtual_subs = db.create_entry(
         var_id,
         make_subscription_id(),
         null_handler);
   BOOST_CHECK_THROW(
         db.create_entry(
               var_id,
               make_subscription_id(),
               null_handler),
         subscription_db::variable_already_exists_error);
}

BOOST_AUTO_TEST_CASE(MustThrowOnCreateEntryWithDupMasterSubscription)
{
   subscription_db db;
   auto master_subs = make_subscription_id();

   auto virtual_subs = db.create_entry(
         variable_id("foobar", "datum1"),
         master_subs,
         null_handler);
   BOOST_CHECK_THROW(
         db.create_entry(
               variable_id("foobar", "datum2"),
               master_subs,
               null_handler),
         subscription_db::master_subscription_already_exists_error);
}

BOOST_AUTO_TEST_CASE(MustAddvirtualSubscriptions)
{
   subscription_db db;
   variable_id var_id("foobar", "datum");
   auto master_subs = make_subscription_id();
   var_receptor receptor[10];

   auto virtual_subs = db.create_entry(
         var_id,
         master_subs,
         std::ref(receptor[0]));
   for (int i = 1; i < 10; i++)
      db.add_virtual_subscription(var_id, std::ref(receptor[i]));

   BOOST_CHECK_NO_THROW(db.invoke_handlers(
         master_subs,
         variable_value::from_dword(112233)));
   for (int i = 0; i < 10; i++)
   {
      BOOST_CHECK_EQUAL("foobar", receptor[i].received_var_group);
      BOOST_CHECK_EQUAL("datum", receptor[i].received_var_name);
      BOOST_CHECK_EQUAL(112233, receptor[i].received_value);
   }
}

BOOST_AUTO_TEST_CASE(MustThrowOnAddvirtualSubscriptionsForUnexistingVar)
{
   subscription_db db;

   BOOST_CHECK_THROW(
         db.add_virtual_subscription(
               variable_id("foobar", "datum"),
               null_handler),
         subscription_db::no_such_variable_error);
}

BOOST_AUTO_TEST_CASE(MustRemovevirtualSubscription)
{
   subscription_db db;
   variable_id var_id("foobar", "datum");
   auto master_subs = make_subscription_id();
   var_receptor receptor[10];

   auto virtual_subs = db.create_entry(
         var_id,
         master_subs,
         std::ref(receptor[0]));
   for (int i = 1; i < 10; i++)
      db.add_virtual_subscription(var_id, std::ref(receptor[i]));
   BOOST_CHECK(!db.remove_virtual_subscription(virtual_subs));

   BOOST_CHECK_NO_THROW(db.invoke_handlers(
         master_subs,
         variable_value::from_dword(112233)));
   for (int i = 1; i < 10; i++)
   {
      BOOST_CHECK_EQUAL("foobar", receptor[i].received_var_group);
      BOOST_CHECK_EQUAL("datum", receptor[i].received_var_name);
      BOOST_CHECK_EQUAL(112233, receptor[i].received_value);
   }
   BOOST_CHECK_EQUAL("nogroup", receptor[0].received_var_group);
   BOOST_CHECK_EQUAL("noname", receptor[0].received_var_name);
   BOOST_CHECK_EQUAL(0, receptor[0].received_value);
}

BOOST_AUTO_TEST_CASE(MustThrowOnRemoveUnexistingvirtualSubscription)
{
   subscription_db db;

   BOOST_CHECK_THROW(
         db.remove_virtual_subscription(make_subscription_id()),
         subscription_db::no_such_virtual_subscription_error);
}

BOOST_AUTO_TEST_CASE(MustRemoveEntry)
{
   subscription_db db;
   variable_id var_id("foobar", "datum");
   auto master_subs = make_subscription_id();
   var_receptor receptor[10];

   auto virtual_subs = db.create_entry(var_id, master_subs, null_handler);

   db.remove_entry(var_id);

   BOOST_CHECK(!db.entry_defined(var_id));
   BOOST_CHECK_THROW(
         db.get_master_subscription_id(var_id),
         subscription_db::no_such_variable_error);
   BOOST_CHECK_THROW(
         db.invoke_handlers(master_subs, variable_value::from_dword(112233)),
         subscription_db::no_such_master_subscription_error);
}

BOOST_AUTO_TEST_CASE(MustRemoveEntryOnLastvirtualSubscriptionRemoval)
{
   subscription_db db;
   variable_id var_id("foobar", "datum");
   auto master_subs = make_subscription_id();
   var_receptor receptor[10];

   auto virtual_subs = db.create_entry(
         var_id,
         master_subs,
         std::ref(receptor[0]));
   BOOST_CHECK(db.remove_virtual_subscription(virtual_subs));

   BOOST_CHECK(!db.entry_defined(var_id));
   BOOST_CHECK_THROW(
         db.get_master_subscription_id(var_id),
         subscription_db::no_such_variable_error);
   BOOST_CHECK_THROW(
         db.invoke_handlers(master_subs, variable_value::from_dword(112233)),
         subscription_db::no_such_master_subscription_error);
}



BOOST_AUTO_TEST_SUITE_END()
