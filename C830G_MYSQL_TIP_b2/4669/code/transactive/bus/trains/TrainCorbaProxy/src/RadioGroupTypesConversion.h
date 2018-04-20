/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$  * Last modified by:  $Author$
  *
  */

#if !defined(RadioGroupTypesConversion_H)
#define RadioGroupTypesConversion_H

#include "bus/trains/TrainCommonLibrary/src/RadioGroupTypes.h"
#include "bus/trains/TrainAgentCorba/idl/src/ITrainRadioGroupCorba.h"

namespace TA_IRS_Bus
{
    /**
     * This is a placeholder for some static methods to covert train radio group types
     * between corba and c++
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:49 PM
     */

    class RadioGroupTypesConversion
    {

    public:

        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainRadioGroupCorba::GroupLocationList convertToCorba( const RadioGroupTypes::GroupLocationList& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainRadioGroupCorba::TrainRadioGroupList convertToCorba( const RadioGroupTypes::TrainRadioGroupMap& cppType );


        /**
         * This will convert the given C++ type to the appropriate CORBA type.
         *
         * @return the CORBA representation of the C++ type
         *
         * @param cppType    The C++ type to be converted
         */
        static TA_Base_Bus::ITrainRadioGroupCorba::TrainRadioGroup convertToCorba( const RadioGroupTypes::TrainRadioGroup& cppType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static RadioGroupTypes::GroupLocationList convertToCpp( const TA_Base_Bus::ITrainRadioGroupCorba::GroupLocationList& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static RadioGroupTypes::TrainRadioGroupMap convertToCpp( const TA_Base_Bus::ITrainRadioGroupCorba::TrainRadioGroupList& CorbaType );


        /**
         * This will convert the given CORBA type to the appropriate C++ type.
         *
         * @return the C++ representation of the CORBA type
         *
         * @param CorbaType    The CORBA type to be converted
         */
        static RadioGroupTypes::TrainRadioGroup convertToCpp( const TA_Base_Bus::ITrainRadioGroupCorba::TrainRadioGroup& CorbaType );

    private:
        
        RadioGroupTypesConversion();

        ~RadioGroupTypesConversion();

    };

}

#endif // !defined(RadioGroupTypesConversion_H)
