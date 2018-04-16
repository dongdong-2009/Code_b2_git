
#pragma once

#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/ITcpEvent.h"

namespace TA_IRS_App
{
   //class RadioEvent;

	enum ETcpControlConnectionStatus
	{
		NoAppliable,
		ConnectToPrimary,
		ConnectToSecondary			
	};

	class IConnectionStatusObserver
	{
	public:
		    virtual void onConnectionFail(bool isPrimary)   = 0;
		    virtual void onConnectionResumed(bool isPrimary) = 0;

	};

	class ITcpConnectionStatusObserver
	{
	public:
		virtual void onConnectionLost() = 0;
		virtual void onConnectionEstablish() = 0;
		virtual void onSwitchConnection() =	0;
	};

	class ITcpEventProcessor
	{
	public:
			/**
			* This method will receive the incoming events from the radio system.  It will notify waiting RadioSession of a new event through the RadioTransactionManager.  It will process all unsolited events.
			* @param    event
			*/
			virtual void onReceiveEvent(ITcpEvent& event)  = 0;
			/**
			* createCompatibleTcpEvent. Create a TcpEvent that is compatible with this type of observer 
			* @param    isPrimary
			*/
			virtual ITcpEvent * createCompatibleTcpEvent() = 0;
		
	};
}