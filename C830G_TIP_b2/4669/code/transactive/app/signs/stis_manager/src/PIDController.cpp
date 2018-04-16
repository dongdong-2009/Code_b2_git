/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/PIDController.cpp $
  * @author:  San Teo
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/02/10 16:25:11 $
  * Last modified by:  $Author: builder $
  *
  * This class captures and performs the following control requests via the run params:
  * 1. Display the current message on a specific PID
  * 2. Turning a PID on/off
  * 3. Lock/unlock a PID.
  * 
  */

#include "stdafx.h"
#include "app/signs/stis_manager/src/PIDController.h"
#include "app/signs/stis_manager/src/GraphworxComms.h"
#include "app/signs/stis_manager/src/UserMessages.h"
#include "RightsManager.h" //Avteam
#include <iomanip>
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "bus/generic_gui/src/TransactiveMessage.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core\data_access_interface\src\LocationAccessFactory.h"
#include "core\message\src\MessagePublicationManager.h"
#include "core\message\types\TISAudit_MessageTypes.h"
#include "core\message\src\AuditMessageSender.h"
#include "boost/tokenizer.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TISAgentAccessFactory;
using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    const std::string PIDController::RPARAM_GETMESSAGE = "GetMessage";
    const std::string PIDController::RPARAM_STATE = "State";
    const std::string PIDController::RPARAM_LOCK = "Lock";
    const std::string PIDController::SWITCH_ON_PID = "ON";
    const std::string PIDController::LOCK_PID = "ON";
	const std::string PIDController::EMPTY_MESSAGE = "There is no Display message in selected PID";
    // This macro catches all exceptions and display the suitable user message 
    #define CATCH_ALL_EXCEPTIONS(action)                                                                            \
        catch (TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException&)                                      \
        {                                                                                                           \
            LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISServerNotConnectedException", "" );          \
            UserMessages::getInstance().displayError(                                                               \
                str( format(UserMessages::ERROR_REQUEST_FAILED)                                                     \
                % action % "TIS Agent not connected to STIS Server" ).c_str());                                     \
        }                                                                                                           \
        catch (TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException&)                                    \
        {                                                                                                           \
            LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISCommunicationTimeoutException", "" );        \
            UserMessages::getInstance().displayError(                                                               \
                str( format(UserMessages::ERROR_REQUEST_FAILED)                                                     \
                % action % "Timed out while waiting for STIS server response" ).c_str());                           \
        }                                                                                                           \
        catch (TA_Base_Bus::ISTISManagerCorbaDef::STISFunctionalityNotAvailableException&)                               \
        {                                                                                                           \
            LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISFunctionalityNotAvailableException", "" );   \
            UserMessages::getInstance().displayError(                                                               \
                str( format(UserMessages::ERROR_REQUEST_FAILED)                                                     \
                % action % "Function not available at this location" ).c_str());                                    \
        }                                                                                                           \
        catch (TA_Base_Bus::ISTISManagerCorbaDef::STISInvalidParameterException& ex)                                     \
        {                                                                                                           \
            LOG_EXCEPTION_CATCH(SourceInfo, "ISTISManagerCorbaDef::STISInvalidParameterException", ex.details.in()); \
            UserMessages::getInstance().displayError(                                                               \
                str( format(UserMessages::ERROR_REQUEST_FAILED)                                                     \
                % action % ex.details ).c_str());                                                                   \
        }                                                                                                           \
		catch ( const TA_Base_Core::ObjectResolutionException& ore )                                                     \
		{                                                                                                           \
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", ore.what() );                     \
            UserMessages::getInstance().displayError(                                                               \
                str( format(UserMessages::ERROR_REQUEST_FAILED)                                                     \
                % action % "Could not resolve TIS Agent" ).c_str());                                                \
		}                                                                                                           \
		catch ( const CORBA::Exception& ce )                                                                        \
		{                                                                                                           \
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception",                                                     \
                TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ce ) );                                        \
            UserMessages::getInstance().displayError(                                                               \
                str( format(UserMessages::ERROR_REQUEST_FAILED)                                                     \
                % action % "Could not resolve TIS Agent" ).c_str());                                                \
		}                                                                                                           \
        catch (const TA_Base_Core::DataException& e)                                                                     \
        {                                                                                                           \
            LOG_EXCEPTION_CATCH(SourceInfo, "Data Exception", e.what());                                            \
            UserMessages::getInstance().displayError(                                                               \
                str( format(UserMessages::ERROR_REQUEST_FAILED)                                                     \
                % action % "Database is not configured correctly" ).c_str());                                       \
        }                                                                                                           \
        catch (const TA_Base_Core::DatabaseException& e)                                                                 \
        {                                                                                                           \
            LOG_EXCEPTION_CATCH(SourceInfo, "Database Exception", e.what());                                        \
            UserMessages::getInstance().displayError(                                                               \
                str( format(UserMessages::ERROR_REQUEST_FAILED)                                                     \
                % action % "Failed to connect to database" ).c_str());                                              \
		}                                                                                                           \
        catch ( ... )                                                                                               \
        {                                                                                                           \
            LOG_EXCEPTION_CATCH(SourceInfo, "...", action);                                                         \
            UserMessages::getInstance().displayError(                                                               \
                str( format(UserMessages::ERROR_REQUEST_FAILED)                                                     \
                % action % "Could not resolve TIS Agent" ).c_str());                                                \
        }
    // end of CATCH_ALL_EXCEPTIONS


    PIDController::PIDController()
    {
    }


    void PIDController::initialise()
    {
	    FUNCTION_ENTRY( "initialise" );

        // Register for the PID selection/deselection 
	    TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_GETMESSAGE.c_str());	
	    TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_STATE.c_str());	
		TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_LOCK.c_str());

	    std::string val = TA_Base_Core::RunParams::getInstance().get(RPARAM_GETMESSAGE.c_str());	
	    if( val != "")
        {
            onRunParamChange(RPARAM_GETMESSAGE, val);
        }
	    val = TA_Base_Core::RunParams::getInstance().get(RPARAM_STATE.c_str());	
	    if( val != "")
        {
            onRunParamChange(RPARAM_STATE, val);
        }
	    val = TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCK.c_str());	
	    if( val != "")
        {
            onRunParamChange(RPARAM_LOCK, val);
        }

        FUNCTION_EXIT;
    }


    void PIDController::onRunParamChange( const std::string& name, const std::string& value )
    {
	    FUNCTION_ENTRY( "onRunParamChange" );
		std::string runParamReceived("");

		// Break up into the header elements and the PID list
		// The value will be prefixed with Station, XPos, pid, for all three runparams.  
        // It will also contain x and y position for GetMessage; 
		std::vector<std::string> valueParts = tokenizeString(value, ",");
		std::string station = valueParts[0];
        std::string entityName = valueParts[2];

		// Check for basic validity of GetMessage/State/Lock param, 
		// based on the number of items in the value
		if( ((name.compare(RPARAM_GETMESSAGE) == 0) ||
			(name.compare(RPARAM_STATE) == 0) ||
			(name.compare(RPARAM_LOCK) == 0) ) &&
			(valueParts.size() < 3 ))
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Invalid %s RunParam given to STIS Manager: %s",
                    name.c_str(),value.c_str());
    	    FUNCTION_EXIT;
			return;
		}

		if(name.compare(RPARAM_GETMESSAGE) == 0)
	    {
		    std::stringstream schematicIDStream( valueParts[1] );
		    long newSchematicId;
		    schematicIDStream >> newSchematicId;
            getCurrentMessage(station, entityName, newSchematicId, value);
    	    FUNCTION_EXIT;
            return;
        }

        if (valueParts.size() < 4)
        {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Invalid %s RunParam given to STIS Manager: %s",
                    name.c_str(),value.c_str());
    	    FUNCTION_EXIT;
            return;
        }

        std::string& val = valueParts[3];
		if (name.compare(RPARAM_STATE) == 0)
	    {
            changeState(entityName, val.compare(SWITCH_ON_PID) == 0);
        }
        else // RPARAM_LOCK
        {
            lock(entityName, val.compare(LOCK_PID) == 0);

			//TD17783
            // get the location name
            std::stringstream locationKeyStream;
            locationKeyStream << TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
            unsigned long locationKey = 0;
            locationKeyStream >> locationKey;

            TA_Base_Core::ILocation* location = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locationKey);

            // insert it into the map
			std::string locationName = location->getName();

			TA_Base_Core::DescriptionParameters desc;
			TA_Base_Core::NameValuePair pidNameVp("PID",  entityName);
			TA_Base_Core::NameValuePair locationVp( "location",  locationName);
			TA_Base_Core::NameValuePair status( "on/off",  (val.compare(LOCK_PID) == 0)? "lock" : "release");

			desc.push_back(&pidNameVp);
			desc.push_back(&locationVp);
			desc.push_back(&status);

			 TA_Base_Core::AuditMessageSender* auditMessageSender = 
					TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::TISAudit::Context );

			std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
			TA_Base_Core::IEntityData* guiEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity( entityName );
			TA_ASSERT ( guiEntity != NULL, "EntityAccessFactory returned a NULL entity and yet did not throw an exception." );

            auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::STISPidStatusChanged, guiEntity->getKey(),
			    desc, 
			    "", // Further description text
			    TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID),
			    "", // alarm ID - not required
			    "", // incident key - not required
			    ""); // event key - not required 

			delete location;
			delete auditMessageSender;
			//TD17783
        }
	    FUNCTION_EXIT;
    }


    void PIDController::getCurrentMessage(const std::string& station, const std::string& name, const long displayId, const std::string& displayCmd)
    {
	    FUNCTION_ENTRY( "getCurrentMessage" );
        std::string action = "Get current message of " + name + " at " + station;
        try
        {
		    std::vector<std::string> levels = tokenizeString( name, "." );
            TA_Base_Core::STISDestination dest;

            dest.station = station.c_str();

            unsigned int numLevels = levels.size();
            dest.levels.length(numLevels - 1);
            // the last token is the pid itself
            unsigned int i;
            for (i = 0; i < numLevels - 1; i++)
            {
                dest.levels[i] = levels[i].c_str();
            }
            dest.pids.length(1);
            dest.pids[0] = levels[i].c_str();

            // get the current message from agent then get graphworx to display the appropriate dialog
            // using the information that we got from the agent
            TA_Base_Core::CurrentDisplayingMessage* mesg = NULL;//  = TISAgentAccessFactory::getInstance().getSTISAgent()->getCurrentDisplayingMessage(dest);

			//libo++
			try
			{
				CORBA_CALL_RETURN(mesg,TISAgentAccessFactory::getInstance().getSTISAgent(),getCurrentDisplayingMessage,(dest));

			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL_RETURN(mesg,TISAgentAccessFactory::getInstance().getSTISAgent(),getCurrentDisplayingMessage,(dest));
			}
			//++libo

            std::string content(mesg->messageContent);
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "The message being shown on %s is %s with priority of %d", name.c_str(), content.c_str(), mesg->priority);

			std::stringstream priority;   // Fix for IFat
					
			std::string startTime(mesg->startTime);
            std::string endTime(mesg->endTime);

			if( !content.empty() )
			{
				priority << mesg->priority;
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
							 "Message content: %s", content.c_str() );

				std::string startTimeDate(""); 
				std::string endTimeDate("");
				
				if(content != EMPTY_MESSAGE)
				{
					startTimeDate = timeDateString(startTime);
					endTimeDate   = timeDateString(endTime);
				}
				else
				{
					content = "";
				}

				bool ok = GraphworxComms::getInstance().displayCurrentMessage(displayCmd, content, priority.str(), displayId, startTimeDate, endTimeDate );
				
				if (!ok)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, action.c_str());
					UserMessages::getInstance().displayError(
						str( format(UserMessages::ERROR_REQUEST_FAILED)
						% action % "Failed to display current message" ).c_str());
				}
			}
			else 
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, 
							 "Error while retrieve the current display message. Invalid message from STIS");
			}
        }
        CATCH_ALL_EXCEPTIONS(action.c_str());
	    FUNCTION_EXIT;
    }


    void PIDController::changeState(const std::string& name, const bool on)
    {
	    FUNCTION_ENTRY( "changeState" );
        //TD17901 Avteam++
		bool canPIDControl = RightsManager::getInstance().canPIDControl();
		if (canPIDControl == FALSE)
		{
			TA_Base_Bus::TransActiveMessage userMsg;
			CString actionName = "ChangState";
			userMsg << actionName;
			userMsg.showMsgBox(IDS_UE_020047);
		}
		else
		{
		//TD17901 ++AVteam

			std::string action = "Turn ";
			action += (on ? "on " : "off ");
			action += name;
		
			try
			{
				TISAgentAccessFactory::getInstance().getSTISAgentAtLocation(getStationName(name))->submitPIDControlRequest(name.c_str(), on ? TA_Base_Core::TURN_ON : TA_Base_Core::TURN_OFF, RunParams::getInstance().get(RPARAM_SESSIONID).c_str());
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, action.c_str());
			}
			CATCH_ALL_EXCEPTIONS(action.c_str());
		}
		FUNCTION_EXIT;
    }


    void PIDController::lock(const std::string& name, const bool on)
    {
	    FUNCTION_ENTRY( "lock" );
        
		//TD17901 Avteam++
		bool canPIDControl = RightsManager::getInstance().canPIDControl();
		
		if (canPIDControl == FALSE)
		{
			TA_Base_Bus::TransActiveMessage userMsg;
			CString actionName = "Lock";
			userMsg << actionName;
			userMsg.showMsgBox(IDS_UE_020047);
		}
		else
		{
		
		//TD17901 ++AVteam

		
			std::string action = (on ? "Lock " : "Unlock ");
			action += name;

			try
			{
				TISAgentAccessFactory::getInstance().getSTISAgentAtLocation(getStationName(name))->setLockStatus(name.c_str(), on, RunParams::getInstance().get(RPARAM_SESSIONID).c_str());
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, action.c_str());
			}
			CATCH_ALL_EXCEPTIONS(action.c_str());
		}
		FUNCTION_EXIT;
    }


    std::vector<std::string> PIDController::tokenizeString(const std::string& theString, const std::string& separatorList )
	{
        std::vector<std::string> parts;
        typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

        boost::char_separator<char> sep( separatorList.c_str() );
        tokenizer tokens(theString, sep);
        
        for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
        {
            parts.push_back(*tok_iter);
        }

        // if parts is empty, then this should return the entire string
        if (parts.size() == 0)
        {
            parts.push_back(theString);
        }

        return parts;
	}


    std::string PIDController::getStationName(const std::string& fullPidName)
    {
	    FUNCTION_ENTRY( "getStationName" );
		std::vector<std::string> levels = tokenizeString( fullPidName, "." );
        FUNCTION_EXIT;
        return levels[0];
    }


	std::string PIDController::timeDateString(const std::string& timeString)
	{
		
		std::string year = timeString.substr(0,4);
		std::string month = timeString.substr(4,2);
		std::string day = timeString.substr(6,2);
		std::string time = timeString.substr(8,6);
		std::string date = day+"/"+month+"/"+year;
		time.insert(2,":");
		time.insert(5,":");
		time.insert(8," ");
		return time+date;
		
	}


} // TA_IRS_App
