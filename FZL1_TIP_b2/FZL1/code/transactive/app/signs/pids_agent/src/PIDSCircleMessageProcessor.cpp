/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3002_TIP/3002/transactive/app/signs/pids_agent/src/PIDSClearRequest.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#include "PIDSCircleMessageProcessor.h"
#include "time.h"
namespace TA_IRS_App
{

    int PIDSCircleMessageProcessor::handle_timeout( const ACE_Time_Value &tv, const void *arg )
    {
        TA_Base_Core::ThreadGuard guard( m_handleAccessLock );

        FUNCTION_ENTRY( "handle_timeout" );
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Time out" );
        

        /*
        ACE_Time_Value currentTime = ACE_OS::gettimeofday(); //Get current time
        long secs = currentTime.sec(); //ACE_Time_Value to Epoch time
        */
        time_t secs = time( NULL );

        std::vector<PIDSAdhocData*>::iterator ite;
        std::vector<PIDSClearRequest*>::iterator itecr;

        //Vector erase in for circle
        for ( ite = m_adHoc.begin(), itecr = m_clearRe.begin();
                ite != m_adHoc.end() && itecr != m_clearRe.end(); )
        {
            //for test
			/*
            std::stringstream ss;
            ss << "start time:" << ( *ite )->m_startTime << " current time:" << secs
            << " end time:" << ( *ite )->m_endTime;
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, ss.str().c_str() );
			*/
			

            //end

            if ( secs >= ( *ite )->m_startTime && secs <= ( *ite )->m_endTime )            
			{
                
                try
                {
					if(!((*ite)->m_isSend))
					{
						LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Send one request" );
						PIDSCommunicationsHandler::getInstance()->sendMessage( ( *ite )->m_reQuest );
						(*ite)->m_isSend = true;
					}
                }
                catch ( ... ) {}
                ite++, itecr++;

            }
            else if ( secs > ( *ite )->m_endTime )
            {
                delete *ite;
                ite = m_adHoc.erase( ite );

                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Send one clear request" );
                //Send clear request
                try
                {
                    PIDSCommunicationsHandler::getInstance()->sendMessage( *itecr );
                }
                catch ( ... ) {}

                delete *itecr;
                itecr = m_clearRe.erase( itecr );
            }
            else
            {
                ite++, itecr++;
            }
        }
        FUNCTION_EXIT;
        return 0;

    }


    PIDSCircleMessageProcessor* PIDSCircleMessageProcessor::m_theClass = NULL;
    ACE_Time_Value PIDSCircleMessageProcessor::m_interval( 1 ); //1 second
    TA_Base_Core::ReEntrantThreadLockable PIDSCircleMessageProcessor::m_handleAccessLock;

    void PIDSCircleMessageProcessor::addCirleMessage( PIDSAdhocData* adhocData )
    {
        TA_Base_Core::ThreadGuard guard( m_handleAccessLock );
        if ( adhocData != NULL )
            m_adHoc.push_back( adhocData );
    }

    void PIDSCircleMessageProcessor::addCirleMessage( PIDSAdHocDisplayRequest* reQuest, MessageTime timeStart, MessageTime timeEnd )
    {
        TA_Base_Core::ThreadGuard guard( m_handleAccessLock );
        PIDSAdhocData *tmpAdhocData = new PIDSAdhocData();
        tm t;

        //Make time to Epoch time,so it is easy to compare
        t.tm_hour = timeStart.hour; // hours since midnight - [0,23]
        t.tm_mday = timeStart.day; // day of the month - [1,31]
        t.tm_min = timeStart.minute; // minutes after the hour - [0,59]
        t.tm_mon = timeStart.month - 1; // months since January - [0,11]
        t.tm_sec = timeStart.second; // seconds after the minute - [0,59]
        t.tm_year = timeStart.year - 1900; // years since 1900

        tmpAdhocData->m_startTime = mktime( &t );

        t.tm_hour = timeEnd.hour;
        t.tm_mday = timeEnd.day;
        t.tm_min = timeEnd.minute;
        t.tm_mon = timeEnd.month - 1;
        t.tm_sec = timeEnd.second;
        t.tm_year = timeEnd.year - 1900;

        tmpAdhocData->m_endTime = mktime( &t );
        tmpAdhocData->m_reQuest = reQuest;

		//
		tmpAdhocData->m_isSend = false;



        if ( tmpAdhocData != NULL )
            m_adHoc.push_back( tmpAdhocData );
    }


    void PIDSCircleMessageProcessor::addClearMessage( PIDSClearRequest* clRequest )
    {
        TA_Base_Core::ThreadGuard guard( m_handleAccessLock );
        if ( clRequest != NULL )
            m_clearRe.push_back( clRequest );
    }

    void PIDSCircleMessageProcessor::addClearMessage(
        TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList destinationList,
        bool isTrain )
    { 
        TA_Base_Core::ThreadGuard guard( m_handleAccessLock );
        PIDSClearRequest *tmpClearRe = new PIDSClearRequest();
        tmpClearRe->setData( destinationList, isTrain );
        if ( tmpClearRe != NULL )
            m_clearRe.push_back( tmpClearRe );
    }


    PIDSCircleMessageProcessor* PIDSCircleMessageProcessor::getInstance()
    {
        if ( m_theClass == NULL )
            m_theClass = new PIDSCircleMessageProcessor();
        return m_theClass;
    }

    PIDSCircleMessageProcessor::PIDSCircleMessageProcessor()
    {
        FUNCTION_ENTRY( "PIDSCircleMessageProcessor" );
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Object of PIDSCircleMessageProcessor created" );
        m_adHoc.clear();
        m_timerMgr.activate();        
        //Add event handle 1 second later
        //m_timerId = m_timerMgr.schedule( this, NULL, ACE_OS::gettimeofday() + ACE_Time_Value( 1 ), m_interval );
		m_timerId = m_timerMgr.schedule( this, NULL, ACE_OS::gettimeofday() + ACE_Time_Value( 1 ), m_interval);
        FUNCTION_EXIT;
    }

    PIDSCircleMessageProcessor::~PIDSCircleMessageProcessor()
    {
        m_timerMgr.cancel( m_timerId ); //cancel timer
        for ( std::vector<PIDSAdhocData*>::iterator ite = m_adHoc.begin();
                ite != m_adHoc.end();
                ++ite )
        {
            delete *ite;
        }

        for ( std::vector<PIDSClearRequest*>::iterator ite = m_clearRe.begin();
                ite != m_clearRe.end();
                ++ite )
        {
            delete *ite;
        }       

    }

}