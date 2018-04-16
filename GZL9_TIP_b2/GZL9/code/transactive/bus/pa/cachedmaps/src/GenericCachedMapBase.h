/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/bus/pa/cachedmaps/src/GenericCachedMapBase.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * Base class (non template) for the GenericCachedMap template class
  * Because template classes need to be compiled inline, this reduces
  * the load on the compiler (removing need to specify /Zm compiler options)
  * by putting non template elements into a base class
  *
  */

#if !defined(GenericCachedMapBase_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
#define GenericCachedMapBase_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_

#include <string>

#include "bus/pa/cachedmaps/src/ICachedMapObserverRegistry.h"
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/configuration_updates/src/EObjectType.h"
#include "core/exceptions/src/PasExceptions.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_IRS_Bus
{
    class GenericCachedMapTestCases;

    // Inherit from IOnlineUpdatable to allow us to listen out to 
    // database changes for this table
    // DbRecordType corresponds to the basic type we use to send around (and use in storage)
    // DaiRecordInterface is the database interface used to extract records
    //   - and corresponds to type returned from database factory methods, like IPaZone
    class GenericCachedMapBase
    :
    public ICachedMapObserverRegistry,
    public TA_Base_Core::IOnlineUpdatable
    {
        // Unit test class
        friend class GenericCachedMapBaseTestCases;

    public:

        /**
         * GenericCachedMapBase (constructor)
         *
         * @param databaseUpdateListenerType the database identifier we send
         *          to the online database update listener
         *        It is assumed all CachedMaps have a corresponding ConfigEditor
         *          update 'object type' - meaning config editor can feasibly
         *          modify any cached maps (may change in future, making this
         *          an optional parameter)
         *
         * @param tableName the literal name of the table we're caching data from
         *                  this is for debug purposes only (for error feedback)
         *
         */
        GenericCachedMapBase(const TA_Base_Core::EObjectType& databaseUpdateListenerType,
                                const std::string& tableName);

        /**
         * ~GenericCachedMapBase (destructor)
         *
         */
        virtual ~GenericCachedMapBase();


        /**
         * registerObserver (interface implementation)
         *
         * @see ICachedMapObserverRegistry::registerObserver for more details
         *
         */
        virtual void registerObserver(ICachedMapObserver* ob);

        /**
         * deRegisterObserver (interface implementation)
         *
         * @see ICachedMapObserverRegistry::deRegisterObserver for more details
         *
         */
        virtual void deRegisterObserver(ICachedMapObserver* ob);


        /**
         * processUpdate (interface implementation)
         *
         * @see IOnlineUpdatable::processUpdate for more details
         *
         */
        virtual void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

        /**
         * getAllTokens
         *
         * @return an array of token values found in input string, if
         *          the delimiter isn't present in input, array size 1 is returned
         *
         * @param input the input string to extract tokens from
         *
         * @param delimiter the token separation delimiter, ie "."
         *
         */
        static std::vector<std::string> getAllTokens(const std::string& input, 
                                                     const std::string& delimiter);

        /**
         * stopListening
         *
         * Stops this cached map from listening out for updates from database
         *      safe to call this if we're not already listening
         *
         */
        void    stopListening();

        /**
         * startListening
         *
         * Starts this cached map listening out for updates from database
         *  (for all items contained in cached map)
         *
         * @pre we're not already listening
         *
         */
        virtual void startListening();

    protected:

        /**
         * getPrimaryKeyArray
         *
         * @return set of primary keys in use by this cached map
         *
         */
        virtual std::vector<unsigned long> getPrimaryKeyArray() = 0;

        /**
         * performDatavalueValidation
         *
         * Simple helper function to validate database parameters
         *
         * @param condition upon which to generate a new PasException
         *
         * @param key the key of the database record that failed the condition
         *
         * @exception PasException if input condition is true
         *
         */
        void performDatavalueValidation(bool condition, 
                                        unsigned long key);
                                        // (TA_Base_Core::InvalidPasConfigurationException);

        /**
         * getTableName (simple accessor)
         *
         */
        const std::string getTableName() const { return m_tableName; }

        /**
         * getUpdateListenerType (simple accessor)
         */
        const TA_Base_Core::EObjectType getUpdateListenerType() const { return m_databaseUpdateListenerType; }

        /**
         * startListening
         *
         * Starts this cached map listening out for updates from database
         *
         * @param primaryKeys an array of primary keys (of database objects) that we're
         *          listening on
         *
         * @pre we're not already listening to the specified key
         *
         */
        void    startListening(std::vector<unsigned long> primaryKeys);

        /**
         * startListeningToAdditionalItem
         *
         * Starts this cached map listening out for updates from database
         *  for this item, as well as all items specified in startListening
         *
         * @param key the key of the record we want to listen for
         *
         * @pre we're not already listening to the specified key
         *
         */
        void    startListeningToAdditionalItem(unsigned long pKey);

        /**
         * stopListeningToItem
         *
         * Stops this cached map listening out for updates from database
         *  for this specific item
         *
         * @return true if operation successful, false if specified key isn't 
         *          being listened to currently (operation failed)
         *
         * @param key the key of the record we want to stop listening for
         *     
         */
        bool    stopListeningToItem(unsigned long pKey);

        /**
         * onDatabaseUpdateProcessError
         *
         * Called when there is an error (failure) processing an update 
         *  (from within processUpdate)
         *
         * @param pKey the primary key of the item that was supposed to be updated
         *
         */
        void    onDatabaseUpdateProcessError(unsigned long pKey);
    
        /**
         * processDatabaseRemovalNotification
         *
         * Processes a request to remove the record specified by the 
         * given pKey from this cached map (called after database has been updated)
         *
         * @param pKey the primary key of the item being removed from cached maps
         *
         * @exception CachedMappingNotFoundException if the specified record couldn't be found
         * 
         * @return true if the cached map was changes as a result of the operation
         *
         */
        virtual bool processDatabaseRemovalNotification(unsigned long pKey) = 0;

        /**
         * processDatabaseUpdateCreateNotification (interface implementation)
         *
         * Called to process a database update or creation notification (and update
         *  the local cache as a result)
         *
         * @param updateEvent details of the event that spawned this create request
         *
         * @return true if the cached map was changes as a result of the operation
         *
         */
        virtual bool processDatabaseUpdateCreateNotification(const TA_Base_Core::ConfigUpdateDetails& updateEvent) = 0;

    private:

        /// The name of the database table this cache represents
        const std::string m_tableName;

        const TA_Base_Core::EObjectType     m_databaseUpdateListenerType;

        /// An array of observers listening to updates made to this cached map
        /// We expect these updates to come from Config Editor (perhaps elsewhere
        /// in future) - the cached map will be up to date before they're notified
        std::vector<ICachedMapObserver*>        m_registeredObservers;
        TA_Base_Core::NonReEntrantThreadLockable     m_updateObserverLock;

        // Locks processUpdate function to block register/deregister functions
        TA_Base_Core::NonReEntrantThreadLockable     m_updateListenerLock;
    };

} // end namespace TA_IRS_Bus

#endif // GenericCachedMapBase_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_