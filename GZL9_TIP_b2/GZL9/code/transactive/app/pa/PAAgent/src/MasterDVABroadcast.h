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

#if !defined(MASTERDVABROADCAST__INCLUDED_)
#define MASTERDVABROADCAST__INCLUDED_

#include "app/pa/PAAgent/src/AbstractNonTrainDVABroadcast.h"
#include "app/pa/PAAgent/src/PasHelpers.h"


namespace TA_IRS_App
{

class MasterDVABroadcast : public virtual AbstractNonTrainDVABroadcast
{

public:

    MasterDVABroadcast( TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                        const std::string& sessionId );
        // throws CorruptInternalStateDataException
    virtual ~MasterDVABroadcast();

    virtual void validateBroadcastConfigSpecifics(); 
        //throw(TA_Base_Core::BadParameterException);

    virtual void writeToMessagesSequenceBroadcastRequestTable(unsigned int announceId);
        // throws (PasWriteErrorException, PasConnectionException)

    virtual void performVersionCheck();

	virtual void updateCacheWithLatestZoneStatus();

    virtual void disconnectSourceId();
    virtual void disconnectSequenceId();

	virtual bool isAllFailureZones();

protected:

    void getEquipmentComponentAndVerify( const std::string& equipmentName, 
                                         std::string& locationName, 
                                         std::string& pidName);
        //throw(TA_Base_Core::InvalidPasConfigurationException);

private:

    MasterDVABroadcast( const MasterDVABroadcast& theMasterDVABroadcast);    
};

} // namespace TA_IRS_App

#endif
