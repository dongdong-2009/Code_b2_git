/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/RunParamListener.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * RunParamListener listens out for general RunParam changes, 
  *     and queue changes (to ensure non are missed)
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "app/pa/PAManager/src/StdAfx.h"
#include "app/pa/PAManager/src/RunParamListener.h"
#include "app/pa/PaManager/src/PaErrorHandler.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "app/pa/PAManager/src/GraphworxComms.h"
#include "app/pa/PAManager/src/PAManagerDlg.h"
#include "app/pa/PAManager/src/IRestartApplicationCallback.h"

#include "bus/pa/cachedmaps/src/CachedMaps.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/PasExceptions.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// RunParams are always capitalised
const std::string RunParamListener::SELECT_TAG = "Select";
const std::string RunParamListener::DESELECT_TAG = "Deselect";

const std::string RunParamListener::DISPLAY_TAG = "Display";
const std::string RunParamListener::UPDATE_SELECTION_TAG = "SynchroniseSelected";
// TD 15349
const std::string RunParamListener::SELECTED_STATION_MSG_TAG = "SelectedStationPAMessage";
const std::string RunParamListener::SELECTED_TRAIN_MSG_TAG = "SelectedTrainPAMessage";
// TD 15349

RunParamListener RunParamListener::s_instance;

RunParamListener::RunParamListener()
:
m_visible(false),
m_restartWindowCallback(0)
{
    registerTag(SELECT_TAG);
    registerTag(DESELECT_TAG);
    registerTag(DISPLAY_TAG);
    registerTag(UPDATE_SELECTION_TAG);
	// TD 15349
	registerTag(SELECTED_STATION_MSG_TAG);
	registerTag(SELECTED_TRAIN_MSG_TAG);
	// TD 15349
    // Need to listen out for any change in these RunParams
    for (std::vector<RunParamChangeStruct>::iterator itr = m_registeredTags.begin(); 
            itr != m_registeredTags.end(); 
            itr ++)
    {
        TA_Base_Core::RunParams::getInstance().registerRunParamUser(
                                                    this, 
                                                    (*itr).m_paramName.c_str());
        // Set initial item in changed queue (to ensure it gets dispatched)
        std::string initialValue;
    	initialValue = TA_Base_Core::RunParams::getInstance().get((*itr).m_paramName.c_str());
	    if ( initialValue != "" )
        {
            (*itr).m_valueChanges.push_back(initialValue);
        }
    }
}


RunParamListener::~RunParamListener()
{
    TA_Base_Core::RunParams::getInstance().deregisterRunParamUser(this);
}


std::string RunParamListener::findIdentityString()
{
    // We always inspect the "Display" string first - as
    // when we're restarting the PA Manager, this may be the
    // only runparam set with the correct identity
    // (the other runparams may still be pointing to the old identity)
    std::string anyCommand = TA_Base_Core::RunParams::getInstance().get(DISPLAY_TAG.c_str());
    
    if (anyCommand.empty())
    {
        // If 'Display' tag not available, fall back to the other tags..
        for (std::vector<RunParamChangeStruct>::iterator itr = m_registeredTags.begin();
                itr != m_registeredTags.end();
                itr ++)
        {
            anyCommand = TA_Base_Core::RunParams::getInstance().get(itr->m_paramName.c_str());
            if (!anyCommand.empty())
            {
                // Found a valid command parameter
                break;
            }
        }
    }

    if (anyCommand.empty())
    {   
        return "";
    }
    
    return extractStringIdentifierString(anyCommand);
}


std::string RunParamListener::findVirtualLocation()
{
    std::string identifier = findIdentityString();
    return getStationName(identifier); 
}


std::string RunParamListener::getSchematicIdentifier()
{
    // Allow exceptions to propogate
    return getSchematicName(m_identityString.getValue()); 
}


