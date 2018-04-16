/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/src/CombinedEventAccessFactory.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2010/06/25 12:06:02 $
  * Last modified by:  $Author: builder $
  *
  * CombinedEventAccessFactory is a singleton that is used by the Event Viewer to
  * retrieve all data required to be presented to the user. It allows filtering and
  * sorting to done on the database side, and returns a vector of partially loaded
  * data to improve the efficiency.
  *
  */

#if !defined(_COMBINED_EVENT_ACCESS_FACTORY_H_)
#define _COMBINED_EVENT_ACCESS_FACTORY_H_

#include <map>
#include <vector>
#include <sys/timeb.h>

#include "core/data_access_interface/src/IApplicationObserver.h"
#include "core/data_access_interface/src/IDatabase.h"

namespace TA_Base_Core
{
    // Forward declarations.
    class IData;
    class ICombinedEventData;
    class IDatabaseCallbackUser;

    class CombinedEventAccessFactory : public IApplicationObserver
    {
    public:
        enum EventSortColumn
        {
            SORT_CREATETIME,
            SORT_SEVERITY,
            SORT_ASSET,
            SORT_DESCRIPTION,
            SORT_VALUE,
            SORT_MMS,
            SORT_DSS,
            SORT_AVL,
            SORT_OPERATOR,
            SORT_COMMENT
        };

        enum EventSortOrder
        {
            SORT_ASCENDING,
            SORT_DESCENDING
        };

        typedef std::vector<unsigned long> NumericList;
		struct PhysicalSubsystemInfo{
			unsigned long	physicalSubsystemId;
			unsigned long	physicalSubsystemState;
		};	// Used for event visibility rules
		typedef std::vector< PhysicalSubsystemInfo > SubSysList;	// Used for event visibility rules
		typedef std::map< unsigned long, SubSysList > FullDutyMap;	// Used for event visibility rules
        typedef std::map<unsigned long,NumericList> NumericMap;

        struct EventFilter
        {
            // The event level.
            unsigned long eventLevel;

            // Duty information required for filtering.
            bool dutyEnabled;
            NumericMap duty;
			FullDutyMap fullDuty;	// Used for event visibility rule
            // The time range.
            time_t fromTime;    // If 0 'from' will be 24 hours before 'to'.
            time_t toTime;      // If 0 'to' will be the current time.

            // Flags used for sorting.
            bool showNonAlarmEvents;

            // Strings to filter by.
            std::string description;
            std::string assetName;
			std::vector<std::string> alarmId;

            // Keys to filter by.
            NumericList eventTypes;
            NumericList operatorIds;
            NumericList severityIds;
            NumericList appSubsystemIds;
            NumericList physSubsystemIds;
            NumericList locationIds;
            NumericList alarmTypes;

            // Order in which to return the events.
            EventSortColumn sortColumn;
            EventSortOrder sortOrder;

			std::string sessionId;	// Used for event visibility rule

			// TD15196 Wenguang++
			std::vector<unsigned long> profiles;	// Used for event visibility rule
        };

        /**
         * getInstance
         *
         * Returns a reference to the only instance of CombinedEventAccessFactory.
         */
        static CombinedEventAccessFactory& getInstance();

        /**
         * getEarliestEventTime
         *
         * Returns the time of the earliest event in the temporary table used
         * by the event viewer.
         *
         * @return time_t The time of the earliest event.
         */
        time_t getEarliestEventTime();


        /**
         * getFirstEventsOnPage
         *
         * Returns the first set of CombinedEventData objects on the specified page
         * that are not excluded by the provided filter.
         *
         * @param filter The filter to apply when retrieving events.
         * @param pageNumber The number of the page to retrieve.
         * @param eventsPerPage The maximum number of events to place on each page.
         *
         * @return std::vector<ICombinedEventData*> The first set of events.
         */
        std::vector<ICombinedEventData*> getFirstEventsOnPage( const EventFilter& filter,
            const unsigned long pageNumber, const unsigned long eventsPerPage );

