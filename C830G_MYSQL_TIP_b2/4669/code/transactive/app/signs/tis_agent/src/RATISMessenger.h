#ifndef RATISMessenger_H_INCLUDED
#define RATISMessenger_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/RATISMessenger.h $
  * @author:   San Teo
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class does nothing other than keeping a list of vetting response that is to be sent to
  * the STIS server, regularly check if it has something in the list and getting the STISManager to 
  * send the vetting response.  The intention of this class it to allow the vetting response to be run on 
  * a separate thread so that the client's call to STISManager's submitRATISVettingResponse could return 
  * before it timed out in a lengthy operation like clearing the train messages.  
  *
  */

#include <string>
#include <queue>
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_IRS_App
{

	class RATISMessenger : public TA_Base_Core::Thread
	{

	public:

		/**
		  * constructor
		  *
		  * @return     nothing.
		  *
		  */
		RATISMessenger();

        
        /**
		  * RATISMessenger
		  *
		  * Destructor
		  *
		  * @return     none.
		  *
		  */
		virtual ~RATISMessenger();


        /** 
          * run
          *
          * This simply keeps looping and checks if it has something in the queue.  If it does, it
          * gets the STISManager to send these messages.
          *
          */
		virtual void run();


        /** 
          * terminate
          *
          * Tells the run method to stop looping once it finished its current operation.
          *
          */
		virtual void terminate();


        /** 
          * addRATISVettingResponse
          *
          * Add the RATIS vetting response to the queue to be processed
          *
          * @param key The id of the incoming message
          * @param approved Set this to true if the message is approved
          * @param priority If the message is overridable AND the operator approved the message AND this is 
          *        set to a non-zero value, then this value will be used in the submission.
          * @param content If the message is overridable AND the operator approved the message AND this is not 
          *        a clear message AND this is set to a non-empty string, then this value will be used in the submission.
          * @param sessionId This is required by the agent to determine whether the session has rights to perform this action
          *
          */
        void addRATISVettingResponse(const long key, const bool approved, const short priority, const char* content, const char* sessionID );



	private:

        /** 
          * setIsTerminating
          *
          * Flag the run method to stop looping.
          *
          */
        void setIsTerminating();


        /** 
          * isTerminating
          *
          * Returns true if the terminate() method has been called.
          *
          *
          * @return true if the terminate() method has been called.
          */
        bool isTerminating();


        struct VettingRATISResponse
        {
            long        key;            // The RATIS message key
            bool        approved;       // True if approved, false if rejected
            short       priority;       // Priority of the RATIS message
            std::string content;        // The string content
            std::string sessionID;      // The sessionID
        };

        // This stores the RATIS vetting response that is yet to be sent to the STIS server
        std::queue<VettingRATISResponse> m_queue;

        // True if terminating
        bool m_isTerminating;

        // Protects the queue
		static TA_Base_Core::ReEntrantThreadLockable m_queueLock;

        // Protects the terminate flag
		static TA_Base_Core::ReEntrantThreadLockable m_terminateLock;

        static const int POLL_INTERVAL;

	};

}

#endif // RATISMessenger_H_INCLUDED
