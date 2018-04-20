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

#if !defined(PecTypesConversion_H)
#define PecTypesConversion_H

#include "bus/trains/TrainCommonLibrary/src/PecTypes.h"
#include "bus/trains/TrainAgentCorba/idl/src/ITrainPecCorba.h"

namespace TA_IRS_Bus
{
    /**
     * This is a placeholder for some static methods to covert train pec types between
     * corba and c++
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:48 PM
     */

    class PecTypesConversion
    {

    public:

        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainPecCorba::PecUpdateList convertToCorba( const PecTypes::PecUpdateList& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainPecCorba::PecPointCollection convertToCorba( const PecTypes::ActivePecPointList& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainExceptionsCorba::EPecCommandResult convertToCorba( const ProtocolPecTypes::EPecCommandResult& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainPecCorba::PecPoint convertToCorba( const PecTypes::PecPoint& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainPecCorba::PecUpdate convertToCorba( const PecTypes::PecUpdate& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainPecCorba::EPecUpdateType convertToCorba( const PecTypes::EPecUpdateType& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainPecCorba::EPecCarNumber convertToCorba( const ProtocolPecTypes::EPecCarNumber& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainPecCorba::EPecNumber convertToCorba( const ProtocolPecTypes::EPecNumber& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainPecCorba::EPecSource convertToCorba( const PecTypes::EPecSource& cppType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static PecTypes::ActivePecPointList convertToCpp( const TA_Base_Bus::ITrainPecCorba::PecPointCollection& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static PecTypes::EPecSource convertToCpp( const TA_Base_Bus::ITrainPecCorba::EPecSource& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static ProtocolPecTypes::EPecNumber convertToCpp( const TA_Base_Bus::ITrainPecCorba::EPecNumber& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static ProtocolPecTypes::EPecCarNumber convertToCpp( const TA_Base_Bus::ITrainPecCorba::EPecCarNumber& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static PecTypes::EPecUpdateType convertToCpp( const TA_Base_Bus::ITrainPecCorba::EPecUpdateType& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static PecTypes::PecUpdate convertToCpp( const TA_Base_Bus::ITrainPecCorba::PecUpdate& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static PecTypes::PecPoint convertToCpp( const TA_Base_Bus::ITrainPecCorba::PecPoint& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static PecTypes::PecUpdateList convertToCpp( const TA_Base_Bus::ITrainPecCorba::PecUpdateList& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static ProtocolPecTypes::EPecCommandResult convertToCpp( const TA_Base_Bus::ITrainExceptionsCorba::EPecCommandResult& CorbaType );

    private:

        PecTypesConversion();

        ~PecTypesConversion();

    };

}

#endif // !defined(PecTypesConversion_H)
