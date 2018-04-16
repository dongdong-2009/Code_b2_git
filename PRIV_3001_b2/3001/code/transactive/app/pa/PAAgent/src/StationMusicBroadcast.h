/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/StationMusicBroadcast.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#if !defined(StationMusicBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define StationMusicBroadcast_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_


#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#include "app/pa/PAAgent/src/ITransactionOwner.h"
#include "app/pa/PAAgent/src/GlobalStructDefine.h"
#include "app/pa/PAAgent/src/AbstractStationBroadcast.h"

namespace TA_IRS_App
{

class StationMusicBroadcast : public virtual AbstractStationBroadcast, public ITransactionOwner
{

public:

    StationMusicBroadcast( TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP, const std::string& sessionId );
        // throws CorruptInternalStateDataException

    virtual ~StationMusicBroadcast();

    virtual void validateBroadcastConfig(); //throw(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException);

    virtual void prepare();
    virtual void execute();
    virtual void aborting();

    virtual void terminate();

    virtual TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage getZoneCoverage()
    {
        // Shouldn't retry this broadcast - just return nothing
        return TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage();
    }

    virtual void transactionExecuted( int nResult );
protected:
    
    virtual void runtimeSpecifics();
    virtual int writeStartToPas();
    virtual int writeStopToPas();


private:

    StationMusicBroadcast( const StationMusicBroadcast& theStationMusicBroadcast);    

protected:

    StdStationMusicConfig   m_theSpecifics;
};


} // namespace TA_IRS_App


#endif

