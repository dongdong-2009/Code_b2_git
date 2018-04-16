/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/StationLiveBroadcast.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */

#if !defined(StationLiveBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define StationLiveBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "app/pa/PAAgent/src/NonTrainLiveBroadcast.h"

namespace TA_IRS_App
{

class StationLiveBroadcast : public NonTrainLiveBroadcast
{
public:
    StationLiveBroadcast( TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP, const std::string& sessionId );
        // throws CorruptInternalStateDataException
    virtual ~StationLiveBroadcast();

    virtual void validateBroadcastConfigSpecifics(); //throw(TA_Base_Core::BadParameterException);

    virtual int writeStartToPas();
    virtual int writeStopToPas();

private:
    StationLiveBroadcast( const StationLiveBroadcast& theStationLiveBroadcast);
    StationLiveBroadcast();
};

} // namespace TA_IRS_App

#endif

