/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioDirectorySynchronisation.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #5 $
 *
 * Last modification: $DateTime: 2010/05/10 14:05:07 $
 * Last modified by:  $Author: builder $
 *
 * This class is responsible for handling the periodic synchronisation of the subscriber database with
 * the TransActive database.
 */
#ifdef __WIN32__
#pragma warning(disable:4786)
#define STD_COUT std::cout
#else // !defined __WIN32__
#define STD_COUT //
#endif

#include <vector>

#include "ace/OS.h"

#include "app/radio/RadioTetraAgent/src/RadioDirectorySynchronisation.h"
#include "app/radio/RadioTetraAgent/src/Radio.h"
#include "app/radio/RadioTetraAgent/src/RadioHelper.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionException.h"
#include "app/radio/RadioTetraAgent/src/IRadioDirectoryCallback.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioProtocolLibrary.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioFields.h"

#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/authentication_library/src/SessionInfo.h"
#include "core/data_access_interface/radio/src/RASubscribers.h"
#include "core/data_access_interface/radio/src/RAGroupMembersAccessFactory.h"
#include "core/data_access_interface/radio/src/RASubscribersAccessFactory.h"
#include "core/data_access_interface/radio/src/RAPatchMembersAccessFactory.h"
#include "core/data_access_interface/radio/src/RASpeedDialSetAccessFactory.h"
#include "core/data_access_interface/radio/src/RASpeedDialButtonAccessFactory.h"
#include "core/data_access_interface/radio/src/RASpeedDialButton.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/DataException.h"
#include "core/synchronisation/src/ThreadGuard.h"

using namespace TA_Base_Core;

namespace TA_IRS_App
{

    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    RadioDirectorySynchronisation::RadioDirectorySynchronisation(time_t synchronisationFrequency,
                                            TA_IRS_App::Radio& radio,
                                            TA_Base_Core::RadioDirectoryEntityDataPtr entityData,
                                            IRadioDirectoryCallback& directoryCallback)
        : m_synchronisationFrequency(synchronisationFrequency),
		  m_elapsedTime(0), 
		  m_isTerminated(false), 
          m_doSynchronise(false),
          m_radio(radio), 
		  m_entityData(entityData),
          m_directoryCallback(directoryCallback)
    {
    }

    RadioDirectorySynchronisation::~RadioDirectorySynchronisation()
    {
		m_isTerminated = true;
    }
    void RadioDirectorySynchronisation::run()
    {
         FUNCTION_ENTRY("run");

		// Let agent start up
        time_t initial_delay=40; // seconds

        Thread::sleep(initial_delay*1000);

        // cause delay through countdown to sync rather than sleep.
		m_elapsedTime = (m_synchronisationFrequency>initial_delay) ?
                            (m_synchronisationFrequency - initial_delay) : 0 ;

        LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"Initial value of elapsed time %ld seconds", m_elapsedTime);

        // Loop, running synchronise() every so often.
        do
        {
			try
			{
                if ( ( m_synchronisationFrequency != 0 ) && ( m_elapsedTime % 10 == 0 ) )
                {
                    LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"%ld(/%ld) seconds since last directory synchronisation attempt", m_elapsedTime, m_synchronisationFrequency);
                }

                if (   m_synchronisationFrequency <= m_elapsedTime
					&& m_synchronisationFrequency != 0
					&& m_doSynchronise
					&& !m_radio.inFallback())
                {
                    m_elapsedTime = 0;

                    LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"Starting directory synchronisation");
#ifdef __WIN32__
					STD_COUT << "Starting directory synchronisation..." << std::endl;
#endif // #ifdef __WIN32__
                    synchronise();
                    LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"Directory synchronisation complete");
#ifdef __WIN32__
					std::cout << "Directory synchronisation complete" << std::endl;
#endif // #ifdef __WIN32__

                }
				else
				{
					std::stringstream currentStatus;
					currentStatus << " m_synchronisationFrequency is " << m_synchronisationFrequency;
					currentStatus << " m_elapsedTime is " << m_elapsedTime;
					currentStatus << " m_doSynchronise is " << (m_doSynchronise ? "true" : "false");
					currentStatus << " Radio Fallback is " << (m_radio.inFallback() ? "true" : "false");

					LOG_GENERIC( SourceInfo,DebugUtil::DebugDebug, 
						"Not starting directory synchronisation. current status is %s.", 
						currentStatus.str().c_str() );
				}

                Thread::sleep(1000);

                //
                // only increase the elapsed time if it would affect whether we
                // sync or not, otherwise we might increment all the way into
                // negative value territory
                if ( m_elapsedTime < m_synchronisationFrequency )
                {
                    m_elapsedTime += 1;
                }
			}
			catch (RadioSessionException rse)
			{
				// catchall - session might not be initialised..
				LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"Caught RadioSessionException during synchronisation: %s", rse.what());
				// TODO - raise an alarm whilst no synchronisation!
#ifdef __WIN32__
				std::cout << "Directory synchronisation error, not completed" << std::endl;
#endif // #ifdef __WIN32__
			}
			catch (TransactiveException te)
			{
				// catchall - session might not be initialised..
				LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"Caught TransactiveException during synchronisation: %s", te.what());
				// TODO - raise an alarm whilst no synchronisation!
#ifdef __WIN32__
				std::cout << "Directory synchronisation error, not completed" << std::endl;
#endif // #ifdef __WIN32__
			}
			catch(...)
			{
				// catchall - session might not be initialised..
				LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"Caught TransactiveException during synchronisation.");
				// TODO - raise an alarm whilst no synchronisation!
#ifdef __WIN32__
				// std::cout << "Directory synchronisation error, not completed" << std::endl;
#endif // #ifdef __WIN32__
			}
        }
        while(!m_isTerminated);
		m_isTerminated = false;
        FUNCTION_EXIT;
    }

    void RadioDirectorySynchronisation::terminate()
    {
       FUNCTION_ENTRY("terminate");
       m_isTerminated = true;
        FUNCTION_EXIT;
    }

