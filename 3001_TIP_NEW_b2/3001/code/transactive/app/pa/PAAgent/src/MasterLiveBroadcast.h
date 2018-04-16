/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/MasterLiveBroadcast.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */

#if !defined(MasterLiveBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define MasterLiveBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "app/pa/PAAgent/src/NonTrainLiveBroadcast.h"


namespace TA_IRS_App
{

class MasterLiveBroadcast : public NonTrainLiveBroadcast
{
public:
    MasterLiveBroadcast( TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP, const std::string& sessionId );
        // throws CorruptInternalStateDataException
    virtual ~MasterLiveBroadcast();

    virtual void validateBroadcastConfigSpecifics(); //throw(TA_Base_Core::BadParameterException);

    virtual int writeStartToPas();
    virtual int writeStopToPas();

private:
    MasterLiveBroadcast( const MasterLiveBroadcast& theMasterLiveBroadcast);    
    MasterLiveBroadcast();
};

} // namespace TA_IRS_App

#endif

