/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/bus/pa/cachedmaps/src/PaZoneGroupCachedMap.h $
  * @author Jade Lee
  * @version $Revision: #6 $
  * Last modification: $DateTime: 2015/11/27 17:46:47 $
  * Last modified by: $Author: qi.huang $
  * 
  * Maintains a cache of Pa Zone Group mappings, also listens
  * out for changes to database regarding the PA_ZONE_GROUP table
  *
  */

#if !defined(PaZoneGroupCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
#define PaZoneGroupCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_

#include <list>

#include "core/data_access_interface/pa_4669/src/PaZoneGroupAccessFactory.h"
#include "bus/pa/cachedmaps/src/GenericCachedMap.h"
#include "bus/pa/cachedmaps/src/CachedMapDefinitions.h"
#include "core/data_access_interface/src/PrimitiveWrapper.h"

namespace TA_Base_Core
{
    class ConfigUpdateMessageSender;
}

namespace TA_Base_Core
{
    class IPaZoneGroup;
}

namespace TA_IRS_Bus
{
    class PaZoneGroupCachedMap 
    : 
    public GenericCachedMap<PaZoneGroupRecord, TA_Base_Core::IPaZoneGroup>
    {
    public:

        /**
         * PaZoneGroupCachedMap (constructor)
         *
         */
        PaZoneGroupCachedMap();

        /**
         * ~PaZoneGroupCachedMap (destructor)
         *
         */
        virtual ~PaZoneGroupCachedMap();

        /** 
         * refreshKeyToPaZoneGroupRecordMap
         *
         * Reads the PA_ZONE_GROUP table and populates the KeyToPaZoneGroupRecordMap collection
         * for all records matching the optional locationKey.  To update the map with all records, 
         * do not specify the the parameter.
         *
         * @param  locationKey (optional) filters the PA_ZONE_GROUP table on the LOCATIONKEY field.
         *
         * @exception DatabaseException 
         *         If there is a problem establishing a connection with the database.
         *         If an error is encountered while retrieving data.
         * @exception DataException      
         *         If the data cannot be converted to the required format.
         *         If the wrong ammount of data is retrieved.
         * @exception InvalidPasConfigurationException 
         *         If a single key is mapped to more than one record (i.e. pkeys are not unique).  
         *         This indicates invalid configuration in the databases.
         *
         */
        void refreshKeyToPaZoneGroupRecordMap(unsigned long locationKey = 0);
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException);

