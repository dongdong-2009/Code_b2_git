/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/DataPointUpdateProcessor.cpp $
  * @author:	Huang Jian
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/02/27 17:12:34 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *
  */

#include "core/data_access_interface/src/DatabaseKey.h"
#include "app/bas/BASAgent/src/DataPointUpdateProcessor.h"
#include "app/bas/BASAgent/src/IDataPointUpdateObserver.h"
#include "app/bas/BASAgent/src/BasCachedConfig.h"
#include "app/bas/BASAgent/src/BasEventSubmitter.h"

namespace TA_IRS_App
{

    DataPointUpdateProcessorImpl::DataPointUpdateProcessorImpl()
    {
        m_dpUpdateObserverMap.clear();
        m_basParameterValueDPSet.clear();
        m_basFireModeRecoverDPSet.clear();
    }

    DataPointUpdateProcessorImpl::~DataPointUpdateProcessorImpl( )
    {
    }

    void DataPointUpdateProcessorImpl::registerDatapointUpdate( const unsigned long pkey, IDataPointUpdateObserver* pUpdateObserver )
    {
        TA_THREADGUARD(m_dpUpdateObserverMapLock);

        DataPointUpdateNofificationObserverMap::iterator observerIter = m_dpUpdateObserverMap.find(pkey);

        if (m_dpUpdateObserverMap.end() == observerIter)
        {
            try
            {
                TA_Base_Bus::DataPoint& dp = BasCachedConfig::getInstance().getDatapointFromKey(pkey);
                dp.registerForUpdateDetails(this);
            }
            catch (const std::exception& expWhat)
            {
                LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
            }
            catch (...)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No datapoint of key:%lu", pkey);
            }
        }

