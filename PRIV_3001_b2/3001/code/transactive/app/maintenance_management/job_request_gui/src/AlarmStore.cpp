/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/maintenance_management/job_request_gui/src/AlarmStore.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/03/20 16:13:46 $
  * Last modified by:  $Author: Noel R. Tajanlangit $
  * 
  * This class provides the interface to the alarm store. It connects to it and sends information
  * to it.
  */

#pragma warning(disable:4786)

#include "AlarmStore.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/TransactiveException.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
 
    AlarmStore::AlarmStore()
    :
    m_bAlarmStoreValid(false)
    {
        FUNCTION_ENTRY("Constructor");
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "AlarmStore::run come in" );
		m_bAlarmStoreValid = false;
		std::ostringstream ior;
		ior << "corbaloc::localhost:";
		ior << TA_Base_Core::IAlarmStoreCorbaDef::DEFAULT_ALARM_STORE_PORT;
		ior << "/" << TA_Base_Core::IAlarmStoreCorbaDef::DEFAULT_ALARM_STORE_NAME;

		std::string alarmStoreIOR = ior.str();
        
		try
		{
			TA_THREADGUARD( m_alarmStoreLock );   
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Resolving Alarm Store");

			CORBA::Object_var obj = TA_Base_Core::CorbaUtil::getInstance().stringToObject( alarmStoreIOR );
			if ( true == CORBA::is_nil(obj) )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Failed to retrieve Corba object with IOR %s",
					alarmStoreIOR );
				
			}

			m_alarmStore = TA_Base_Core::IAlarmStoreCorbaDef::_narrow( obj );
			if ( true == CORBA::is_nil(m_alarmStore) )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Failed to narrow obj into IAlarmStoreCorbaDef" );
				
			}     
			else
			{
				m_bAlarmStoreValid = true;
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Resolve Alarm Store successful.");
			}
		}
		catch( const TA_Base_Core::ObjectResolutionException& ore )
		{
			m_alarmStore = NULL;
			std::ostringstream desc;
			desc << "Caught ObjectResolutionException while resolving AlarmStore with IOR "
				<< alarmStoreIOR << "; Reason: " << ore.what();
			LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", desc.str().c_str() );
		}
		catch( const TA_Base_Core::IAlarmStoreCorbaDef::InvalidFilterException& ife )
		{
			//TODO: How to get this back out to the calling class
			m_alarmStore = NULL;
			std::ostringstream desc;
			desc << "Filter specified is invalid; Reason: " << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ife);
			LOG_EXCEPTION_CATCH( SourceInfo, "InvalidFilterException", desc.str().c_str() );
		}
		catch ( const CORBA::SystemException& se )
		{
			//m_alarmStore = NULL;
			std::string errinfo=TA_Base_Core::CorbaUtil::getInstance().exceptionToString(se).c_str();
			std::ostringstream desc;
			desc << "Caught CORBA::SystemException resolving AlarmStore with IOR "
				<< alarmStoreIOR << "; Reason: " << errinfo;
			LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::SystemException", desc.str().c_str() );			 
		}
		catch ( const std::exception& e )
		{
			m_alarmStore = NULL;
			std::ostringstream desc;
			desc << "Caught std::exception resolving AlarmStore with IOR "
				<< alarmStoreIOR << "; Reason: " << e.what();
			LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", desc.str().c_str() );
		}
		catch( ... )
		{
			m_alarmStore = NULL;
			// Catch all exception to prevent thread from pre-maturely terminating
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Caught unknown exception in AlarmStore::run()!" );
		}
			
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "AlarmStore::run come out" );
        FUNCTION_EXIT;
    }


    AlarmStore::~AlarmStore()
    {
        FUNCTION_ENTRY("Destructor");

        TA_THREADGUARD( m_alarmStoreLock );
        if ( false == CORBA::is_nil(m_alarmStore) )
        {
            m_alarmStore = NULL;
        }

        FUNCTION_EXIT;
    }

    TA_Base_Core::AlarmDetailCorbaDef* AlarmStore::getAlarm(std::string alarmUUID)
    {
        
        if ( false == isAlarmStoreValid() )
        {
            throw TA_Base_Core::TransactiveException("Could not retrieve the specified alarm");
        }

        try
        {
            TA_THREADGUARD( m_alarmStoreLock );
            TA_Base_Core::AlarmDetailCorbaDef* alarmDetail = m_alarmStore->getAlarm(alarmUUID.c_str());
            
            return alarmDetail;
        }
        catch(const TA_Base_Core::IAlarmStoreCorbaDef::AlarmNotFoundException& )
        {
            std::ostringstream errorMsg;
            errorMsg << "Alarm " << alarmUUID << " not found in alarm store";
            LOG_EXCEPTION_CATCH( SourceInfo, "AlarmNotFoundException", errorMsg.str().c_str() );
        }
        catch(const TA_Base_Core::ObjectResolutionException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve alarm store. Will indicate that this has to be found");
        }
        catch (CORBA::SystemException& ex)
        {
            std::string errinfo=TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str();
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", errinfo.c_str());
			
        }
        catch (CORBA::Exception& ce)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ce).c_str());
        }
        catch (const std::exception& e)
        {
            std::ostringstream desc;
            desc << "Caught std::exception in generateAlarmCache; Reason: " << e.what();
            LOG_EXCEPTION_CATCH(SourceInfo, "std::exception", desc.str().c_str());
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Caught unknown exception in generateAlarmCache");
        }

        throw TA_Base_Core::TransactiveException("Could not retrieve the specified alarm");
    }
}
