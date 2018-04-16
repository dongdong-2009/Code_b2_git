/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File$
  * @author Jade Lee
  * @version $Revision$
  * Last modification: $DateTime$
  * Last modified by: $Author$
  * 
  * RunParamListener listens out for general RunParam changes, 
  *     and queue changes (to ensure non are missed)
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "app/pa/PAManager/src/StdAfx.h"

#include "core/exceptions/src/PasExceptions.h"

#include "app/pa/PAManager/src/IRunParamChangedListener.h"
#include "app/pa/PAManager/src/PAManagerConsts.h"
#include "app/pa/PaManager/src/PaErrorHelper.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "app/pa/PAManager/src/PaDataManager.h"
#include "app/pa/PAManager/src/RunParamListener.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// RunParams are always capitalised
// const std::string RunParamListener::DISPLAY_TAG = "Display";
// const std::string RunParamListener::UPDATE_SELECTION_TAG = "SynchroniseSelected";

RunParamListener* RunParamListener::s_pInstance = NULL;

static std::string strtolower(const std::string& input)
{
    std::string result(input);
    std::transform(result.begin(), result.end(), result.begin(), tolower);
    return result;
}


RunParamListener::RunParamListener() : RunParamUser()
{
    registerTag(RUN_PARAM_SELECT_TAG);
    registerTag(RUN_PARAM_DESELECT_TAG);
    registerTag(RUN_PARAM_UPDATE_SELECTION_TAG);
    // Need to listen out for any change in these RunParams
    for (std::vector<RunParamChangeStruct>::iterator itr = m_registeredTags.begin(); itr != m_registeredTags.end(); ++itr )
    {
        TA_Base_Core::RunParams::getInstance().registerRunParamUser( this, (*itr).m_paramName.c_str());
        // Set initial item in changed queue (to ensure it gets dispatched)
        std::string initialValue;
    	initialValue = TA_Base_Core::RunParams::getInstance().get((*itr).m_paramName.c_str());
	    if ( !initialValue.empty() )
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
    std::string anyCommand = "";
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

    if (anyCommand.empty())
    {   
        return "";
    }
    
    return extractStringIdentifierString(anyCommand);
}

std::string RunParamListener::extractStringIdentifierString(const std::string& value)
{
    std::string::size_type endOffset = findEndOfIdentityString(value);

    if (std::string::npos == endOffset)
    {
        // Unexpected format (couldn't find end of identity string)
        return "";
    }

    TA_ASSERT(endOffset > 0, "Invalid internal state");

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


void RunParamListener::onRunParamChange(const std::string& name, const std::string& value)
{
    try
    {
        processInternalRunParamChange(name, value);
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


void RunParamListener::processInternalRunParamChange(const std::string& name, const std::string& value)
{
    LOG( SourceInfo, 
         TA_Base_Core::DebugUtil::GenericLog, 
         TA_Base_Core::DebugUtil::DebugInfo, 
         "ParamChange: %s=%s", name.c_str(), value.c_str());

    TA_Base_Core::ThreadGuard sync(m_tagLock);

    std::vector<RunParamChangeStruct>::iterator itr = findChangeStruct(name);

    if (itr == m_registeredTags.end())
    {
        // Unrecognized runparam change, we shouldn't have received it..
        TA_ASSERT(false, "Internal error");
        return;
    }

    RunParamChangeStruct& changeStruct = (*itr);
    // If a listener is attached to iterator, dispatch
    // the change to it, otherwise queue for now (until listener attached)
    if (0 == changeStruct.m_listener)
    {
        changeStruct.m_valueChanges.push_back(value);

	    if (changeStruct.m_valueChanges.size() > MAX_VALUE_CHANGES)
	    {
            // This probably means no listener attached to this change item
            LOG( SourceInfo, 
                TA_Base_Core::DebugUtil::GenericLog, 
                TA_Base_Core::DebugUtil::DebugError, 
                "A listener is probably not registered to param %s", name.c_str());

    		// Clear the oldest element as it becomes oversize
		    changeStruct.m_valueChanges.pop_front();
	    }
    }
    else
    {
        dispatchChangeToListener(*(changeStruct.m_listener), name, value);
    }
}


bool RunParamListener::dispatchChangeToListener(IRunParamChangedListener& listener, 
                                                const std::string& name, 
                                                const std::string& value)
{
	std::string identifier = extractStringIdentifierString(value);
	
	// Only pass through the end of string to the listener (not the ID info)
	std::string modifiedString = extractActualComand(identifier, value);

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

	TA_ASSERT(foundOneOrMoreListener, "Input listener was unrecognized");
}


void RunParamListener::registerListener(const std::string& name, IRunParamChangedListener& listener)
{
    TA_Base_Core::ThreadGuard sync(m_tagLock);

    std::vector<RunParamChangeStruct>::iterator itr = findChangeStruct(name);
    
    TA_ASSERT(itr != m_registeredTags.end(), "Listener not found");

    TA_ASSERT((*itr).m_listener == 0, "Listener already attached to name");

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
        if (0 == (*itr).m_paramName.compare( name ))
        {
            // Found our target
            break;
        }
    }

    return itr;
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

        if (nv.name.compare( RUN_PARAM_SELECT_TAG ) == 0 ||
            nv.name.compare( RUN_PARAM_DESELECT_TAG ) == 0)
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

    TA_ASSERT(commandLine.str().size() < MAX_COMMAND_LINE, 
                "RunParamListener::getCommandLine: Command line exceeds max length!");

    return commandLine.str();
}

std::string RunParamListener::getStationName(const std::string& identifierString)
{
    return identifierString.substr(0, identifierString.find(","));
}

std::string RunParamListener::findVirtualLocation()
{
    std::string identifier = findIdentityString();
    return getStationName(identifier); 
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

        m_identityString.setValue(identifier);                
        return true;    // Notify of identity change
    }
    
    return false;
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

RunParamListener& RunParamListener::getInstance()
{
	return (*s_pInstance);
}

void RunParamListener::createInstance()
{
	if ( NULL == s_pInstance )
	{
		s_pInstance = new RunParamListener();
	}
}

void RunParamListener::destroyInstance()
{
	if ( NULL != s_pInstance )
	{
		delete s_pInstance;
		s_pInstance = NULL;
	}
}