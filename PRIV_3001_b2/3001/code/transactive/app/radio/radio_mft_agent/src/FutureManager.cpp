/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent/src/FutureManager.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */


#include "app/radio/radio_mft_agent/src/FutureManager.h"
#include "app/radio/radio_mft_agent/src/TimeoutCallback.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/RadioProfileEntityData.h"


#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

// for debugging only
FutureManager* gFutureManager;
// !!!!!!!


FutureManager::FutureManager() : m_timeout(10), m_deactivate(false)
{
    gFutureManager = this;

	activate();

	getTimeoutValue();	
}

FutureManager::~FutureManager()
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	m_deactivate = true;
    wait();

	cleanup();
}


int FutureManager::svc()
{
    ACE_OS::sleep(1);
	while ( !m_deactivate )
    {

        ACE_Time_Value currentTime = ACE_OS::gettimeofday();

        {
    	    TA_Base_Core::ThreadGuard guard( m_lock );

            int lSize = m_timeoutCallbacks.size();
            for (int i = lSize - 1; i >= 0; --i)
            {
                TimeoutCallback* callback = m_timeoutCallbacks[i];
                if (callback->getTimeoutTime() <= currentTime)
                {
                    if (0 == callback->handle_timeout(currentTime, this))
                    {
                        delete callback;
                    }
                }
            }
        }
        ACE_OS::sleep(1);
	}

	return 0;
}


void FutureManager::attachFuture(ACE_Future<FutureResultT>& future)
{
    FUNCTION_ENTRY("attachFuture");

	RadioFuture rFuture;
	rFuture.m_future = future;


	ACE_Time_Value xTime(m_timeout); 
	ACE_Time_Value expireTime = ACE_OS::gettimeofday() + xTime;
	TimeoutCallback* timerHandle = new TimeoutCallback(expireTime);

	timerHandle->setFuture(future);

    {
	    TA_Base_Core::ThreadGuard guard( m_lock );
	    m_FutureResults.push_back(rFuture);
    	m_timeoutCallbacks.push_back(timerHandle);
    }

    FUNCTION_EXIT;
}


void FutureManager::detachFuture(ACE_Future<FutureResultT>& future)
{
    FUNCTION_ENTRY("detachFuture");

	if (!future.ready())
	{
		return;
	}

	FutureResultT fResult;
	future.get(fResult);

	if ( fResult.m_exceptionIndicator == FutureResultT::NO_STATE)
	{
		return;
	}


	// search timer threadID
	std::vector<TimeoutCallback*>::iterator iter;

    {
	    TA_Base_Core::ThreadGuard guard( m_lock );
	    for ( iter = m_timeoutCallbacks.begin(); iter != m_timeoutCallbacks.end(); ++iter)
	    {
		    if ( future == (*iter)->getFuture() )
		    {
			    m_timeoutCallbacks.erase(iter);
			    
			    fResult.m_exceptionIndicator = FutureResultT::ABORTED;
			    future.set(fResult);
			    
			    break;
		    }
	    }
    }

	FUNCTION_EXIT;
}


void FutureManager::updateFuture(ACE_Future<FutureResultT>& future)
{
    FUNCTION_ENTRY("updateFuture");
	
	if (!future.ready())
	{
		return;
	}
	
	FutureResultT fResult;
	future.get(fResult);

	if ( fResult.m_exceptionIndicator == FutureResultT::NO_STATE)
	{
		return;
	}

	// search timer threadID
	std::vector<TimeoutCallback*>::iterator iter;

    {
	    TA_Base_Core::ThreadGuard guard( m_lock );
	    for ( iter = m_timeoutCallbacks.begin(); iter != m_timeoutCallbacks.end(); ++iter)
	    {
		    if ( future == (*iter)->getFuture() )
		    {
			    m_timeoutCallbacks.erase(iter);

			    fResult.m_exceptionIndicator = FutureResultT::COMPLETED;

			    future.set(fResult);

			    break;
		    }
	    }
    }



    FUNCTION_EXIT;
}


void FutureManager::removeFuture(ACE_Future<FutureResultT>& future, bool bTimeout)
{
    FUNCTION_ENTRY("removeFuture");

	if (!future.ready())
	{
		return;
	}

	// delete TimeoutCallbacks
	std::vector<TimeoutCallback*>::iterator iter1;

    {
	    TA_Base_Core::ThreadGuard guard( m_lock );
	    for ( iter1 = m_timeoutCallbacks.begin(); iter1 != m_timeoutCallbacks.end(); ++iter1)
	    {
		    if (future == (*iter1)->getFuture())
		    {
                // should always cancel the timer
			    if (bTimeout) // this function is invoked by handle_timeout()
			    {
			    }
			    else
			    {
				    delete (*iter1);	
			    }
			    m_timeoutCallbacks.erase(iter1);
			    break;
		    }
	    } 
    

	    // remove from list for RadioFuture
	    std::vector<RadioFuture>::iterator iter2;

	    for ( iter2 = m_FutureResults.begin(); iter2 != m_FutureResults.end(); ++iter2)
	    {
		    if ( future == (*iter2).m_future )
		    {
			    FutureResultT result;
			    future.get(result);

			    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[FUTURE] removeFuture(%d)",result.get());

			    m_FutureResults.erase(iter2);
			    break;
		    }
	    }
    }

    FUNCTION_EXIT;
}

