/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioTetraAgent/src/RadioServerMonitor.h $
 * @author:  albertus
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef __RadioServerMonitor309204029340234__
#define __RadioServerMonitor309204029340234__

#include <vector>
#include "ace/Task.h"
#include "app/radio/RadioTetraAgent/src/RadioSDSHandler.h"
namespace TA_IRS_App
{
//class RadioServerAgentCommandImpl;
class RadioSDS;

class RadioServerMonitor : public virtual ACE_Task_Base
{
public:
    RadioServerMonitor();
    ~RadioServerMonitor();

//    void setCommandImpl(RadioServerAgentCommandImpl* pImpl);
    //void setRadioSDS(const std::vector<RadioSDS*>& pSDS);
	void setRadioSDS(RadioSDS* pSDS, bool isPrimary);
	void setRadioSDSHandler(RadioSDSHandler* radioSDSHandler);

	void switchRadioSDS(bool toPrimary);

	void start();
    void stop();
	
	virtual int  svc (void);		

private:
    RadioServerMonitor(const RadioServerMonitor&);
    void operator=(const RadioServerMonitor&);


private:
    //RadioServerAgentCommandImpl* m_commandImpl;
    //std::vector<RadioSDS*> m_radioSDS;
	RadioSDS* m_pPrimaryRadioSDS;
	RadioSDS* m_pSecondaryRadioSDS;
	RadioSDS* m_pCurrentRadioSDS;
	RadioSDSHandler* m_RadioSDSHandler;

    bool m_isRunning;

	/**
	*	Used for safe acess;
	*/
	TA_Base_Core::ReEntrantThreadLockable m_lock;
};
}

#endif
