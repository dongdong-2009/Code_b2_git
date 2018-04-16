#include "DataPointCache.h"
#include "StringHelper.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Bus::DataPoint;
using TA_Base_Bus::DpValue;
using TA_Base_Bus::DataPointState;
using std::string;
using std::set;
using std::map;
using std::make_pair;
using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    DataPointCache::DataPointCache(void)
        : m_occServerNormalAlarmDatapoint(NULL),
          m_iscsCdbCurrentStisLibraryVersionDataPoint(NULL),
          m_iscsCdbNextStisLibraryVersionDataPoint(NULL),
          m_iscsCurrentStisLibraryVersionDataPoint(NULL),
          m_iscsNextStisLibraryVersionDataPoint(NULL),
          m_stisLibrarySynchronisedDataPoint(NULL),
          m_iscsCdbCurrentTrainLibraryVersionDataPoint(NULL),
          m_iscsCdbNextTrainLibraryVersionDataPoint(NULL),
          m_iscsCurrentTrainLibraryVersionDataPoint(NULL),
          m_iscsNextTrainLibraryVersionDataPoint(NULL),
          m_trainLibrarySynchronisedDataPoint(NULL),
          m_stisCurrentTrainLibraryVersionDataPoint(NULL),
          m_stisNextTrainLibraryVersionDataPoint(NULL)
    {
    }

    DataPointCache::~DataPointCache(void)
    {
        m_occServerNormalAlarmDatapoint = NULL;
        m_iscsCdbCurrentStisLibraryVersionDataPoint = NULL;
        m_iscsCdbNextStisLibraryVersionDataPoint = NULL;
        m_iscsCurrentStisLibraryVersionDataPoint = NULL;
        m_iscsNextStisLibraryVersionDataPoint = NULL;
        m_iscsCdbCurrentTrainLibraryVersionDataPoint = NULL;
        m_iscsCdbNextTrainLibraryVersionDataPoint = NULL;
        m_iscsCurrentTrainLibraryVersionDataPoint = NULL;
        m_iscsNextTrainLibraryVersionDataPoint = NULL;
        m_trainLibrarySynchronisedDataPoint = NULL;
        m_stisCurrentTrainLibraryVersionDataPoint = NULL;
        m_stisNextTrainLibraryVersionDataPoint = NULL;
        m_linkNormalAlarmDataPoints.clear();
        m_linkNormalAlarmDataPointMap.clear();
        m_stisCurrentStisLibraryVersionDataPointMap.clear();
        m_stisNextStisLibraryVersionDataPointMap.clear();
        m_pidDataPoints.clear();
        m_pidDataPointMap.clear();
        for(set<DataPoint*>::iterator it = m_allDataPoints.begin(); it != m_allDataPoints.end(); ++it)
        {
            DataPoint* dataPoint = *it;
            delete dataPoint;
        }
        m_allDataPoints.clear();
    }

    void DataPointCache::addDataPoint(DataPoint* dataPoint)
    {
        string name = dataPoint->getDataPointName();
#define ADD_DATAPOINT_IF_CONTAIN_TYPENAME(dataPoint, dataPointName, typeName, addFunc)\
    if(dataPointName.find(typeName) != string::npos)\
    {\
        addFunc(dataPoint);\
        return;\
    }
#define ADD_DATAPOINT_IF_END_WITH_TYPENAME(dataPoint, dataPointName, typeName, addFunc)\
    if(StringHelper::isStr1EndWithStr2(dataPointName, typeName))\
    {\
        addFunc(dataPoint);\
        return;\
    }
        ADD_DATAPOINT_IF_CONTAIN_TYPENAME(dataPoint, name, LCD_PID_NAME_CONTAIN_STR, addPidDataPoint);
        ADD_DATAPOINT_IF_CONTAIN_TYPENAME(dataPoint, name, LED_PID_NAME_CONTAIN_STR, addPidDataPoint);
        ADD_DATAPOINT_IF_END_WITH_TYPENAME(dataPoint, name, STIS_CURRENTLIBRARY_VERSION_TYPE, addStisCurrentStisLibraryVersionDataPoint);
        ADD_DATAPOINT_IF_END_WITH_TYPENAME(dataPoint, name, STIS_NEXTLIBRARY_VERSION_TYPE, addStisNextStisLibraryVersionDataPoint);
        ADD_DATAPOINT_IF_END_WITH_TYPENAME(dataPoint, name, LINKNORMALALARM_TYPE, addLinkNormalAlarmDataPoint);
        ADD_DATAPOINT_IF_END_WITH_TYPENAME(dataPoint, name, ISCS_STIS_CURRENTLIBRARY_VERSION_TYPE, addIscsCurrentStisLibraryVersionDatapoint);
        ADD_DATAPOINT_IF_END_WITH_TYPENAME(dataPoint, name, ISCS_STIS_NEXTLIBRARY_VERSION_TYPE, addIscsNextStisLibraryVersionDatapoint);
        ADD_DATAPOINT_IF_END_WITH_TYPENAME(dataPoint, name, ISCS_CDB_STIS_NEXTLIBRARY_VERSION_TYPE, addIscsCdbNextStisLibraryVersionDatapoint);
        ADD_DATAPOINT_IF_END_WITH_TYPENAME(dataPoint, name, ISCS_CDB_STIS_CURRENTLIBRARY_VERSION_TYPE, addIscsCdbCurrentStisLibraryVersionDatapoint);
        ADD_DATAPOINT_IF_END_WITH_TYPENAME(dataPoint, name, STATION_LIBRARIES_SYNCHRONISED_TYPE, addStisLibrarySynchronisedDatapoint);
        ADD_DATAPOINT_IF_END_WITH_TYPENAME(dataPoint, name, OCC_SERVERSTATUS_ALARM_TYPE, addOccServerNormalAlarmDataPoint);
        // Train
        ADD_DATAPOINT_IF_END_WITH_TYPENAME(dataPoint, name, TTIS_CURRENTLIBRARY_VERSION_TYPE, addStisCurrentTrainLibraryVersionDataPoint);
        ADD_DATAPOINT_IF_END_WITH_TYPENAME(dataPoint, name, TTIS_NEXTLIBRARY_VERSION_TYPE, addStisNextTrainLibraryVersionDataPoint);
        ADD_DATAPOINT_IF_END_WITH_TYPENAME(dataPoint, name, ISCS_TTIS_CURRENTLIBRARY_VERSION_TYPE, addIscsCurrentTrainLibraryVersionDatapoint);
        ADD_DATAPOINT_IF_END_WITH_TYPENAME(dataPoint, name, ISCS_TTIS_NEXTLIBRARY_VERSION_TYPE, addIscsNextTrainLibraryVersionDatapoint);
        ADD_DATAPOINT_IF_END_WITH_TYPENAME(dataPoint, name, ISCS_CDB_TTIS_NEXTLIBRARY_VERSION_TYPE, addIscsCdbNextTrainLibraryVersionDatapoint);
        ADD_DATAPOINT_IF_END_WITH_TYPENAME(dataPoint, name, ISCS_CDB_TTIS_CURRENTLIBRARY_VERSION_TYPE, addIscsCdbCurrentTrainLibraryVersionDatapoint);
        ADD_DATAPOINT_IF_END_WITH_TYPENAME(dataPoint, name, TRAIN_LIBRARIES_SYNCHRONISED_TYPE, addTrainLibrarySynchronisedDatapoint);
#undef ADD_DATAPOINT_IF_END_WITH_TYPENAME
#undef ADD_DATAPOINT_IF_CONTAIN_TYPENAME
    }

    DataPoint* DataPointCache::getPidDataPoint(const string& firstLevelName,
                                               const int address,
                                               const string& pidType)
    {
        DataPoint* r = NULL;
        if(m_pidDataPointMap.find(firstLevelName) != m_pidDataPointMap.end())
        {
            PidAddressMap& pidAddressMap = m_pidDataPointMap[firstLevelName];
            if(pidAddressMap.find(address) != pidAddressMap.end())
            {
                PidTypeMap& pidTypeMap = pidAddressMap[address];
                if(pidTypeMap.find(pidType) != pidTypeMap.end())
                {
                    r = pidTypeMap[pidType];
                }
            }
        }
        return r;
    }

    void DataPointCache::addPidDataPoint(DataPoint* dataPoint)
    {
        string address = dataPoint->getDataPointAddress();
        string dataPointName = dataPoint->getDataPointName();
        // Address of PID should be a number
        if(address.find_first_not_of("0123456789") == string::npos)
        {
            int addressValue = atoi(address.c_str());
            string pidFirstLevelName = getDataPointFirstLevelName(dataPoint);
            string typeName = dataPointName.substr(dataPointName.find_last_of('-'));
            if(typeName == string("-") + PID_STATUS_TYPE)
            {
                typeName = PID_STATUS_TYPE;
            }
            else if(typeName == string("-") + PID_ON_TYPE)
            {
                typeName = PID_ON_TYPE;
            }
            else if(typeName == string("-") + PID_OFF_TYPE)
            {
                typeName = PID_OFF_TYPE;
            }
            if(m_pidDataPointMap.find(pidFirstLevelName) == m_pidDataPointMap.end())
            {
                PidAddressMap m;
                m_pidDataPointMap.insert(make_pair(pidFirstLevelName, m));
            }
            PidAddressMap& addressMap = m_pidDataPointMap[pidFirstLevelName];
            if(addressMap.find(addressValue) == addressMap.end())
            {
                PidTypeMap m;
                addressMap.insert(make_pair(addressValue, m));
            }
            PidTypeMap& typeMap = addressMap[addressValue];
            typeMap.insert(make_pair(typeName, dataPoint));
            m_pidDataPoints.insert(dataPoint);
            m_allDataPoints.insert(dataPoint);
        }
    }

    DataPoint* DataPointCache::getOccServerNormalAlarmDataPoint()
    {
        return m_occServerNormalAlarmDatapoint;
    }

    void DataPointCache::addOccServerNormalAlarmDataPoint(DataPoint* dataPoint)
    {
        m_occServerNormalAlarmDatapoint = dataPoint;
        m_allDataPoints.insert(dataPoint);
    }

    DataPoint* DataPointCache::getLinkNormalAlarmDataPoint(const string& firstLevelName)
    {
        DataPoint* r = NULL;
        if(m_linkNormalAlarmDataPointMap.find(firstLevelName) != m_linkNormalAlarmDataPointMap.end())
        {
            r = m_linkNormalAlarmDataPointMap[firstLevelName];
        }
        return r;
    }

    void DataPointCache::addLinkNormalAlarmDataPoint(DataPoint* dataPoint)
    {
        m_linkNormalAlarmDataPoints.insert(dataPoint);
        string firstLevelName = getDataPointFirstLevelName(dataPoint);
        m_linkNormalAlarmDataPointMap.insert(make_pair(firstLevelName, dataPoint));
        m_allDataPoints.insert(dataPoint);
    }

    DataPoint* DataPointCache::getIscsCdbCurrentStisLibraryVersionDatapoint()
    {
        return m_iscsCdbCurrentStisLibraryVersionDataPoint;
    }

    void DataPointCache::addIscsCdbCurrentStisLibraryVersionDatapoint(DataPoint* dataPoint)
    {
        m_iscsCdbCurrentStisLibraryVersionDataPoint = dataPoint;
        m_allDataPoints.insert(dataPoint);
    }

    DataPoint* DataPointCache::getIscsCdbNextStisLibraryVersionDatapoint()
    {
        return m_iscsCdbNextStisLibraryVersionDataPoint;
    }

    void DataPointCache::addIscsCdbNextStisLibraryVersionDatapoint(DataPoint* dataPoint)
    {
        m_iscsCdbNextStisLibraryVersionDataPoint = dataPoint;
        m_allDataPoints.insert(dataPoint);
    }

    DataPoint* DataPointCache::getIscsCurrentStisLibraryVersionDatapoint()
    {
        return m_iscsCurrentStisLibraryVersionDataPoint;
    }

    void DataPointCache::addIscsCurrentStisLibraryVersionDatapoint(DataPoint* dataPoint)
    {
        m_iscsCurrentStisLibraryVersionDataPoint = dataPoint;
        m_allDataPoints.insert(dataPoint);
    }

    DataPoint* DataPointCache::getIscsNextStisLibraryVersionDatapoint()
    {
        return m_iscsNextStisLibraryVersionDataPoint;
    }

    void DataPointCache::addIscsNextStisLibraryVersionDatapoint(DataPoint* dataPoint)
    {
        m_iscsNextStisLibraryVersionDataPoint = dataPoint;
        m_allDataPoints.insert(dataPoint);
    }

    DataPoint* DataPointCache::getStisCurrentStisLibraryVersionDataPoint(const string& firstLevelName)
    {
        DataPoint* r = NULL;
        if(m_stisCurrentStisLibraryVersionDataPointMap.find(firstLevelName)
                != m_stisCurrentStisLibraryVersionDataPointMap.end())
        {
            r = m_stisCurrentStisLibraryVersionDataPointMap[firstLevelName];
        }
        return r;
    }

    void DataPointCache::addStisCurrentStisLibraryVersionDataPoint(DataPoint* dataPoint)
    {
        string firstLevelName = getDataPointFirstLevelName(dataPoint);
        m_stisCurrentStisLibraryVersionDataPointMap.insert(make_pair(firstLevelName, dataPoint));
        m_allDataPoints.insert(dataPoint);
    }

    DataPoint* DataPointCache::getStisNextStisLibraryVersionDataPoint(const string& firstLevelName)
    {
        DataPoint* r = NULL;
        if(m_stisNextStisLibraryVersionDataPointMap.find(firstLevelName)
                != m_stisNextStisLibraryVersionDataPointMap.end())
        {
            r = m_stisNextStisLibraryVersionDataPointMap[firstLevelName];
        }
        return r;
    }

    void DataPointCache::addStisNextStisLibraryVersionDataPoint(DataPoint* dataPoint)
    {
        string firstLevelName = getDataPointFirstLevelName(dataPoint);
        m_stisNextStisLibraryVersionDataPointMap.insert(make_pair(firstLevelName, dataPoint));
        m_allDataPoints.insert(dataPoint);
    }

    DataPoint* DataPointCache::getStisLibrarySynchronisedDatapoint()
    {
        return m_stisLibrarySynchronisedDataPoint;
    }

    void DataPointCache::addStisLibrarySynchronisedDatapoint(DataPoint* dataPoint)
    {
        m_stisLibrarySynchronisedDataPoint = dataPoint;
        m_allDataPoints.insert(dataPoint);
    }

    void DataPointCache::setDataPointsToControlMode()
    {
        for(set<DataPoint*>::iterator it = m_allDataPoints.begin();
                it != m_allDataPoints.end(); ++it)
        {
            DataPoint* dataPoint = *it;
            dataPoint->setToControlMode();
        }
    }

    void DataPointCache::setDataPointsToMonitorMode()
    {
        for(set<DataPoint*>::iterator it = m_allDataPoints.begin();
                it != m_allDataPoints.end(); ++it)
        {
            DataPoint* dataPoint = *it;
            dataPoint->setToMonitorMode();
        }
    }

    void DataPointCache::setDataPointsToBadQuality()
    {
        setOccServerNormalAlarmDataPointToBadQuality();
        setLinkNormalAlarmDataPointsToBadQuality();
        setPidDataPointsToBadQuality();
    }

    string DataPointCache::getDataPointFirstLevelName(DataPoint* dataPoint)
    {
        string name = dataPoint->getDataPointName();
        return name.substr(0, name.find_first_of('.'));
    }

    void DataPointCache::setPidDataPointsToBadQuality()
    {
        time_t now = time(NULL);
        for(set<DataPoint*>::iterator it = m_pidDataPoints.begin();
                it != m_pidDataPoints.end(); ++it)
        {
            setDataPointToBadQuality(*it);
        }
    }

    void DataPointCache::setLinkNormalAlarmDataPointsToBadQuality()
    {
        time_t now = time(NULL);
        for(set<DataPoint*>::iterator it = m_linkNormalAlarmDataPoints.begin();
                it != m_linkNormalAlarmDataPoints.end(); ++it)
        {
            setDataPointToBadQuality(*it);
        }
    }

    void DataPointCache::setOccServerNormalAlarmDataPointToBadQuality()
    {
        if(m_occServerNormalAlarmDatapoint != NULL)
        {
            setDataPointToBadQuality(m_occServerNormalAlarmDatapoint);
        }
    }

    void DataPointCache::setDataPointToBadQuality(DataPoint* dataPoint)
    {
        time_t now = time(NULL);
        DpValue dpValue = dataPoint->getCurrentValue();
        DataPointState dataPointState(dpValue, now, TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE);
        try
        {
            dataPoint->updateFieldState(dataPointState);
        }
        catch(...)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed set datapoint:%s to bad quality",
                        dataPoint->getDataPointName().c_str());
        }
    }

    TA_Base_Bus::DataPoint* DataPointCache::getIscsCdbCurrentTrainLibraryVersionDatapoint()
    {
        return m_iscsCdbCurrentTrainLibraryVersionDataPoint;
    }

    TA_Base_Bus::DataPoint* DataPointCache::getIscsCdbNextTrainLibraryVersionDatapoint()
    {
        return m_iscsCdbNextTrainLibraryVersionDataPoint;
    }

    TA_Base_Bus::DataPoint* DataPointCache::getIscsCurrentTrainLibraryVersionDatapoint()
    {
        return m_iscsCurrentTrainLibraryVersionDataPoint;
    }

    TA_Base_Bus::DataPoint* DataPointCache::getIscsNextTrainLibraryVersionDatapoint()
    {
        return m_iscsNextTrainLibraryVersionDataPoint;
    }

    TA_Base_Bus::DataPoint* DataPointCache::getStisCurrentTrainLibraryVersionDataPoint()
    {
        return m_stisCurrentTrainLibraryVersionDataPoint;
    }

    TA_Base_Bus::DataPoint* DataPointCache::getStisNextTrainLibraryVersionDataPoint()
    {
        return m_stisNextTrainLibraryVersionDataPoint;
    }

    TA_Base_Bus::DataPoint* DataPointCache::getTrainLibrarySynchronisedDatapoint()
    {
        return m_trainLibrarySynchronisedDataPoint;
    }

    void DataPointCache::addStisNextTrainLibraryVersionDataPoint(TA_Base_Bus::DataPoint* dataPoint)
    {
        m_stisNextTrainLibraryVersionDataPoint = dataPoint;
        m_allDataPoints.insert(dataPoint);
    }

    void DataPointCache::addStisCurrentTrainLibraryVersionDataPoint(TA_Base_Bus::DataPoint* dataPoint)
    {
        m_stisCurrentTrainLibraryVersionDataPoint = dataPoint;
        m_allDataPoints.insert(dataPoint);
    }

    void DataPointCache::addTrainLibrarySynchronisedDatapoint(TA_Base_Bus::DataPoint* dataPoint)
    {
        m_trainLibrarySynchronisedDataPoint = dataPoint;
        m_allDataPoints.insert(dataPoint);
    }

    void DataPointCache::addIscsNextTrainLibraryVersionDatapoint(TA_Base_Bus::DataPoint* dataPoint)
    {
        m_iscsNextTrainLibraryVersionDataPoint = dataPoint;
        m_allDataPoints.insert(dataPoint);
    }

    void DataPointCache::addIscsCurrentTrainLibraryVersionDatapoint(TA_Base_Bus::DataPoint* dataPoint)
    {
        m_iscsCurrentTrainLibraryVersionDataPoint = dataPoint;
        m_allDataPoints.insert(dataPoint);
    }

    void DataPointCache::addIscsCdbNextTrainLibraryVersionDatapoint(TA_Base_Bus::DataPoint* dataPoint)
    {
        m_iscsCdbNextTrainLibraryVersionDataPoint = dataPoint;
        m_allDataPoints.insert(dataPoint);
    }

    void DataPointCache::addIscsCdbCurrentTrainLibraryVersionDatapoint(TA_Base_Bus::DataPoint* dataPoint)
    {
        m_iscsCdbCurrentTrainLibraryVersionDataPoint = dataPoint;
        m_allDataPoints.insert(dataPoint);
    }

}
