/**
 * Copyright (c) 2016-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include "ServiceAction.h"
#include "StoredState.h"
#include "SessionCredit.h"

namespace magma {

// Used to keep track of credit grants from Gy. Some features of this type of
// grants include:
//    1. Final Unit Handling: Instructions on what is to happen to the session
//       on final grant exhaustion. Relevant fields are is_final_grant and
//       final_action_info. We currently support TERMINATE and REDIRECT.
//    2. Expiry/Validity Time: This grant can be received with an int to
//       indicate the number of seconds the grant is valid for. The expiry_time
//       field indicates the time at which the grant is no longer valid.
// ChargingGrant is a struct because all fields are public
struct ChargingGrant {
  // Keep track of used/reported/allowed bytes
  SessionCredit credit;
  // When this is true, final_action should be acted on upon credit exhaust
  bool is_final_grant;
  // Only valid if is_final_grant is true
  FinalActionInfo final_action_info;
  // The expiry time for the credit's validity
  std::time_t expiry_time;
  ServiceState service_state;
  ReAuthState reauth_state;

  // Default states
  ChargingGrant() : credit(), is_final_grant(false),
    service_state(SERVICE_ENABLED), reauth_state(REAUTH_NOT_NEEDED) {}

  // ChargingGrant -> StoredChargingGrant
  StoredChargingGrant marshal();

  // StoredChargingGrant -> ChargingGrant
  static ChargingGrant unmarshal(const StoredChargingGrant &marshaled);

  void receive_charging_grant(const magma::lte::ChargingCredit& credit,
                              SessionCreditUpdateCriteria* uc=NULL);

  // Returns a SessionCreditUpdateCriteria that reflects the current state
  SessionCreditUpdateCriteria get_update_criteria();

  // Determine whether the charging grant should send an update request
  // Return true if an update is required, with the update_type set to indicate
  // the reason.
  // Return false otherwise. In this case, update_type is untouched.
  bool get_update_type(CreditUsage::UpdateType* update_type) const;

  // get_action returns the action to take on the credit based on the last
  // update. If no action needs to take place, CONTINUE_SERVICE is returned.
  ServiceActionType get_action(SessionCreditUpdateCriteria &update_criteria);

  // Get unreported usage from credit and return as part of CreditUsage
  // The update_type is also included in CreditUsage
  // If the grant is final or is_terminate is true, we include all unreported
  // usage, otherwise we only include unreported usage up to the allocated amount.
  CreditUsage get_credit_usage(CreditUsage::UpdateType update_type,
    SessionCreditUpdateCriteria& uc, bool is_terminate);

  // Return true if the service needs to be deactivated
  bool should_deactivate_service() const;

  // Convert FinalAction enum to ServiceActionType
  ServiceActionType final_action_to_action(
    const ChargingCredit_FinalAction action) const;

  // Set is_final_grant and final_action_info values
  void set_final_action_info(
    const magma::lte::ChargingCredit& credit, SessionCreditUpdateCriteria& uc);

  // Set the object and update criteria's reauth state to new_state.
  void set_reauth_state(const ReAuthState new_state, SessionCreditUpdateCriteria &uc);

  // Set the object and update criteria's service state to new_state.
  void set_service_state(const ServiceState new_service_state, SessionCreditUpdateCriteria &uc);

  // Convert rel_time_sec, which is a delta value in seconds, into a timestamp
  // and assign it to expiry_time
  void set_expiry_time_as_timestamp(uint32_t rel_time_sec);

  // Log final action related information
  void log_final_action_info() const;
};

}  // namespace magma
