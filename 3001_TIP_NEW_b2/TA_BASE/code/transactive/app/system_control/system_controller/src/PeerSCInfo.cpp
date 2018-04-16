/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/system_control/system_controller/src/PeerSCInfo.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2015/09/29 18:25:34 $
  * Last modified by:  $Author: limin.zhu $
  *
  * This class maintains information about a peer
  * System Controller (or System Controller that is
  * observing a group we are in). It contains a pointer to
  * an IHeartbeatReceiverCorbaDef that it uses to send
  * heartbeats.
  *
  */

#ifdef _MSC_VER
    #pragma warning(disable:4786)
#endif

#include "PeerSCInfo.h"
#include "SystemController.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include <string>
#include <sstream>

#include <sys/timeb.h>

#ifdef WIN32
	#include <sys/types.h>
	#define ftime _ftime   // #defines to make Windows behave like Unix.
	#define timeb _timeb
#endif

using TA_Base_Core::CorbaUtil;
using TA_Base_Core::DebugUtil;


namespace TA_Base_App
{

    //
    // Constructor
    //
    PeerSCInfo::PeerSCInfo(const std::string& myHostname, const std::string& peerHostname, int heartbeatSendPeriod, int heartbeatSendNumber)
        : m_myHostname(myHostname),
          m_peerHostname(peerHostname),
          m_isPeerResolved(false),
          m_heartbeatSendPeriod(heartbeatSendPeriod),
          m_heartbeatSendNumber(heartbeatSendNumber),
          m_systemControllerMonitor(NULL), 
		  //m_firstHeartbeatTime(0),
          //m_heartbeatCount(0), 
		  m_isRunning(false),
          m_port(0)
    {
    }

    //
    // Destructor
    //
    PeerSCInfo::~PeerSCInfo()
    {
    }

