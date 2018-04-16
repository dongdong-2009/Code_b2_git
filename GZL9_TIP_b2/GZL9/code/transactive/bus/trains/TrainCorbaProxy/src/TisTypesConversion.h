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

#if !defined(TisTypesConversion_H)
#define TisTypesConversion_H

#include "bus/trains/TrainCommonLibrary/src/TtisTypes.h"
#include "bus/trains/TrainAgentCorba/idl/src/ITrainTisCorba.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolTtisTypes.h"
#include "bus/trains/TrainAgentCorba/idl/src/ITrainStateUpdateCorba.h"

namespace TA_IRS_Bus
{
    /**
     * This is a placeholder for some static TtisTypes::methods to covert train tis types between
     * corba and c++
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:50 PM
     */

    class TisTypesConversion
    {

    public:

        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::ETtisJustification convertToCorba( const ProtocolTtisTypes::ETtisJustification& cppType );//TA_IRS_BUS::


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::TtisPredefinedDisplayParameters convertToCorba( const TtisTypes::TtisPredefinedDisplayParameters& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::TisLibraryVersionUpdate convertToCorba( const TtisTypes::TisLibraryVersionUpdate& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::LibraryDataType convertToCorba( const TtisTypes::LibraryDataType& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::TtisPidList convertToCorba( const TtisTypes::TtisPidList& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::ETtisPid convertToCorba( const ProtocolTtisTypes::ETtisPid& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::ETtisOriginalCommand convertToCorba( const TtisTypes::ETtisOriginalDisplayCommand& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::ETtisDisplayMode convertToCorba( const ProtocolTtisTypes::ETtisDisplayMode& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::ETtisCommandResult convertToCorba( const TtisTypes::ETtisCommandResult& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::TisReturnStatus convertToCorba( const TtisTypes::TisReturnStatus& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::EDownloadResult convertToCorba( const TtisTypes::EDownloadResult& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::TisLibraryVersionList convertToCorba( const TtisTypes::TisLibraryVersionList& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::ETtisPriority convertToCorba( const ProtocolTtisTypes::ETtisPriority& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::ETtisResetMessageType convertToCorba( const ProtocolTtisTypes::ETtisResetMessageType& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::ETisUpdateType convertToCorba( const TtisTypes::ETisUpdateType& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::ETtisFontSize convertToCorba( const ProtocolTtisTypes::ETtisFontSize& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::TtdFrameList convertToCorba( const TtisTypes::TtisFrameList& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::TtdFrame convertToCorba( const TtisTypes::TtisFrameData& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::ETtisDownloadType convertToCorba( const TtisTypes::ETtisDownloadType& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::ETtisIntensity convertToCorba( const ProtocolTtisTypes::ETtisIntensity& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::TtisResetParameters convertToCorba( const TtisTypes::TtisResetParameters& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::TimeInterval convertToCorba( const TtisTypes::TimeInterval& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::TisLibraryUpdate convertToCorba( const TtisTypes::TisLibraryUpdate& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::TtisFreeTextDisplayParameters convertToCorba( const TtisTypes::TtisFreeTextDisplayParameters& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainTisCorba::DisplayParameters convertToCorba( const TtisTypes::DisplayParameters& cppType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TtisTypes::TisReturnStatus convertToCpp( const TA_Base_Bus::ITrainTisCorba::TisReturnStatus& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TtisTypes::ETtisOriginalDisplayCommand convertToCpp( const TA_Base_Bus::ITrainTisCorba::ETtisOriginalCommand& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static ProtocolTtisTypes::ETtisPid convertToCpp( const TA_Base_Bus::ITrainTisCorba::ETtisPid& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TtisTypes::TtisPidList convertToCpp( const TA_Base_Bus::ITrainTisCorba::TtisPidList& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TtisTypes::TisLibraryUpdate convertToCpp( const TA_Base_Bus::ITrainTisCorba::TisLibraryUpdate& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TtisTypes::TimeInterval convertToCpp( const TA_Base_Bus::ITrainTisCorba::TimeInterval& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static ProtocolTtisTypes::ETtisDisplayMode convertToCpp( const TA_Base_Bus::ITrainTisCorba::ETtisDisplayMode& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TtisTypes::DisplayParameters convertToCpp( const TA_Base_Bus::ITrainTisCorba::DisplayParameters& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static ProtocolTtisTypes::ETtisJustification convertToCpp( const TA_Base_Bus::ITrainTisCorba::ETtisJustification& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TtisTypes::TtisPredefinedDisplayParameters convertToCpp( const TA_Base_Bus::ITrainTisCorba::TtisPredefinedDisplayParameters& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TtisTypes::TisLibraryVersionUpdate convertToCpp( const TA_Base_Bus::ITrainTisCorba::TisLibraryVersionUpdate& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TtisTypes::TisLibraryVersionList convertToCpp( const TA_Base_Bus::ITrainTisCorba::TisLibraryVersionList& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TtisTypes::TtisResetParameters convertToCpp( const TA_Base_Bus::ITrainTisCorba::TtisResetParameters& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TtisTypes::ETtisCommandResult convertToCpp( const TA_Base_Bus::ITrainTisCorba::ETtisCommandResult& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TtisTypes::EDownloadResult convertToCpp( const TA_Base_Bus::ITrainTisCorba::EDownloadResult& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static ProtocolTtisTypes::ETtisPriority convertToCpp( const TA_Base_Bus::ITrainTisCorba::ETtisPriority& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static ProtocolTtisTypes::ETtisResetMessageType convertToCpp( const TA_Base_Bus::ITrainTisCorba::ETtisResetMessageType& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TtisTypes::ETisUpdateType convertToCpp( const TA_Base_Bus::ITrainTisCorba::ETisUpdateType& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static ProtocolTtisTypes::ETtisFontSize convertToCpp( const TA_Base_Bus::ITrainTisCorba::ETtisFontSize& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TtisTypes::TtisFrameList convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::TtdFrameList& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TtisTypes::TtisFrameData convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::TtdFrame& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TtisTypes::ETtisDownloadType convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::ETtisDownloadType& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TtisTypes::LibraryDataType convertToCpp( const TA_Base_Bus::ITrainTisCorba::LibraryDataType& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static TtisTypes::TtisFreeTextDisplayParameters convertToCpp( const TA_Base_Bus::ITrainTisCorba::TtisFreeTextDisplayParameters& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static ProtocolTtisTypes::ETtisIntensity convertToCpp( const TA_Base_Bus::ITrainTisCorba::ETtisIntensity& CorbaType );

    private:
        
        TisTypesConversion();

        ~TisTypesConversion();

    };

}

#endif // !defined(TisTypesConversion_H)
