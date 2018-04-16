/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioTetraAgent/src/RadioServerMonitor.cpp $
 * @author:  albertus
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/RadioTetraAgent/src/RadioServerMonitor.h"
//#include "app/radio/RadioTetraAgent/src/RadioServerAgentCommandImpl.h"
#include "app/radio/RadioTetraAgent/src/RadioSDS.h"

using namespace TA_IRS_App;

RadioServerMonitor::RadioServerMonitor() ://m_commandImpl(NULL), 
m_isRunning(false),
m_pPrimaryRadioSDS(NULL),
m_pSecondaryRadioSDS(NULL),
m_pCurrentRadioSDS(NULL),
m_RadioSDSHandler(NULL)
{

}

RadioServerMonitor::~RadioServerMonitor()
{

}


//void RadioServerMonitor::setCommandImpl(RadioServerAgentCommandImpl* pImpl)
//{
//    m_commandImpl = pImpl;
//}

void RadioServerMonitor::setRadioSDSHandler(RadioSDSHandler* radioSDSHandler)
{
	m_RadioSDSHandler =  radioSDSHandler;
}

void RadioServerMonitor::setRadioSDS(RadioSDS* pSDS, bool isPrimary)
{
	if (isPrimary) {
		m_pPrimaryRadioSDS = pSDS;
	} else {
		m_pSecondaryRadioSDS = pSDS;
	}
}

void RadioServerMonitor::switchRadioSDS(bool toPrimary)
{
	TA_Base_Core::ThreadGuard lock(m_lock);
	m_pCurrentRadioSDS = (toPrimary) ? m_pPrimaryRadioSDS : m_pSecondaryRadioSDS;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "switchRadioSDS toPirmary=%d",toPrimary );
}


void RadioServerMonitor::start()
{
    m_isRunning = true;
    activate();
}

void RadioServerMonitor::stop()
{
    m_isRunning = false;
    wait();
}

int RadioServerMonitor::svc()
{
    while (m_isRunning)
    {
        try
        {
            //m_commandImpl->initializeCADI();
			TA_Base_Core::ThreadGuard lock(m_lock);
			if ( NULL != m_pCurrentRadioSDS ) {
				if ( m_pCurrentRadioSDS->initializeSDS() ) {
                    // if registration was done, give a slight delay for next registration
                    ACE_OS::sleep(1);
				}

				if(m_RadioSDSHandler != NULL)
				{
					if(m_pCurrentRadioSDS->needReconnection())
					{
						m_pCurrentRadioSDS->resetRetryCount();
						m_RadioSDSHandler->clearSDS();
						m_RadioSDSHandler->connectToSDS(m_pCurrentRadioSDS->getSDSHost(),m_pCurrentRadioSDS->getSDSPort());

					}
				}
				
			}
            //for (std::vector<RadioSDS*>::iterator it = m_radioSDS.begin(); it != m_radioSDS.end(); ++it)
            //{
            //    if ((*it)->initializeSDS())
            //    {
            //        // if registration was done, give a slight delay for next registration
            //        ACE_OS::sleep(1);
            //    }
            //}
        }
        catch (...)
        {
        }
		
		//after sleeping for one sec, check if thread is set to stop 
		//sleep for 5 sec in total
		for(int i =0; i < 5; i++)
		{	
			if(!m_isRunning)
				break;
			ACE_OS::sleep(1);
		}
        
    }

    return 0;
}
