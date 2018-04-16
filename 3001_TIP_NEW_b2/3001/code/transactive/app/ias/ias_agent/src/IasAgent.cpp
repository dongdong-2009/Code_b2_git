/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ias/ias_agent/src/IasAgent.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
///////////////////////////////////////////////////////////
//  IasAgent.cpp
//  Implementation of the Class IasAgent
//  Created on:      04-Mar-2005 08:24:05 AM
///////////////////////////////////////////////////////////

#include "IasAgent.h"


IasAgent::IasAgent(){

}



IasAgent::~IasAgent(){

}



IasAgent::IasAgent(const IasAgent& theIasAgent){

}





/**
 * Called by TransActive framework when entering runtime processing.  All required
 * threads should be spawned and control returned to the framework.
 */
void IasAgent::onStart(){

}


/**
 * Called by TransActive framework when entering runtime processing.  All runtime
 * threads should be terminated using terminateAndWait() before returning to the
 * framework.
 */
void IasAgent::onTerminate(){

}


/**
 * Called by TransActive framework when switching to Control Mode.  Processing
 * must complete in a timely manner (roughly <10secs).
 */
void IasAgent::onControlMode(){

}


/**
 * Called by TransActive framework when switching to Monitor Mode.  Processing
 * must complete in a timely manner (roughly <10secs).
 */
void IasAgent::onMonitorMode(){

}


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
void IasAgent::onAgentConfigParameterUpdate(const std::string& name, const std::string& value){

}


/**
 * Callback for the ConnectionTester instances to indicate a change in the server
 * connection state.  When one of the ConnectionTester classes notify the IasAgent
 * class of a change in state, the IasAgent class should obtain the connection
 * state of both instances via isConnectionDown().  If both are down, then the
 * quality of all datapoints should be set to bad using the ItaScada interface.
 * @param whichConnectionTester    Which ConnectionTester instance changing state.
 * 
 */
void IasAgent::notifyConnectionState(ConnectionTester* whichConnectionTester){

}


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
bool IasAgent::validateAgentConfigParameter(const std::string& name, std::string& value){

}


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
void IasAgent::setOverallQuality(bool isPrimaryConnectionDown, bool isSecondaryConnectionDown){

}


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
void IasAgent::sendMessage(AbstractOutgoingIasMessage& theOutgoingMessage){

}


/**
 * Notify the class to process a received IasStatusReport packet.  This is
 * delegated directly to the StatusReportProcessor instance.
 * 
 * If the StatusReportProcessor instance is NULL (i.e. during startup), return.
 * @param theIasStatusReport
 * 
 */
void IasAgent::processIasStatusReport(const IasStatusReport& theIasStatusReport){

}


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
void IasAgent::processTestMessageReport(const TestMessageReport& testMessageReport){

}


