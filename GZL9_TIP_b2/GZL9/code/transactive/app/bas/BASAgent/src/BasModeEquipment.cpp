/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/BasModeEquipment.cpp $
  * @author:	Huang Jian
  * @version:	$Revision: #4 $
  *
  * Last modification:	$DateTime: 2016/01/28 15:26:34 $
  * Last modified by:	$Author: qi.huang $
  *
  * Description:
  *
  *
  */
#include "app/bas/Common/BasDefinitions.h"
#include "app/bas/BASAgent/src/BasModeEquipment.h"
#include "app/bas/BASAgent/src/BasCachedConfig.h"
#include "app/bas/BASAgent/src/BasEventSubmitter.h"
#include "app/bas/BASAgent/src/DataPointUpdateProcessor.h"
#include "app/bas/BASAgent/src/ModeExecutionException.h"
#include "app/bas/BASAgent/src/BASAgent.h"
#include "bus/bas/bas_agent/IDL/src/IStationBasAgentCorbaDef.h"
#include "core/utilities/src/DataConversion.h"
#include "core/message/types/ECSAudit_MessageTypes.h"

namespace TA_IRS_App
{
	BasModeEquipment::BasModeEquipment(const std::string& nodeName) : 
    m_nodeName(nodeName),
	m_modeControl(NULL),
	m_modeAutoManual(NULL),
	m_modeInProgress(NULL),
	m_modeValue(NULL),
    m_modeRightCtrlPt(NULL),
    m_modeValueInit(false)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BAS Equipement [%s] created.", m_nodeName.c_str());
		m_modeControl = BasCachedConfig::getInstance().getModeCtrlDPFromNode(m_nodeName);
		m_modeAutoManual = BasCachedConfig::getInstance().getModeAutoManualDPFromNode(m_nodeName);
		m_modeInProgress = BasCachedConfig::getInstance().getModeInProgressDPFromNode(m_nodeName);
		m_modeValue = BasCachedConfig::getInstance().getModeValueDPFromNode(m_nodeName);
        m_modeRightCtrlPt = BasCachedConfig::getInstance().getModeRightControlDPFromNode(m_nodeName);

        if (NULL != m_modeControl)
        {
            DataPointUpdateProcessor::instance()->registerDatapointUpdate(m_modeControl->getEntityKey(), this);
        }

        if (NULL != m_modeRightCtrlPt)
        {
            DataPointUpdateProcessor::instance()->registerDatapointUpdate(m_modeRightCtrlPt->getEntityKey(), this);
        }

