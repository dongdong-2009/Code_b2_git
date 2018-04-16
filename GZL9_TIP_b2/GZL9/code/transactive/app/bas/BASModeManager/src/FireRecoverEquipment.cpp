#include "StdAfx.h"
#include "app/bas/BASModeManager/src/FireRecoverEquipment.h"

namespace TA_IRS_App
{
	FireRecoverEquipment::FireRecoverEquipment(const std::string& strDatapointName) : 
	m_dpFireRecover(),
	m_semInitDone( 0 ),
	m_bInitDone( false ),
	m_strEquipmentName( strDatapointName.c_str() )
	{
	}

	FireRecoverEquipment::~FireRecoverEquipment(void)
	{
	}

	void FireRecoverEquipment::initializeEquipment()
	{
		// get a handle to the ScadaProxyFactory
	    TA_Base_Bus::ScadaProxyFactory& refProxyFactory = TA_Base_Bus::ScadaProxyFactory::getInstance();

		refProxyFactory.setProxiesToControlMode();

	    try
	    {
		    std::string strEquipmentDataNodeName = m_strEquipmentName.substr(0, m_strEquipmentName.find_last_of('.'));
		    TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(strEquipmentDataNodeName);
		    TA_Base_Core::DataNodeEntityData* dnEntityData = dynamic_cast<TA_Base_Core::DataNodeEntityData*>(entityData);

		    TA_Base_Core::SharedIEntityDataList myDatapoints = dnEntityData->getChildDataPoints();

		    for ( TA_Base_Core::SharedIEntityDataList::const_iterator iterPt = myDatapoints.begin(); iterPt != myDatapoints.end(); ++iterPt)
		    {
			    if (m_strEquipmentName == (*iterPt)->getName())
			    {
	                TA_Base_Core::DataPointEntityDataPtr ptrProxy(dynamic_cast<TA_Base_Core::DataPointEntityData*>(iterPt->get()));
	                refProxyFactory.createDataPointProxy( ptrProxy, *this, m_dpFireRecover );
	                break;
			    }
	        }
	    }
	    catch ( const std::exception& expWhat )
	    {
	        LOG_EXCEPTION_CATCH(SourceInfo, typeid(expWhat).name(), expWhat.what());
	    }
	    catch (...)
	    {
	        LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Caught unknown exception when createDataPointProxy");
	    }

	    TA_Base_Bus::ScadaProxyFactory::removeInstance();
	}

	void FireRecoverEquipment::uninitEquipment()
	{
	    m_dpFireRecover.reset();
	}

	void FireRecoverEquipment::setFireRecoverFlag()
	{
	    if ( !m_bInitDone )
	    {
	        if ( !m_semInitDone.timedWait( 1000 ))
	        {
	            TA_THROW( TA_Base_Core::TransactiveException("Failed to initialize DataPoint") );
	        }

	        m_bInitDone = true;
	    }

	    std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

	    if ( TA_Base_Bus::DPT_BOOLEAN == m_dpFireRecover->getType() )
	    {
	        m_dpFireRecover->setBoolValue( true, session );
	    }
	    if ( TA_Base_Bus::DPT_ANALOGUE == m_dpFireRecover->getType() )
	    {
	        m_dpFireRecover->setAnalogueValue( static_cast<double>(eModeReset), session );
	    }
	    else if ( TA_Base_Bus::DPT_COMBINED_OUTPUT == m_dpFireRecover->getType() )
	    {
	        std::string strValue = m_dpFireRecover->getDerivedStateAsString( eModeReset );
	        m_dpFireRecover->setValue( strValue, session );
	    }
	}

	void FireRecoverEquipment::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
	    if ( entityKey != m_dpFireRecover->getEntityKey() )
	    {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "process Entity Update Event, EntityKey isn't unequal with fire recover data point. ScadaEntityUpdateType=%d,EntityKey=%lu,EntityKeyForFireCover=%lu", 
				updateType, entityKey, m_dpFireRecover->getEntityKey());
	        return;
	    }

	    if ( updateType != TA_Base_Bus::AgentConnected )
	    {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "process Entity Update Event,Agent cannot be connected. ScadaEntityUpdateType=%d,EntityKey=%lu", 
				updateType, entityKey);
	        return;
	    }

	    while ( m_semInitDone.tryWait() )
	    {
	    }

	    m_semInitDone.post();
	}

}

