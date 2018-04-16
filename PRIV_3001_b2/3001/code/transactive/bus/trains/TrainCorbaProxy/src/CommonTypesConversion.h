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

#if !defined(CommonTypesConversion_H)
#define CommonTypesConversion_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCommonTypes.h"
#include "bus/trains/TrainAgentCorba/idl/src/ITrainCommonCorba.h"
#include "bus/trains/TrainAgentCorba/idl/src/ITrainExceptionsCorba.h"

namespace TA_IRS_Bus
{
    /**
     * This is a placeholder for some static methods to covert train comon types
     * between corba and c++
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:43 PM
     */

    class CommonTypesConversion
    {

    public:

        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         *
         *
         * @return the CORBA representation of the C++ type
         *
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainCommonCorba::TrainId convertToCorba( const CommonTypes::TrainIdType& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         *
         *
         * @return the CORBA representation of the C++ type
         *
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainCommonCorba::TrainList convertToCorba( const CommonTypes::TrainIdList& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         *
         *
         * @return the CORBA representation of the C++ type
         *
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainExceptionsCorba::ERadioErrorType convertToCorba( const CommonTypes::ERadioErrorType& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         *
         *
         * @return the CORBA representation of the C++ type
         *
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainExceptionsCorba::EBadCommandError convertToCorba( const ProtocolCommonTypes::EBadCommandError& cppType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         *
         *
         * @return the C++ representation of the CORBA type
         *
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static CommonTypes::TrainIdType convertToCpp( const TA_Base_Bus::ITrainCommonCorba::TrainId& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         *
         *
         * @return the C++ representation of the CORBA type
         *
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static CommonTypes::TrainIdList convertToCpp( const TA_Base_Bus::ITrainCommonCorba::TrainList& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         *
         *
         * @return the C++ representation of the CORBA type
         *
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static CommonTypes::ERadioErrorType convertToCpp( const TA_Base_Bus::ITrainExceptionsCorba::ERadioErrorType& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         *
         *
         * @return the C++ representation of the CORBA type
         *
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static ProtocolCommonTypes::EBadCommandError convertToCpp( const TA_Base_Bus::ITrainExceptionsCorba::EBadCommandError& CorbaType );


    private:

        CommonTypesConversion();

        ~CommonTypesConversion();

    };

}

#endif // !defined(CommonTypesConversion_H)
