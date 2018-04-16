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

#if !defined(TrainInformationConversion_H)
#define TrainInformationConversion_H


#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"
#include "bus/trains/TrainAgentCorba/idl/src/ITrainTisCorba.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolTtisTypes.h"
#include "bus/trains/TrainAgentCorba/idl/src/ITrainInformationCorba.h"

namespace TA_IRS_Bus
{
    /**
     * This is a placeholder for some static methods to covert train information types
     * between corba and c++
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:55 PM
     */

    class TrainInformationConversion
    {

    public:

        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainInformationCorba::TrainSelectionUpdate convertToCorba( const TrainInformationTypes::TrainSelectionUpdate& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainInformationCorba::EChangeType convertToCorba( const TrainInformationTypes::EChangeType& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainInformationCorba::TrainSelectionList convertToCorba( const TrainInformationTypes::TrainSelectionList& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainInformationCorba::AgentOperationMode convertToCorba( const TrainInformationTypes::AgentOperationMode& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainInformationCorba::EAtsTrack convertToCorba( const TrainInformationTypes::EAtsTrack& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainInformationCorba::TrainDetails convertToCorba( const TrainInformationTypes::TrainDetails& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainInformationCorba::TrainDetailsList convertToCorba( const TrainInformationTypes::TrainDetailsList& cppType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TrainInformationTypes::TrainSelectionUpdate convertToCpp( const TA_Base_Bus::ITrainInformationCorba::TrainSelectionUpdate& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TrainInformationTypes::EChangeType convertToCpp( const TA_Base_Bus::ITrainInformationCorba::EChangeType& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TrainInformationTypes::TrainSelectionList convertToCpp( const TA_Base_Bus::ITrainInformationCorba::TrainSelectionList& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TrainInformationTypes::AgentOperationMode convertToCpp( const TA_Base_Bus::ITrainInformationCorba::AgentOperationMode& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TrainInformationTypes::EAtsTrack convertToCpp( const TA_Base_Bus::ITrainInformationCorba::EAtsTrack& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TrainInformationTypes::TrainDetails convertToCpp( const TA_Base_Bus::ITrainInformationCorba::TrainDetails& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TrainInformationTypes::TrainDetailsList convertToCpp( const TA_Base_Bus::ITrainInformationCorba::TrainDetailsList& CorbaType );

    private:
        
        TrainInformationConversion();

        ~TrainInformationConversion();

    };

}

#endif // !defined(TrainInformationConversion_H)
