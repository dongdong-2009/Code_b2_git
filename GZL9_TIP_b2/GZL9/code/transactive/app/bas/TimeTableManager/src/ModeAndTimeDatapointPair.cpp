#include "StdAfx.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "app/bas/TimeTableManager/src/ModeAndTimeDatapointPair.h"
#include "app/bas/TimeTableManager/src/TimeTableDatapointPairManager.h"


namespace TA_IRS_App
{
	ModeAndTimeDatapointPair::ModeAndTimeDatapointPair(unsigned long locationKey, const std::string& strModeCodeName, const std::string& strBeginTime) : 
    m_dpModeCode(),
    m_dpBeginTime(),
    m_smaInitialization( 0 ),
    m_strModeCodeName(strModeCodeName),
    m_strBeginTime(strBeginTime),
    m_locationKey(locationKey),
    m_ulValidProxyNum(0u),
    m_ulInitedProxyNum(0u)
	{
	}

	ModeAndTimeDatapointPair::~ModeAndTimeDatapointPair(void)
	{
	}

	void ModeAndTimeDatapointPair::initializeDatapoint(const std::vector<TA_Base_Core::DataPointEntityDataPtr>& myDatapoints)
	{
		TA_Base_Bus::ScadaProxyFactory& refScadaFactory = TA_Base_Bus::ScadaProxyFactory::getInstance();
		refScadaFactory.setProxiesToControlMode();

        std::vector<TA_Base_Core::DataPointEntityDataPtr>::const_iterator iterPt;
		bool modeCodeOK = false;
		bool beginTimeOK = false;
        m_ulValidProxyNum = 0u;
        m_ulInitedProxyNum = 0u;
		for ( iterPt = myDatapoints.begin(); iterPt != myDatapoints.end(); ++iterPt)
		{
			std::string strDataPointName = (*iterPt)->getName();

			if (strDataPointName == m_strModeCodeName)
			{
				refScadaFactory.createDataPointProxy((*iterPt), *this, m_dpModeCode);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "datapoint [%s] is created.", strDataPointName.c_str());
				modeCodeOK = true;
                ++m_ulValidProxyNum;
			}
			else if(strDataPointName == m_strBeginTime)
			{
				refScadaFactory.createDataPointProxy((*iterPt), *this, m_dpBeginTime);				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "datapoint [%s] is created.", strDataPointName.c_str());
                beginTimeOK = true;
                ++m_ulValidProxyNum;
			}
			if (modeCodeOK && beginTimeOK)
			{
				break;
			}
		}

		TA_Base_Bus::ScadaProxyFactory::removeInstance();
	}

	void ModeAndTimeDatapointPair::uninit()
	{
		FUNCTION_ENTRY("prepareToExit");

		m_dpBeginTime.reset();
		m_dpModeCode.reset();

		FUNCTION_EXIT;
	}

	bool ModeAndTimeDatapointPair::getModeCodeAndBeginTime(unsigned long& modeCode, time_t& beginTime)
	{
		FUNCTION_ENTRY("getModeCodeAndBeginTime");

		bool bSuccess = false;
		BAS_TRY
		{
			unsigned long qualityStatusTemp = TA_Base_Bus::QUALITY_MASK & (m_dpModeCode->getStatus());
			if(TA_Base_Bus::QUALITY_GOOD == qualityStatusTemp)
			{
				modeCode = static_cast<unsigned long>(m_dpModeCode->getValueAsDouble());
				beginTime = translateDisplayBeginTime(static_cast<unsigned long>(m_dpBeginTime->getValueAsDouble()));
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getModeCodeAndBeginTime modeCode=%lu beginTimeorg=%f BeginTime=%lu", 
					modeCode, m_dpBeginTime->getValueAsDouble(), beginTime);
				bSuccess = true;
			}
		}
		BAS_CATCH;

		FUNCTION_EXIT;
		return bSuccess;
	}

	void ModeAndTimeDatapointPair::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY("processEntityUpdateEvent");

        LOG_GENERIC(SourceInfo ,TA_Base_Core::DebugUtil::DebugDebug, 
            "processEntityUpdateEvent datapoint key:%d  TA_Base_Bus::ScadaEntityUpdateType:%d",
            entityKey, static_cast<unsigned long>(updateType) );

		switch (updateType)
        {
        case TA_Base_Bus::AgentConnected:
            m_smaInitialization.post();
            break;
		default:
			break;
        }

        if ( updateType != TA_Base_Bus::ValueStateUpdate )
        {
            return;
        }

        if ( entityKey == getTimeDpKey() )
        {
            TimeTableDatapointPairManagerInstance::instance()->updateModeBeginTime( m_dpModeCode->getLocationKey(),  getModeDpKey() );
        }
        else if ( entityKey == getModeDpKey() )
        {
            TimeTableDatapointPairManagerInstance::instance()->updateModeBeginTime( m_dpModeCode->getLocationKey(),  entityKey );
        }

		FUNCTION_EXIT;
	}

	std::string ModeAndTimeDatapointPair::getModeDataPointName()
	{
		return m_strModeCodeName;
	}

	std::string ModeAndTimeDatapointPair::getBeginTimeDataPointName()
	{
		return m_strBeginTime;
	}

	bool ModeAndTimeDatapointPair::getDpValueByDataPointKey(unsigned long dpEntityKey, unsigned long& dpValue, bool& isModeCode)
	{
		FUNCTION_ENTRY("getDpValueByDataPointKey");
		bool bSuccess = false;
		isModeCode = true;

		BAS_TRY
		{
			if (dpEntityKey == getModeDpKey())
			{
				dpValue = static_cast<unsigned long>(m_dpModeCode->getValueAsDouble());
				isModeCode = true;
				bSuccess = true;
			}
			else if (dpEntityKey == getTimeDpKey())
			{
				dpValue = static_cast<unsigned long>(m_dpBeginTime->getValueAsDouble());
				isModeCode = false;
				bSuccess = true;
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unknown entity key %lu, will ignore it update", dpEntityKey);
			}
		}
		BAS_CATCH;

		FUNCTION_EXIT;
		return bSuccess;
	}

	std::string ModeAndTimeDatapointPair::regulateFeedbackBeginTime(const std::string& strBeginTime)
	{
        FUNCTION_ENTRY("regulateFeedbackBeginTime");

		std::string strNewBeginTime = strBeginTime;

		if (strBeginTime.size() == 3) //format is "744" , in this case the original begin time is 0744
		{
			strNewBeginTime = "0" + strNewBeginTime;
		}
		else if (strBeginTime.size() == 2) //format is "44" , in this case the original begin time is 0044
		{
			strNewBeginTime = "00" + strNewBeginTime;
		}
		else if (strBeginTime.size() == 1) //format is "4", in this case the original begin time is 0004
		{
			strNewBeginTime = "000" + strNewBeginTime;
		}
		else if (strBeginTime.size() == 0) //format is "", in this case the original begin time is 0000
		{
			strNewBeginTime = "0000";
		}

		FUNCTION_EXIT;
		return strNewBeginTime;
	}

	time_t ModeAndTimeDatapointPair::translateDisplayBeginTime(unsigned long modbusValue)
	{
		int ulHour = (modbusValue & 0x0000FF00) >> 8;
        int ulMinute = modbusValue & 0x000000FF;

		// translate 0x1030 to 1648
        time_t tmtNow = time( NULL );
        tm tmNow = *localtime( &tmtNow );
        tmNow.tm_min = ulMinute;
        tmNow.tm_hour = ulHour;

		return mktime( &tmNow );
	}

    void ModeAndTimeDatapointPair::waitInitialization()
    {
        for ( unsigned long ulLoop = m_ulInitedProxyNum; ulLoop < m_ulValidProxyNum; ++ulLoop )
        {
            m_smaInitialization.wait();
            ++m_ulInitedProxyNum;
        }
    }

    void ModeAndTimeDatapointPair::cancelWaitInitialization()
    {
        for ( unsigned long ulLoop = m_ulInitedProxyNum; ulLoop < m_ulValidProxyNum; ++ulLoop )
        {
            m_smaInitialization.post();
            --m_ulInitedProxyNum;
        }
    }

    const unsigned long ModeAndTimeDatapointPair::getDpKey()
    {
        return getModeDpKey();
    }

    const unsigned long ModeAndTimeDatapointPair::getModeDpKey()
    {
        unsigned long ulEntityKey = TA_Base_Core::DatabaseKey::getInvalidKey();

        try
        {
            ulEntityKey = m_dpModeCode->getEntityKey();
        }
        catch ( const std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Caught unknown exception when getModeDpKey" );
        }

        return ulEntityKey;
    }

    const unsigned long ModeAndTimeDatapointPair::getTimeDpKey()
    {
        unsigned long ulEntityKey = TA_Base_Core::DatabaseKey::getInvalidKey();

        try
        {
            ulEntityKey = m_dpBeginTime->getEntityKey();
        }
        catch ( const std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Caught unknown exception when getTimeDpKey" );
        }

        return ulEntityKey;
    }
}