bool RunParamListener::retrieveInitialRunParamIdentity()
{
    std::string identifier = findIdentityString();

    if (identifier.empty())
    {
        // No identity if no recognized commands available
        m_identityString.setValue("");
        return false;
    }

    // Extract command line identifier from associated parameter value
    setNewIdentityFromIdentifier(identifier);

    if (m_identityString.hasBeenSet() && 
        m_identityString.getValue().size() > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool RunParamListener::setNewIdentityFromIdentifier(const std::string& identifier)
{
    // We may switch (always at OCC, but must check for station)
    if (CachedConfig::getInstance()->isAtOcc())
    {
        m_identityString.setValue(identifier);
        return true;    // Notify of identity change
    }
    else
    {
        // Check if the station identified at head of identifier string
        // matches our physical location
        std::string stationName = getStationName(identifier);
        
        // Removed: Now able to run for foreign station (might need to later on
        // only allow operation on the station to the left - or perhaps just leave
        // this up to rights checking..)
        //if (isPhysicallyLocatedAt(stationName))
        {
            m_identityString.setValue(identifier);                
            return true;    // Notify of identity change
        }
        /*else
        {
            // Operator attempting to operate for foreign station, must disallow..
            // Removed for TD #2593, display code shifted outside here..
            //PaErrorHandler::displayError(PaErrorHandler::NO_EXTERNAL_PA_INTERACTION_ALLOWED);
        }*/
    }

    return false;
}


std::string RunParamListener::extractStringIdentifierString(const std::string& value)
{
    std::string::size_type endOffset = findEndOfIdentityString(value);

    if (std::string::npos == endOffset)
    {
        // Unexpected format (couldn't find end of identity string)
        return "";
    }

	if ( endOffset <= 0)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid internal state" );
		return "";
	}

    return value.substr(0, endOffset);
}


std::string::size_type RunParamListener::findEndOfIdentityString(const std::string& input)
{
    std::string delimiter(",");

    // The end of the identity string is the second comma
    std::string::size_type offset = input.find(delimiter);

    if (std::string::npos != offset)
    {
        // Now find second delimiter
        offset = input.find(delimiter, offset + 1);
    }

    if (std::string::npos != offset)
    {
        // Shift one past the comma (so trailing comma is included in identity)
        return offset + 1;
    }
    else
    {
        // Identity not found
        return offset;
    }
}


void RunParamListener::registerTag(const std::string& tagName)
{
    TA_Base_Core::ThreadGuard sync(m_tagLock);
    RunParamChangeStruct changeStruct;
    changeStruct.m_paramName = tagName;

    m_registeredTags.push_back(changeStruct);
}


void RunParamListener::onRunParamChange(const std::string& name, const std::string& strValue)
{
    try
    {
        processInternalRunParamChange(name, strValue);
    }
    catch (const TA_Base_Core::InvalidPasConfigurationException&)
    {
        // Invalid parameters received from HMI, do not respond to the update
    }
    catch (...)
    {
        // Unexpected
    }
}


void RunParamListener::processInternalRunParamChange(const std::string& strName, const std::string& strValue)
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
         "ParamChange: %s=%s", strName.c_str(), strValue.c_str());

    TA_Base_Core::ThreadGuard sync(m_tagLock);

    std::vector<RunParamChangeStruct>::iterator itr = findChangeStruct(strName);

    if (itr == m_registeredTags.end())
    {
        // Unrecognized runparam change, we shouldn't have received it..
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Internal error" );
        return;
    }

    RunParamChangeStruct& changeStruct = (*itr);
    // If a listener is attached to iterator, dispatch
    // the change to it, otherwise queue for now (until listener attached)
    if (0 == changeStruct.m_listener)
    {
        changeStruct.m_valueChanges.push_back(strValue);

	    if (changeStruct.m_valueChanges.size() > MAX_VALUE_CHANGES)
	    {
            // This probably means no listener attached to this change item
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "A listener is probably not registered to param %s", strName.c_str());

    		// Clear the oldest element as it becomes oversize
		    changeStruct.m_valueChanges.pop_front();
	    }
    }
    else
    {
        dispatchChangeToListener(*(changeStruct.m_listener), strName, strValue);
    }
}


