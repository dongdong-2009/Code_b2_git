#ifndef STISADHOCDISPLAYREQUEST_H_INCLUDED
#define STISADHOCDISPLAYREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISAdHocDisplayRequest.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/STISMessage.h"

// IDL compiler-produced header file
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"

namespace TA_IRS_App
{

	class STISAdHocDisplayRequest : public STISMessage
	{
	public:

		STISAdHocDisplayRequest();

		~STISAdHocDisplayRequest();

		//hongran++ TD17502
		virtual void log(bool sendSuccess);
		//++hongran TD17502

		virtual std::vector<unsigned char> createProtocolMessage();

        void setData( const TA_Base_Core::STISDestination& destination, const std::string& messageContent, const std::string& startTime,
			const std::string& endTime, unsigned short priority, const TA_Base_Core::DisplayAttributes& displayAttributes,
			const TA_Base_Core::PlasmaAttributes& plasmaAttributes, const TA_Base_Core::LEDAttributes& ledAttributes, const std::string& operatorName  );



	private:

		/**
		  * displayAttributesToString
		  *
		  * Converts the display requests data members to a string
		  * to be added to the protocol message
		  *
		  * @return     std::string
		  *             The string containing the display attributes
		  */
		std::string getDisplayAttributesAsString();

		/**
		  * plasmaFontToString
		  *
		  * Converts the plasma font related data members to a string
		  * to be added to the protocol message
		  *
		  * @return     std::string
		  *             The string containing the plasma font attributes
		  */
		std::string getPlasmaFontAttributesAsString();

		/**
		  * ledFontToString
		  *
		  * Converts the led font related data members to a string
		  * to be added to the protocol message
		  *
		  * @return     std::string
		  *             The string containing the led attributes
		  */
		std::string getLedFontAttributesAsString();

		// The actual ad-hoc message content
		std::string m_message;

		// Where the message is to be displayed
        TA_Base_Core::STISDestination m_destination;

		// Unused at present
		int m_tag;

		std::string m_startTime;

		std::string m_endTime;

		unsigned short m_priority;

        TA_Base_Core::DisplayAttributes m_displayAttributes;

        TA_Base_Core::PlasmaAttributes m_plasmaAttributes;

        TA_Base_Core::LEDAttributes m_LEDAttributes;

		std::string m_submittedBy;

	};

}

#endif