		/**
         * getFirstEvents
		 *
         * This function used for event viewer with event visibility rules.
		 *
         * Returns the first set of CombinedEventData objects
         * that are not excluded by the provided filter.
         *
         * @param filter The filter to apply when retrieving events.
         *
         * @return std::vector<ICombinedEventData*> The first set of events.
         */
//         std::vector<ICombinedEventData*> getFirstEvents( const EventFilter& filter);

		/**
         * getEventsOfLatestHour
         *
         * Returns the CombinedEventData objects of the latest hour.
         * that are not excluded by the provided filter.
         *
         * @param filter The filter to apply when retrieving events.
		 * @param iterationNum The iteration number of this function. 0 means to retrieve events from 1 hour before to current server time.
		 *					   1 means to retrieve events from 2 hours before to 1 hour before.
         *
         * @return std::vector<ICombinedEventData*> The first set of events.
         */
//         std::vector<ICombinedEventData*> getEventsOfLatestHour( const EventFilter& filter, unsigned int iterationNum);

		/**
         * getEventsBeforeLatestHour
         *
         * Returns the CombinedEventData objects of the latest hour.
         * that are not excluded by the provided filter.
         *
         * @param filter The filter to apply when retrieving events.
         *
         * @return std::vector<ICombinedEventData*> The first set of events.
         */
//         std::vector<ICombinedEventData*> getEventsBeforeLatestHour( const EventFilter& filter, unsigned int iterationNum);

        /**
         * getNextEvents
         *
         * Gets the next set of CombinedEventData objects resulting from the last call
         * to getFirstEventsOnPage.
         *
         * @return std::vector<ICombinedEventData*> The next set of events.
         */
        std::vector<ICombinedEventData*> getNextEvents();
		 

        /**
         * getEventsSinceLastLoad
         *
         * Returns a list of events that have occurred since the last time getEventsOnPage was called.
         * Only events which satisfy the provided filter will be returned.
         *
         * @return std::vector<ICombinedEventData*> The new events.
         */
        std::vector<ICombinedEventData*> getEventsSinceLastLoad();

		/**
         * getVisibleEventsSinceLastLoad
         *
		 * This function is used in event viewer with event visibility rules.
		 *
         * Returns a list of events that have occurred since the last time getEventsOnPage was called.
         * Only events which satisfy the provided filter will be returned.
         *
         * @return std::vector<ICombinedEventData*> The new events.
         */
//         std::vector<ICombinedEventData*> getVisibleEventsSinceLastLoad();

     	std::vector<ICombinedEventData*> getDesignatedEventsSinceLastLoad(bool enableVisibilitRule);

		std::vector<ICombinedEventData*> loadDesignatedEvents(bool enableVisibilitRule, const EventFilter& filter, const std::string pageStartTime, const unsigned long endRow);

		 


        void resetStopFlag();

        //-------- Methods from IApplicationObserver --------//

        virtual void applicationTerminating();
        virtual void applicationPausing();
        virtual void applicationResuming();

		unsigned long getMaxEventPKey();
    private:
        struct LatestKeys
        {
            unsigned long auditPkey;
            unsigned long datapointPkey;
			unsigned long maxPkey;
			LatestKeys()
			{
				auditPkey=0;
				datapointPkey=0;
				maxPkey=0;
			}
        };

        // Disabled methods.
        CombinedEventAccessFactory() : m_stopLoad(false), m_pauseLoad(false), m_curMaxPkey(0),
              m_isLoadingAll(false)
        {
        }
        
        virtual ~CombinedEventAccessFactory()
        {
        }

        CombinedEventAccessFactory( const CombinedEventAccessFactory& );
        CombinedEventAccessFactory& operator=( const CombinedEventAccessFactory& );

        // Singleton object.
        static CombinedEventAccessFactory* m_instance;

        /**
         * getLatestPkeys
         *
         * Retrieves information on the newest element of each type in the database.
         *
         * @exception DatabaseException Thrown if there was a problem executing the query.
         *
         * @exception DataException Thrown if there was a problem extracting data.
         */
        LatestKeys getLatestPkeys();