bool RunParamListener::dispatchChangeToListener(IRunParamChangedListener& listener, 
                                                const std::string& name, 
                                                const std::string& value)
{
    // If assert fails, user has not initialized identity
	if ( false == m_identityString.hasBeenSet())
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "User has not set identity string" );
		return false;
	}

	std::string modifiedString = "";

	if (( SELECTED_STATION_MSG_TAG.compare( name ) == 0 ) || 
        ( SELECTED_TRAIN_MSG_TAG.compare( name ) == 0 ))
	{
		modifiedString = value;
	}
	else
	{
		std::string identifier = extractStringIdentifierString(value);
		
		if (!m_identityString.hasBeenSetToValue(identifier))
		{
			// Only allowed to respond to schematic we were instances with
			// - so if receive a new identity string - provided it's a launch
			// request, close down current instance and restart..
			if (checkIfNeedToRestartPaManager(identifier, name, value))
			{
				restartApplication(identifier);
			}
			
			// Don't have this instance respond to any foreign commands
			return false;
		}
		
		// Only pass through the end of string to the listener (not the ID info)
		modifiedString = extractActualComand(identifier, value);
	}
	
    // Allow any exceptions to propogate
    listener.processRunParamChange(name, modifiedString);

    return true;
}


std::string RunParamListener::extractActualComand(const std::string& identifier,
                                                  const std::string& fullValue)
{
    std::string::size_type offset = identifier.length();

    // Go past all whitespace (following identification tag)
    offset = fullValue.find_first_not_of(" ", offset);

    if (std::string::npos == offset)
    {
        return "";
    }

    return fullValue.substr(offset, std::string::npos);
}

void RunParamListener::deRegisterListener(IRunParamChangedListener& listener)
{
    TA_Base_Core::ThreadGuard sync(m_tagLock);

	bool foundOneOrMoreListener = false;    
    std::vector<RunParamChangeStruct>::reverse_iterator itr = m_registeredTags.rbegin();

    while (itr != m_registeredTags.rend())
    {       
		if ((*itr).m_listener == &listener)
		{
			foundOneOrMoreListener = true;
            // Unlink listener pointer
            (*itr).m_listener = 0;
		}                
        else
        {
            itr++;
        }
	}
}


void RunParamListener::registerListener(const std::string& name, IRunParamChangedListener& listener)
{
    TA_Base_Core::ThreadGuard sync(m_tagLock);

    std::vector<RunParamChangeStruct>::iterator itr = findChangeStruct(name);
    
	if ( itr == m_registeredTags.end())
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Listener not found" );
		return;
	}

	if ( NULL != (*itr).m_listener)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Listener already attached to name" );
		return;
	}

    (*itr).m_listener = &listener;

    // Go through and dispatch all present queued values
    while ((*itr).m_valueChanges.size() > 0)
    {
        try
        {
            dispatchChangeToListener(listener, name, (*itr).m_valueChanges.front());
        }
        catch (const TA_Base_Core::InvalidPasConfigurationException&)
        {
            // Invalid parameters received from HMI, don't allow to propogate
            // (just ignore these changes)
        }

        // Clear item from queue
        (*itr).m_valueChanges.pop_front();
    }
}


std::vector<RunParamListener::RunParamChangeStruct>::iterator 
            RunParamListener::findChangeStruct(const std::string& name)
{
    std::vector<RunParamChangeStruct>::iterator itr = m_registeredTags.begin();

    for (; itr != m_registeredTags.end(); itr ++)
    {
        if (_stricmp((*itr).m_paramName.c_str(), name.c_str()) == 0)
        {
            // Found our target
            break;
        }
    }

    return itr;
}


bool RunParamListener::checkIfNeedToRestartPaManager(
                                          const std::string identifier, 
                                          const std::string& name,
                                          const std::string& value)
{
    std::string modifiedString = extractActualComand(identifier, value);
    bool isLaunchCommand = false;
        
    if (( DISPLAY_TAG.compare(name) == 0 ) && ( _stricmp(modifiedString.c_str(), "SHOW") == 0 ))
    {
        isLaunchCommand = true;
    }

    // Upon receive a launch command from a different schematic, we need to 
    // restart PA Manager (for the new schematic)
    return isLaunchCommand;
}


