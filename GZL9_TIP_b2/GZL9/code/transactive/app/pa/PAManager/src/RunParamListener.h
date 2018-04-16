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

#ifndef RUNPARAMLISTENER__INCLUDED_
#define RUNPARAMLISTENER__INCLUDED_

#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/data_access_interface/src/PrimitiveWrapper.h"

class IRunParamChangedListener;

class RunParamListener : public TA_Base_Core::RunParamUser
{
public:
    // Only store up to 50 changes (to stop any queue runaway)
    enum { MAX_VALUE_CHANGES = 50 };

	static void createInstance();

	static void destroyInstance();

    /**
     * getInstance 
     *
     * @return the singleton instance of the run param listener
     */
    static RunParamListener& getInstance();

    virtual ~RunParamListener();

    /**
     * onRunParamChange (interface implementation)
     *
     * @see RunParamUser::onRunParamChange for more details
     *
     */
	virtual void onRunParamChange(const std::string& name, const std::string& value);
    
    /**
     * registerListener
     *
     * Registers a listener for run param changes - if any run param changes are 
     *      queued, the listener will be immediately sent the queue updates
     *
     * @param name the name of the parameter value
     *
     * @param listener the object to receive update notifications
     *
     * @pre name corresponds to a known (registered) tag
     *
     * @pre there isn't already a listener associated with the input tag
     *
     */
    void registerListener(const std::string& name, IRunParamChangedListener& listener);


    /**
     * deRegisterListener
     *
     * deRegisterListener a listener for run param changes -
     *      to ensure no more updates sent to it
     *
     * @param listener the object we want to stop receiving notifications
     *
     * @pre the input listener is associated with at least one tag
     *
     */
    void deRegisterListener(IRunParamChangedListener& listener);


    /**
     * retrieveInitialRunParamIdentity
     *
     * Examines any available runparam to determine what the identity of this
     *  session is (required for subsequent filtering of input messages)
     *
     * @return true if the application was identified successfully
     *
     */
    bool retrieveInitialRunParamIdentity();

    /**
     * findVirtualLocation
     *
     * @return the location information from the result of findIdentityString
     *          - this represents the virtual (operating) location for the application
     *
     */
    std::string findVirtualLocation();

    /**
     * getSchematicIdentifier
     *
     * Extracts the schematic name from the current identity information of runparams
     *
     * @return the schematic name as defined by the runparams that this class has processed
     * 
     * @exception ValueNotSetException if the schematic name hasn't been set
     *
     */
    std::string getSchematicIdentifier();

    /**
     * getCommandLine
     *
     * @return the command line required for forward all the runparams to the new application
     *
     */
    std::string getCommandLine();

    std::string getStationName(const std::string& identifierString);

    bool setNewIdentityFromIdentifier(const std::string& identifier);

private:

    /// Maintains details on registered RunParam strings
    struct RunParamChangeStruct
    {
        RunParamChangeStruct() 
            : 
            m_listener(0)
            {}

        std::string             m_paramName;
        IRunParamChangedListener* m_listener;
        std::list<std::string>  m_valueChanges;
    };


private:
    // Disable default constructor in case of singleton
    RunParamListener();
    // Copy constructor not used so made private
    RunParamListener(const RunParamListener&);
    // Assignment not used so it is made private
	RunParamListener& operator=(const RunParamListener&);

    /**
     * processInternalRunParamChange
     *
     * Performs internal processing of run parameter changes
     *
     * @param name the name of the parameter value
     *
     * @param value the string identifying the cahnged value
     *
     * @exception InvalidPasConfigurationException if expected input parameters 
     *      (contained in 'value' were not found - meaning bad format)
     *
     * @see onRunParamChange
     *
     */
    void    processInternalRunParamChange(const std::string& name, const std::string& value);

    /**
     * extractActualComand
     * 
     * Extracts actual command from the input strings
     *
     * @return the actual command (stripping any leading whitespace)
     *
     * @param identifier the leading part of the substring in fullValue that identifies
     *          the string source (and is not part of actual command)
     *
     * @param fullValue the original, raw param (with identifier information)
     *
     */
    std::string extractActualComand(const std::string& identifier,
                                    const std::string& fullValue);

    /**
     * findIdentityString
     *
     * Searches through all known runparams searching for a valid command
     *  and extracts the string identifier part from command
     *
     * @return a string identifier of the current application, if present (empty string if not)
     *
     */
    std::string findIdentityString(); 

    /**
     * registerTag
     *
     * Registers a new name value to listen out for, this should be called
     *  from within the constructor to set up listening set
     *
     * @param tagName the name of the value to listen out for
     *
     */
    void registerTag(const std::string& tagName);

    /**
     * findChangeStruct (helper function)
     *
     * Finds the change struct defined by specified name
     *  Not threadlocked, so ensure lock acquired before calling
     *
     * @param name the name value to search for in change struct array
     *
     * @return the change struct whose parameter name corresponds to name, or
     *          m_registeredTags.end() if not found
     *
     */
    std::vector<RunParamChangeStruct>::iterator findChangeStruct(const std::string& name);

    /**
     * dispatchChangeToListener
     *
     * Dispatches a run parameter change notification to the input listener
     *      provided the data 'belongs' to this application instance (since
     *      multiple applications listen out to same RunParam data)
     *
     * @param listener the listener who is to receive the update information
     *
     * @param name the parameter name of the update
     *
     * @param value the parameter value to be passed onto listener - only
     *          a substring will be passed along (the identification information
     *          which determines whether it's passed on in the first place will
     *          be stripped out before dispatching to listener)
     *
     * @return true if the name/value pair was passed onto the listener,
     *          false if the data wasn't for this session (and was not
     *          passed on..)
     *
     * @pre m_identityString has been set internally (this should be set 
     *          on startup by analysing the command line,
     *          @see setRunParamIdentityFromCommandLine)
     *
     * @exception InvalidPasConfigurationException if expected input parameters 
     *      (contained in 'value' were not found - meaning bad format)
     *
     */
    bool dispatchChangeToListener(IRunParamChangedListener& listener, 
                                                const std::string& name, 
                                                const std::string& value);

    /**
     * findEndOfIdentityString
     *
     * @param input the 'value' data from which we should determine the end offset
     *          of the identifier information
     *
     * @return the offset at which the identity part of value ends, or npos
     *          if the format is unrecognized
     *
     */
    std::string::size_type findEndOfIdentityString(const std::string& input);


    /**
     * extractStringIdentifierString
     *
     * @return the identification portion of the input string, or an empty
     *          string if no identifier was found
     *
     * @param value the parameter value we want to extract an identifier from
     *
     * @see findEndOfIdentityString
     *
     */
    std::string extractStringIdentifierString(const std::string& value);

    std::vector<RunParamChangeStruct>    m_registeredTags;

    // Singleton instance (must be immediately instanced to ensure
    // we get all run param changes)
    static RunParamListener* s_pInstance;

    TA_Base_Core::NonReEntrantThreadLockable  m_tagLock;

    /// String storing the identity of this instance (as passed through in RunParams)
    TA_Base_Core::PrimitiveWrapper<std::string>  m_identityString;
};


#endif // !defined(RUNPARAMLISTENER__INCLUDED_)
   

