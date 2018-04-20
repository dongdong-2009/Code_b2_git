/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/StationPAAgent.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/StationPAAgent.h $
  * @author:   Andy Parker
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(StationStationPAAgent_85831B60_1CBB_43c8_A752_5EB30E0DE611__INCLUDED_)
#define StationStationPAAgent_85831B60_1CBB_43c8_A752_5EB30E0DE611__INCLUDED_


#include "app/pa/PAAgent/src/AbstractPAAgent.h"


namespace TA_IRS_App
{
class AtsAgentSubscriber;

class StationPAAgent : public virtual AbstractPAAgent
{
public:

    StationPAAgent(int argc, char* argv[]);
    virtual ~StationPAAgent();

protected:

    virtual void processSpecificConfiguration() ;
        //throw(TA_Base_Core::InvalidPasConfigurationException, TA_Base_Core::DatabaseException, TA_Base_Core::DataException);

    virtual void initialiseAndValidateCachedMaps() ;
        //throw(TA_Base_Core::InvalidPasConfigurationException, TA_Base_Core::CachedMappingNotFoundException, TA_Base_Core::DatabaseException, TA_Base_Core::DataException);

    virtual void createSpecificComponents();
        //throw(TA_Base_Core::GenericAgentException, TA_Base_Core::InvalidPasConfigurationException, TA_Base_Core::CachedMappingNotFoundException);
    virtual void destroySpecificComponents();
    virtual void startSpecificComponents();
    virtual void stopSpecificComponents();

    virtual void agentSetControlSpecifics();
    virtual void agentSetMonitorSpecifics();

    // Disable copy constructor and assignment operator
    StationPAAgent( const StationPAAgent& theAgent);
    StationPAAgent& operator=(const StationPAAgent &);

        FireCountdownAckMonitor*            m_fireCountdownAckMonitor;
    AtsAgentSubscriber*                 m_atsAgentSubscriber;
};


} // namespace TA_IRS_App

#endif // !defined(StationStationPAAgent_85831B60_1CBB_43c8_A752_5EB30E0DE611__INCLUDED_)

