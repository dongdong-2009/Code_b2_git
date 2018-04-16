/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/traffic_historian/src/HistoryMonitor.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
//
// HistoryMonitor.cpp
//
// Author: Craig Grant 22/01/2001
//
// Implements the HistoryMonitor class which monitors the TrafficDetectors and
// has them save the data when the sampling period is up.
//
// KT 19/02/02: The Historian no longer averages the received samples 
// over a long period.  It just logs received data for the statistics
// it is interested in straight to the database.  Therefore, this
// class is no longer required.

/*
//
// Ignore STL warnings on Windows.
//
#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include "TDMap.h"
#include "HistoryMonitor.h"
#include "MiThreadGuard.h"

HistoryMonitor::HistoryMonitor() : m_Done( false )
{
}

// The main loop for this thread
void HistoryMonitor::run()
{
	time_t sampleInterval = 0;

	TDMap* theTDMap = TDMap::Instance();

	do
	{
		// Get the time interval until logging is required
		if( theTDMap->NextSampleTime( sampleInterval ) )
		{
			if( sampleInterval > 0 )
				this->sleep( 1000*sampleInterval );
			else
				this->sleep( 200 );
		}
		else
		{
			// Probably a database problem, wait 200ms and try again
			this->sleep( 200 );
		}

		// Synchronise this so it can't be interrupted.
		{
			MiThreadGuard threadGuard( m_securityGuard );
			// Write out data for any traffic detectors that need it.
			theTDMap->LogSamples();
		}
		
	} while( !m_Done );
}

// Termination method
void HistoryMonitor::terminate()
{
    MiThreadGuard threadGuard( m_securityGuard );
    m_Done = true;
}
*/