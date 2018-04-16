#ifndef TTISADHOCDISPLAYREQUEST_H_INCLUDED
#define TTISADHOCDISPLAYREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/TTISAdHocDisplayRequest.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/TTISMessage.h"

// IDL compiler-produced header file
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"

namespace TA_IRS_App
{

	class TTISAdHocDisplayRequest : public TTISMessage
	{
	public:

		TTISAdHocDisplayRequest();

		~TTISAdHocDisplayRequest();

		virtual void log(bool sendSuccess);

        virtual void setData( CORBA::UShort trainNumber,
                              const TA_Base_Core::TrainPIDList& pidList,
                              const char* messageContent,
                              TA_Base_Core::ETTISLEDFontSize fontSize,
                              TA_Base_Core::EJustification justification,
                              TA_Base_Core::ETTISLEDIntensity intensity, 
                              TA_Base_Core::EDisplayMode displayMode,
                              CORBA::UShort priority,
                              const char* startTime,
                              const char* endTime,
                              CORBA::UShort repeatInterval,
                              const std::string& operatorName );

      

	private:

		// List of PIDs this display request is intended for.
        std::vector<TA_Base_Core::EPIDSelection> m_pidSelectionList;

        TA_Base_Core::ETTISLEDFontSize m_fontSize;

		std::string m_messageText;

        TA_Base_Core::EJustification m_justification;

        TA_Base_Core::ETTISLEDIntensity m_intensity;

        TA_Base_Core::EDisplayMode m_displayMode;

		int m_priority;

		std::string m_startTime;

		std::string m_endTime;

		int m_repeatInterval;

		std::string m_submittedBy;

	};

}

#endif