        std::set<IDataPointUpdateObserver*>& observerSet = m_dpUpdateObserverMap[pkey];
        observerSet.insert(pUpdateObserver);
    }

    void DataPointUpdateProcessorImpl::deregisterDatapointUpdate( const unsigned long pkey, IDataPointUpdateObserver* pUpdateObserver )
    {
        TA_THREADGUARD(m_dpUpdateObserverMapLock);

        DataPointUpdateNofificationObserverMap::iterator observerIter = m_dpUpdateObserverMap.find(pkey);
        if (m_dpUpdateObserverMap.end() != observerIter)
        {
            std::set<IDataPointUpdateObserver*>::iterator itr = observerIter->second.find(pUpdateObserver);
            if (observerIter->second.end() != itr)
            {
                observerIter->second.erase(itr);
            }

            if (observerIter->second.empty())
            {
                m_dpUpdateObserverMap.erase(observerIter);
                try
                {
                    TA_Base_Bus::DataPoint& dp = BasCachedConfig::getInstance().getDatapointFromKey(pkey);
                    dp.deregisterForUpdateDetails(this);
                }
                catch (const std::exception& expWhat)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
                }
                catch (...)
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No datapoint of key:%lu", pkey);
                }
            }
        }

    }

    void DataPointUpdateProcessorImpl::processDataPointUpdate( unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> updateDetails )
    {
        TA_THREADGUARD(m_dpUpdateObserverMapLock);
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "DataPointUpdateProcessorImpl, process datapoint update from key = %lu", pkey);

        DataPointUpdateNofificationObserverMap::iterator observerSetIter = m_dpUpdateObserverMap.find(pkey);
        if (m_dpUpdateObserverMap.end() == observerSetIter || observerSetIter->second.empty())
        {
            return;
        }

        if (TA_Base_Bus::ControlUpdate == updateDetails->updateType)
        {
            DataPointControlStateType stateType;
            switch(updateDetails->controlState.controlState)
            {
            case TA_Base_Bus::conditionalSuccess:
            case TA_Base_Bus::success :
                stateType = E_STATE_SUCCESS;
                break;
            case TA_Base_Bus::LCC_failed:
            case TA_Base_Bus::comms_failed:
            case TA_Base_Bus::failed:
                stateType = E_STATE_FAIL_GENERAL;
                break;
            case TA_Base_Bus::RCC_failed:
                stateType = E_STATE_FAIL_TIME_OUT;
                break;
            default:
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unrecognized datapoint:%lu, controlState:%lu", pkey, updateDetails->controlState.controlState);
                return;
            }

            std::set<IDataPointUpdateObserver*>::iterator obrIter = (observerSetIter->second).begin();
            for (;obrIter != (observerSetIter->second).end(); ++obrIter)
            {
                (*obrIter)->processDataPointControlStateUpdate(pkey, stateType, updateDetails->controlState);
            }
        }
        else if (TA_Base_Bus::ValueUpdate == updateDetails->updateType)
        {
            std::set<IDataPointUpdateObserver*>::iterator obrIter = (observerSetIter->second).begin();
            for (;obrIter != (observerSetIter->second).end(); ++obrIter)
            {
                (*obrIter)->processDataPointValueUpdate(pkey, updateDetails->value);
            }
        }
    }

    void DataPointUpdateProcessorImpl::registerBasParameterChangeEvent()
    {
        TA_IRS_Core::ParameterValueTypeInfoVct paraValueType;
        BasCachedConfig::getInstance().getParameterValueType(paraValueType);

        for (TA_IRS_Core::ParameterValueTypeInfoVct::iterator itPara = paraValueType.begin(); itPara != paraValueType.end(); itPara++)
        {
            TA_Base_Core::IEntityDataList entityData;
            try
            {
                std::string strDatapointTokenString = BasCachedConfig::getInstance().getStationName() + (*itPara).aoToken;				
                entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeWithNameLikeToken(TA_Base_Core::DataPointEntityData::getStaticType(), strDatapointTokenString);
            }
            catch(std::exception& e)
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what() );
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Failed to register BasParameterChange Event" );
            }

            for (TA_Base_Core::IEntityDataList::iterator itEntity = entityData.begin(); itEntity != entityData.end(); itEntity++)
            {
                std::auto_ptr<TA_Base_Core::IEntityData> ptrEntity(*itEntity);
                unsigned long ulPkey = TA_Base_Core::DatabaseKey::getInvalidKey();

                try
                {
                    ulPkey = ptrEntity->getKey();
                }
                catch(std::exception& e)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what() );
                    continue;
                }
                catch(...)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Failed to get pkey of entity" );
                    continue;
                }

                try
                {
                    TA_Base_Bus::DataPoint& dp = BasCachedConfig::getInstance().getDatapointFromKey(ulPkey);
                    dp.registerForUpdateDetails(this);
                }
                catch (const std::exception& expWhat)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
                }
                catch (...)
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No datapoint of key:%lu", ulPkey);
                }

                try
                {
                    m_basParameterValueDPSet.insert(ulPkey);
                    registerDatapointUpdate(ulPkey, this);
                }
                catch (const std::exception& expWhat)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
                }
                catch(...)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Failed to register BasParameterChange Event" );
                }
            }
        }
    }

    void DataPointUpdateProcessorImpl::registerFireModeChange()
    {
        try
        {
            TA_Base_Bus::DataPoint* dp = BasCachedConfig::getInstance().getFireModeRecoverDPFromNode( BasCachedConfig::getInstance().getStationName());
            if (NULL != dp)
            {
                dp->registerForUpdateDetails(this);
                m_basFireModeRecoverDPSet.insert(dp->getEntityKey());
                registerDatapointUpdate(dp->getEntityKey(), this);
            }
        }
        catch(std::exception& e)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", e.what() );
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Failed to registerFireModeChange" );
        }
    }

    void DataPointUpdateProcessorImpl::processDataPointControlStateUpdate( const unsigned long dpEntityKey, const int nControlState, const TA_Base_Bus::DataPointControl& controlState )
    {
        std::string strEventMsg = "";
        std::string strSession = "";
		std::string strAssetName = "";

        try
        {
            TA_Base_Bus::DataPoint& dp = BasCachedConfig::getInstance().getDatapointFromKey(dpEntityKey);
            strEventMsg = dp.getEventMessage();
            strSession = dp.getLastOperatorSession();
			strAssetName = dp.getAssetName();
        }
        catch (const std::exception& expWhat)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
        }
        catch (...)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No datapoint of key:%lu", dpEntityKey);
        }

        if (m_basParameterValueDPSet.end() != m_basParameterValueDPSet.find(dpEntityKey))
        {
            BasEventSubmitter::instance()->submitBasValveParamDownloadEvent(strEventMsg, nControlState, controlState.triggerringDpValue, strSession, dpEntityKey, strAssetName);
        }
        else if(m_basFireModeRecoverDPSet.end() != m_basFireModeRecoverDPSet.find(dpEntityKey))
        {
            BasEventSubmitter::instance()->submitBasCancelFireModeEvent(strEventMsg, nControlState, strSession, dpEntityKey, strAssetName);
        }
    }

    void DataPointUpdateProcessorImpl::processDataPointValueUpdate( const unsigned long dpEntityKey, const TA_Base_Bus::DataPointValue& updateValue )
    {
        // Do nothing...
    }

}