/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/trains/TrainBorne_PA/src/GenericStatusModel.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  * GenericStatusModel controls the shared business logic for broadcast status dialog screens
  */

#if !defined(GenericStatusModel_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
#define GenericStatusModel_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_

#include "app/trains/TrainBorne_PA/src/PAProgressReportSubscriber.h"
#include "app/trains/TrainBorne_PA/src/GenericStatusPage.h"
#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#include "app/trains/TrainBorne_PA/src/GenericBroadcastModel.h"


class GenericStatusModel
{
friend class UnitTestAccessBridge;

public:

    enum ERetryMode
    {
        ALL,    // Retry on all
        FAILED  // Retry only on failed zones
    };

    enum EModelSubType
    {
        STATION,
        TRAIN
    };

	enum ERightsCheckingMode
	{
		USE_RIGHTS_CHECKING,
		IGNORE_RIGHTS_CHECKING
	};

    /**
     * GenericStatusModel (constructor)
     *
     * @param page the view associated with this model
     *
     * @param broadcastId the initial broadcast ID of this model
     *
     * @param subType the type of status model (station or train)
     *
     */
    GenericStatusModel(GenericStatusPage& page, 
                       const std::string& broadcastId,
                       const EModelSubType& subType);
   
    virtual ~GenericStatusModel();
     	
    /**
     * processProgressUpdate
     *
     * Called by the view associated with this model, when it receives a notification
     * that there is new progress report data available
     * This function will collect the data and process it as required
     *
     * @return new state of the broadcast, as contained in progress report
     *
     * @exception ValueNotSetException if there is actually no data waiting to be collected
     *
     */
    TA_Base_Bus::IPAAgentCorbaDef::EBroadcastState processProgressUpdate();

    /**
     * retryFailedBroadcast
     *
     * Retrys the broadcast as defined by our locally stored broadcast ID for only those
     * areas that currently have a failed state (either overriden, or generally failed etc)
     *
     * @exception BroadcastIdNotFound local copy of broadcast ID must be out of date
     *
     * @exception BadParameterException there were unexpected (malformed) input parameters
     *
     * @exception AccessDeniedException (from PA Agent - insufficient rights)
     *
     */
    void    retryFailedBroadcast();

    /**
     * retryAllBroadcast
     *
     * Retrys the broadcast as defined by our locally stored broadcast ID
     *
     * @exception BroadcastIdNotFound local copy of broadcast ID must be out of date
     *
     * @exception BadParameterException there were unexpected (malformed) input parameters
     *
     * @exception AccessDeniedException (from PA Agent - insufficient rights)
     *
     */
    void    retryAllBroadcast();
   
    /**
     * retryBroadcast
     *
     * Retrys the broadcast using the mode to identify which areas should and shouldn't
     *  be retried
     *
     * @exception BroadcastIdNotFound local copy of broadcast ID must be out of date
     *
     * @exception BadParameterException there were unexpected (malformed) input parameters
     *
     * @exception AccessDeniedException (from PA Agent - insufficient rights)
     *
     */
    void    retryBroadcast(const GenericStatusModel::ERetryMode& mode);


    /**
     * forceProgressReportUpdate
     * 
     * Manually forces progress report to be updated within our subscriber listener - this is
     * important for functions that need to access the progress report for various tasks
     * Should also call on startup of a dialog (page), to ensure initial refresh of dialog information
     *
     * When pShowErrorMessageDialog is true, a dialog when there is an error.
     * Otherwise this function throws exception.
     */
    // AZ++ TD10377 -- added parameter
    void    forceProgressReportUpdate(bool pShowErrorMessageDialog);
    // ++AZ TD10377


    /**
     * getBroadcastConfig
     *
     * Retrieves the current broadcast configuration from the PA Agent - all exceptions passed back
     *
     * @exception BroadcastIdNotFound if input broadcast ID invalid
     *
     * @exception ... if the CORBA call times out - will get a CORBA exception
     *
     */
    std::auto_ptr<TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig>   getBroadcastConfig();


    /**
     * getBroadcastId
     * 
     * Returns current broadcast ID contained by this status model
     *
     */
    const std::string getBroadcastId() const { return m_broadcastId; }

    /**
     * terminateBroadcast
     *
     * teminates the broadcast as defined by our locally stored broadcast ID     
     *
     * @exception BroadcastIdNotFound local copy of broadcast ID must be out of date
     *
     * @exception AccessDeniedException (from PA Agent - insufficient rights)
     *
     */
    void    terminateBroadcast();

    /**
     * terminateAndRemoveBroadcast
     *
     * teminates, then removes the broadcast as defined by our locally stored broadcast ID 
     *
     * @exception BroadcastIdNotFound local copy of broadcast ID must be out of date
     *  
     * @param origin defines the creation category (origin) of this broadcast
     *
     * @return true if the PA Agent returned a successful return code
     *
     */
    bool    terminateAndRemoveBroadcast(EBroadcastOrigin bcc);


