/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_log_viewer/src/DisplayRequestSender.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This is used to resubmit display requests.
  * All error messages are handled here.
  */

#if !defined(AFX_DisplayRequestSender_H__B1FC56F1_91F3_4680_9193_F5B9A34FE350__INCLUDED_)
#define AFX_DisplayRequestSender_H__B1FC56F1_91F3_4680_9193_F5B9A34FE350__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/tis_agent/src/IDisplayRequest.h"
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/message/src/BackwardsCompatibility.h"


namespace TA_IRS_App
{
    class DisplayRequestCondition;
}

namespace TA_IRS_App
{

    class DisplayRequestSender : public TA_Base_Core::SpecialisedMessageSubscriber< TA_Base_Core::CommsMessageCorbaDef >
    {

    public:

        /**
         * Constructor
         */
        DisplayRequestSender( unsigned long locationKey );

        
        /**
         * Destructor
         */
        virtual ~DisplayRequestSender();


        /**
         * sendRequest
         *
         * sends the request, displaying any errors encountered.
         *
         * @param request the request to resubmit. not null
         */
        void sendRequest( TA_IRS_Core::IDisplayRequest* request );


        /**
         * This will receive display request result messsages
         *
         * @param payload    The incoming message
         */
        virtual void receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload );


    private:

        /**
         * Private default constructor
         */
        DisplayRequestSender();

        
        /**
         * Subscribes to TTIS display request updates from the local TIS agent
         */
        void subscribeToMessages();


        /**
         * This will add a display request to the internal list of pending requests
         */
        void addPendingDisplayRequest( DisplayRequestCondition* requestCondition );


        /**
         * This will remove a display request from the internal list of pending requests
         */
        void removeCompletedDisplayRequest( DisplayRequestCondition* requestCondition );

        
        /**
         * resubmitTtisDisplayRequest
         *
         * resubmits the TTIS display request
         *
         * @param request the request to resubmit. Asserts if null
         */
        void resubmitTtisDisplayRequest( TA_IRS_Core::IDisplayRequest* request );


        /**
         * resubmitStisDisplayRequest
         *
         * resubmits the STIS display request
         *
         * @param request the request to resubmit. Asserts if null
         */
        void resubmitStisDisplayRequest( TA_IRS_Core::IDisplayRequest* request );


        /**
         * submitTtisPreDefinedDisplayRequest
         *
         * resubmits a ttis pre defined display request
         *
         * @param request the request. Asserts if null
         */
        void submitTtisPreDefinedDisplayRequest( TA_IRS_Core::IDisplayRequest* request );


        /**
         * submitTtisAdHocDisplayRequest
         *
         * resubmits a ttis adhoc display request
         *
         * @param request the request. Asserts if null
         */
        void submitTtisAdHocDisplayRequest( TA_IRS_Core::IDisplayRequest* request );


        /**
         * submitStisPreDefinedDisplayRequest
         *
         * resubmits an stis pre defined display request
         *
         * @param request the request. Asserts if null
         */
        void submitStisPreDefinedDisplayRequest( TA_IRS_Core::IDisplayRequest* request );


        /**
         * submitStisAdHocDisplayRequest
         *
         * resubmits an ttis ad-hoc display request
         *
         * @param request the request. Asserts if null
         */
        void submitStisAdHocDisplayRequest( TA_IRS_Core::IDisplayRequest* request );


        /**
         * submitStisRatisApprovalRequest
         *
         * resubmits as stis ratis approval request
         *
         * @param request the request. Asserts if null
         */
        void submitStisRatisApprovalRequest( TA_IRS_Core::IDisplayRequest* request );


        /**
         * getTtisPidList
         *
         * given a string build the vector of PID's
         *
         * @param destinationPidList the space delimited list of pid numbers
         *
         * @return the vector of PIDs
         */
        std::vector<TA_Base_Core::EPIDSelection> getTtisPidList( const std::string& destinationPidList ) const;


        /**
         * getStisDestination
         *
         * build the stis destination given the details
         *
         * @param station the station
         * @param levels space delimited list of levels
         * @param pids space delimited list of pids
         *
         * @return the corba structure
         *
         */
        TA_Base_Core::STISDestinationList getStisDestination( const std::string& station,
                                                              const std::string& levels,
                                                              const std::string& pids) const;

        /**
         * The GUI location key
         */
        unsigned long m_locationKey;


        /**
         * The list of outstanding requests waiting for responses
         */
        typedef std::vector< DisplayRequestCondition* > DisplayRequests;
        DisplayRequests m_displayRequests;

        /**
         * Lock to protect m_displayRequests
         */
        TA_Base_Core::ReEntrantThreadLockable m_displayRequestsLock;

    };


} // TA_IRS_App

#endif // !defined
