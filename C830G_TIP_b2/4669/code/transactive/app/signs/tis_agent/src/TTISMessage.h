#ifndef TTISMESSAGE_H_INCLUDED
#define TTISMESSAGE_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/TTISMessage.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include <string>
#include <vector>

//#include "app/signs/tis_agent/src/TTISManager.h"


namespace TA_IRS_App
{

	class TTISMessage
	{


	public:


		/**
		  * constructor
		  *
		  * @return     nothing.
		  *
		  */
		TTISMessage();

		/**
		  * STISMessage
		  *
		  * Destructor
		  *
		  * @return     none.
		  *
		  */
		virtual ~TTISMessage();


		// Other methods

		virtual void log(bool sendSuccess) = 0;

		virtual void setData();

        std::string getTimestamp();

	protected:

		// indicates the message type
		int m_messageNumber;

		// train identifier
		int m_trainNumber;

		int m_origin;
		
		bool m_dataIsSet;

		std::string m_timestamp;

		// Only create one new timestamp at any point in time
		TA_Base_Core::ReEntrantThreadLockable m_timestampLock;


	private:

		std::string getNewTimestampString();

	};

}

#endif // STATUSMONITOR_H_INCLUDED