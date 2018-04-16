/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Huang Qi
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#ifndef PADATAMANAGER_H__INCLUDED_
#define PADATAMANAGER_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct DVAMsgInfo
{
    std::string strID;
    std::string strName;
    std::string strDesc;
};

namespace TA_Base_Core
{
    class RunParams;
    class PaDvaMessageAccessFactory;
    class PaBroadcastShortcutAccessFactory;
    class PaZoneGroupAccessFactory;
    class PaZoneAccessFactory;
}

namespace TA_IRS_Bus
{
    class CachedMaps;
}

class IDVAMsgShortcutConfigObserver;
class HmiSelectionListener;
class CachedConfig;

typedef std::pair<unsigned long, std::string> ZonesKeyNamePair;
typedef std::vector<ZonesKeyNamePair> ZonesKeyNamesVector;
typedef std::map<unsigned long, ZonesKeyNamesVector> GroupToZonesMap;
typedef std::map<unsigned long, DVAMsgInfo*> DvaMsgKeyToDVAMsgPtrMap;

class PaDataManager
{
public:
    static void createInstance();
    static void destoryInstance();
	static PaDataManager& getInstance();
	virtual ~PaDataManager();

    // PA DVA message functions
    void getDVAMsgs( const std::string& strFilterWord, int nType, DvaMsgKeyToDVAMsgPtrMap& mapRequestedRecords );
    DVAMsgInfo* getDVAMsgByKey( unsigned long ulKey );
    DVAMsgInfo* getDVAMsgByTypeAndKey( int nType, unsigned long ulKey );
    void updateDVAMsgMap();
    int getDVAMsgTypeByKey( unsigned long ulKey );
    int modifyDvaMsgDesc( unsigned long ulMsgKey, const std::string& strMsgDesc );

    // RunParams functions
    void updateRunParams();

    // PA broadcast shortcuts functions
    void updateDVAMsgShortcuts();
    unsigned long getDVAMsgKeyByShortcutIndex( unsigned long ulIndex );
    bool getDVAMsgShortcutNameByIndex( std::string& strName, unsigned long ulIndex );
    bool modifyDVAMsgShortcut( unsigned long ulIndex, unsigned long ulDVAMsgKey );
    std::map<unsigned long, unsigned long>& getExistShortcutNos();

    // DVA message shortcut observer function
    void registerDVAMsgShortcutObserver( IDVAMsgShortcutConfigObserver* pObserver );
    void deregisterDVAMsgShortcutObserver( IDVAMsgShortcutConfigObserver* pObserver );

    // PA zone and zone group functions
    void updateZoneGroups();
    std::map<unsigned long, std::string>& getAllZoneGroups();
    std::vector<std::string> getZoneNamesByGroupKey( unsigned long ulGroupKey );
    void getStationZoneNameByZoneKey( std::vector<std::string>& vecZones, const std::vector<unsigned long>& listZoneKeys );
    bool isGroupNameExist( const std::string& strGroupName );
    std::vector<unsigned long> getZoneKeysByGroupKey( unsigned long ulGroupKey );
    /* Return SUCCESS_NO_ERROR if successful, return others if failed */
    int deleteZoneGroupByGroupKey( unsigned long ulGroupKey );
    /* Return SUCCESS_NO_ERROR if successful, return others if failed */
    int createZoneGroupByZoneKeys( unsigned long& ulNewGroupKey, const std::string& strGroupName, const std::vector<unsigned long>& lstZoneKeys );
    bool isZoneGroupsCountOverLimit();

    // GUI settings
    int updateDataFromAgent();
    bool isAgentDataInvalid();

protected:
    PaDataManager();
    // PA DVA message functions
    void getAllDVAMsgFromDatabase();
    void clearAllDVAMsgInfos();

protected:
    static PaDataManager* s_pInstance;

    TA_Base_Core::PaDvaMessageAccessFactory& m_refDVAMsgAccessFactory;
    TA_Base_Core::PaBroadcastShortcutAccessFactory& m_refPaBrcScAccessFactory;
    TA_Base_Core::PaZoneGroupAccessFactory& m_refPaZoneGroupAccessFactory;
    TA_Base_Core::PaZoneAccessFactory& m_refPaZoneAccessFactory;
    TA_Base_Core::RunParams& m_refRunParams;
    CachedConfig& m_refCachedConfig;

    // Separation types DVA message for fast search
    DvaMsgKeyToDVAMsgPtrMap m_mapEmergencyDVAMsgInfos;
    DvaMsgKeyToDVAMsgPtrMap m_mapNormalDVAMsgInfos;
    DvaMsgKeyToDVAMsgPtrMap m_mapOtherDVAMsgInfos;

    std::vector<IDVAMsgShortcutConfigObserver*> m_vecDVAMsgShortcutObservers;

    std::map<unsigned long, unsigned long> m_mapBroadcastShortcuts;
    std::map<unsigned long, std::string> m_mapZoneGroups;
    GroupToZonesMap m_mapGroupToZones;

    unsigned long m_ulLocationkey;
    unsigned long m_ulSigBroadcastDelayTime;

    bool m_bSigBroadcastEnable;
    bool m_bAgentDataInvalid;
};

#endif // !defined(PADATAMANAGER_H__INCLUDED_)
