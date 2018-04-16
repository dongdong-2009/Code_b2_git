/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/CachedConfig.h $
  * @author:  Ripple
  * @version: $Revision: #6 $
  *
  * Last modification: $DateTime: 2014/06/02 17:42:46 $
  * Last modified by:  $Author: huangjian $
  *
  */
#if !defined(AFX_CachedConfig_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
#define AFX_CachedConfig_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_

#include <string>
#include <map>
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/naming/src/NamedObject.h"
#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#include "core/utilities/src/RunParams.h"

class CachedConfig  : public TA_Base_Core::RunParamUser
{

public:

    enum ELocationType
    {
        LOCATION_TYPE_INVALID,
        LOCATION_TYPE_OCC,
        LOCATION_TYPE_DEPOT,
        LOCATION_TYPE_STATION
    };

    /*
     * onRunParamChange (interface implementation)
     *
     * @see RunParamUser::onRunParamChange for more details
     *
     */
	virtual void onRunParamChange(const std::string& name, const std::string& value);

    /**
     * getInstance
	 *
	 * Returns an instance of the class
     *
     * @return the pointer to this singleton instance
     *
     */
	static CachedConfig* getInstance();

    /**
     * removeInstance
	 *
	 * Removes the instance of the class (if already created) and cleanup the members.  Primarily 
     * used upon program termination (e.g. from main()) so that Purify does not consider this class
     * and its members to be leaks.
     *
     */
    static void removeInstance();

    void setEntityKey(unsigned long param);
    void setAgentLocKey(unsigned long param);

    /**
     * setSessionId
     *
     * Sets the session Id, and in doing so sets some internal data related
     *  to the session Id (via database access)
     *     
     */
    void setSessionId(const std::string& sessionId);

    void setPAAgentCorbaName(const TA_Base_Core::CorbaName& param);

    // The standard entity details below should not assert / throw since we may need to raise 
    // alarms with half-complete configuration.
    unsigned long       getEntityKey();
    unsigned long       getAgentLocKey();
    std::string         getSessionId();

    /**
     * getLocalConsoleRecordingSourceId
     *
     * @pre setSessionId has been called (this is point at which this 
     *          information loaded)
     */
    unsigned short getLocalConsoleRecordingSourceId();

    /**
     * getLocalConsolePlaybackZoneId
     *
     * @pre setSessionId has been called (this is point at which this 
     *          information loaded)
     */
    unsigned short getLocalConsolePlaybackZoneId();

    void setMftLocKey(unsigned long key);
    void setSchematicLocKey(unsigned long key);
    unsigned long getMftLocKey();
    unsigned long getSchematicLocKey();


    ELocationType getSchematicLocType();
	ELocationType getAgentLocType();

    bool isAtOcc();
    bool isAtDepot();
    bool isSchematicAtOcc();

    unsigned long getOccLocationKey();

    PAAgentNamedObject& getAgentObject() { return m_agentObject; }

private:

    CachedConfig();
	virtual ~CachedConfig();

   	static CachedConfig* m_me;
    static TA_Base_Core::NonReEntrantThreadLockable m_lockForConfigUpdate;

    // global parameters
    unsigned long m_entityKey;
    // Present the PAManager should connect to which PAAgent
    unsigned long m_ulAgentLocKey;
    // Present the PAManager launch from which location of the MFT
    unsigned long m_ulMftLocKey;
    // Present the PAManager is launch from which location of Schematic
    unsigned long m_ulSchematicLocKey;
    unsigned short m_pagingConsoleSourceId;
    unsigned short m_pagingConsoleZoneId;
    ELocationType m_eSchematicLocType;
    ELocationType m_eMftLocType;
	ELocationType m_eAgentLocType;
    
    // The Session (or GUI) identifier for this operator terminal
    std::string         m_sessionId;

    PAAgentNamedObject m_agentObject;
};


#endif // !defined(AFX_CachedConfig_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
