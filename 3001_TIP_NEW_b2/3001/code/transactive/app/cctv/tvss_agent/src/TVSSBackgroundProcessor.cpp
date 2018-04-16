/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/radio/RadioTetraAgent/src/RadioBackgroundProcessor.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by:  $Author: grace.koh $
  *
  * This class is used to send QueryReference Methods as appropriate to
  * determine information about a subscriber activity to send in the 
  * TA_Base_Bus::IRadioCorbaDef::MonitoredCallDetailsType corba struct to the
  * RadioMonitor
  *
  * See Radio::onSubscriberActivity for comments on why we do this.
  */


#include "app/cctv/tvss_agent/src/TVSSBackgroundProcessor.h"
#include "app/cctv/tvss_agent/src/TVSS.h"

#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using namespace TA_IRS_App;

TVSSBackgroundProcessor::TVSSBackgroundProcessor(TVSS* tvss)
: m_tvss(*tvss),
  m_threadPool(TA_Base_Core::ThreadPoolSingletonManager::getInstance())
{
    FUNCTION_ENTRY("RadioBackgroundProcessor");

    //not enough threads? let it grow
    m_threadPool->setUpperLimit( 6000 );

    FUNCTION_EXIT;
}


TVSSBackgroundProcessor::~TVSSBackgroundProcessor()
{
    FUNCTION_ENTRY("~RadioBackgroundProcessor");

    TA_Base_Core::ThreadPoolSingletonManager::removeInstance(m_threadPool);
    
    FUNCTION_EXIT;
}


//void TVSSBackgroundProcessor::addTask(RadioTaskPtr task)
//{
//    FUNCTION_ENTRY("addTask()");
//
//    m_threadPool->queueWorkItem(task);
//
//    FUNCTION_EXIT;
//}

