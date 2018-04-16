/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#if !defined(CallTypesConversion_H)
#define CallTypesConversion_H

#include "bus/trains/TrainAgentCorba/idl/src/ITrainStateUpdateCorba.h"
#include "bus/trains/TrainAgentCorba/idl/src/ITrainDriverCallCorba.h"
#include "bus/trains/TrainCommonLibrary/src/CallTypes.h"

namespace TA_IRS_Bus
{
    /**
     * This is a placeholder for some static methods to covert train call types
     * between corba and c++
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:42 PM
     */

    class CallTypesConversion
    {

    public:

        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::ETrainCallType convertToCorba( const CallTypes::ETrainCallType& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainDriverCallCorba::OccCallReset convertToCorba( const CallTypes::OccCallReset& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainDriverCallCorba::OccCallRequestList convertToCorba( const CallTypes::OccCallRequestList& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainDriverCallCorba::ItsiList convertToCorba( const CallTypes::ItsiList& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainDriverCallCorba::VoiceCallResponseList convertToCorba( const CallTypes::VoiceCallResponseList& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainDriverCallCorba::DriverCallResult convertToCorba( const CallTypes::DriverCallResult& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainDriverCallCorba::EVoiceCallResult convertToCorba( const CallTypes::EVoiceCallResult& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainDriverCallCorba::VoiceCallResponse convertToCorba( const CallTypes::VoiceCallResponse& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainDriverCallCorba::OccCallRequest convertToCorba( const CallTypes::OccCallRequest& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * The CORBA type is from the Radio Agent IDL.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::IRadioSessionCorbaDef::ECallType convertToCorba( const CallTypes::ECallType& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * The CORBA type is from the Radio Agent IDL.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::IRadioSessionCorbaDef::ECallPriority convertToCorba( const CallTypes::ECallPriority& cppType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static CallTypes::ETrainCallType convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::ETrainCallType& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static CallTypes::VoiceCallResponse convertToCpp( const TA_Base_Bus::ITrainDriverCallCorba::VoiceCallResponse& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static CallTypes::EVoiceCallResult convertToCpp( const TA_Base_Bus::ITrainDriverCallCorba::EVoiceCallResult& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static CallTypes::DriverCallResult convertToCpp( const TA_Base_Bus::ITrainDriverCallCorba::DriverCallResult& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static CallTypes::VoiceCallResponseList convertToCpp( const TA_Base_Bus::ITrainDriverCallCorba::VoiceCallResponseList& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static CallTypes::ItsiList convertToCpp( const TA_Base_Bus::ITrainDriverCallCorba::ItsiList& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static CallTypes::OccCallRequestList convertToCpp( const TA_Base_Bus::ITrainDriverCallCorba::OccCallRequestList& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static CallTypes::OccCallRequest convertToCpp( const TA_Base_Bus::ITrainDriverCallCorba::OccCallRequest& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static CallTypes::OccCallReset convertToCpp( const TA_Base_Bus::ITrainDriverCallCorba::OccCallReset& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * The CORBA type is from the Radio Agent IDL.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static CallTypes::ECallType convertToCpp( const TA_Base_Bus::IRadioSessionCorbaDef::ECallType& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * The CORBA type is from the Radio Agent IDL.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static CallTypes::ECallPriority convertToCpp( const TA_Base_Bus::IRadioSessionCorbaDef::ECallPriority& CorbaType );

    private:
        
        CallTypesConversion();

        ~CallTypesConversion();

    };

}

#endif // !defined(CallTypesConversion_H)
