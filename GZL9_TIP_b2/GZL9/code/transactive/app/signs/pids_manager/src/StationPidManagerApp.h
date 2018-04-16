/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/signs/pids_manager/src/StationPidManagerApp.h $
 * @author:  
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * Starting point of the SPID Manager. Responsible for instantiating an object
 * that conforms to the IStationPidUpdater interface, and for passing a pointer to
 * this interface to the StationPidManagerGUI class for use by all dialogs that
 * require it.
 */

#include "resource.h"
#include "bus/generic_gui/src/TransActiveWinApp.h"
//#include <afxmt.h> //TD18416 AVteam
#include "app/signs/pids_manager/src/StationPidUpdater.h"
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

namespace TA_IRS_App
{

class StationPidManagerApp :public TA_Base_Bus::TransActiveWinApp 
{

public:
	StationPidManagerApp();
	virtual ~StationPidManagerApp();
    
//	virtual BOOL InitInstance();  //TD18416 AVteam

    DECLARE_MESSAGE_MAP()

	    //virtual ~StationPidManagerApp();

    /**
     * InitInstance
     *
     * This method must be implemented to ensure that when the application is
     * started it is started correctly and the same for each TransActive application.
     */
    virtual BOOL InitInstance();

    /**
     * ExitInstance (extension of base class implementation)
     *
     * Performs some additional cleanup (singleton instances etc)
     *
     */
    virtual int ExitInstance();

    void createSingletons();

    /**
     * destroySingletons
     *
     * Calls removeInstance on singletons in use by application
     * This can be called as many times as application wants to
     * (and in some cases, is known to be called twice when known
     * the singleton destruction can't be postponed, because of 
     * late subscriber destruction issues)
     *
     */ 
    void destroySingletons();


private:
//	CMutex * m_mutex; //TD18416 AVteam
//removed, not needed here
//private:
//	IStationPidUpdater* m_pidUpdater;
	//HANDLE appHandle;
};
/*
class StationPidManagerApp : public CWinApp 
{

public:
	StationPidManagerApp();
	virtual ~StationPidManagerApp();
    
	virtual BOOL InitInstance();  //TD18416 AVteam

    DECLARE_MESSAGE_MAP()

private:
//	CMutex * m_mutex; //TD18416 AVteam
//removed, not needed here
//private:
//	IStationPidUpdater* m_pidUpdater;
//	HANDLE appHandle;
};
*/

}