        /**
         * createNewPaZoneGroup
         *
         * Creates a new PA Zone group in the database
         *
         * @param locationKey location group should exist at
         * @param label name for this new group
         * @param isEventGroup specifies whether this is an event group or not (typically not)
         * @param zoneKeys the list of zones to be associated with this group
         * @param existingKey a key value if we want to overwrite an existing group
         *
         * @param entityKey, sessionId parameters required for transmission of audit messages
         *
         * @return the primary key of the new zone group
         *
         * @exception DatabaseException if there is a database error.
         * @exception DataException if there is an internal error interpreting the input parameters
         * @exception DataConfigurationException if there is an internal error interpreting the input parameters
         *
         */
        unsigned long createNewPaZoneGroup(unsigned int locationKey, 
                                                std::string label, 
                                                bool isEventGroup,
                                                const std::list<unsigned long>& zoneKeys,
                                                TA_Base_Core::PrimitiveWrapper<unsigned long> existingKey);
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException);


        /**
         * getKeyFromLocationAndLabel
         *
         * @return a key value (or returned key unset if no matches found)
         *
         * @param locationKey the location key of the group
         *
         * @param label the label of the zone group
         *
         */
        TA_Base_Core::PrimitiveWrapper<PaZoneGroupRecord>
            getKeyFromLocationAndLabel(unsigned long locationKey, const std::string& label);


        /**
         * removePaZoneGroup
         * 
         * Removes the specified zone group
         *
         * @param locationKey the location key of the group
         *
         * @param label the label of the zone group
         *
         *
         * @param entityKey, sessionId parameters required for transmission of audit messages
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *           
         * @throws  DataException 
         *          - if there is no PaZoneGroup matching the supplied parameters
         *
         */
        void removePaZoneGroup(unsigned int locationKey, const std::string& label);


        /**
         * getZonesAssociatedWithGroup
         *
         * @param locationKey the location key of the group
         *
         * @param label the label of the zone group
         *
         * @exception  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *           
         * @exception  DataException 
         *          - if there is no PaZoneGroup matching the supplied parameters
         *
         */
        std::list<unsigned long> getZonesAssociatedWithGroup(unsigned long locationKey, const std::string& label);

        /**
         * getPaZoneGroupsByEventFlag
         *
         * @return set of all PA Zone Groups whose isEventGroup flag
         *          corresponds to input parameter
         *
         * @param isEventGroup defines the filter used to fetch result
         *
         */
        PaZoneGroupRecords getPaZoneGroupsByEventFlag(bool isEventGroup);

    protected:

        /**
         * validateRecordData (interface implementation)
         *
         * @see GenericCachedMap::validateRecordData for more details
         *
         */
        void    validateRecordData(const PaZoneGroupRecord& paZone);

        /**
         * getRecordFromDatabaseUsingKey (interface implementation)
         *
         * @see GenericCachedMap::getRecordFromDatabaseUsingKey for more details
         *
         */
        TA_Base_Core::IPaZoneGroup* getRecordFromDatabaseUsingKey(unsigned long pKey, bool readWrite);
                                //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException);

        /**
         * createRecordFromInterface (interface implementation)
         *
         * @see GenericCachedMap::createRecordFromInterface for more details
         *
         */
        virtual PaZoneGroupRecord createRecordFromInterface(TA_Base_Core::IPaZoneGroup& item);
            //throw (TA_Base_Core::InvalidPasConfigurationException);


    private:

        /**
         * Called whenever there is a change being made by this class to the database
         *  - Note: Call AFTER the changes has been made
         *
         * This will post out ConfigUpdate messages to everyone listening out for
         *  configEditor updates (with relevant change information)
         *
         * @param recordKey the primary key of the record that has been changed
         *
         * @param modificationType the type of modification (Update, Create, Delete etc)
         *
         * @param changes list of the columns that have changed (if applicable)
         *
         */
        void submitConfigUpdate(unsigned long recordKey, 
                                TA_Base_Core::EModificationType modificationType, 
                                const std::vector<std::string>& changes);
                     
        // Sends out config update notifications whenever a change
        // made to database via this class (so other PAManagers know about it)
        TA_Base_Core::ConfigUpdateMessageSender* m_configUpdateSender;

        unsigned long m_refreshLocationKey;

    public: // Give public access to these filter classes
    
        /// Custom filter to exclude any items we don't want to be able to be
        /// placed into a zone group (paging consoles, and depot locations when at OCC)
        class ZoneGroupFilter
            : public ICachedMapFilter<PaZoneRecord>
        {
        public:

            /// @param isOccZoneGroup true if this zone group is for use at OCC
            /// @param depotLocationKey the location key of the depot terminal
            ZoneGroupFilter();
            virtual bool isMatch(const PaZoneRecord& record) const;       
        };


        /// Custom filter for getKeyFromLocationAndLabel
        class FilterPaZoneGroupByLocationAndLabel
            : public ICachedMapFilter<PaZoneGroupRecord>
        {
        public:
            FilterPaZoneGroupByLocationAndLabel(
                unsigned long locationKey,
                const std::string& label);

            virtual bool isMatch(const PaZoneGroupRecord& record) const;
        
        private:
    
            unsigned long m_locationKey;
            const std::string m_label;
        };

        /// Custom filter for getKeyFromLocationAndLabel
        class FilterPaZoneGroupByLocationKeyAndEventGroup
            : public ICachedMapFilter<PaZoneGroupRecord>
        {
        public:
            FilterPaZoneGroupByLocationKeyAndEventGroup(
                                          unsigned long locationKey,
                                          bool isEventGroup);

            virtual bool isMatch(const PaZoneGroupRecord& record) const;
        
        private:
    
            unsigned long m_locationKey;
            bool m_isEventGroup;
        };

        class FilterPaZoneGroupByEventGroup
            : public ICachedMapFilter<PaZoneGroupRecord>
        {
        public:
            FilterPaZoneGroupByEventGroup(bool isEventGroup) :
                m_isEventGroup(isEventGroup) {}

            virtual bool isMatch(const PaZoneGroupRecord& record) const {
                return record.m_isEventGroup == m_isEventGroup; }
        
        private:    
            bool m_isEventGroup;
        };
    };

} // end namespace TA_IRS_Bus

#endif //!defined(PaZoneGroupCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