//************************************
// Helper Methods                    *
//************************************

    void RadioDirectorySynchronisation::synchronise()
    {
        ThreadGuard synchroniseGuard(m_synchroniseLock);

        FUNCTION_ENTRY("synchronise");

        // Determine the last time an update was performed
        std::string dbTimeString = getRadioTimeString();
        std::string zeroTimeString = getRadioTimeString(true);
        
        // Change the reference to full reporting mode
        //
        // But stick with NORMAL if this is the initial sync
        RadioMethodChangeReference::ESearchType searchType = RadioMethodChangeReference::FULL;
        // date string match taken from real log files
        if ( dbTimeString == zeroTimeString )
        {
            searchType = RadioMethodChangeReference::NORMAL;
        }

        LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"Attempting to change to %s subscriber search mode", ((searchType==RadioMethodChangeReference::FULL) ? "FULL" : "NORMAL"));
        if (!changeSearchType(searchType))
        {
            return;
        }


        // Now that we have the correct timestamp, search the subscribers using the timestamp as a basis.
        // First, the fields we want need to be stored in a vector, so create the vector and add the fields.
        std::vector<std::string> fields;
        fields.push_back(NAME);
        fields.push_back(TYPE);
        fields.push_back(ITSI);
        fields.push_back(DELETED);
        fields.push_back(REGION);

        //std::vector<RadioEvent*> groupEvents = searchSubscribers(dbTimeString,RadioMethodSearchSubscribers::NORMAL,fields);


        //
        // set up field positions
        addPosition("textReference",0); // not used: added for clarity
        addPosition("namePosition",1);
        addPosition("typePosition",2);
        addPosition("itsiPosition",3);
        addPosition("deletedPosition",4);
        addPosition("regionPosition", 5);

        std::vector<RadioEvent*> groupEvents = searchSubscribers(dbTimeString,RadioMethodSearchSubscribers::NORMAL,fields);
        processIndividualEvents(groupEvents);
        groupEvents.clear();

        //
        // ============= SUBSCRIBER GROUP MEMBERSHIP =============
        //
        // Note: reuses the individuals' fields
        groupEvents = searchSubscribers(dbTimeString,RadioMethodSearchSubscribers::K_PARENTS,fields);
		
        //
        // Note: reuses the individuals' field positions
        //processGroupEvents(groupEvents,dbTimeString,fields);
        processGroupEvents( groupEvents, "", fields ); //limin++, CL-20729, CL-20948
        groupEvents.clear();

        // 
        // ============= PATCH GROUP MEMBERSHIP =============
        // 
        fields.clear();
        fields.push_back(PATCH_ID);
        fields.push_back(TEMPORARY_TSI);
        fields.push_back(DELETED);

        groupEvents = searchSubscribers(dbTimeString,RadioMethodSearchSubscribers::K_PATCH,fields);

        //
        // set up field positions
        // K_PATCH_ID is used like K_NAME
        // K_TEMPORARY_TSI is like K_TSI
        clearPositions();
        addPosition("textReference",0); // not used: added for clarity
        addPosition("patchIdPosition",1);
        addPosition("itsiPosition",2);
        addPosition("deletedPosition",3);
        //
        processPatchEvents(groupEvents,dbTimeString,fields);
        groupEvents.clear();


#if defined RADIODIRECTORYSYNCHRONISATION_FETCH_DEFINEDLOCATIONAREAS
        //
        // ============= DEFINED LOCATION AREAS =============
        //
        // a.k.a. DefinedLA
        fields.clear();
        fields.push_back(SCN_ID);
        fields.push_back(LOCATION_AREA_ID);
        fields.push_back(LOCATION_AREA_LABEL);
        fields.push_back(LOCATION_AREA_DN);
        groupEvents = searchSubscribers("",RadioMethodSearchSubscribers::K_DEFINED_LA,fields,"0=0"); // funny hack to make HLR where clause work
        // no need to process these events, just log the GetGroupDetails events
        // for harvesting information for configuration