    //
    // resolvePeer
    //
    void PeerSCInfo::resolvePeer()
    {
        FUNCTION_ENTRY("resolvePeer");

        CORBA::Object_var obj;
        m_systemControllerMonitor = TA_Base_Core::IHeartbeatReceiverCorbaDef::_nil();

        // Create a string in the form: 
        // "corbaloc::hostname:port/SystemControllerMonitor"
        std::ostringstream systemControllerMonitorString;
        systemControllerMonitorString << "corbaloc::" << m_peerHostname << ":"
			<< m_port << "/SystemControllerMonitor";

        // Attempt to resolve an IHeartbeatReceiverCorbaDef object
        try
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Resolving object: %s", systemControllerMonitorString.str().c_str());

            obj = CorbaUtil::getInstance().stringToObject(systemControllerMonitorString.str());
            if (!CORBA::is_nil(obj))
		    {
                m_systemControllerMonitor = 
                        TA_Base_Core::IHeartbeatReceiverCorbaDef::_narrow(obj);
                m_isPeerResolved = true;

                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Resolved System Controller");

				// Reset the heartbeat count.
				//m_heartbeatCount = 0;
            }
            else
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                            "Error resolving peer SystemControllerMonitor: %s",
                            m_peerHostname.c_str());
                m_isPeerResolved = false;
            }
        }
        catch (CORBA::SystemException& e)
        {
            std::string desc = "Error resolving peer SystemControllerMonitor: " + m_peerHostname + " - ";
            desc += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::SystemException", desc);

            m_isPeerResolved = false;
        }
        catch (CORBA::Exception& e)
        {
            std::string desc = "Error resolving peer SystemControllerMonitor: " + m_peerHostname + " - ";
            desc += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", desc);

            m_isPeerResolved = false;
        }
        catch (...)
        {
            std::string desc = "Error resolving peer SystemControllerMonitor: " + m_peerHostname;
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", desc);

            m_isPeerResolved = false;
        }

        FUNCTION_EXIT;
    }

    //
    // beatHeart
    //
    void PeerSCInfo::beatHeart()
    {
        if (!m_isPeerResolved)
        {
            resolvePeer();
        }

        if (m_isPeerResolved)
        {
            try
            {
				// yanrong++: CL-21195, 2010-01-22  -- no need to count the heartbeat times
				/*
                static unsigned long MAX_HEARTBEAT_COUNT = 100000; // to prevent nextHeartbeatTime overflow;

				// Reset the timestamp if this is the first heartbeat.
				if ((m_heartbeatCount == 0) || (m_heartbeatCount >= MAX_HEARTBEAT_COUNT))
				{
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Sending heartbeat...");

					struct timeb timebuffer;
					ftime( &timebuffer );
					// yanrong++: CL-21195, 2010-01-18
					//m_firstHeartbeatTime = (timebuffer.time * 1000) + timebuffer.millitm;
					m_firstHeartbeatTime = timebuffer.time;
					m_firstHeartbeatTime *= 1000; // second -> milliseconds
					m_firstHeartbeatTime += timebuffer.millitm;
					// ++yanrong
                    m_heartbeatCount = 0;
				}
				*/
				// ++yanrong

				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[Group offline log]sending heartbeat to peer...");
				// Actually send the heartbeat.
                m_systemControllerMonitor->receiveHeartbeat(m_myHostname.c_str());

				// Increment the count.
				//m_heartbeatCount++;
            }
            catch(const std::exception& ex)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "std::exception", ex.what());

                // An error occurred, better re-resolve the peer next time.
                m_isPeerResolved = false;
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "An error occurred while seding heartbeat");
                
                // An error occurred, better re-resolve the peer next time.
                m_isPeerResolved = false;
            }
        }
    }

    //
    // run
    //
    void PeerSCInfo::run()
    {
		// yanrong++: CL-21195, 2010-01-22 -- no need calculate the sleep time, 
		// just simply sleep the heartbeat send period
		/*
		// yanrong++: CL-21195, 2010-01-18
		//unsigned long currentTime = 0;			// Time after sending latest heartbeat.
		//unsigned long nextHeartbeatTime = 0;	// Time to send the next heartbeat.
		double currentTime = 0;			// Time after sending latest heartbeat.
		double nextHeartbeatTime = 0;	// Time to send the next heartbeat.
		// ++yanrong
		*/
		// ++yanrong

		//m_heartbeatCount = 0;
		m_isPeerResolved = false;

		m_port = atoi(TA_Base_Core::RunParams::getInstance().get(RPARAM_MGRPORT).c_str());
		// yanrong++: CL-21195
		//struct timeb timebuffer; // Used to get the current time from ftime.
		// ++yanrong

        m_isRunning = true;

		while (m_isRunning)
        {
			// yanrong++: CL-21195, 2010-01-22
			timeb hbBeforeTime, hbAfterTime;
			ftime( &hbBeforeTime );
			// ++yanrong

            // Sends a heartbeat from m_myHostname to m_peerHostname
            beatHeart();

			// yanrong++: CL-21195, 2010-01-22
			ftime( &hbAfterTime);
			// ++yanrong

			// yanrong++: CL-21195, 2010-01-22 -- no need calculate the sleep time, 
			// just simply sleep the heartbeat send period
			/*
			// Get the current time.
			ftime( &timebuffer );
			// yanrong++: CL-21195, 2010-01-18
			//currentTime = (timebuffer.time * 1000) + timebuffer.millitm;
			currentTime = timebuffer.time;
			currentTime *= 1000;
			currentTime += timebuffer.millitm;
			// ++yanrong

			if (m_isPeerResolved)
			{
				// Calculate the sleep time.
				nextHeartbeatTime = m_firstHeartbeatTime 
								+ m_heartbeatCount * m_heartbeatSendPeriod;

                unsigned long sleepTimeMsecs = (unsigned long)(nextHeartbeatTime - currentTime); // yanrong: prevent compile warning C4244.

				// Sleep until it is the required time for the next heartbeat.
				if ( (nextHeartbeatTime > currentTime) && (sleepTimeMsecs < m_heartbeatSendPeriod) )
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[Group offline log]PeerSCInfo::run sleeping for %ul ms...", sleepTimeMsecs);
                    sleep(sleepTimeMsecs);
				}
				else
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "[Group offline log] current time is too far from next heartbeat time... m_heartbeatCount:%ul, m_firstHeartbeatTime:%ul, nextHeartbeatTime:%ul, currentTime:%ul", m_heartbeatCount, m_firstHeartbeatTime, nextHeartbeatTime, currentTime);
                    // reset nextHeartbeatTime if server time got problem.
					m_firstHeartbeatTime = currentTime;
                    m_heartbeatCount = 0;
				}
			}
			else
			{
				// Haven't resolved System Controller yet. Just sleep for
				// heartbeat send period before trying again.
				sleep(m_heartbeatSendPeriod);
			}
			*/
			if(m_isPeerResolved)
			{
				unsigned long hbElapseTimeMs = hbAfterTime.time - hbBeforeTime.time;
				hbElapseTimeMs *= 1000;
				hbElapseTimeMs += (hbAfterTime.millitm - hbBeforeTime.millitm);

				if(hbElapseTimeMs < m_heartbeatSendPeriod)
				{
					sleep(m_heartbeatSendPeriod - hbElapseTimeMs);
				}
			}
			else
			{
				// just simply sleep the heartbeat send period
				sleep(m_heartbeatSendPeriod);
			}
			// ++yanrong
        }
    }

    //
    // terminate
    //
    void PeerSCInfo::terminate()
    {
        m_isRunning = false;
    }
}
