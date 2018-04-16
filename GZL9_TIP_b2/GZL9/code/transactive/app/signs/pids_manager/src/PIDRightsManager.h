/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/signs/pids_manager/src/PIDRightsManager.h $
  * @author:  Wu Min Tao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#ifndef _PIDRightsManager_HEADER_
#define _PIDRightsManager_HEADER_

#include <map>

namespace TA_Base_Bus
{
    class RightsLibrary;
};

class PIDRightsManager
{
public:

    /**
     * getInstance
     *
     * @return singleton instance of the rights manager
     *
     */
    static PIDRightsManager& getInstance();

    /**
     * removeInstance
     *
     * Destroys the singleton instance of PIDRights manager
     *
     */
    static void removeInstance();
    
    /// Publically available rights - each time enum updated, must
    /// update PIDRightsManager constructor to apply mappings
    
    /**
     * hasRights
     *
     * Public access to rights checking, passes on to internal implementation
     *
     */
    bool hasRight(unsigned long right) const;

	/**
	  * hasRightToDisplayPredefinedMessage
	  * 
	  * check whether the operator has the right to disply predefined message
	  * 
	  * @return bool 
	  * 
	  * @exception <exceptions> Optional
	  */
    bool hasRightToDisplayPredefinedMessage();

	/**
	  * hasRightToDisplayAdhocMessage
	  * 
	  * check whether the operator has the right to disply ad hoc message
	  * 
	  * @return bool 
	  * 
	  * @exception <exceptions> Optional
	  */
    bool hasRightToDisplayAdhocMessage();

	/**
	  * hasRightToClearStationMessage
	  * 
	  * check whether the operator has the right to clear the station message
	  * 
	  * @return bool 
	  * 
	  * @exception <exceptions> Optional
	  */
    bool hasRightToClearStationMessage();

	/**
	  * hasRightToModifyPIDGroups
	  * 
	  * <description>
	  * 
	  * @return bool 
	  * 
	  * @exception <exceptions> Optional
	  */
    bool hasRightToModifyPIDGroups();

	/**
	  * hasRightToAccessRedLine
	  * 
	  * <description>
	  * 
	  * @return bool 
	  * 
	  * @exception <exceptions> Optional
	  */
    bool hasRightToAccessRedLine();

	/**
	  * hasRightToAccessOrangeLine
	  * 
	  * <description>
	  * 
	  * @return bool 
	  * 
	  * @exception <exceptions> Optional
	  */
    bool hasRightToAccessOrangeLine();

	/**
	  * hasRightToModifyAdhocMessage
	  * 
	  * <description>
	  * 
	  * @return bool 
	  * 
	  * @exception <exceptions> Optional
	  */
    bool hasRightToModifyAdhocMessage();


	/**
	  * hasRightToControlPID
	  * 
	  * to check the user whether has the right to blank or unblank the PIDs
	  * 
	  * @return bool 
	  * 
	  * @exception <exceptions> Optional
	  */
	bool hasRightToControlPID();
    

private:
    enum EPIDRight
    {
	    STATION_TIS_PRE_DEFINED, 
        STATION_TIS_AD_HOC,
	    STATION_TIS_RESET,
		STATION_TIS_PID_CONTROL,
	    STATION_TIS_PID_GROUPS,
	    RED_LINE_ACCESS,
	    ORANGE_LINE_ACCESS,
        STATION_MODIFY_AD_HOC,
        PID_RIGHT_MAX
    };


private:
    /**
     * convertRightIdentifier
     *
     * Converts the input locally defined (enum) right into the globally
     *  recognized right for system
     *
     * @param localRight the index of local right
     *
     * @return the identifier of global right (for use with RightsManager)
     *
     * @exception ValueNotSetException if no mapping has been set up
     *
     */
    unsigned long convertLocalRightIdentifier(unsigned long localRight) const;

    /**
     * debugConfirmRightsExist (purely for debugging purposes)
     *
     * returns true if all rights exist (all checks passed)
     *
     * @pre all rights exist, and there are no many to one relationships
     *
     */
    bool debugConfirmRightsExist();

    /**
     * getResourceKey
     *
     * Caches the resource key once successfully obtained
     *
     * @return the resource key for this entity (for use
     *      with the rights manager
     *
     * @exception ValueNotSetException if the resource key could
     *              not be identified
     *
     */
    unsigned long getResourceKey() const;
	
    //disabled method
    PIDRightsManager();
	PIDRightsManager(const PIDRightsManager&);
	PIDRightsManager& operator= (const PIDRightsManager&);

	/**
     * Destructor
     *
     * Default destructor
     *
     */
    virtual ~PIDRightsManager();
	    
    // Cached copy for use in hasRight
    mutable TA_Base_Bus::RightsLibrary  *m_rightsLibrary;

    //the singleton instance
    static PIDRightsManager* m_instance;

    // Maintains mapping to global rights identifiers (we use local enum identifiers
    // within the PA Manager code)
    std::map<unsigned long, unsigned long> m_localToGlobalRightsMap;

	mutable unsigned long m_resourceKey;

};

#endif // _PIDRightsManager_HEADER_

