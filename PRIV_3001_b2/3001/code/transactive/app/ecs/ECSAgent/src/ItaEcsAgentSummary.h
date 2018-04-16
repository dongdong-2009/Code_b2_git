/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ECSAgent/src/ItaEcsAgentSummary.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
///////////////////////////////////////////////////////////
//  ItaEcsAgentSummary.h
//  Implementation of the Interface ItaEcsAgentSummary
//  Created on:      13-Apr-2005 05:36:08 PM
//  Original author: Sean Liew
///////////////////////////////////////////////////////////

#if !defined(ItaEcsAgentSummary_F8EA240E_3B85_4410_864F_4AC9963F08E5__INCLUDED_)
#define ItaEcsAgentSummary_F8EA240E_3B85_4410_864F_4AC9963F08E5__INCLUDED_


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
 * @created 13-Apr-2005 05:36:08 PM
 */
class ItaEcsAgentSummary
{

public:

	virtual void cleanup() =0;
    virtual void setControl(bool isOnStartup=false) =0;
    virtual void setMonitor(bool isOnStartup=false) =0;

	virtual ItaEcsComms& getItaEcsComms() =0;
	virtual ItaEcsAuditing& getItaEcsAuditing() =0;
	virtual ItaEcsSynchronisation& getItaEcsSynchronisation() =0;
	virtual ItaEcsAgentCompleteState& getItaEcsAgentCompleteState() =0;
	virtual ItaEcsAlarms& getItaEcsAlarms() =0;
	virtual ItaEcsCachedConfig& getItaEcsCachedConfig() =0;
    virtual ItaEcsCachedConfigInitialiser& getItaEcsCachedConfigInitialiser() =0;
	virtual ItaEcsCachedMaps& getItaEcsCachedMaps() =0;
    virtual ItaEcsCachedMapsInitialiser& getItaEcsCachedMapsInitialiser() =0;
	virtual ItaEcsDatapoints& getItaEcsDatapoints() =0;

};
#endif // !defined(ItaEcsAgentSummary_F8EA240E_3B85_4410_864F_4AC9963F08E5__INCLUDED_)