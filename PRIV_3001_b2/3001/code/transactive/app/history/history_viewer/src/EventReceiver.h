/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/history/history_viewer/src/EventReceiver.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// EventReceiver.h: interface for the CEventReceiver class.

//

//////////////////////////////////////////////////////////////////////



#if !defined(AFX_EVENTRECEIVER_H__518C6763_9929_11D6_B25F_0050BAB25370__INCLUDED_)

#define AFX_EVENTRECEIVER_H__518C6763_9929_11D6_B25F_0050BAB25370__INCLUDED_



#if _MSC_VER > 1000

#pragma once

#endif // _MSC_VER > 1000



#include "EventListener.h" //<CosEventComm.hh>

//#include "CommonDmIf.h"

#include <string>

#include <time.h>

#include "SafeQueue.h"



struct HistoryUpdate

{

	unsigned long historyConfigPkey;

	time_t updateTime;

};



class CEventReceiver : CallbackUser // public virtual POA_CosEventComm::PushConsumer  

{

public:

	CEventReceiver();

	virtual ~CEventReceiver();



	// This exception is thrown when there is problems with events

	// A string should be supplied describing the failure.

	class ExEvent 

	{

	public:

		//Constructor - str should describe nature of failure

		ExEvent(const std::string& str) : m_str(str){}

		

		//Destructor

		virtual ~ExEvent(){}

				

		//Return the string describing the failure

		std::string GetDesc() const {return m_str;} 



	private:

		std::string m_str;

	};



	// This method registers this object to listen for events on the system channel

	// This method must be called before events can be received

	virtual void Register() throw (ExEvent&);



	// This should do the following:

	// 1) determine if the event is a History Update

	// 2) If it is push the status string onto the safeQ

	// 3) Call Notify()

	//virtual void push(const CORBA::Any& any) throw (ExEvent&);

	void callback(const CallbackSubmission* submission)

		throw( CORBA::SystemException);



	// This has just been overridden because it is pure virtual in the base class

	// The method currently does nothing

	//virtual void disconnect_push_consumer() throw (ExEvent&);



	// This method send a message to the main window indicating that History has changed

	void Notify();



	// Get a HistoryUpdate from the queue.

	// Returns true if there is a HistoryUpdate on the queue.

	bool GetHistoryUpdate(HistoryUpdate& historyUpdate);



private:

	SafeQueue<HistoryUpdate> m_SafeQueue;

};



#endif // !defined(AFX_EVENTRECEIVER_H__518C6763_9929_11D6_B25F_0050BAB25370__INCLUDED_)





