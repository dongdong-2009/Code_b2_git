/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source$
 * @author:  Adam Radics
 * @version: $Revision$
 *
 * Last modification: $Date$
 * Last modified by:  $Author$
 *
 */
#if !defined(IMessageSender_H)
#define IMessageSender_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/CctvTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/RadioGroupTypes.h"
#include "bus/trains/TrainCommonLibrary/src/CallTypes.h"
#include "bus/trains/TrainCommonLibrary/src/PaTypes.h"
#include "bus/trains/TrainCommonLibrary/src/PecTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TtisTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"

namespace TA_IRS_App
{

    /**
     * Sends all the messages destined for train agent clients.
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:47:41 PM
     */
    class IMessageSender
    {

    public:

        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::CctvTypes::CctvSwitchState& update ) = 0;


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::RadioGroupTypes::TrainRadioGroup& update ) = 0;


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::PaTypes::TrainLibraryInfo& update ) = 0;


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::TtisTypes::TisLibraryVersionUpdate& update ) = 0;


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::TtisTypes::TisLibraryUpdate& update ) = 0;

		/*
		@param1 trainId
		@param2 cameraId
		@param3 monitorId
		*/

		virtual void sendStateUpdate( unsigned long trainId,unsigned char cameraId, unsigned long monitorId  ) =0 ;


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::TrainInformationTypes::TrainDetails& update ) = 0;


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::TrainInformationTypes::TrainSelectionUpdate& update ) = 0;


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::TrainInformationTypes::AgentOperationMode& update ) = 0;


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::CallTypes::OccCallReset& update ) = 0;


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::CallTypes::OccCallRequest& update ) = 0;


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::PecTypes::PecUpdate& update ) = 0;


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::PaTypes::PaUpdateEvent& update ) = 0;


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::PaTypes::PaAnnouncementUpdate& update ) = 0;


        /**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::TtisTypes::TisReturnStatus& update ) = 0;

		/**
         * This sends a status update to a registered client.
         *
         * @param update    The update to send
         */
        virtual void sendClientUpdate( const TA_IRS_Bus::CallTypes::TrainChangeover& update ) = 0;

    };

} // TA_IRS_App

#endif // !defined(IMessageSender_H)