void RunParamListener::setRestartApplicationCallback(IRestartApplicationCallback* pCallback)
{
    m_restartWindowCallback = pCallback;
}


void RunParamListener::restartApplication(const std::string& targetIdentifier)
{
    try
    {
        std::string targetLeftPos = getSchematicName(targetIdentifier);

        int leftPos = atoi(targetLeftPos.c_str());

        if (0 != m_restartWindowCallback)
        {                    
            m_restartWindowCallback->restartApplication(leftPos);
        }               
    }
    catch (const std::exception& expWhat)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
    }
    catch(...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", "Caught unknown exception" );
    }
}

std::string RunParamListener::getStationName(const std::string& identifierString)
{
    return identifierString.substr(0, identifierString.find(","));
}

std::string RunParamListener::getSchematicName(const std::string& identifierString)
{
    std::string::size_type offset1 = identifierString.find(",") + 1;
    std::string::size_type offset2 = identifierString.find(",", offset1);

    if (std::string::npos == offset1 || std::string::npos == offset2)
    {
        // invalid format - schematic not set
        TA_THROW(TA_Base_Core::ValueNotSetException("Schematic identifier not set"));
    }

    // Return string between first and second comma
    return identifierString.substr(offset1, offset2 - offset1);
}

std::string RunParamListener::getCommandLine()
{
    std::ostringstream commandLine;

    // Control station will ditch all the runparams sent to PA Manager
    // once it re-registers itself.  Therefore we have to pass in all
    // the runparams via the command line..
    // We don't have to send in selection specific runparams..
    TA_Base_Core::RunParams::ParamVector params;

    // Fetch all present runparams
    TA_Base_Core::RunParams::getInstance().getAll(params);

    for (TA_Base_Core::RunParams::ParamVector::iterator itr = params.begin();
            itr != params.end();
            itr ++)
    {
        TA_Base_Core::RunParams::ParamNameValue& nv = *itr;

        if (SELECT_TAG.compare( nv.name ) == 0 ||
            DESELECT_TAG.compare( nv.name ) == 0 ||
            UPDATE_SELECTION_TAG.compare( nv.name ) == 0)
        {
            // Don't need to forward these selection parameters on..
            // Particularly the UPDATE_SELECTION_TAG which can be very large
            continue;
        }

        // Some parameters that contains space characters will mess
        // up the runparams command line parsing, so must ignore these ones..
        // (we don't have a choice, they cannot be passed in at present)            
        if (nv.value.find(" ") != std::string::npos)
        {
            // Found a space character somewhere in runparam, ignore this runparam
            continue;
        }
    
        // Otherwise add parameter to command line
        commandLine << "--" << nv.name << "=" << nv.value << " ";
    }

    enum { MAX_COMMAND_LINE = 2000 };

	if ( commandLine.str().size() >= MAX_COMMAND_LINE)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "RunParamListener::getCommandLine: Command line exceeds max length!" );
		return "";
	}

    return commandLine.str();
}

// TD 15349
const std::string RunParamListener::getSelectedStationMessage()
{
	return getSelectedMessage(SELECTED_STATION_MSG_TAG);
}

std::string RunParamListener::getSelectedMessage(const std::string tagName)
{
	std::string paramValue = "";	

    // Fetch all present parameters

    if (TA_Base_Core::RunParams::getInstance().isSet(tagName.c_str()))
    {
        std::string strValue = TA_Base_Core::RunParams::getInstance().get(tagName.c_str());

        // To represent space between words or characters in selected message, use underscore ( _ ). 
        // So underscore is reserved for parsing the command-line and substituted with space. 
        // It must not be used as a character in the name.
        std::string::size_type index = 0;
        std::string::size_type pindex = 0;

        pindex = strValue.find("_", index);
        while (pindex != std::string::npos)
        {
            paramValue.append(strValue.substr(index,pindex-index));
            paramValue.append(" ");
            index = pindex + 1;
            pindex = strValue.find("_", index);
        }
        paramValue.append(strValue.substr(index,strValue.size()-index));
    }

	return paramValue;
}
// TD 15349
