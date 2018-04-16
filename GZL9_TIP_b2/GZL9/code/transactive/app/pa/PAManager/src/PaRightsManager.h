/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File$
  * @author Sean Liew
  * @version $Revision$
  * Last modification: $DateTime$
  * Last modified by: $Author$
  * 
  * Contains passthrough functions for querying the right to perform
  * various operations
  *
  */

#ifndef _PaRightsManager_HEADER_
#define _PaRightsManager_HEADER_

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/src/PrimitiveWrapper.h"

interface IRightsChangeListener;

namespace TA_Base_Bus
{
    class RightsLibrary;
};

class PaRightsManager
{
public:

    /**
     * getInstance
     *
     * @return singleton instance of the rights manager
     *
     */
    static PaRightsManager& getInstance();

    /**
     * removeInstance
     *
     * Destroys the singleton instance of PaRights manager
     *
     */
    static void removeInstance();

    virtual ~PaRightsManager();

    /// Publically available rights - each time enum updated, must
    /// update PaRightsManager constructor to apply mappings
    enum EPublicRight 
    {
		PA_BROADCAST_STATION_DVA,
		PA_BROADCAST_OCC_DVA, 
		PA_MODIFY_ZONE_GROUPS,
		PA_CONFIGURE_PA_MESSAGE_DESCRIPTION, 
        // This define for local checking
        PUBLIC_RIGHT_MAX
    };
    
    /**
     * hasRights
     *
     * Public access to rights checking, passes on to internal implementation
     *
     */
    bool hasRight(EPublicRight right) const;

    /**
     * obtainReEntrantLock
     *
     * Obtains a thread re-entrant lock on all the local data information
     *
     */
    std::auto_ptr<TA_Base_Core::ThreadGuard> obtainReEntrantLock();

    /**
     * hasRightToBroadcastLive
     *
     * @return true if the operator has a right to make live station broadcast
     *      
     */
    bool hasRightToBroadcastLive();

	/**
     * hasRightToBackgroundMusic
     *
     * @return true if the operator has a right to make background music station broadcast
     *      
     */
    bool hasRightToBackgroundMusic();

    /**
     * hasRightToBroadcastDva
     *
     * @return true if the operator has a right to make dva station broadcast
     *      
     */
    bool hasRightToBroadcastDva();

   /**
     * hasRightToStartMonitor
     *
     * @return true if the operator has a right to make dva station broadcast Monitor
     *      
     */
    bool hasRightToStartMonitor();//added by lixiaoxia

	 /**
     * hasRightToStopMonitor
     *
     * @return true if the operator has a right to stop dva station broadcast Monitor
     *      
     */
    bool hasRightToStopMonitor();//added by lixiaoxia
    /**
     * hasRightToStopDva
     *
     * @return true if the operator has a right to stop dva station broadcast
     *      
     */
    bool hasRightToStopDva();

    /**
     * hasRightToModifyDvaMsgDescription
     *
     * @return true if the operator has a right to modify dva message description
     *      
     */
    bool hasRightToModifyDvaMsgDescription();

    /**
     * hasRightToModifyZoneGroup
     *
     * @return true if the operator has a right to modify pa zone group
     *      
     */
    bool hasRightToModifyZoneGroup();

    /**
     * hasRightToBroadcastTrainLive
     *
     * @return true if the operator has a right to make live train broadcast
     *      
     */
    bool hasRightToBroadcastTrainLive();

    /**
     * hasRightToRecordDvaMessage
     * 
     * @return true if operator has right to record DVA (ad-hoc) messages
     *          from this terminal
     *
     */
    bool hasRightToRecordDvaMessage();

    /**
     * hasRightToPlaybackDvaMessage
     * 
     * @return true if operator has right to play a DVA (ad-hoc) message
     *          from this terminal
     *
     */
    bool hasRightToPlaybackDvaMessage();

    /**
     * flagChangeInRights
     *
     * This is called whenever part of the using application has knowledge
     *  that there has been a change in rights - subsequently
     *  notifications are sent out to all registered rights change listeners
     * 
     * @see registerRightsChangeListener
     *
     */
    void flagChangeInRights();

    /**
     * registerRightsChangeListener
     *
     * Registers a listener that is to receive notification of a rights change
     *
     * @pre listener is not already registered
     *
     */
    void registerRightsChangeListener(IRightsChangeListener& listener);

    /**
     * deRegisterRightsChangeListener
     *
     * Deregisters a listener that had been registered with registerRightsChangeListener
     *
     * @pre listener has been registered with registerRightsChangeListener
     *
     */
    void deRegisterRightsChangeListener(IRightsChangeListener& listener);

private:


    /// Private use rights - each time enum updated, must
    /// update PaRightsManager constructor to apply mappings
    /// Private rights cane be indirectly accessed publicly by using
    /// the functions mentioned next to each enumerated value
    enum EPrivateRight
    {
        // Start off at last (MAX) public rights ID
		PA_BROADCAST_STATION_LIVE = PUBLIC_RIGHT_MAX,
		PA_BROADCAST_OCC_LIVE,
		PA_BROADCAST_STN_BACKGROUND_MUSIC,
		PA_BROADCAST_OCC_BACKGROUND_MUSIC,
		PA_BROADCAST_STN_MONITOR,
		PA_BROADCAST_OCC_MONITOR,

        PA_RIGHT_MAX
    };

    /**
     * hasRights
     *
     * @param operation the operation we are checking the rights of
     *
     */
    bool hasRight(unsigned long right) const;
        
    /**
     * isPhysicallyLocatedAtOperatingLocation
     *
     * @return true if the PAManager's physical location corresponds to the
     *      location it is operating on (or running for)
     *      (ie a terminal may be running the PAManager for the neighbouring
     *          station, meaning this returns false)
     *
     */
    bool isPhysicallyLocatedAtOperatingLocation();

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

    // Singleton
    PaRightsManager();

    // Lock to ensure any local data information that need to be check
    // in order to determine rights can be blocked from asynchronous
    // updates (requests) from other threads (must maintain as a ReEntrant!)
    mutable TA_Base_Core::ReEntrantThreadLockable    m_lock;
        
    // Maintains mapping to global rights identifiers (we use local enum identifiers
    // within the PA Manager code)
    std::map<unsigned long, unsigned long> m_localToGlobalRightsMap;

    // Cached copy for use in hasRight
    mutable TA_Base_Bus::RightsLibrary  *m_rightsLibrary;
    // Maintains cached copy from getResourceKey
    mutable TA_Base_Core::PrimitiveWrapper<unsigned long> m_resourceKey;
    
    std::set<IRightsChangeListener*> m_rightsChangeListeners;

    static PaRightsManager* s_me;
};

#endif // _PaRightsManager_HEADER_
