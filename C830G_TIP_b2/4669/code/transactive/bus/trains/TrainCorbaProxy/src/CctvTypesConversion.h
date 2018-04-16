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

#if !defined(CctvTypesConversion_H)
#define CctvTypesConversion_H

#include "bus/trains/TrainAgentCorba/idl/src/ITrainExceptionsCorba.h"
#include "bus/trains/TrainAgentCorba/idl/src/ITrainStateUpdateCorba.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCctvTypes.h"
#include "bus/trains/TrainCommonLibrary/src/CctvTypes.h"

namespace TA_IRS_Bus
{
    /**
     * This is a placeholder for some static methods to covert train cctv types
     * between corba and c++
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:43 PM
     */

    class CctvTypesConversion
    {

    public:

        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainExceptionsCorba::ECctvCommandStatus convertToCorba( const ProtocolCctvTypes::ECctvCommandStatus& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainCctvCorba::ECctvSequenceNumber convertToCorba( const ProtocolCctvTypes::ECctvSequenceNumber& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainCctvCorba::ECctvSequenceMode convertToCorba( const ProtocolCctvTypes::ECctvSequenceMode& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainCctvCorba::ECctvSwitchMode convertToCorba( const CctvTypes::ECctvSwitchMode& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainCctvCorba::CctvSwitchState convertToCorba( const CctvTypes::CctvSwitchState& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainCctvCorba::ECctvCamera convertToCorba( const ProtocolCctvTypes::ECctvCamera& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainCctvCorba::TrainCameraSequence convertToCorba( const CctvTypes::TrainCameraSequence& cppType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static ProtocolCctvTypes::ECctvCommandStatus convertToCpp( const TA_Base_Bus::ITrainExceptionsCorba::ECctvCommandStatus& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static ProtocolCctvTypes::ECctvSequenceNumber convertToCpp( const TA_Base_Bus::ITrainCctvCorba::ECctvSequenceNumber& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static ProtocolCctvTypes::ECctvSequenceMode convertToCpp( const TA_Base_Bus::ITrainCctvCorba::ECctvSequenceMode& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static CctvTypes::ECctvSwitchMode convertToCpp( const TA_Base_Bus::ITrainCctvCorba::ECctvSwitchMode& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static CctvTypes::CctvSwitchState convertToCpp( const TA_Base_Bus::ITrainCctvCorba::CctvSwitchState& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static ProtocolCctvTypes::ECctvCamera convertToCpp( const TA_Base_Bus::ITrainCctvCorba::ECctvCamera& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static CctvTypes::TrainCameraSequence convertToCpp( const TA_Base_Bus::ITrainCctvCorba::TrainCameraSequence& CorbaType );

    private:

        CctvTypesConversion();

        ~CctvTypesConversion();

    };

}

#endif // !defined(CctvTypesConversion_H)