    /**
     * formatDateIntoString
     *
     * Formats the input value into an appropriate date string
     *
     * @param time time (in seconds) that can be passed into a CTime
     *              CTime constructor to create a date
     *
     * @return date as string representation
     */
    static CString formatDateIntoString(time_t time);

    /**
     * formatTimeOfDayIntoString
     *
     * Formats the input values into an appropriate hour:minute string
     *
     * @param hour in 24 hour time
     *
     * @param min
     *
     * @return date as string representation
     *
     */
    static CString formatTimeOfDayIntoString(int hour, int min);


    /**
     * isStationModel
     *
     * @return true if this generic status model is specifically a base class for a station status model
     *
     */
    bool isStationModel() { return STATION == m_modelSubType; }

    /**
     * isTrainModel
     *
     * @return true if this generic status model is specifically a base class for a train status model
     *
     */
    bool isTrainModel() { return TRAIN == m_modelSubType; }

    /**
     * getStatusString
     *     
     * @param status the current status (same status object for trains and zones)
     *
     * @return string representation of this status
     *
     */
    static const std::string getStatusString(const TA_Base_Bus::IPAAgentCorbaDef::ELocalState& status);

protected:


    /**
     * setBroadcastId
     * 
     * Sets a new broadcast ID for this model
     *
     */
    void setBroadcastId(const std::string& str) { m_broadcastId = str; }
        
    /**
     * refreshSubscription
     *
     * re-subscribes using the current broadcast ID (because the subscription
     * is filtered by the broadcast ID, when it changes we need to refresh..)
     *
     */
    void    refreshSubscription();
   
    /**
     * processProgressUpdateData
     *
     * see processProgressUpdate()
     *
     * Additionally this function can be extended by derived classes, typically
     *  they should call this function (in addition to their own implementation)
     *
     * @param lockedData the data that is currently locked, that is to be processed
     *
     */
    virtual void processProgressUpdateData(const TA_Base_Bus::IPAAgentCorbaDef::BroadcastProgressUpdateData& lockedData);

    /**
     * getHasOverrideOption [Train specific]
     *
     * @pre isTrainModel() is true [only train models should call this function]
     *
     * Function must be overriden as applicable, otherwise it will assert
     *
     * @return true if the override button is set for retry mode (present in train status dialogs only)
     *
     */
    virtual bool getHasOverrideOption();

	/*
     * getDvaBroadcastZones
     *
     * Returns all the 'visible' broadcast zones - only the Dva broadcast zones
     * should be made visible to the user (live & music are hidden from the PA Manager GUI)
     * The returned array is context sensitive - that is, the stations will not receive
     * the OCC broadcast zones, and vice-versa.  Also any broadcast IDs that the
     * operator doesn't have the privilege to view (necessary rights) will not be
     * returned
     * 
     * @param paAgent the reference to the agent used to perform request
     *
     * @param filter the type of broadcast zones to retrieve (ie train or station)
     *
     * @param filterByRightToView a flag defining whether to filter out all broadcast
     *              zones that we don't have a right to view.  Can also set to retrieve
     *              all broadcast zones, regardless of whether we have viewing rights or not.
     *
     * @return an array of all the zone names that match the filter criteria
     *
     * @exception ObjectResolutionException if there is an error retrieving the PA Agent
     *
     */
    static std::vector<std::string> getDvaBroadcastZones(PAAgentNamedObject& paAgent,
                                                         const TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType& filter,
                                                         ERightsCheckingMode filterByRightToView = USE_RIGHTS_CHECKING);

	    /**
     * terminateAndRemoveBroadcastIds
     *
     * Terminates and removes all the broadcastIds specified in array
     *
     * @param broadcastIds array of broadcast Ids we want to have removed
     *
     * @return number of broadcasts that couldn't be removed, because they couldn't
     *          be terminated properly (ie another terminal activating them)
     *
     * @exception ObjectResolutionNotSet if there is a CORBA resolving conflict (comm error)
     *
     * @param origin defines the creation category (origin) of this broadcast - if available
     *
     */
    static int terminateAndRemoveBroadcastIds(const std::vector<std::string>& broadcastIds,
                                                PAAgentNamedObject& paAgent,
                                                EBroadcastOrigin origin = BO_UNKNOWN);

private:

    // Copy constructor not used so made private
	GenericStatusModel( const GenericStatusModel&);
    // Assignment not used so it is made private
	GenericStatusModel& operator=(const GenericStatusModel&);

    // Subscribes us to updated progress report data
    PaProgressReportSubscriber      m_progressReportSubscriber;

    /// Interface to the page view object we affect
    GenericStatusPage&              m_pageView;

    // The string defining our broadcast ID (aka 'status name')
    std::string m_broadcastId;

    EModelSubType   m_modelSubType;
};
#endif // !defined(GenericStatusModel_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