#endif // #if defined RADIODIRECTORYSYNCHRONISATION_FETCH_DEFINEDLOCATIONAREAS
        //

        m_directoryCallback.onSubscriberChange(0, IRadioDirectoryCallback::SCAN_SUBSCRIBERS);

        m_entityData->setLastUpdateTime(time(0));

        FUNCTION_EXIT;
    }



    TA_Base_Core::IRASubscribers::SubscriberType RadioDirectorySynchronisation::getSubscriberType(const std::string& typeString) const
    {
         FUNCTION_ENTRY("getSubscriberType");

        TA_Base_Core::IRASubscribers::SubscriberType subscriberType = TA_Base_Core::IRASubscribers::INVALID;
        if (typeString == "D")
        {
            subscriberType = TA_Base_Core::IRASubscribers::DISPATCHER;
        }
        else if (typeString == "G")
        {
            subscriberType = TA_Base_Core::IRASubscribers::GROUP;
        }
        else if (typeString == "M")
        {
            subscriberType = TA_Base_Core::IRASubscribers::MOBILE;
        }
        else if (typeString == "P")
        {
            subscriberType = TA_Base_Core::IRASubscribers::PATCH_GROUP;
        }
        else if (typeString == "I")
        {
            subscriberType = TA_Base_Core::IRASubscribers::INDIVIDUAL;
        }

        LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"getSubscriberType(\"%s\") returning %d", typeString.c_str(), (int)subscriberType);
        FUNCTION_EXIT;
        return subscriberType;
    }

    const unsigned long RadioDirectorySynchronisation::getLocationKey(const std::string& locationName) const
    {
         FUNCTION_ENTRY("getLocationKey");

        unsigned long locationKey = 0;
        try
        {
            ILocation* location = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByName(locationName);
            locationKey = location->getKey();

            delete location;
            location = NULL;
        }
        catch(...)
        {
            // leave the location key as 0
        }

        LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"getLocationKey(\"%s\") returning %lu", locationName.c_str(), locationKey);
        FUNCTION_EXIT;
        return locationKey;
    }


    const std::string RadioDirectorySynchronisation::getRadioTimeString(bool doGetZero) const
    {
        FUNCTION_ENTRY("getRadioTimeString");

        time_t lastUpdate = 0;

        if (!doGetZero)
        {
			try
			{
				lastUpdate = m_entityData->getLastUpdateTime();
				// NO WAY.  This should only be done on a fully successful update.
				// m_entityData.setLastUpdateTime(time(NULL));
			}
			catch(...)
			{
				// If there is an error getting the value, leave it at 0, otherwise, if there was an error setting the 
				// date, reloading from the current timestamp won't hurt next time.            
			}
        }


        std::string timeString = "";

        // if (lastUpdate != 0)
        // {
            // Assuming everything is alright, now need to convert the last update time to a format acceptable by the
            // database.
            RadioMethodConvertToDBTime convertTimeMethod(m_radio.getISCSRadioSessionRef(),lastUpdate);
            RadioEvent* event = m_radio.sendMethod(convertTimeMethod);

            // When the event comes back, need to retrieve the date text from it to use in the query (the next thing
            // we'll send).
            if (event == NULL || event->getStatus() != 0)
            {
                LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"Failed to convert previous time to database time. Database update aborted.");
                TA_THROW(TransactiveException("Error in radio communication."));;            
            }

            ConvertToDBTimeEventReader timeReader(*event);
            timeString = timeReader.getDBTimeString();
			LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"LastUpdate time string in DB format: %s", timeString.c_str());

            // Delete the radio event
            delete event;
            event = NULL;
        // }

        return timeString;
        FUNCTION_EXIT;
    }

    std::vector<RadioEvent*> RadioDirectorySynchronisation::searchSubscribers
    (   const std::string& dbTime,
        const RadioMethodSearchSubscribers::EView searchView,
        std::vector<std::string>& fields,
        const std::string & extraSearchCriteria) const
    {
          FUNCTION_ENTRY("searchSubscribers");

        RadioMethodSearchSubscribers searchSubscribersMethod(m_radio.getISCSRadioSessionRef());
        searchSubscribersMethod.setSearchType(searchView);
        std::string searchCriteria = "";
        if ( ! dbTime.empty() )
        {
            searchCriteria = "TIMESTAMP>" + dbTime;
        }
        searchCriteria += extraSearchCriteria;
	    searchSubscribersMethod.setCriteria(searchCriteria);
		RadioEvent* event = m_radio.sendMethod(searchSubscribersMethod);

        // if event->getStatus()<STATUS_OK a RadioSessionException will be thrown already
        if (event == 0) // || event->getStatus() < STATUS_OK)
        {
            LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"Searching for individual subscribers failed. Database update aborted.");
            TA_THROW(TransactiveException("Error while searching for subscribers."));
        }

        // The event will contain a "search reference" that we will need to pass
        // to a GetGroupDetails method to retrieve the details of this event.
        // NOTE: this is a very interesting way of utilising an event reader,
        // this may need investigating to see if it is as appropriate as the
        // usual dynamic_cast<DerivedEventReader *>(event->getReader()) paradigm
        SearchSubscribersEventReader searchSubscribersReader(*event);
        unsigned long searchReference = searchSubscribersReader.getSubscriberReferenceResultSet();

        // Delete the event
        delete event;
        event = NULL;
        // delete searchSubscribersReader;
        // searchSubscribersReader = NULL;
        
        // before calling GetGroupDetails, need to ensure that there were some
        // returned results. If there were none, GetGroupDetails won't just return
        // an empty result set, it will return STATUS_KO_INVALID_FIELD
        std::string num_results_str = queryReference(searchReference, RadioMethodQueryReference::K_MEMBERS_COUNT);
        long num_results = atol(num_results_str.c_str());

        std::vector<RadioEvent*> events;
        if (num_results < 1)
        {
			// First however, delete the search reference
			RadioMethodDeleteReference deleteReferenceMethod(m_radio.getISCSRadioSessionRef());
			deleteReferenceMethod.setReference(RadioMethodDeleteReference::SEARCH_REFERENCE,searchReference);
			m_radio.sendMethod(deleteReferenceMethod);

            FUNCTION_EXIT;
            return events;
        }

        // Consturct the get group details method
        RadioMethodGetGroupDetails getGroupDetailsMethod(m_radio.getISCSRadioSessionRef(),searchReference);

        // And add it to the message
        getGroupDetailsMethod.setFields(fields);

        // This time, there may (and probably will) be many radio events sent in response to the single query.
        // So use the alternate send method
        events =  m_radio.sendMethodMultipleResponse(getGroupDetailsMethod);
        
        // Now we have all the individual subscriber updates, need to move onto groups.
        // First however, delete the search reference
        RadioMethodDeleteReference deleteReferenceMethod(m_radio.getISCSRadioSessionRef());
        deleteReferenceMethod.setReference(RadioMethodDeleteReference::SEARCH_REFERENCE,searchReference);

        m_radio.sendMethod(deleteReferenceMethod);

        FUNCTION_EXIT;
        return events;
    }

    static bool discardSubscriber
    (   GroupDetailsTable & details,
        unsigned int row,
        unsigned int namePosition,
        unsigned int regionPosition,
        const std::string & tsi,
        int fussiness = 1)
    {
        // RvH 20061003 Taipei PreSAT fix
        // the GetGroupDetailsEventReader no longer populates the final
        // column with an empty string if required, so do a check that the
        // fields exist before using them
        GroupDetailsRow & detailsRow = details[row];
        std::string regionStr = "";
        std::string nameStr = "";
		std::string tsiStr = "";
        unsigned int requiredsize = ((namePosition > regionPosition) ? namePosition : regionPosition) + 1;

        while ( detailsRow.size() < requiredsize )
        {
            detailsRow.push_back(std::string(""));
        }
        regionStr = detailsRow[regionPosition];
        nameStr = detailsRow[namePosition];
		tsiStr  = tsi;

        if (fussiness == 1)
        {
            // Both region and name empty == DISCARD
            // This applies to DGNA groups.
            if ( (regionStr.empty()) && (nameStr.empty()) )
            {
                LOG_GENERIC(SourceInfo,DebugUtil::DebugWarn, "Subscriber %s has neither name nor region (location). Discarding", tsiStr.c_str());
                return true;
            }

            // location empty == BAD CONFIG
            // But is not useless, and has been known to occur with useful subscribers
            // populate with "<No Location>"
            if (regionStr.empty())
            {
                LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "Subscriber %s has no location. Setting to \"OCC\"", nameStr.c_str());
                detailsRow[regionPosition] = "OCC";
                return false;
            }

            // name empty == SAD, but not useless - populate with TSI
            if (nameStr.empty())
            {
                LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "Subscriber %s has no name, setting to TSI", tsi.c_str());
                detailsRow[namePosition] = tsi;
                return false;
            }
        }
        else if (fussiness == 2)
        {
            // fussier: if *either* location or name are empty, discard
            if ( (regionStr.empty()) || (nameStr.empty()) )
            {
                LOG_GENERIC(SourceInfo,DebugUtil::DebugWarn, "Subscriber %s has either no name or no location. Discarding record.", tsi.c_str());
                return true;
            }
        }

        return false;
    }