       /**
         * createCombinedEvent
         *
         * Creates a CombinedEventData object from the IData object.
         *
         * @param row The row number to retrieve.
         * @param data The object containing the data.
         *
         * @return CombinedEventData* The combined event data.
         */
        ICombinedEventData* createCombinedEvent( unsigned long row, IData* data );

        /**
         * generateWhereClause
         *
         * Generates conditions that can be placed in the WHERE clause to
         * retrieve only the required events.
         *
         * @param filter The filter to use when generating the WHERE clause.
         * @param databaseConnection Reference to the database object that is used.
         * @param timeRestrictionClause 
		 *
         * @return std::string The SQL that can be placed directly into the WHERE clause.
         */
        std::string generateWhereClause( const EventFilter& filter, const std::string& timeRestrictionClause, TA_Base_Core::IDatabase& databaseConnection );

		/**
         * generateDutyEventsWhereClause
         * 
		 * This function is used only in event viewer with visibility rules.
		 *
         * Generates conditions that can be placed in the WHERE clause to
         * retrieve only the required physical subsystem events restricted by duty and rights.
		 *
         * @param filter The filter to use when generating the WHERE clause.
		 * @param timeRestrictionClause 
         * @param databaseConnection Reference to the database object that is used.
         *
         * @return std::string The SQL that can be placed directly into the WHERE clause.
         */
        std::string generateDutyEventsWhereClause( const EventFilter& filter, const std::string& timeRestrictionClause, TA_Base_Core::IDatabase& databaseConnection );
		/**
         * generateOwnerEventsWhereClause
         *
		 * This function is used only in event viewer with visibility rules.
		 *
         * Generates conditions that can be placed in the WHERE clause to
         * retrieve only the required operator owned events.
		 *
         * @param filter The filter to use when generating the WHERE clause.
		 * @param timeRestrictionClause
         * @param databaseConnection Reference to the database object that is used.
         *
         * @return std::string The SQL that can be placed directly into the WHERE clause.
         */
		std::string generateOwnerEventsWhereClause( const EventFilter& filter, const std::string& timeRestrictionClause, TA_Base_Core::IDatabase& databaseConnection );

		/**
         * generateSystemEventsWhereClause
         *
		 * This function is used only in event viewer with visibility rules.
		 *
         * Generates conditions that can be placed in the WHERE clause to
         * retrieve only the required system generated events.
         *
         * @param filter The filter to use when generating the WHERE clause.
		 * @param timeRestrictionClause
         * @param databaseConnection Reference to the database object that is used.
         *
         * @return std::string The SQL that can be placed directly into the WHERE clause.
         */
		std::string generateSystemEventsWhereClause( const EventFilter& filter, const std::string& timeRestrictionClause, TA_Base_Core::IDatabase& databaseConnection );

        /**
         * determineSortColumn
         *
         * Since the required columns will change depending on what column is being
         * sorted (refreshing in the event viewer requires the existance of this data
         * to ensure new elements are placed in the correct location) the FROM clause
         * needs to be generated dynamically. This method aids with this by producing
         * the name of the column being sorted on.
         *
         * @param filter The filter to use when determine the column being sorted.
         *
         * @return std::string& The name of the column that is being sorted.
         */
        const std::string& determineSortColumn( const EventFilter& filter );
    
        /**
         * determineSortOrder
         *
         * Determines the order in which the appropriate column should be sorted.
         *
         * @param filter The filter to use when determining the sorting information.
         *
         * @return std::string& The direction (ASC or DESC).
         */
        const std::string& determineSortOrder( const EventFilter& filter );

        /**
         * generateORList
         *
         * Generates a list of equality expressions separated by the OR operation.
         *
         * @param columnName The column name to compare.
         * @param keyList The list of keys to compare against.
         * @param databaseConnection Reference to the database object that is used.
         *
         * @return std::string The list of expressions separated by OR.
         */
        std::string generateORList( const std::string& columnName, const NumericList& keyList, TA_Base_Core::IDatabase& databaseConnection );

