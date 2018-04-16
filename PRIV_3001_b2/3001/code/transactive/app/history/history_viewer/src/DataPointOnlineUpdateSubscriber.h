#if !defined(DATAPOINT_ONLINE_UPDATE_SUBSCRIBER_H)
#define DATAPOINT_ONLINE_UPDATE_SUBSCRIBER_H

/**
  * DataPointOnlineUpdateSubscriber.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/history/history_viewer/src/DataPointOnlineUpdateSubscriber.h $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class listens for datapoint online updates.
  *
  */

#include <string>
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h"
#include "core/threads/src/thread.h"


using TA_Core::ConfigUpdateDetails;


namespace TA_App
{
	//
	// Forward declaration
	//
    class CHistoryViewerDlg;
    class DataPointInterestRegisterThread;

    //
    // DataPointOnlineUpdateSubscriber class
    //
    class DataPointOnlineUpdateSubscriber :  public TA_Core::IOnlineUpdatable
    {

    public:

		/**
		  * DataPointOnlineUpdateSubscriber
		  * 
		  * Constructor.
		  *
		  */
        DataPointOnlineUpdateSubscriber(CHistoryViewerDlg* parent);

		/**
		  * ~DataPointOnlineUpdateSubscriber
		  * 
		  * Destructor.
		  *
		  */
        virtual ~DataPointOnlineUpdateSubscriber();

        /**
		  * receiveSpecialisedMessage
		  * 
		  * Receives and processes the message.
		  *
		  * @param message The message
		  *
		  */
        virtual void processUpdate(const ConfigUpdateDetails& updateEvent);

        /**
		  * addInterestedKey
		  * 
		  * Register a new key.
		  *
		  * @param entitykey Pkey of new datapoint
		  *
		  */
        void addInterestedKey(unsigned int entitykey);

        /**
		  * removeInterestedKey
		  * 
		  * Deregister a key.
		  *
		  * @param entitykey Pkey of datapoint to remove
		  *
		  */
        void removeInterestedKey(unsigned int entitykey);

    private:

        DataPointOnlineUpdateSubscriber();
        DataPointOnlineUpdateSubscriber( const DataPointOnlineUpdateSubscriber& theSubscriber);
	    DataPointOnlineUpdateSubscriber& operator=(const DataPointOnlineUpdateSubscriber &);

        void handleUpdateEvent(const ConfigUpdateDetails& updateEvent);
        void handleCreateEvent(const ConfigUpdateDetails& updateEvent);
        void handleDeleteEvent(const ConfigUpdateDetails& updateEvent);

    private:

        CHistoryViewerDlg* m_parentProcess;
        DataPointInterestRegisterThread* m_interestRegisterThread;
    };

    //
    // DataPointInterestRegisterThread class
    //
    class DataPointInterestRegisterThread : public TA_Core::Thread
    {
    public:
        DataPointInterestRegisterThread(DataPointOnlineUpdateSubscriber* subscriber) 
            : m_parentProcess(subscriber), m_terminated(false) {}
        virtual ~DataPointInterestRegisterThread() {}

        virtual void run();
        virtual void terminate();

    private:
        DataPointInterestRegisterThread();
        DataPointInterestRegisterThread( const DataPointInterestRegisterThread& theThread);
	    DataPointInterestRegisterThread& operator=(const DataPointInterestRegisterThread &);

        DataPointOnlineUpdateSubscriber* m_parentProcess;
        bool m_terminated;
    };

} // TA_App


#endif // !defined(DATAPOINT_ONLINE_UPDATE_SUBSCRIBER_H)



