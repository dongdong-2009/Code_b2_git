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

#if !defined(StateUpdateTypesConversion_H)
#define StateUpdateTypesConversion_H

#include "bus/trains/TrainCommonLibrary/src/StateUpdateTypes.h"
#include "bus/trains/TrainAgentCorba/idl/src/ITrainStateUpdateCorba.h"

namespace TA_IRS_Bus
{
    /**
     * This is a placeholder for some static methods to covert train state update
     * types between corba and c++
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:49 PM
     */

    class StateUpdateTypesConversion
    {

    public:

        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::CctvState convertToCorba( const StateUpdateTypes::TrainCctvSyncState& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::FullLocalState convertToCorba( const StateUpdateTypes::FullLocalState& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::ETrainResource convertToCorba( const CommonTypes::ETrainResource& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::CommsState convertToCorba( const StateUpdateTypes::TrainCommsSyncState& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::CctvStateList convertToCorba( const StateUpdateTypes::CctvStateList& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::FullGlobalState convertToCorba( const StateUpdateTypes::FullGlobalState& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::CommsStateList convertToCorba( const StateUpdateTypes::CommsStateList& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::TisState convertToCorba( const StateUpdateTypes::TrainTisSyncState& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::PecState convertToCorba( const StateUpdateTypes::TrainPecSyncState& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::PaState convertToCorba( const StateUpdateTypes::TrainPaSyncState& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::TisStateList convertToCorba( const StateUpdateTypes::TisStateList& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::SavedTrainSelectionSequence convertToCorba( const StateUpdateTypes::TrainSelectionMap& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::PaStateList convertToCorba( const StateUpdateTypes::PaStateList& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::RadioCallDetails convertToCorba( const StateUpdateTypes::RadioCallDetails& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::ETimsOrigin convertToCorba( const ProtocolCommonTypes::ETimsOrigin& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::RadioCallDetailsList convertToCorba( const StateUpdateTypes::RadioCallDetailsSet& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::CallState convertToCorba( const StateUpdateTypes::TrainCallSyncState& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::AlarmStateList convertToCorba( const StateUpdateTypes::AlarmStateList& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::AlarmState convertToCorba( const StateUpdateTypes::TrainAlarmSyncState& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::CallStateList convertToCorba( const StateUpdateTypes::CallStateList& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::PecStateList convertToCorba( const StateUpdateTypes::PecStateList& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainStateUpdateCorba::NonDutyTrainTSI convertToCorba( const StateUpdateTypes::NonDutyTrainTSI& cppType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static StateUpdateTypes::FullGlobalState convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::FullGlobalState& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static StateUpdateTypes::TisStateList convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::TisStateList& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static StateUpdateTypes::TrainPaSyncState convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::PaState& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static StateUpdateTypes::TrainPecSyncState convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::PecState& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static StateUpdateTypes::TrainTisSyncState convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::TisState& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static StateUpdateTypes::CommsStateList convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::CommsStateList& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static StateUpdateTypes::CctvStateList convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::CctvStateList& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static StateUpdateTypes::TrainCommsSyncState convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::CommsState& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static StateUpdateTypes::PaStateList convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::PaStateList& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static StateUpdateTypes::TrainCctvSyncState convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::CctvState& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static StateUpdateTypes::TrainSelectionMap convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::SavedTrainSelectionSequence& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static ProtocolCommonTypes::ETimsOrigin convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::ETimsOrigin& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static StateUpdateTypes::PecStateList convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::PecStateList& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static StateUpdateTypes::CallStateList convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::CallStateList& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static StateUpdateTypes::TrainAlarmSyncState convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::AlarmState& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static StateUpdateTypes::AlarmStateList convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::AlarmStateList& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static CommonTypes::ETrainResource convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::ETrainResource& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static StateUpdateTypes::RadioCallDetailsSet convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::RadioCallDetailsList& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static StateUpdateTypes::RadioCallDetails convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::RadioCallDetails& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static StateUpdateTypes::NonDutyTrainTSI convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::NonDutyTrainTSI& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static StateUpdateTypes::FullLocalState convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::FullLocalState& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static StateUpdateTypes::TrainCallSyncState convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::CallState& CorbaType );

    private:
        
        StateUpdateTypesConversion();

        ~StateUpdateTypesConversion();

    };

}

#endif // !defined(StateUpdateTypesConversion_H)
