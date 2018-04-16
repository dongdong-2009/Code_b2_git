/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/trains/train_agent/src/TrainAgentServant.h $
 * @author:  Khalil Dowlut
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 15:10:25 $
 * Last modified by:  $Author: builder $
 *
 * Servant Object.
 * Delegates all commands down to the TrainAgent object - m_trainAgent.
 * CORBA types are translated into local C++ types and local C++ types back
 * into CORBA types for corba decoupling.
 *
 * See standard transactive agents for other servant behaviour.
 */

#if !defined(TRAINAGENTSERVANT_H__2350C72B_F21F_4183_BF15_F3207038D7DC__INCLUDED_)
#define TRAINAGENTSERVANT_H__2350C72B_F21F_4183_BF15_F3207038D7DC__INCLUDED_

#include "core/corba/src/ServantBase.h"
#include"TrainAgent.h"
#include "bus/trains/train_agent/idl/src/IRecvSDSTrainAgentCorbaDef.h"

namespace TA_Base_Core
{
    class ItaRuntime;
}

namespace TA_Base_Bus
{
	class GenericAgent;
}

namespace TA_IRS_App
{

class TrainAgent;
class TrainAgentFramework;

class TrainAgentServant : public virtual POA_TA_IRS_Bus_4677::IRecvSDSTrainAgentCorbaDef,
                          public virtual TA_Base_Core::ServantBase
{
public:

    /**
      * TrainAgentServant
      *
      * Initialization constructor.
      *
      * @param : TrainAgent& trainAgent [in]
      * @param : TA_Base_Core::ItaRuntime& runtime [in]
      */
    TrainAgentServant(TrainAgent& trainAgent);

    /**
      * ~TrainAgentServant
      *
      * Destructor
      */
    virtual ~TrainAgentServant();

	virtual void ReceiveSDSData(const TA_IRS_Bus_4677::IRecvSDSTrainAgentCorbaDef::SDSData& data, CORBA::ULong srcRadioId,CORBA::ULong destRadioId);

private:

    /**
      * TrainAgentServant
      *
      * Default constructor (disabled)
      */
    TrainAgentServant();

    /**
      * TrainAgentServant
      *
      * Copy constructor (disabled)
      *
      * @param : const TrainAgentServant&
      */
    TrainAgentServant(const TrainAgentServant&);

    /**
      * operator=
      *
      * Assignment operator (disabled)
      *
      * @return TrainAgentServant&
      * @param : const TrainAgentServant&
      */
    TrainAgentServant& operator=(const TrainAgentServant&);

	virtual void checkOpeartionMode();

private:    // attributes

    TrainAgent&                 m_trainAgent;
};
}
#endif // !defined(TRAINAGENTSERVANT_H__2350C72B_F21F_4183_BF15_F3207038D7DC__INCLUDED_)
