/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodSetupCall.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//
//  RadioMethodSetupCall.h
//  Implementation of the Class RadioMethodSetupCall
//  Generated by Enterprise Architect
//  Created on:      11-Nov-2003 13:31:43
//  Original author: 
//  
///////////////////////////////////////////////////////////
//  Modification history:
//  
//
///////////////////////////////////////////////////////////


#if !defined(RadioMethodSetupCall_F8201FA1_E0E9_4596_BBAE_8255E49AA844__INCLUDED_)
#define RadioMethodSetupCall_F8201FA1_E0E9_4596_BBAE_8255E49AA844__INCLUDED_

#include "RadioMethodCallReference.h"
#include <string>
/**
* This method is used to place a call. . If call is a group call, Tx permission
* will be requested. To notify that Tx right has been granted an unsolicited
* event DemandTx will be sent.
* @version 1.0
* @generator Enterprise Architect
* @created 11-Nov-2003 13:31:43
*/


namespace TA_IRS_App
{

	class RadioMethodSetupCall : public RadioMethodCallReference
	{

		public:

			enum ECallType {SINGLE_CALL,GROUP_UNACKNOWLEDGE,GROUP_ACKNOWLEDGE,BROADCAST, CIRCUITMODEDATA};
            enum ECommunicationType
            {
                CMT_Speech = 0,
                CMT_Lo_72_0 = 0x1,
                CMT_Lo_48_1 = 0x2,
                CMT_Lo_48_4 = 0x3,
                CMT_Lo_48_8 = 0x4,
                CMT_Hi_24_1 = 0x5,
                CMT_Hi_24_4 = 0x6,
                CMT_Hi_24_8 = 0x7
            };

            enum ERequestTx
            {
                NORMAL          = 0,
                REQUEST_TX      = 1,
                NO_REQUEST_TX   = 2
            };

            enum
            {
                PriorityNotDefined = 0,
                LowPriority = 1,
                Priority1 = 1,
                Priority2 = 2,
                Priority3 = 3,
                Priority4 = 4,
                Priority5 = 5,
                Priority6 = 6,
                Priority7 = 7,
                Priority8 = 8,
                Priority9 = 9,
                Priority10 = 10,
                Priority11 = 11,
                HighPriority = 11,
                PreemptivePriority1 = 12,
                PreemptivePriority2 = 13,
                PreemptivePriority3 = 14,
                PreemptivePriority4 = 15,
                EmergencyPriority = 15,
            };
			
            /**
			* Constructor
			* @param    sessionRef
			* @param    callRef
			*/
			RadioMethodSetupCall(RadioMethod::RadioSessionReference sessionRef, 
								 RadioMethodCallReference::RadioCallReference callRef);


		public:
			void setCallType(ECallType callType);
            void setHookBit(bool on=true);

            void setFullDuplex(bool on=true);

            void setCommunicationType(ECommunicationType cmt);

			/**
             * Set the call priority
             * @param priority see the enumeration in RadioMethodSetupCall.h
             */
			void setCallPriority(int priority);
			void setCallDestination(std::string destination);

            void setRequestTx(ERequestTx rqstx);

		private:

			unsigned short m_callType;


	};
};

#endif // !defined(RadioMethodSetupCall_F8201FA1_E0E9_4596_BBAE_8255E49AA844__INCLUDED_)
