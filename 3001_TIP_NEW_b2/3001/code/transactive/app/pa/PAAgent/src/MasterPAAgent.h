/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/MasterPAAgent.h $
  * @author:   Andy Parker
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */

#if !defined(MasterPAAgent_85831B60_1CBB_43c8_A752_5EB30E0DE611__INCLUDED_)
#define MasterPAAgent_85831B60_1CBB_43c8_A752_5EB30E0DE611__INCLUDED_


#include "app/pa/PAAgent/src/AbstractPAAgent.h"

namespace TA_IRS_App
{

class MasterPAAgent : public virtual AbstractPAAgent
{
public:

    MasterPAAgent(int argc, char* argv[]);
    virtual ~MasterPAAgent();

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
    MasterPAAgent( const MasterPAAgent& theAgent);
    MasterPAAgent& operator=(const MasterPAAgent &);


};


} // namespace TA_IRS_App

#endif // !defined(MasterPAAgent_A7EE4E27_1FF9_4bb8_BBB7_6B1F2C675F52__INCLUDED_)

