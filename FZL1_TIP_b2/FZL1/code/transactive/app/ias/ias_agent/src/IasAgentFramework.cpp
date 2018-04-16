/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ias/ias_agent/src/IasAgentFramework.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//  IasAgentFramework.cpp
//  Implementation of the Class IasAgentFramework
//  Created on:      04-Mar-2005 08:24:07 AM
///////////////////////////////////////////////////////////

#include "IasAgentFramework.h"


IasAgentFramework::IasAgentFramework(){

}



IasAgentFramework::~IasAgentFramework(){

}


/**
 * agentTerminate  This pure virtual method is called from the onTerminate method
 * in GenericAgent. This allows the agent to do any of its own cleaning up before
 * exiting.  If the agent's entities implement CORBA interfaces, this method
 * should take care of deactivating them. It is not possible for GenericAgent to
 * do this because the IEntity  interface is not a ServantBase.  If the class
 * implementing this interface holds the GenericAgent as a member variable, DO NOT
 * delete it inside this method. GenericAgent waits for this call to return before
 * shutting down the ORB. Deleting the GenericAgent from within this method will
 * cause obvious problems.
 */
void IasAgentFramework::agentTerminate(){

}


/**
 * createEntity  This method takes Entity Configuration data stored in the
 * database and generates an object that implements the IEntity interface. The
 * returned objects are stored and managed within Generic Agent. The Agent can get
 * a  pointer to the objects via the getEntityMap method in GenericAgent.  If
 * create entity throws an exception when Generic Agent is creating its vector of
 * entities (in the GenericAgent::doEntityConfiguration method),  the Generic
 * Agent construction will fail.
 * @param Entity data retrieved from the database.
 * @return The constructed entity
 * @param EntityData
 * 
 */
IEntity* IasAgentFramework::createEntity(TA_Core::IEntityData& EntityData){

}


/**
 * agentSetMonitor  This pure virtual method is called by GenericAgent whenever it
 * receives a request from the Process Monitor to change its operating state to
 * monitor. The derived agent should respond by subscribing/unsubscribing to/from
 * the  appropriate message channels and performing any other internal house
 * keeping.
 */
void IasAgentFramework::agentSetMonitor(){

}


/**
 * agentSetControl  This pure virtual method is called by GenericAgent whenever it
 * receives a request from the Process Monitor to change its operating state to
 * control. The derived agent should respond by subscribing/unsubscribing to/from
 * the  appropriate message channels and performing any other internal house
 * keeping.
 */
void IasAgentFramework::agentSetControl(){

}


/**
 * notifyGroupOffline  This request is passed on by GenericAgent when it receives
 * a call to  notifyGroupOffline from the Process Monitor.
 * @param The group name
 * @param group
 * 
 */
void IasAgentFramework::notifyGroupOffline(const std::string& group){

}


/**
 * notifyGroupOnline  This request is passed on by GenericAgent when it receives a
 * call to  notifyGroupOnline from the Process Monitor.
 * @param The group name
 * @param group
 * 
 */
void IasAgentFramework::notifyGroupOnline(const std::string& group){

}


/**
 * registerForStateUpdates  This pure virtual method forces agents to subscribe
 * for either state updates or state update requests, depending on whether or not
 * they are operating in monitor or control mode.
 */
void IasAgentFramework::registerForStateUpdates(){

}


/**
 * receiveSpecialisedMessage  This method is called whenever the Agent receives a
 * state update or state update request (depending on their mode of operation)
 * @param message
 * 
 */
void IasAgentFramework::receiveSpecialisedMessage(const TA_Core::StateUpdateMessageCorbaDef& message){

}


/**
 * processOnlineUpdate  Generic Agent will handle the receiving of online updates.
 * However, in some cases, it will be necessary to pass the updates on to the
 * registered agent. For example, if a child entity needs to be updated.
 * @param The update event
 * @param updateEvent
 * 
 */
void IasAgentFramework::processOnlineUpdate(const TA_Core::ConfigUpdateDetails& updateEvent){

}


