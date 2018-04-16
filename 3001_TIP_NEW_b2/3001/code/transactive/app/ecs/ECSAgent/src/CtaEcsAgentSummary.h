/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/CtaEcsAgentSummary.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
///////////////////////////////////////////////////////////
//  CtaEcsAgentSummary.h
//  Implementation of the Class CtaEcsAgentSummary
//  Created on:      13-Apr-2005 05:36:16 PM
//  Original author: Sean Liew
///////////////////////////////////////////////////////////

#if !defined(CtaEcsAgentSummary_8FE2B017_A55C_4d02_8D08_209F9EB97669__INCLUDED_)
#define CtaEcsAgentSummary_8FE2B017_A55C_4d02_8D08_209F9EB97669__INCLUDED_

#include "app/ecs/ECSAgent/src/CtaEcsAgentCompleteState.h"
#include "app/ecs/ECSAgent/src/CtaEcsAlarms.h"
#include "app/ecs/ECSAgent/src/CtaEcsAuditing.h"
#include "app/ecs/ECSAgent/src/CtaEcsCachedConfig.h"
#include "app/ecs/ECSAgent/src/CtaEcsCachedMaps.h"
#include "app/ecs/ECSAgent/src/CtaEcsComms.h"
#include "app/ecs/ECSAgent/src/CtaEcsSynchronisation.h"
#include "app/ecs/ECSAgent/src/CtaEcsDatapoints.h"
#include "app/ecs/ECSAgent/src/ItaEcsAgentSummary.h"
#include "app/ecs/ECSAgent/src/AbstractFailoverable.h"



class ItaEcsComms;
class ItaEcsAgentCompleteState;
class ItaEcsCachedConfig;
class ItaEcsCachedConfigInitialiser;
class ItaEcsSynchronisation;
class ItaEcsAuditing;
class ItaEcsAlarms;
class ItaEcsCachedMaps;
class ItaEcsCachedMapsInitialiser;
class ItaEcsDatapoints;


/**
 * @author Sean Liew
 * @version 1.0
 * @created 13-Apr-2005 05:36:16 PM
 */
class CtaEcsAgentSummary : public virtual ItaEcsAgentSummary,
                           public virtual AbstractFailoverable
{

public:
	CtaEcsAgentSummary();
	virtual ~CtaEcsAgentSummary();

public:
    virtual void setup();
	virtual void cleanup();
    virtual void setControl(bool isOnStartup=false);
    virtual void setMonitor(bool isOnStartup=false);


	virtual ItaEcsComms& getItaEcsComms()
    {
        return *m_ctaEcsComms;
    }

	virtual ItaEcsAuditing& getItaEcsAuditing()
    {
        return *m_ctaEcsAuditing;
    }

    virtual ItaEcsSynchronisation& getItaEcsSynchronisation()
    {
        return *m_ctaEcsSynchronisation;
    }

	virtual ItaEcsAgentCompleteState& getItaEcsAgentCompleteState()
    {
        return *m_ctaEcsAgentCompleteState;
    }

	virtual ItaEcsAlarms& getItaEcsAlarms()
    {
        return *m_ctaEcsAlarms;
    }

	virtual ItaEcsCachedConfig& getItaEcsCachedConfig()
    {
        return *m_ctaEcsCachedConfig;
    }

    virtual ItaEcsCachedConfigInitialiser& getItaEcsCachedConfigInitialiser()
    {
        return *m_ctaEcsCachedConfig;
    }

	virtual ItaEcsCachedMaps& getItaEcsCachedMaps()
    {
        return *m_ctaEcsCachedMaps;
    }

    virtual ItaEcsCachedMapsInitialiser& getItaEcsCachedMapsInitialiser()
    {
        return *m_ctaEcsCachedMaps;
    }

    virtual ItaEcsDatapoints& getItaEcsDatapoints()
    {
        return *m_ctaEcsDatapoints;
    }



private:
  	CtaEcsComms*                m_ctaEcsComms;
	CtaEcsAgentCompleteState*   m_ctaEcsAgentCompleteState;
	CtaEcsCachedConfig*         m_ctaEcsCachedConfig;
	CtaEcsSynchronisation*      m_ctaEcsSynchronisation;
	CtaEcsAuditing*             m_ctaEcsAuditing;
	CtaEcsAlarms*               m_ctaEcsAlarms;
	CtaEcsCachedMaps*           m_ctaEcsCachedMaps;
	CtaEcsDatapoints*			m_ctaEcsDatapoints;

};
#endif // !defined(CtaEcsAgentSummary_8FE2B017_A55C_4d02_8D08_209F9EB97669__INCLUDED_)