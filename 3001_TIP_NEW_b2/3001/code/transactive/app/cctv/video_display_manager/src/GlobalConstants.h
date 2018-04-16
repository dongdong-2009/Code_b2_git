#ifndef GLOBALCONSTANTS_H
#define GLOBALCONSTANTS_H

#include "core/exceptions/src/TransactiveException.h"

const std::string ERROR_STR_UNABLE_CONNECT_VSS = "Unable to establish Connection with the VSS System";
const std::string ERROR_STR_UNABLE_CONNECT_DATABASE = "Unable to connect to database";
const std::string ERROR_STR_UNABLE_CONTACT_AGENT = "Agent cannot be contacted";
const std::string ERROR_STR_ENCOUNTER_ERROR = "Error encounter when performing the requested operation";

const TA_Base_Core::TransactiveException EXCEPTION_PERFORMING_REQUEST(ERROR_STR_ENCOUNTER_ERROR);
const TA_Base_Core::TransactiveException EXCEPTION_CANNOT_CONTACT_AGENT(ERROR_STR_UNABLE_CONTACT_AGENT);

#endif