/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/PRIV_3001/TA_BASE/transactive/app/security/authentication_agent/src/QueueStateUpdater.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 *
 * Sends state updates to allow the queue on the monitor mode agent to be
 * kept in sync.
 */


#if !defined(QUEUESTATEUPDATER_H_INCLUDED)
#define QUEUESTATEUPDATER_H_INCLUDED

#include "core/data_access_interface/src/ISession.h"
#include "core/message/src/StateUpdateMessageSender.h"

namespace TA_Base_App
{

    class QueueStateUpdater
    {

    public:  
        
	    QueueStateUpdater(unsigned long entityKey);
		
        /**
         * sendStateUpdateMessage
         *
         * Sends a state update message for the addtion or removal of the specified queue item
         *
         * @param item The item to place in the stateupdate message
         * @param isNew True if the item was added to the queue, false if it was removed from the queue
         */
        void sendStateUpdateMessage(TA_Base_Core::ISession* item, bool isNew);

    private:
        /**
         * Copy constructor
         *
         * Hidden to prevent accidental use
         */
	    QueueStateUpdater( const QueueStateUpdater& theSessionQueueProcessor);

        /**
         * operator=
         *
         * Hidden to prevent accidental use
         */
        QueueStateUpdater& operator=(const QueueStateUpdater& rhs);

		std::auto_ptr<TA_Base_Core::StateUpdateMessageSender> m_sender;
		unsigned long m_entityKey;
    };
}

#endif // !defined(QUEUESTATEUPDATER_H_INCLUDED)