        if (NULL != m_modeValue)
        {
            DataPointUpdateProcessor::instance()->registerDatapointUpdate(m_modeValue->getEntityKey(), this);
        }
	}

	BasModeEquipment::~BasModeEquipment()
	{
	}

	void BasModeEquipment::executeStationMode(unsigned long modeValue, bool canOverrideStationModeInProgress, const std::string& sessionID)
	{
		FUNCTION_ENTRY("executeStationMode");

		std::string strException = "";
		if (m_modeControl == NULL)
		{
			strException = m_nodeName + " do not have mode control point!";
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "%s", strException.c_str());
			TA_THROW(TA_Base_Core::TransactiveException(strException));
		}

		if (m_modeAutoManual == NULL)
		{
			strException = m_nodeName + " do not have mode auto manual point!";
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "%s", strException.c_str());
			TA_THROW(TA_Base_Core::TransactiveException(strException));
		}

		if (m_modeInProgress == NULL)
		{
			strException = m_nodeName + " do not have mode in progress point!";
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "%s", strException.c_str());
			TA_THROW(TA_Base_Core::TransactiveException(strException));
		}

		//check auto manual status
        int nAutoManStatus = 0;

        if ( TA_Base_Bus::DPT_ANALOGUE == m_modeAutoManual->getDataPointType() )
        {
            nAutoManStatus = static_cast<int>(m_modeAutoManual->getCurrentValue().getFloat());
        }
        else if ( (TA_Base_Bus::DPT_DERIVED == m_modeAutoManual->getDataPointType())  || (TA_Base_Bus::DPT_ENUM == m_modeAutoManual->getDataPointType()) )
        {
            nAutoManStatus = m_modeAutoManual->getCurrentValue().getEnum();
        }
        else if ( TA_Base_Bus::DPT_BOOLEAN == m_modeAutoManual->getDataPointType() )
        {
            nAutoManStatus = m_modeAutoManual->getCurrentValue().getBoolean() ? eControlManual : eControlAuto;
        }

		if ( nAutoManStatus != eControlManual )
		{
 			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "can not execute station mode, auto manual value is: %d", nAutoManStatus);
			TA_THROW(ModeExecutionException(ModeExecutionException::MODE_NOT_IN_SINGLE_CONTROL, "Auto Manual State is not Manual"));
		}

		//check mode in progress
        int curModeInProgressValue = 0;

        if ( TA_Base_Bus::DPT_ANALOGUE == m_modeInProgress->getDataPointType() )
        {
            curModeInProgressValue = static_cast<int>(m_modeInProgress->getCurrentValue().getFloat());
        }
        else if ( (TA_Base_Bus::DPT_DERIVED == m_modeInProgress->getDataPointType()) || (TA_Base_Bus::DPT_ENUM == m_modeInProgress->getDataPointType()))
        {
            curModeInProgressValue = m_modeInProgress->getCurrentValue().getEnum();
        }
        else if ( TA_Base_Bus::DPT_BOOLEAN == m_modeInProgress->getDataPointType() )
        {
            curModeInProgressValue = m_modeInProgress->getCurrentValue().getBoolean() ? eModeExecuting : eModeStateUnknown;
        }

		if ( (!canOverrideStationModeInProgress) && (curModeInProgressValue == eModeExecuting))
		{
			TA_THROW(ModeExecutionException(ModeExecutionException::MODE_IN_PROGRESS, "last mode is in progress"));
		}

		struct timeb currentTime;
		ftime(&currentTime);

		TA_Base_Bus::DataPointValue dpValue;
		dpValue.dpType = TA_Base_Bus::DPT_INT16_TYPE;
		dpValue.quality = TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON;
		dpValue.timeStamp = currentTime;
		dpValue.floatValue = modeValue;

		try
		{
			m_modeControl->WriteDataPointValue(sessionID, dpValue, 0, "");
		}
		catch (const TA_Base_Core::BadParameterException& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "BadParameterException", e.reason.in());
			throw;
		}
		catch (const std::exception& e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, typeid(e).name(), e.what());
			throw;
		}
		catch ( ... )
		{
			strException = "Unknown exception when call WriteDataPointValue.";
			LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", strException);
			throw;
		}

		FUNCTION_EXIT;
	}

	unsigned long BasModeEquipment::getCurrentModeValue()
	{
		unsigned long modeValue = 0;

		if(NULL != m_modeValue)
		{
			modeValue = static_cast<unsigned long>(m_modeValue->getCurrentValue().getFloat());
		}
		else
		{
			TA_THROW(TA_Base_Core::TransactiveException("Mode value datapoint not initialized"));
		}

        return modeValue;
	}

    void BasModeEquipment::processDataPointControlStateUpdate( const unsigned long dpEntityKey, const int nControlState, const TA_Base_Bus::DataPointControl& controlState )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "processDataPointControlStateUpdate %d", dpEntityKey);

        if (dpEntityKey == m_modeRightCtrlPt->getEntityKey())
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "process auto/manual control state %d", nControlState);

			BasEventSubmitter::instance()->submitBasSystemModeControlEvent(m_modeRightCtrlPt->getEventMessage(), nControlState, controlState.triggerringDpValue, m_modeRightCtrlPt->getLastOperatorSession(), dpEntityKey, m_modeRightCtrlPt->getAssetName());
        }
        else if (dpEntityKey == m_modeControl->getEntityKey())
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "process bas mode control state %d", nControlState);

            TA_IRS_Core::Mode_SmartPtr basMode = BasCachedConfig::getInstance().getBasModeFromValue(TA_Base_Core::DataConversion::toUnsignedInteger(controlState.triggerringDpValue));

            BasEventSubmitter::instance()->submitBasSystemModeExecutionEvent( basMode->getDescription(), nControlState, basMode->getName(), m_modeControl->getLastOperatorSession(), dpEntityKey, m_modeControl->getAssetName());
        }
    }

    void BasModeEquipment::processDataPointValueUpdate( const unsigned long dpEntityKey, const TA_Base_Bus::DataPointValue& updateValue )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "processDataPointValueUpdate %d", dpEntityKey);

        if (dpEntityKey == m_modeValue->getEntityKey())
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "process mode value change %f", updateValue.floatValue);

            TA_THREADGUARD(m_modeValueInitLock);
			if (TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON != updateValue.quality)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "process mode value change with not good quality: %d", updateValue.quality);
				m_modeValueInit = false;
				return;
			}
            
            if (m_modeValueInit)
            {
                try
                {
                    TA_IRS_Core::Mode_SmartPtr basMode = BasCachedConfig::getInstance().getBasModeFromValue(static_cast<unsigned long>(updateValue.floatValue));

                    BasEventSubmitter::instance()->submitBasSystemModeChangeEvent(basMode->getDescription(), basMode->getName(), m_modeValue->getLastOperatorSession(), dpEntityKey, m_modeValue->getAssetName());
                }
                catch(const std::exception& e)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what() );
                }
                catch(...)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Generate BasSystemModeChanged event failed" );
                }
            }
            m_modeValueInit = true;
        }
    }
}