void FutureManager::cleanup()
{
    FUNCTION_ENTRY("cleanup");

	TA_Base_Core::ThreadGuard guard( m_lock );

	// delete TimeoutCallbacks
	std::vector<TimeoutCallback*>::iterator iter1;
	
	for ( iter1 = m_timeoutCallbacks.begin(); iter1 != m_timeoutCallbacks.end(); ++iter1)
	{
		delete (*iter1);
	} 
	m_timeoutCallbacks.clear();
    m_FutureResults.clear();

    FUNCTION_EXIT;
}

ACE_Future<FutureResultT> FutureManager::getFutureResult(CommandID id)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	
	std::vector<RadioFuture>::iterator iter;
	
	ACE_Future<FutureResultT> result;

	for ( iter = m_FutureResults.begin(); iter != m_FutureResults.end(); ++iter)
	{
		FutureResultT future;

        if ((*iter).m_future.ready())
        {
		    (*iter).m_future.get(future);

		    if ((future.get() == id) && (future.m_exceptionIndicator==FutureResultT::NEW))
		    {
			    result = (*iter).m_future;	
                break;
		    }
        }
	}

	return result;
}

ACE_Future<FutureResultT> FutureManager::getFutureResultForResource(CommandID id, unsigned long resourceId)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	
	std::vector<RadioFuture>::iterator iter;
	
	ACE_Future<FutureResultT> result;

	for ( iter = m_FutureResults.begin(); iter != m_FutureResults.end(); ++iter)
	{
		FutureResultT future;

        if ((*iter).m_future.ready())
        {
		    (*iter).m_future.get(future);

		    if (future.get() == id && future.m_resourceId == resourceId &&
                future.m_exceptionIndicator==FutureResultT::NEW)
		    {
			    result = (*iter).m_future;	
                break;
		    }
        }
	}

	return result;
}

ACE_Future<FutureResultT> FutureManager::getFutureResultForChannel(CommandID id, unsigned long channel)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	
	std::vector<RadioFuture>::iterator iter;
	
	ACE_Future<FutureResultT> result;

	for ( iter = m_FutureResults.begin(); iter != m_FutureResults.end(); ++iter)
	{
		FutureResultT future;

        if ((*iter).m_future.ready())
        {
		    (*iter).m_future.get(future);

		    if (future.get() == id && future.m_channel == channel &&
                future.m_exceptionIndicator==FutureResultT::NEW)
		    {
			    result = (*iter).m_future;	
                break;
		    }
        }
	}

	return result;
}

std::string FutureManager::getFutureList()
{
	TA_Base_Core::ThreadGuard guard( m_lock );

    std::ostringstream bufstr;
	
	std::vector<RadioFuture>::iterator iter;
	
	ACE_Future<FutureResultT> result;

    int i = 0;
	for ( iter = m_FutureResults.begin(); iter != m_FutureResults.end(); ++iter, ++i)
	{
		FutureResultT future;

        if ((*iter).m_future.ready())
        {
		    (*iter).m_future.get(future);
            bufstr << "F" << i << ":" << TimeoutCallback::commandToString(future.get())
                << " RID=" << future.m_resourceId 
                << " S=" << future.m_state << " E=" << future.m_exceptionIndicator << "\r\n";
        }
	}

    return bufstr.str();
}


void FutureManager::getTimeoutValue()
{
	// Get timeout value
	try
	{
		std::vector<TA_Base_Core::IEntityData*> entityList =
						 TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType("RadioProfile");

		TA_ASSERT(entityList.size()>0,"Radio entity not found for this location.");
		TA_ASSERT(entityList.size()<2,"More than one radio entity defined for this location.");

		TA_Base_Core::RadioProfileEntityData* pData = dynamic_cast<TA_Base_Core::RadioProfileEntityData*>(entityList[0]);

		TA_ASSERT(pData != NULL,"RadioProfileEntityData is NULL");

		m_timeout = pData->getEventTimeout();

		delete pData;
	}
    catch(TA_Base_Core::TransactiveException& e)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
    }
    catch (...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Cannot retrieve parameter for timeout value");
    }
}

