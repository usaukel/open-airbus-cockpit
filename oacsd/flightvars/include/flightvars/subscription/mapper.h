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

#ifndef OAC_FV_SUBSCRIPTION_MAPPER_H
#define OAC_FV_SUBSCRIPTION_MAPPER_H

#include <boost/bimap.hpp>

#include <flightvars/subscription/errors.h>
#include <flightvars/subscription/types.h>
#include <flightvars/var.h>

namespace oac { namespace fv { namespace subs {

/**
 * An object able to map subscriptions and variables.
 */
class subscription_mapper
{
public:

   /**
    * Clear all registered subscriptions.
    */
   void clear();

   /**
    * Check whether there is a subscription for given variable.
    */
   bool subscription_exists(
         const variable_id& var_id) const;

   /**
    * Check whether there is a subscription for given subscription ID.
    */
   bool subscription_exists(
         const subscription_id& subs_id) const;

   /**
    * Register a new subscription.
    */
   void register_subscription(
         const variable_id& var_id,
         const subscription_id& subs_id)
   throw (variable_already_exists_error, subscription_already_exists_error);

   /**
    * Execute the given action for each mapped subscription.
    */
   void for_each_subscription(
         const std::function<void(const subscription_id&)>& action);

   /**
    * Obtain the variable ID for given subscription ID.
    * @throw no_such_subscription_error when given subscription ID is unknown
    */
   variable_id get_var_id(
         const subscription_id& subs_id)
   throw (no_such_subscription_error);

   /**
    * Obtain the subscription ID for given variable ID.
    * @throw no_such_variable_error when given variable ID is unknown
    */
   subscription_id get_subscription_id(
         const variable_id& var_id)
   throw (no_such_variable_error);

   /**
    * Unregister a mapping from its variable ID.
    * @throw no_such_variable_error when given variable ID is unknown
    */
   void unregister(
         const variable_id& var_id)
   throw (no_such_variable_error);

   /**
    * Unregister a mapping from its subscription ID.
    * @throw no_such_subscription_error when given subscription ID is unknown
    */
   void unregister(
         const subscription_id& subs_id)
   throw (no_such_subscription_error);

private:

   typedef boost::bimap<variable_id, subscription_id> map_type;

   map_type _map;
};

}}} // namespace oac::fv::subs

#endif
