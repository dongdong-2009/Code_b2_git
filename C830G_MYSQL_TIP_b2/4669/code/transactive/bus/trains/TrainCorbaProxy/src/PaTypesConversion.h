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

#if !defined(PaTypesConversion_H)
#define PaTypesConversion_H

#include "bus/trains/TrainCommonLibrary/src/PaTypes.h"
#include "bus/trains/TrainAgentCorba/idl/src/ITrainPaCorba.h"


namespace TA_IRS_Bus
{
    /**
     * This is a placeholder for some static methods to covert train pa types between
     * corba and c++
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:47 PM
     */

    class PaTypesConversion
    {

    public:

        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainPaCorba::PaAnnouncementUpdate convertToCorba( const PaTypes::PaAnnouncementUpdate& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainPaCorba::EPaAnnouncementUpdateType convertToCorba( const PaTypes::EPaAnnouncementUpdateType& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainPaCorba::TrainLibraryInfo convertToCorba( const PaTypes::TrainLibraryInfo& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainPaCorba::EMessageCompletionStatus convertToCorba( const ProtocolPaTypes::EMessageCompletionStatus& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainPaCorba::EPaUpdateType convertToCorba( const PaTypes::EPaUpdateType& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainPaCorba::MessageSchedule convertToCorba( const PaTypes::MessageSchedule& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainPaCorba::EPaCommandResult convertToCorba( const PaTypes::EPaCommandResult& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainPaCorba::AnnouncementId convertToCorba( const PaTypes::AnnouncementId& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainPaCorba::TrainLibraryInfoList convertToCorba( const PaTypes::TrainLibraryInfoList& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainPaCorba::PaUpdateEvent convertToCorba( const PaTypes::PaUpdateEvent& cppType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static PaTypes::EPaAnnouncementUpdateType convertToCpp( const TA_Base_Bus::ITrainPaCorba::EPaAnnouncementUpdateType& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static PaTypes::PaUpdateEvent convertToCpp( const TA_Base_Bus::ITrainPaCorba::PaUpdateEvent& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static PaTypes::TrainLibraryInfoList convertToCpp( const TA_Base_Bus::ITrainPaCorba::TrainLibraryInfoList& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static PaTypes::AnnouncementId convertToCpp( const TA_Base_Bus::ITrainPaCorba::AnnouncementId& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static PaTypes::EPaCommandResult convertToCpp( const TA_Base_Bus::ITrainPaCorba::EPaCommandResult& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static PaTypes::MessageSchedule convertToCpp( const TA_Base_Bus::ITrainPaCorba::MessageSchedule& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static PaTypes::EPaUpdateType convertToCpp( const TA_Base_Bus::ITrainPaCorba::EPaUpdateType& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static ProtocolPaTypes::EMessageCompletionStatus convertToCpp( const TA_Base_Bus::ITrainPaCorba::EMessageCompletionStatus& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static PaTypes::PaAnnouncementUpdate convertToCpp( const TA_Base_Bus::ITrainPaCorba::PaAnnouncementUpdate& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static PaTypes::TrainLibraryInfo convertToCpp( const TA_Base_Bus::ITrainPaCorba::TrainLibraryInfo& CorbaType );

    private:

        PaTypesConversion();

        ~PaTypesConversion();

    };

}

#endif // !defined(PaTypesConversion_H)