#define CATCH_DATA_EXCEPTION(extra) catch(DataException & dataexception) { logDataException(SourceInfo, dataexception, extra); }
    static void logDataException(const std::string& file, int line, const DataException & dataex, const char * extra)
    {
        std::string datatype = "INVALID_VALUE";
        switch (dataex.getFailType())
        {
            case DataException::WRONG_TYPE:
                datatype = "WRONG_TYPE";
                break;
            case DataException::NO_VALUE:
                datatype = "NO_VALUE";
                break;
            case DataException::NOT_UNIQUE:
                datatype = "NOT_UNIQUE";
                break;
            case DataException::CANNOT_DELETE:
                datatype = "CANNOT_DELETE";
                break;
            case DataException::MISSING_MANDATORY:
                datatype = "MISSING_MANDATORY";
                break;
            default:
                datatype = "INVALID_VALUE";
        }
        
        LOG_GENERIC(file, line, DebugUtil::DebugError,
                    "Caught DataException %s, msg=\"%s\", type=%s, data=\"%s\"",
                    extra, dataex.what(), datatype.c_str(), dataex.getWhichData().c_str());
    }

    const void RadioDirectorySynchronisation::processIndividualEvents(std::vector<RadioEvent*> groupEvents) const
    {
          FUNCTION_ENTRY("processIndividualEvents");
        // Get out the positions for easier use
        unsigned int namePosition = getPosition("namePosition");
        unsigned int typePosition = getPosition("typePosition");
        unsigned int itsiPosition = getPosition("itsiPosition");
        unsigned int deletedPosition = getPosition("deletedPosition");
        unsigned int regionPosition = getPosition("regionPosition");
        // Process each event, dealing with the data they contain as necessary
        // unsigned int i = 0;
        // for (i = 0;i < groupEvents.size();i++)
        for (long i = groupEvents.size()-1 ; i>=0 ; i--)
        {
            if (groupEvents[i] == NULL ||  (groupEvents[i]->getStatus() < 0))
            {
                LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
                    "Group detail event %lu of %lu (individual subscribers) was invalid, and has been dropped.",
                    i,groupEvents.size());
            }
            else
            {
                GetGroupDetailsEventReader detailsReader(*(groupEvents[i]));
                // const std::vector<std::vector<std::string> > details = detailsReader.getDetailsTable();
                GroupDetailsTable details;
                /* int numRows = */ detailsReader.getDetailsTable(details);

                // The details vector contains the rows and columns of the table, with each inner vector
                // representing an entire row (i.e. details[0] is the first row, which details[0][0] is the
                // first column in the first row
                for (long row = details.size()-1 ; row>=0 ; row--)
                {
                    // RvH 20050129 Pre-Radio-iFAT HACK
                    // if the size of the row is not correct, add a blank
                    // it should really only be short by one at most, due to the
                    // way that the GetGroupDetailsEventReader pads blank fields
                    // NOTE that we shouldn't make it a blank space, because we
                    // still want to discard rows with a blank location AND name
                    unsigned int numPositions=getNumPositions();
                    if (details[row].size() < numPositions)
                    {
						//details[row][numPositions] = "";
						for (unsigned int j = details[row].size(); j <= numPositions; j++)
						{
							details[row].push_back("");
						}
                    }

                    LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
                            "Processing GroupDetailsEvent subscriber ITSI:%s Name:%s Type:%s Location:%s Deleted:%s",
                            details[row][itsiPosition].c_str(),
                            details[row][namePosition].c_str(),
                            details[row][typePosition].c_str(),
                            details[row][regionPosition].c_str(),
                            details[row][deletedPosition].c_str());

                    if (discardSubscriber(details, row, namePosition, regionPosition, details[row][itsiPosition]))
                    {
                        // row--;
                        continue;
                    }

                    // Column order is always the same - name, type, itsi, deleted flag, and region
                    // If the row represents a deleted item, then need to remove it from the database
                    if (RadioHelper::isOTETrue(details[row][deletedPosition]))
                    {
                        // To delete an object, need to load it first
                        try
                        {
                            IRASubscribers* subscriber = TA_Base_Core::RASubscribersAccessFactory::getInstance().getSubscriberDetails(details[row][itsiPosition]);
                            
                            if (details[row][typePosition] == "G")
                            {
                                TA_Base_Core::RASubscribersAccessFactory::getInstance().deleteGroup(details[row][itsiPosition]);
                            }
                            else
                            {
                                TA_Base_Core::RASubscribersAccessFactory::getInstance().deleteSubscriber(subscriber);
                            }
                            
                            // RvH TEMPORARY LOG
                            LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "About to call onSubscriberChange for DELETE_SUBSCRIBER %s ...", details[row][itsiPosition].c_str());
                            m_directoryCallback.onSubscriberChange(subscriber, IRadioDirectoryCallback::DELETE_SUBSCRIBER);
                            // RvH TEMPORARY LOG
                            LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "onSubscriberChange successful");
                            
                            // RvH TEMPORARY LOG
                            LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "About to delete RASubscriber %s object...", details[row][itsiPosition].c_str());
                            delete subscriber;
                            subscriber = NULL;
                            // RvH TEMPORARY LOG
                            LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "...subscriber deleted");
                        }
                        catch(...)
                        {
                            LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
                                "Error while deleting subscriber with TSI: %s This could simply be because the subscriber was already deleted, or it could be another problem.",
                                details[row][itsiPosition].c_str());
                        }
                    }
                    else
                    {
                        // This means that something to do with this subscriber has been altered (or
                        // it has been created)
                        // Create the subscribers object...
                        IRASubscribers* subscriber;

                        // And try to load it from the database
                        try
                        {
                            subscriber = TA_Base_Core::RASubscribersAccessFactory::getInstance().
                                getSubscriberDetails(details[row][itsiPosition]);
                            subscriber->setSubscriberName(details[row][namePosition]);
                            subscriber->setLocation(details[row][regionPosition]);
                            subscriber->setSubscriberType(getSubscriberType(details[row][typePosition]));
                            subscriber->setLocationKey(getLocationKey(details[row][regionPosition]));
                            
                            // Now that we have the subscriber, simply apply the changes 
                            subscriber->applyChanges();

                            // RvH TEMPORARY LOG
                            LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "About to call onSubscriberChange for UPDATE_SUBSCRIBER %s ...", details[row][itsiPosition].c_str());
                            // also notify any GUIs
                            m_directoryCallback.onSubscriberChange(subscriber, IRadioDirectoryCallback::UPDATE_SUBSCRIBER);
                            // RvH TEMPORARY LOG
                            LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "onSubscriberChange successful");

                            // RvH TEMPORARY LOG
                            LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "About to delete RASubscriber %s object...", details[row][itsiPosition].c_str());
                            delete subscriber;
                            subscriber = NULL;
                            // RvH TEMPORARY LOG
                            LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "...subscriber deleted");
                        }
                        catch(...)
                        {
                            // The subscriber didn't exist...so create it
                            // But to do so, need to work out the region key of the region speicified for the
                            // subscriber (if it has one)
							try
							{
								unsigned long locationKey = getLocationKey(details[row][regionPosition]);

								// And need to determine the subscriber type
								TA_Base_Core::IRASubscribers::SubscriberType subscriberType = getSubscriberType(details[row][typePosition]);

								subscriber = new TA_Base_Core::RASubscribers(details[row][itsiPosition],
																		details[row][namePosition],
																		subscriberType,
																		details[row][regionPosition],
																		locationKey,
																		true);
								LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
                                        "New subscriber created. ITSI:%s Name:%s Type:%s Location:%s (%lu)",
                                        details[row][itsiPosition].c_str(),
                                        details[row][namePosition].c_str(),
                                        details[row][typePosition].c_str(),
                                        details[row][regionPosition].c_str(),
                                        locationKey);

		                        // Now that we have the subscriber, simply apply the changes 
								subscriber->applyChanges();
								LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"New subscriber applied to database");

                                // RvH TEMPORARY LOG
                                LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "About to call onSubscriberChange for NEW_SUBSCRIBER %s ...", details[row][itsiPosition].c_str());
                                // also notify any GUIs
                                m_directoryCallback.onSubscriberChange(subscriber, IRadioDirectoryCallback::NEW_SUBSCRIBER);
                                // RvH TEMPORARY LOG
                                LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "onSubscriberChange successful");

                                // RvH TEMPORARY LOG
                                LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "About to delete RASubscriber %s object...", details[row][itsiPosition].c_str());
                                delete subscriber;
                                subscriber = NULL;
                                // RvH TEMPORARY LOG
                                LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "...subscriber deleted");
							}
                            CATCH_DATA_EXCEPTION("while creating or updating a subscriber")
							catch(...)
							{
								// An error occurred while applying changes to the subscriber.
								LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
									"Error while creating or updating subscriber with TSI: %s",
									details[row][itsiPosition].c_str());
							}
                        }
                    }
                }

                // RvH TEMPORARY LOG
                LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "About to clear details table...");
                details.clear();
                // RvH TEMPORARY LOG
                LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug, "...details table cleared");
            }
        }
        
        // Need to delete the event object associated with it
        for (unsigned int j = 0;j < groupEvents.size();j++)
        {
            delete groupEvents[j];
            groupEvents[j] = NULL;
        }
        FUNCTION_EXIT;
    }
    
    const void RadioDirectorySynchronisation::processGroupEvents(std::vector<RadioEvent*> groupEvents,
        const std::string& dbTime,std::vector<std::string>& fields) const
    {
          FUNCTION_ENTRY("processGroupEvents");

	    unsigned int namePosition = getPosition("namePosition");
        // unsigned int typePosition = getPosition("typePosition");
        unsigned int itsiPosition = getPosition("itsiPosition");
        unsigned int deletedPosition = getPosition("deletedPosition");
        unsigned int regionPosition = getPosition("regionPosition");
		size_t i;

        // Now loop for each event returned
        for (i = groupEvents.size() ; i>0 ; i--)
        // for (i = 0;i < groupEvents.size();i++)
        {
            // Within each event is a list of the groups modified since the last synchronisation
			if(groupEvents[i-1] == NULL)
				continue;
            GetGroupDetailsEventReader detailsReader(*(groupEvents[i-1]));
            // const std::vector<std::vector<std::string> > details = detailsReader.getDetailsTable();
            GroupDetailsTable details;
            /* int numRows = */ detailsReader.getDetailsTable(details);

            // now, for each row on the table, need get the group members (which weren't returned last time round)
            // for (unsigned int row = 0;row < details.size();row++)
            for (long row = details.size()-1 ; row>=0 ; row--)
            {
                // RvH 20050129 Pre-Radio-iFAT HACK
                // if the size of the row is not correct, add a blank
                // it should really only be short by one at most, due to the
                // way that the GetGroupDetailsEventReader pads blank fields
                // NOTE that we shouldn't make it a blank space, because we
                // still want to discard rows with a blank location AND name
                unsigned int numPositions = getNumPositions();
                if (details[row].size() < numPositions)
                {
                    //details[row][numPositions] = "";
					for (unsigned int j = details[row].size(); j <= numPositions; j++)
					{
						details[row].push_back("");
					}
                }

                LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
                        "Processing GroupDetailsEvent subscriber ITSI:%s Name:%s Location:%s Deleted:%s",
                        details[row][itsiPosition].c_str(),
                        details[row][namePosition].c_str(),
                        details[row][regionPosition].c_str(),
                        details[row][deletedPosition].c_str());

                if (discardSubscriber(details, row, namePosition, regionPosition, details[row][itsiPosition]))
                {
                    // row--;
                    continue;
                }

                std::string itsiStr = details[row][itsiPosition];
                std::string regionStr = details[row][regionPosition];

                // If the group has been deleted, then can simply remove the group full stop...
                if (RadioHelper::isOTETrue(details[row][deletedPosition]))
                {
                    // Attempt to delete the group from the database
                    try
                    {
                        TA_Base_Core::RASubscribersAccessFactory::getInstance().deleteGroup(itsiStr);
                    }
                    catch(...)
                    {
                        LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
                            "Caught miscellaneous exception while deleting group with TSI: %s",
                            itsiStr.c_str());
                    }
                }
                else
                {
                    // This group hasn't been deleted, so either create it or load it
                    IRASubscribers* group = NULL;

                    try
                    {
                        group = TA_Base_Core::RASubscribersAccessFactory::getInstance().getSubscriberDetails(itsiStr);
                    }
                    catch(TA_Base_Core::DataException & de)
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                                    "Caught DataException, msg:\"%s\", data:\"%s\"",
                                    de.what(), de.getWhichData().c_str());
                        LOG_EXCEPTION_CATCH(SourceInfo, "DataException","Caught exception while attempting to load subscriber. This could simply be because the subscriber does not exist yet, or it could be another error");
                        // If there was an error loading the item from the database, then try to create one instead
                        try
                        {   
                            // Need to work out the location key for the group
                            unsigned long locationKey = getLocationKey(regionStr);

                            group = new TA_Base_Core::RASubscribers(itsiStr,
                                                                    details[row][namePosition],
                                                                    TA_Base_Core::IRASubscribers::GROUP,
                                                                    regionStr,
                                                                    locationKey,
                                                                    true);

                            // And save the group to the database
                            group->applyChanges();
                        }
                        CATCH_DATA_EXCEPTION("while converting a string to location key or adding a group subscriber")
                        catch(...)
                        {
                            // If this fails - well, that's bad. Give up on this group and continue to the next one
                            LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
                                        "Non-recoverable error while attempting to create group with TSI: %s",
                                        itsiStr.c_str());
                            // move onto the next group
                            row--;
                            continue;
                        }
                    }
                    catch(...)
                    {
                        // Otherwise this group is a lost cause
                        LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
                                    "Miscellaneous exception while attempting to load group with TSI: %s",
                                    itsiStr.c_str());
                        // move onto the next group
                        row--;
                        continue;
                    }


                    // we find the group hasn't been deleted, so we must query
                    // for the list of group members
                    std::string extra = ( dbTime.empty() ? std::string( "" ) : std::string( " AND " ) ); //limin++, CL-20729, CL-20948
                    extra += " PARENT='" + details[row][itsiPosition] + "'";

                    // limin++ IRS97
                    std::vector<RadioEvent*> groupMemberEvents = searchSubscribers
                                                                ( dbTime,
                                                                  RadioMethodSearchSubscribers::K_MEMBERS,
                                                                  fields,
                                                                  extra + " AND DELETED_FLAG='N'" );

                    processGroupMembershipEvents( groupMemberEvents, itsiStr, group, false );
                    // ++limin IRS97
                }
            }

            details.clear();
        }
        
        // Need to delete the event object associated with it
        for (i = 0;i < groupEvents.size();i++)
        {
            delete groupEvents[i];
            groupEvents[i] = NULL;
        }
        FUNCTION_EXIT;
    }
       

    const void RadioDirectorySynchronisation::processGroupMembershipEvents
    (   std::vector<RadioEvent*> groupMemberEvents,
        const std::string & gitsi,
        TA_Base_Core::IRASubscribers * group,
        bool isDeletedFromGroup ) const
    {
          FUNCTION_ENTRY("processGroupMembershipEvents");

	    unsigned int namePosition = getPosition("namePosition");
        unsigned int itsiPosition = getPosition("itsiPosition");
        unsigned int deletedPosition = getPosition("deletedPosition");
        unsigned int regionPosition = getPosition("regionPosition");

        // Now loop for each response
        for (long j=groupMemberEvents.size()-1 ; j>=0 ; j--)
        // for (unsigned int j = 0;j < groupMemberEvents.size();j++)
        {
            // And retrieve the table from the response
            GetGroupDetailsEventReader groupDetailsReader(*(groupMemberEvents[j]));
            // const std::vector<std::vector<std::string> > groupMembers = groupDetailsReader.getDetailsTable();
            GroupDetailsTable groupMembers;
            /* int numRows = */ groupDetailsReader.getDetailsTable(groupMembers);

            // for (unsigned int groupMembersRow = 0;groupMembersRow < groupMembers.size();groupMembersRow++)
            for (long groupMembersRow = groupMembers.size()-1 ; groupMembersRow>=0 ; groupMembersRow--)
            {
                if (discardSubscriber(groupMembers, groupMembersRow, namePosition, regionPosition, groupMembers[groupMembersRow][itsiPosition]))
                {
                    continue;
                }

                // For each group member determine if they've been deleted or not
                // if (RadioHelper::isOTETrue(groupMembers[groupMembersRow][deletedPosition])) // limin-- IRS97
                if ( true == isDeletedFromGroup ) // limin++ IRS97
                {
                    // Attempt to remove this member from the group - put it in a try/catch in case there are any problems
                    try
                    {
                        // Load the group member from the database
                        IRASubscribers* subscriber = TA_Base_Core::RASubscribersAccessFactory::getInstance().
                            getSubscriberDetails(groupMembers[groupMembersRow][itsiPosition]);

                        // And remove it from the group
                        TA_Base_Core::RAGroupMembersAccessFactory::getInstance().deleteMemberFromGroup(*group, *subscriber);
                    }
                    CATCH_DATA_EXCEPTION("while loading a group or deleting a member from a group")
                    catch(...)
                    {
                        LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
                            "Error while removing member with TSI = %s from group with TSI = %s. This could simply be because the subscriber was already deleted, or it could be another problem.",
                            groupMembers[groupMembersRow][itsiPosition].c_str(),gitsi.c_str());
                    }
                }
                else
                {
                    // This member has been added to the group, so load it then add it
                    try
                    {
                        IRASubscribers* subscriber = TA_Base_Core::RASubscribersAccessFactory::getInstance().
                            getSubscriberDetails(groupMembers[groupMembersRow][itsiPosition]);

                        TA_Base_Core::RAGroupMembersAccessFactory::getInstance().addMemberToGroup(*group, *subscriber);
                    }
                    catch(...)
                    {
                        LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
                            "Error while adding member with TSI = %s to group with TSI = %s. This could simply be because the subscriber was already deleted, or it could be another problem.",
                            groupMembers[groupMembersRow][itsiPosition].c_str(),gitsi.c_str());
                    }
                }
            }
            groupMembers.clear();
        }
    }


    void RadioDirectorySynchronisation::addPosition(const std::string& positionName,const unsigned int position)
    {
        FUNCTION_ENTRY("addPosition");
        // use operator[] - it does the insert-or-assign that we want
        m_positionMap[positionName] = position;
        FUNCTION_EXIT;
    }

    void RadioDirectorySynchronisation::removePosition(const std::string& positionName)
    {
        FUNCTION_ENTRY("removePosition");
        FieldPositionMap::iterator iter = m_positionMap.find(positionName);
        if (iter != m_positionMap.end())
        {
            m_positionMap.erase(iter);
        }
        FUNCTION_EXIT;
    }


    unsigned int RadioDirectorySynchronisation::getPosition(const std::string& positionName) const
    {
        FUNCTION_ENTRY("getPosition");

        FieldPositionMap::const_iterator iter = m_positionMap.find(positionName);
        
        if (iter != m_positionMap.end())
        {
            // Position is in map
            return iter->second;
        }
        else
        {
            // Not on map so need to add it
            TA_ASSERT(false,"Attempted to access position that has not been added to the map");
        }
        
        FUNCTION_EXIT;
        return 0;
    }

    void RadioDirectorySynchronisation::clearPositions()
    {
        FUNCTION_ENTRY("getPosition");
        m_positionMap.clear();
        FUNCTION_EXIT;
    }

    unsigned int RadioDirectorySynchronisation::getNumPositions() const
    {
        return m_positionMap.size();
    }

    const void RadioDirectorySynchronisation::processPatchEvents(std::vector<RadioEvent*> patchEvents,
        const std::string& dbTime,std::vector<std::string>& fields) const
    {
        FUNCTION_ENTRY("processPatchEvents");

        // Patch events work like the group events - if the particular patch has not
        // been deleted then need to get the members of the patch, and work with them.
        // Patch events are *different* to group events - because patches have no
        // location/region, we assign them to Occ.
        unsigned int patchIdPosition = getPosition("patchIdPosition");
        unsigned int itsiPosition = getPosition("itsiPosition");
        unsigned int deletedPosition = getPosition("deletedPosition");
		size_t i;

        unsigned long occLocationKey = TA_Base_Core::LocationAccessFactory::getInstance().getOccLocationKey();
        std::string occLocationName = "";
        ILocation * occLocation = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(occLocationKey);
        if (occLocation != 0)
        {
            occLocationName = occLocation->getName();
        }

            
        // Now loop for each event returned
        // for (i = 0;i < patchEvents.size();i++)
        for (i = patchEvents.size() ; i>0 ; i--)
        {
            // Within each event is a list of the patches modified since the last synchronisation
            GetGroupDetailsEventReader detailsReader(*(patchEvents[i-1]));
            // const std::vector<std::vector<std::string> > details = detailsReader.getDetailsTable();
            GroupDetailsTable details;
            /* int numRows = */ detailsReader.getDetailsTable(details);

            // Loop for each "row" in the details table (each row is a single, updated, patch record)
            // for (unsigned int row = 0;row < details.size();row++)
            for (long row = details.size()-1 ; row>=0 ; row--)
            {
                // RvH 20050129 Pre-Radio-iFAT HACK
                // if the size of the row is not correct, add a blank
                // it should really only be short by one at most, due to the
                // way that the GetGroupDetailsEventReader pads blank fields
                // NOTE that we shouldn't make it a blank space, because we
                // still want to discard rows with a blank location AND name
                unsigned int numPositions=getNumPositions();
                if (details[row].size() < numPositions)
                {
					for (unsigned int j = details[row].size(); j <= numPositions; j++)
					{
						details[row].push_back("");
					}
                }

                LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
                        "Processing GroupDetailsEvent Patch subscriber TSI:%s Name:%s Deleted:%s",
                        details[row][itsiPosition].c_str(),
                        details[row][patchIdPosition].c_str(),
                        details[row][deletedPosition].c_str() );

                // If the patch has been deleted, then can simply remove the patch
                if (RadioHelper::isOTETrue(details[row][deletedPosition]))
                {
                    // Attempt to delete the patch from the database
                    try
                    {
                        TA_Base_Core::RASubscribersAccessFactory::getInstance().deletePatch(details[row][patchIdPosition]);
                    }
                    catch(...)
                    {
                        LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
                            "Error while deleting Patch with TSI: %s (could be because the subscriber was already deleted)",
                            details[row][itsiPosition].c_str());
                    }
                }
                else
                {
                    // This patch hasn't been deleted. Need to determine if it has been modified, or created.
                    IRASubscribers* patch = NULL;

                    try
                    {
                        patch = TA_Base_Core::RASubscribersAccessFactory::getInstance().
                            getSubscriberDetails(details[row][patchIdPosition]);
                    }
                    catch(TA_Base_Core::DataException&)
                    {
                        // fetching the patch from DB caused an exception, we
                        // probably need to create the record

                        LOG_EXCEPTION_CATCH(SourceInfo, "DataException","Caught non-fatal exception while attempting to load subscriber (could be because the subscriber does not yet exist)");

                        try
                        {   
                            // patches don't have locations (in the NMS), so we
                            // arbitrarily associate them with Occ. We don't
                            // display them in the SearchPage under a location
                            // anyhow.
                            // Need to work out the location key for the patch

                            patch = new TA_Base_Core::RASubscribers(details[row][patchIdPosition],
                                                                    details[row][patchIdPosition],
                                                                    TA_Base_Core::IRASubscribers::PATCH_GROUP,
                                                                    occLocationName,
                                                                    occLocationKey,
                                                                    true);

                            // And save the Patch to the database
                            patch->applyChanges();
                        }
                        catch(...)
                        {
                            // If this fails - well, that's bad. Give up on this patch and continue to the next one
                            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Non-recoverable error while attempting to create patch with owner TSI: %s",
                                                                           details[row][itsiPosition].c_str());
                            // move onto the next Patch
                            row--;
                            continue;
                        }
                    }
                    catch(...)
                    {
                        // Otherwise this Patch is a lost cause
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Non-recoverable error while attempting to load patch with owner TSI: %s",
                                                                       details[row][itsiPosition].c_str());
                        // move onto the next Patch
                        row--;
                        continue;
                    }

