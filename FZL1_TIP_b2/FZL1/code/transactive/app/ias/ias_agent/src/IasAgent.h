/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ias/ias_agent/src/IasAgent.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//  IasAgent.h
//  Implementation of the Class IasAgent
//  Created on:      04-Mar-2005 08:24:05 AM
///////////////////////////////////////////////////////////

#if !defined(IasAgent_68A2CE87_D7D2_4773_AE7E_4D5E2AB31000__INCLUDED_)
#define IasAgent_68A2CE87_D7D2_4773_AE7E_4D5E2AB31000__INCLUDED_

#include "ConnectionTester.h"
#include "StatusReportProcessor.h"
#include "MessageProcessor.h"
#include "IasCommunicationsHandler.h"
#include "IConnectionStateObserver.h"
#include "IIncomingMessageProcessor.h"
#include "IOutgoingMessageSender.h"

/**
 * The IasAgent is the entry-point class for the outsourcer.  This class is
 * responsible for setting up the outsourced components to enable runtime
 * operations.  During runtime, the TransActive framework may send various events
 * to the IasAgent class.  These handlers need to be propogated to the affected
 * classes.  In addition, the IasAgent acts as a hub for the interactions between
 * its members and is also responsible for detecting dual IAS server failures.
 * @version 1.0
 * @created 04-Mar-2005 08:24:05 AM
 */
class IasAgent : public IConnectionStateObserver, public IIncomingMessageProcessor, public IOutgoingMessageSender 
{

public:
	IasAgent();
	virtual ~IasAgent();
	IasAgent(const IasAgent& theIasAgent);

public:
	/**
	 * Constructs 2 IasCommunicationsHandlers, 2 ConnectionTesters, a MessageProcessor
	 * and a StatusReportProcessor.
	 * @param transactiveInterfaces    Concrete instance of the ItaIasAgentSummary
	 * interface providing access to ItaRuntime and ItaScada interfaces.
	 * 
	 */
	IasAgent(ItaIasAgentSummary& transactiveInterfaces){

	}

