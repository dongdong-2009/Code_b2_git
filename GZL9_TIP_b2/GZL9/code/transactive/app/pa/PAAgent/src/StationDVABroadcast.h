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

#if !defined(STATIONDVABROADCAST__INCLUDED_)
#define STATIONDVABROADCAST__INCLUDED_

#include "app/pa/PAAgent/src/AbstractNonTrainDVABroadcast.h"
#include "app/pa/PAAgent/src/PasHelpers.h"

namespace TA_IRS_App
{
class StationDVABroadcast : public virtual AbstractNonTrainDVABroadcast
{

public:

    StationDVABroadcast( TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                         const std::string& sessionId );
        // throws CorruptInternalStateDataException
    virtual ~StationDVABroadcast();

    virtual void validateBroadcastConfigSpecifics(); 
        //throw(TA_Base_Core::BadParameterException);

    virtual void writeToMessagesSequenceBroadcastRequestTable(unsigned int announceId);
        // throws (PasWriteErrorException, PasConnectionException)

	virtual void updateCacheWithLatestZoneStatus();

    virtual void disconnectSourceId();
    virtual void disconnectSequenceId();

    virtual bool isAllFailureZones();

protected:

    std::string getEquipmentComponentAndVerify(const std::string& equipmentName);
        //throw(TA_Base_Core::InvalidPasConfigurationException);

private:

    StationDVABroadcast( const StationDVABroadcast& theStationDVABroadcast);    

protected:
    std::string                                     m_locationName;

};

} // namespace TA_IRS_App

#endif

