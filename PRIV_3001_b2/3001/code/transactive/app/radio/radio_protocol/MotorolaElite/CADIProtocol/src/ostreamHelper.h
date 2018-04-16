/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/ostreamHelper.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Helper for stream functions
  */

#ifndef __OSTREAMHELPER_H
#define __OSTREAMHELPER_H

#include <sstream>
#include "app/radio/radio_protocol/MotorolaElite/CADIRPC/src/CADITypes.h"

std::ostream &operator<< (std::ostream &o, CADI_TIMESTAMP &x);
std::ostream &operator<< (std::ostream &o, CADI_ZONE_INFO &x);
std::ostream &operator<< (std::ostream &o, CADI_RADIO_EVENT_INFO &x);
std::ostream &operator<< (std::ostream &o, CADI_ZC_SEQ &x);
std::ostream &operator<< (std::ostream &o, CADI_CMD_STATUS_DATA &x);
std::ostream &operator<< (std::ostream &o, CADI_GROUP_CALL_DATA &x);
std::ostream &operator<< (std::ostream &o, CADI_PRIVATE_CALL_DATA &x);
std::ostream &operator<< (std::ostream &o, CADI_EA_DATA &x);
std::ostream &operator<< (std::ostream &o, CADI_INTERCONNECT_DATA &x);
std::ostream &operator<< (std::ostream &o, CADI_RADIO_ACK_DATA &x);
std::ostream &operator<< (std::ostream &o, CADI_STATUS_DATA &x);
std::ostream &operator<< (std::ostream &o, CADI_ZC_STATUS_DATA &x);

#endif
