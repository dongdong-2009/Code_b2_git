/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/DataNodeAgentProxy.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// DataNodeAgentProxy.cpp: implementation of the DataNodeAgentProxy class.
//
//////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "stdafx.h"
#include <sstream>
#include "time.h"
#include "boost/cast.hpp"

#include "app/scada/EquipmentStatusViewer/src/DataNodeAgentProxy.h"
#include "app/scada/EquipmentStatusViewer/src/EquipmentStatusViewer.h"
#include "app/scada/EquipmentStatusViewer/src/CustomDefines.h"
#include "bus/scada/common_library/IDL/src/DataNodeCorbaTypes.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"

#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/ScadaCommonException.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
  	DataNodeAgentProxy::DataNodeAgentProxy(std::string & agentName, CEquipmentStatusViewerDlg* dlg)
		:
        m_toBeTerminated(false),
		m_pollSwitchOn(true),
		m_agentDnInterface(NULL),
		m_observerId(0),
		m_dlg(dlg)
	{
		m_agentDnInterface = new DataNodeAgentInterface();
		m_agentDnInterface->setObjectTimeout(20); 
        m_agentDnInterface->setCorbaName ( agentName, agentName + TA_Base_Bus::DN_BULK_POLLED_INTERFACE);
	}
    
    DataNodeAgentProxy::~DataNodeAgentProxy()
    {
        terminateAndWait();
    }
    
    void DataNodeAgentProxy::terminate()
    {
        m_toBeTerminated = true;
		pausePoll();
    }

    void DataNodeAgentProxy::run()
    {		
        while( !m_toBeTerminated )
        {
			TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayStateSequence* dataNodeUpdates = NULL;

			if (m_pollSwitchOn)
			{
				try
				{
					if (0 == m_observerId)
					{
						m_storeOITPDn.clear();

						CORBA_CALL_RETURN(	m_observerId,
											(*m_agentDnInterface),
											regiesterDisplayDataNodeClient,
											() );

						if (m_observerId > 0 && m_pollSwitchOn)
						{
							CORBA_CALL_RETURN(	dataNodeUpdates,
												(*m_agentDnInterface),
												getDisplayDataNodeUpdate,
												( m_observerId, true ) );
						}
					}
					else
					{
						CORBA_CALL_RETURN(  dataNodeUpdates,
											(*m_agentDnInterface),
											getDisplayDataNodeUpdate,
											( m_observerId, false ) );

					}

					if (NULL != dataNodeUpdates)
					{
						unsigned int updates = 0;
						updates = dataNodeUpdates->length();        
						try
						{
							std::vector<unsigned long> dnKeys;
							for (unsigned int i = 0; i < updates && m_pollSwitchOn; ++i)
							{
								TA_Base_Bus::DataNodeCorbaTypes::DataNodeDisplayState&  dataNodeUpdate = (*dataNodeUpdates)[i];
								TA_Base_Bus::DataNodeDisplayState toupdate;
								TA_Base_Bus::convertDnDisplayUpdate(dataNodeUpdate, toupdate);

								bool isInStore = (m_storeOITPDn.find(toupdate.entityKey) != m_storeOITPDn.end()) ? true : false;
								bool isOITP = (!toupdate.ONITP.empty() && toupdate.ONITP.compare("N") != 0);
								if (isOITP && !isInStore)
								{
									dnKeys.push_back(toupdate.entityKey);
									m_storeOITPDn.insert(toupdate.entityKey);
								}
								else if (!isOITP && isInStore)
								{
									m_storeOITPDn.erase(toupdate.entityKey);
								}
							}

							if (dnKeys.size() > 0 && m_pollSwitchOn)
							{
								m_dlg->notifyDnUpdate(dnKeys);
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"DataNode:OITP count:%d", dnKeys.size());
							}
						}
						catch (...) 
						{
							LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception caught during processing updates");
						}

						delete dataNodeUpdates;
						dataNodeUpdates = NULL;	
					}                
				}//end of try
				catch ( const TA_Base_Core::ObjectResolutionException & ex )
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException" ,ex.what() );
					setToBadNotConnectedQuality ();
				}
				catch ( const TA_Base_Core::ScadaProxyException & ex )
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ScadaProxyException" ,ex.what() );
					setToBadNotConnectedQuality ();
				}
				catch ( const TA_Base_Core::BadParameterException & )
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::BadParameterException", "Caught Bad Parameter exception" );
					setToBadNotConnectedQuality ();
				}
				catch ( const TA_Base_Core::OperationModeException & )
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught Operation Mode exception" );
					// force to rebind named object next time.
					m_agentDnInterface->clear();
					// set all data point to bad quality status and restart the polling cycle
					// by requesting the agent for new observer ID
					setToBadNotConnectedQuality ();
				}
				catch ( const CORBA::Exception & ex )
				{
					LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
					// force to rebind named object next time.
					m_agentDnInterface->clear();
					setToBadNotConnectedQuality ();
				}
				catch ( ... )
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception...");
					setToBadNotConnectedQuality ();
				}
			}// end of if (m_pollSwitchOn)

			TA_Base_Core::Thread::sleep(500);
        }

		if ( m_observerId > 0 )
        {
            try
            {  
				CORBA_CALL( (*m_agentDnInterface),
				            unregiesterDisplayDataNodeClient,
				            ( m_observerId ) );
			}
            catch (...) 
            {
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "%s", "DataNodeAgentProxy::run(), when call unregiesterDisplayDataNodeClient()" );
				LOG_EXCEPTION_CATCH(SourceInfo, "...","Unknown Exception caught during processing updates");
            }
        }
		delete m_agentDnInterface;
    }

	void DataNodeAgentProxy::setToBadNotConnectedQuality ()
    {		
		m_observerId = 0;
    }

	void DataNodeAgentProxy::pausePoll()
	{
		m_pollSwitchOn = false;
	}

	void DataNodeAgentProxy::resumePoll()
	{
		m_pollSwitchOn = true;
	}
}