        /**
         * generateLocAndSubsystemExpr
         *
		 *
         * Generates the expressions for correctly matching the specified locations and
         * subsystems.
		 * This function is only used when visibility rule is not enabled
         *
         * @param filter The filter.
         * @param databaseConnection Reference to the database object that is used.
         *
         * @return std::string The expression.
         */
        std::string generateLocAndSubsystemExpr( const EventFilter& filter, TA_Base_Core::IDatabase& databaseConnection);

		/**
         * generateLocAndAppSubsystemExpr
         *
		 * This function is used only in event viewer with visibility rules.
		 *
         * Generates the expressions for correctly matching the specified locations and
         * application subsystems.
         *
         * @param filter The filter.
         * @param databaseConnection Reference to the database object that is used.
         *
         * @return std::string The expression.
         */        
		std::string generateLocAndAppSubsystemExpr( const EventFilter& filter, IDatabase& databaseConnection);

        /**
         * generateDutyExpr
         *
         * Parses the duty information into a string that can be inserted into the WHERE
         * clause of a SQL statement.
         *
         * @param duty The duty map.
         * @param databaseConnection Reference to the database object that is used.
         *
         * @return std::string The duty string.
         */
        std::string generateDutyExpr( const NumericMap& duty, TA_Base_Core::IDatabase& databaseConnection );

        /**
         * generateDutyExpr
		 *
         * This function is only used in event viewer with event visibility rules.
		 *
         * Parses the duty information into a string that can be inserted into the WHERE
         * clause of a SQL statement.
         *
         * @param duty The duty map.
         * @param databaseConnection Reference to the database object that is used.
         *
         * @return std::string The duty string.
         */
		std::string generateFullDutyExpr( const FullDutyMap& duty, TA_Base_Core::IDatabase& databaseConnection );

        /**
         * joinExpressions
         *
         * Joins the two expressions with the specified operation
         *
         * @param expr1 The first expression.
         * @param operation The operation.
         * @param expr2 The second expression.
         * @param bracketExpr Places each expression inside brackets.
         *
         * @return std::string The joined expression.
         */
        std::string joinExpressions( const std::string& expr1, const std::string& operation,
                                     const std::string& expr2, bool bracketExpr =true );

        /**
         * filterExcludesAll
         *
         * Determines if the specified filter is guaranteed to return no results.
         *
         * @param filter The filter to check.
         *
         * @return bool True if the filter will never return any rows.
         */
        bool filterExcludesAll( const EventFilter& filter, bool isVisibilityEnabled );

		std::string generateDesignatedWhereClause( const EventFilter& filter, const std::string& timeRestrictionClause ,TA_Base_Core::IDatabase& databaseConnection );

		std::string generateDesignatedDutyExpr( const FullDutyMap& duty, IDatabase& databaseConnection, bool haveAppSubsystem );

        // Variables required for application states.
        bool m_stopLoad;
        bool m_pauseLoad;

        // Variables to keep track of the latest events loaded.
        //unsigned long m_latestAuditPkey;
        //unsigned long m_latestDatapointPkey;


		unsigned long m_curMaxPkey;
		//unsigned long m_nextPkey;
        // Variable to remember the where clause.
        std::string m_lastWhereClause;
		std::vector<ICombinedEventData*> loadAllEvents( const EventFilter& filter);

//		std::vector<ICombinedEventData*> loadAllEventsOfLatestHour( const EventFilter& filter, unsigned int iterationNum);

//		std::vector<ICombinedEventData*> loadAllEventsBeforeLatestHour( const EventFilter& filter, unsigned int iterationNum);

		// Variable to remember whether load all is selected.
		bool m_isLoadingAll;

		// Variables to remember the 3 where clause used in the query
		std::string m_lastDutyWhereClause;
		std::string m_lastOwnerWhereClause;
		std::string m_lastSystemWhereClause;
		std::string m_latestHourTimestamp;
    };
};

#endif // !defined(_COMBINED_EVENT_ACCESS_FACTORY_H_)