	/**
	 * Called by TransActive framework when entering runtime processing.  All required
	 * threads should be spawned and control returned to the framework.
	 */
	virtual void onStart();
	/**
	 * Called by TransActive framework when entering runtime processing.  All runtime
	 * threads should be terminated using terminateAndWait() before returning to the
	 * framework.
	 */
	virtual void onTerminate();
	/**
	 * Called by TransActive framework when switching to Control Mode.  Processing
	 * must complete in a timely manner (roughly <10secs).
	 */
	void onControlMode();
	/**
	 * Called by TransActive framework when switching to Monitor Mode.  Processing
	 * must complete in a timely manner (roughly <10secs).
	 */
	void onMonitorMode();
	/**
	 * Allows the framework to notify the entry-point class to updates in
	 * configuration.
	 * 
	 * Expecting either:
	 * - ConnectionTestPollTimeInSecs
	 * - ConnectionTestResponseInMsecs
	 * - ConnectionTestRetries
	 * - PrimaryOccServerLocationKey
	 * - PrimaryIasServerAddress
	 * - PrimaryIasServerPort
	 * - SecondaryIasServerAddress
	 * - SecondaryIasServerPort
	 * 
	 * These parameters must be propagated to the respective classes where they are to
	 * be utilised in runtime operations.  Ensure that these parameters are updated in
	 * a thread-safe manner.
	 * 
	 * For the special case of PrimaryOccServerLocationKey, this value must be
	 * converted to a MyServerId value for the ConnectionTester and MessageProcessor
	 * classes.  To do this, the current IAS Agent process location can be retreived
	 * via ItaRuntime::getMyProcessLocationKey().  If it is equal to
	 * PrimaryOccServerLocationKey, then the MyServerId value should be set to 1
	 * (otherwise 2).
	 * 
	 * If the parameter is not recognised, log a debug warning message and return.
	 * @param name    Name of the Agent Entity Parameter
	 * @param value    Value of the Agent Entity Parameter
	 * 
	 */
	virtual void onAgentConfigParameterUpdate(const std::string& name, const std::string& value);
	/**
	 * Callback for the ConnectionTester instances to indicate a change in the server
	 * connection state.  When one of the ConnectionTester classes notify the IasAgent
	 * class of a change in state, the IasAgent class should obtain the connection
	 * state of both instances via isConnectionDown().  If both are down, then the
	 * quality of all datapoints should be set to bad using the ItaScada interface.
	 * @param whichConnectionTester    Which ConnectionTester instance changing state.
	 * 
	 */
	void notifyConnectionState(ConnectionTester* whichConnectionTester);
	/**
	 * This method sets the quality of all local datapoints to ITA_QUALITY_BAD if both
	 * connections are down.  If at least one connection is up, the quality of all
	 * local datapoints are set to ITA_QUALITY_GOOD.
	 * 
	 * It is used in conjunction with m_currentOverallQuality to ensure that
	 * datapoints are updated only when the overallQuality has changed.
	 * @param isPrimaryConnectionDown
	 * @param isSecondaryConnectionDown
	 * 
	 */
	void setOverallQuality(bool isPrimaryConnectionDown, bool isSecondaryConnectionDown);
	/**
	 * Notify the class to send an AbstractOutgoingIasMessage packet.
	 * 
	 * The packet's receiveParty attribute is analysed to determine which of IAS
	 * Servers it is destined for.  Then the operation is delegated to the required
	 * communications handler instance.
	 * 
	 * If the required comms handler instance is NULL (i.e. during startup), return.
	 * @param theOutgoingMessage
	 * 
	 */
	virtual void sendMessage(AbstractOutgoingIasMessage& theOutgoingMessage);
	/**
	 * Notify the class to process a received IasStatusReport packet.  This is
	 * delegated directly to the StatusReportProcessor instance.
	 * 
	 * If the StatusReportProcessor instance is NULL (i.e. during startup), return.
	 * @param theIasStatusReport
	 * 
	 */
	virtual void processIasStatusReport(const IasStatusReport& theIasStatusReport);
	/**
	 * Notify the class to process a received TestMessageReport packet.
	 * 
	 * The TestMessageReport packet's hostId attribute is analysed to determine which
	 * IAS Server it is from.  Then it is delegated to one of the two ConnectionTester
	 * instances.
	 * 
	 * If the ConnectionTester instance is NULL (i.e. during startup), return.
	 * @param testMessageReport
	 * 
	 */
	virtual void processTestMessageReport(const TestMessageReport& testMessageReport);

protected:
	/**
	 * Concrete instance of the ItaIasAgentSummary interface providing access to
	 * ItaRuntime and ItaScada interfaces.
	 */
	ItaIasAgentSummary& m_transactiveInterfaces;
	/**
	 * Connection handler for IAS Server 1
	 */
	IasCommunicationsHandler* m_primaryConnection;
	/**
	 * Connection handler for IAS Server 2
	 */
	IasCommunicationsHandler* m_secondaryConnection;
	/**
	 * Connection tester for IAS Server 1.
	 */
	ConnectionTester* m_primaryConectionTester;
	/**
	 * Connection tester for IAS Server 2.
	 */
	IasCommunicationsHandler* m_secondaryConnectionTester;
	StatusReportProcessor* m_statusReportProcessor;
	MessageProcessor* m_messageProcessor;
	/**
	 * A flag used to keep track of the last set overall quality.  This is used to
	 * prevent unnecessary datapoint quality updates being sent when
	 * notifyConnectionState() is called.  
	 */
	EDataPointQuality m_currentOverallQuality;
	/**
	 * This method will take an agent config parameter name-value and validate it.
	 * The method will perform type and range checking based on the parameter name.
	 * If the parameter is determined to be invalid, the method will log a debug
	 * warning message.  The value input parameter will be modified to some default
	 * value so that the agent can continue to run.
	 * 
	 * The method returns true to indicate that the parameter is valid, and false to
	 * indicate that it is invalid (and to indicate that the input value has been
	 * modified).
	 * 
	 * This method should be used to validate all entity parameters on start-up before
	 * using the parameter values to construct the affected class instances.
	 * Additionally in runtime, when the agent receives
	 * onOnlineEntityParameterUpdate() call, the received parameter name-value should
	 * be validated before updating the affected classes.
	 * @param name    Agent entity parameter name
	 * @param value    Agent entity parameter value
	 * 
	 */
	bool validateAgentConfigParameter(const std::string& name, std::string& value);

};
#endif // !defined(IasAgent_68A2CE87_D7D2_4773_AE7E_4D5E2AB31000__INCLUDED_)