//                    // Now that we have an entry in the database for the patch, need to find out what patch members
//                    // have changed
//                    RadioMethodSearchSubscribers searchPatchMembersMethod(m_radio.getISCSRadioSessionRef());
//                    searchPatchMembersMethod.setSearchType(RadioMethodSearchSubscribers::K_PATCH_MEMBERS);
//                    searchPatchMembersMethod.setCriteria("TIMESTAMP>" + dbTime + " AND PARENT='"
//                                                         + details[row][patchIdPosition] +"'");
//
//                    RadioEvent* searchEvent = m_radio.sendMethod(searchPatchMembersMethod);
//
//                    SearchSubscribersEventReader searchPatchMembersReader(*searchEvent);
//                    unsigned long patchMembersSearchReference = searchPatchMembersReader.getSubscriberReferenceResultSet();
//
//                    // And use get group details to retrieve the members of the Patch
//                    RadioMethodGetGroupDetails patchMembersDetailsMethod(m_radio.getISCSRadioSessionRef(),patchMembersSearchReference);
//                    
//                    std::vector<std::string> individualFields;
//                    fields.push_back(NAME);
//                    fields.push_back(TYPE);
//                    fields.push_back(ITSI);
//                    fields.push_back(DELETED);
//                    fields.push_back(REGION);
//                    patchMembersDetailsMethod.setFields(individualFields);
//
//                    // Can't reuse the previous vector ('cause it's still in use in the outer loop of where we are now)
//                    // so need to create a new one
//                    std::vector<RadioEvent*> patchMemberEvents = m_radio.sendMethodMultipleResponse(patchMembersDetailsMethod);
//
//                    // Now loop for each response
//                    // for (unsigned int j = 0;j < patchMemberEvents.size();j++)
//                    for (long j = patchMemberEvents.size()-1 ; j>=0 ; j--)
//                    {
//                        // And retrieve the table from the response
//                        GetGroupDetailsEventReader patchDetailsReader(*(patchMemberEvents[j]));
//                        // const std::vector<std::vector<std::string> > patchMembers = patchDetailsReader.getDetailsTable();
//                        GroupDetailsTable patchMembers;
//                        /* int numRows = */ detailsReader.getDetailsTable(details);
//
//                        // for (unsigned int patchMembersRow = 0;patchMembersRow < patchMembers.size();patchMembersRow++)
//                        for (long patchMembersRow = patchMembers.size()-1 ; patchMembersRow>=0 ; patchMembersRow--)
//                        {
//                            // not bothering to check if the subscriber is valid - patch members are always current groups
//
//                            // For each group member determine if they've been deleted or not
//                            if (RadioHelper::isOTETrue(patchMembers[patchMembersRow][deletedPosition]))
//                            {
//                                // Attempt to remove this member from the group - put it in a try/catch in case there are any problems
//                                try
//                                {
//                                    // Load the group member from the database
//                                    IRASubscribers* subscriber = TA_Base_Core::RASubscribersAccessFactory::getInstance().
//                                        getSubscriberDetails(patchMembers[patchMembersRow][itsiPosition]);
//
//                                    // And remove it from the group
//                                    TA_Base_Core::RAPatchMembersAccessFactory::getInstance().deleteGroupFromPatch(*patch,
//                                        *subscriber);
//                                }
//                                catch(...)
//                                {
//                                    LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
//                                        "Error while removing member with TSI = %s from patch with owner TSI = %s. This could simply be because the subscriber was already deleted, or it could be another problem.",
//                                        patchMembers[patchMembersRow][itsiPosition].c_str(),details[row][itsiPosition].c_str());
//                                }
//                            }
//                            else // if (patchMembers[patchMembersRow][deletedPosition] == "N")
//                            {
//                                // This member has been added to the group, so load it then add it
//                                try
//                                {
//                                    IRASubscribers* subscriber = TA_Base_Core::RASubscribersAccessFactory::getInstance().
//                                        getSubscriberDetails(patchMembers[patchMembersRow][itsiPosition]);
//
//                                    TA_Base_Core::RAPatchMembersAccessFactory::getInstance().addGroupToPatch(*patch,
//                                        *subscriber);
//                                }
//                                catch(...)
//                                {
//                                    LOG_GENERIC(SourceInfo,DebugUtil::DebugError,
//                                        "Error while adding member with TSI = %s to patch with owner TSI = %s. This could simply be because the subscriber was already deleted, or it could be another problem.",
//                                        patchMembers[patchMembersRow][itsiPosition].c_str(),details[row][itsiPosition].c_str());
//                                }
//                            }
//                        }
//                        patchMembers.clear();
//                    }
                }
            }
            details.clear();
        }
        
        // Need to delete the event object associated with it
        for (i = 0;i < patchEvents.size();i++)
        {
            delete patchEvents[i];
            patchEvents[i] = NULL;
        }
        FUNCTION_EXIT;
    }

    
    void RadioDirectorySynchronisation::triggerSynchronisation()
    {
        FUNCTION_ENTRY("triggerSynchronisation");
        // synchronise in 4 or so seconds, to allow other more immediate
        // tcpserver activities to occur
		
	    m_elapsedTime = m_synchronisationFrequency - 4;
        m_doSynchronise = true;
		LOG_GENERIC( SourceInfo,DebugUtil::DebugInfo, "Will start subscriber synchronisation in 4 seconds." );

        FUNCTION_EXIT;
    }

    void RadioDirectorySynchronisation::preventSynchronisation()
    {
        FUNCTION_ENTRY("preventSynchronisation");

        m_doSynchronise = false;
		LOG_GENERIC( SourceInfo,DebugUtil::DebugInfo, "Stop subscriber synchronisation." );

        FUNCTION_EXIT;
    }

    long RadioDirectorySynchronisation::getLastSynchroniseTime()
    {
        FUNCTION_ENTRY("getLastSynchroniseTime");
        ThreadGuard synchGuard(m_synchroniseLock);
        FUNCTION_EXIT;
        return (m_entityData->getLastUpdateTime());
        // return (ACE_OS::time(0) - m_elapsedTime);
    }

    bool RadioDirectorySynchronisation::changeSearchType(RadioMethodChangeReference::ESearchType searchType) const
    {
        FUNCTION_ENTRY("changeSearchType");
        RadioMethodChangeReference changeReference(m_radio.getISCSRadioSessionRef(),m_radio.getISCSRadioSessionRef());
        changeReference.setSearchType(searchType);
        RadioEvent* event = m_radio.sendMethod(changeReference);

        // If the event comes back, need to check that the change went ahead correctly
        if (event == NULL || event->getStatus() != 0)
        {
            LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"Failed to set radio search mode to %s for synchronisation. Database update aborted.", (searchType==RadioMethodChangeReference::FULL) ? "FULL" : "NORMAL");
            FUNCTION_EXIT;
            return false;
        }

        // Delete the radio event 
        delete event;

        FUNCTION_EXIT;
        return true;
    }


    std::string RadioDirectorySynchronisation::queryReference(RadioReference queryRef, RadioMethodQueryReference::EKeys key) const
    {
	    FUNCTION_ENTRY("queryReference()");

	    // Need to perform a query reference on the operator's session reference.
	    RadioMethodQueryReference queryReferenceMethod(m_radio.getISCSRadioSessionRef());
        queryReferenceMethod.setQueryType(key);
	    queryReferenceMethod.setQueryReference(queryRef);
	    RadioEvent* reply = m_radio.sendMethod(queryReferenceMethod);

        // The reply should have the ITSI for the current session
        QueryReferenceEventReader queryReferencereader(*reply);
        std::string resultText = queryReferencereader.getDetails();

        // Delete the reply
        delete reply;
        reply = NULL;

	    FUNCTION_EXIT;
	    return resultText;
    }


}


