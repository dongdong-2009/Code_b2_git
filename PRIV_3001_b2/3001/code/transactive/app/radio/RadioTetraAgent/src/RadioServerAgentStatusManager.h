/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioServerAgent/src/RadioServerAgentStatusManager.h $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef RADIO_SERVER_AGENT_STATUS_MANAGER_H_INCLUDED
#define RADIO_SERVER_AGENT_STATUS_MANAGER_H_INCLUDED

#include <string>

//Forward class declaration;
//class DynGroupCollection;

class RadioServerAgentStatusManager
{

public:
	RadioServerAgentStatusManager();
	virtual ~RadioServerAgentStatusManager();

    // TODO: what is this for?

	void setFeatureAttributeXXX();
	void getFeatureAttributeXXX();


private:
//	DynGroupCollection *m_dynGroupCollection;

public:
	/**
	 *	Has yet to know what is "ResourceFeatureMapping";
	 */ 
//	ResourceFeatureMapping *m_ResourceFeatureMapping;

};
#endif
