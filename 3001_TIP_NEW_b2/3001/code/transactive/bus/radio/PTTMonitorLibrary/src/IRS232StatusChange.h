/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/radio/PTTMonitorLibrary/src/IRS232StatusChange.h $
  * @author:  A. Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This class defines the callback interface used by the RS232MonitorThread class
  * to notify CTS change events.  All the methods are defined as pure.
  */

// IRS232StatusChange.h: interface for the IRS232StatusChange class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IRS232STATUSCHANGE_H__51FB481F_D61D_4563_8434_8AA498DB4DD6__INCLUDED_)
#define AFX_IRS232STATUSCHANGE_H__51FB481F_D61D_4563_8434_8AA498DB4DD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_IRS_Bus
{

	class IRS232StatusChange  
	{
		public:
			
			/**
			  * onCTSEnable
			  * 
			  * This method is called when the CTS changes from disabled to enabled
			  */
			virtual void onCTSEnable() = 0;


			/**
			  * onCTSDisable
			  * 
			  * This method is called when the CTS changes from enabled to disabled
			  */
			virtual void onCTSDisable() = 0;


			/**
			  * onPortBusy
			  * 
			  * This method is called when the communications port is busy and cannot
			  * be connected to.
			  */
			virtual void onPortBusy() = 0;

			/**
			  * onPortReady
			  * 
			  * This method is called when the communications port is ready and monitoring.
			  */
			virtual void onPortReady() = 0;

	};
}

#endif // !defined(AFX_IRS232STATUSCHANGE_H__51FB481F_D61D_4563_8434_8AA498DB4DD6__INCLUDED_)
