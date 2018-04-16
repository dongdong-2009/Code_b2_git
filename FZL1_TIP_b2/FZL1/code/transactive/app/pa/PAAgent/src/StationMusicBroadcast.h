/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */

#if !defined(STATIONMUSICBROADCAST__INCLUDED_)
#define STATIONMUSICBROADCAST__INCLUDED_

#include "app/pa/PAAgent/src/AbstractStationBroadcast.h"
#include "app/pa/PAAgent/src/PasHelpers.h"

namespace TA_IRS_App
{
class StationMusicBroadcast : public virtual AbstractStationBroadcast
{

public:

    StationMusicBroadcast( TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                          const std::string& sessionId);
        // throws CorruptInternalStateDataException

    virtual ~StationMusicBroadcast();

    virtual void validateBroadcastConfig(); //throw(TA_Base_Core::BadParameterException);

    virtual void execute();

    virtual void terminate();

    virtual TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage getZoneCoverage()
    {
        // Shouldn't retry this broadcast - just return nothing
        return TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage();
    }

protected:
    
    virtual void runtimeSpecifics();


private:

    StationMusicBroadcast( const StationMusicBroadcast& theStationMusicBroadcast);    

protected:

    TA_Base_Bus::IPAAgentCorbaDef::StationMusicConfig&   m_theSpecifics; // For convenience

};


} // namespace TA_IRS_App


#endif

