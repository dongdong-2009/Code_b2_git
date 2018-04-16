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

#include "bus/trains/TrainCorbaProxy/src/RadioGroupTypesConversion.h"

namespace TA_IRS_Bus
{

    TA_Base_Bus::ITrainRadioGroupCorba::GroupLocationList RadioGroupTypesConversion::convertToCorba( const RadioGroupTypes::GroupLocationList& cppType )
    {
        TA_Base_Bus::ITrainRadioGroupCorba::GroupLocationList groupLocationList;

        groupLocationList.length( cppType.size() );

        RadioGroupTypes::GroupLocationList::const_iterator ite = cppType.begin();

        for ( unsigned long i = 0; i < cppType.size(); ++ite, ++i )
        {
            groupLocationList[i] = *ite;
        }

        return groupLocationList;
    }


    TA_Base_Bus::ITrainRadioGroupCorba::TrainRadioGroupList RadioGroupTypesConversion::convertToCorba( const RadioGroupTypes::TrainRadioGroupMap& cppType )
    {
        TA_Base_Bus::ITrainRadioGroupCorba::TrainRadioGroupList trainRadioGroupList;

        trainRadioGroupList.length( cppType.size() );

        RadioGroupTypes::TrainRadioGroupMap::const_iterator ite = cppType.begin();

        for ( unsigned long i = 0;ite != cppType.end(); ++i, ++ite )
        {
            trainRadioGroupList[i] = convertToCorba( ite->second );
        }

        return trainRadioGroupList;
    }


    TA_Base_Bus::ITrainRadioGroupCorba::TrainRadioGroup RadioGroupTypesConversion::convertToCorba( const RadioGroupTypes::TrainRadioGroup& cppType )
    {
        TA_Base_Bus::ITrainRadioGroupCorba::TrainRadioGroup trainRadioGroup;

        trainRadioGroup.groupName = cppType.groupName.c_str();
        trainRadioGroup.groupTsi = cppType.groupTsi.c_str();
        trainRadioGroup.lastUpdateTime = cppType.lastUpdateTime;
        trainRadioGroup.locationCoverage = convertToCorba( cppType.locationCoverage );
        trainRadioGroup.locked = cppType.locked;

        return trainRadioGroup;
    }


    RadioGroupTypes::GroupLocationList RadioGroupTypesConversion::convertToCpp( const TA_Base_Bus::ITrainRadioGroupCorba::GroupLocationList& CorbaType )
    {
        RadioGroupTypes::GroupLocationList groupLocationList;

        for ( unsigned long i = 0; i < CorbaType.length(); ++i )
        {
            groupLocationList.insert( CorbaType[i] );
        }

        return groupLocationList;
    }


    RadioGroupTypes::TrainRadioGroupMap RadioGroupTypesConversion::convertToCpp( const TA_Base_Bus::ITrainRadioGroupCorba::TrainRadioGroupList& CorbaType )
    {
        RadioGroupTypes::TrainRadioGroupMap trainRadioGroupMap;

        for ( unsigned long i = 0; i < CorbaType.length(); ++i )
        {
            RadioGroupTypes::TrainRadioGroup trainRadioGroup = convertToCpp( CorbaType[i] );

            trainRadioGroupMap.insert( RadioGroupTypes::TrainRadioGroupMap::value_type( trainRadioGroup.groupTsi, trainRadioGroup ) );
        }

        return trainRadioGroupMap;
    }


    RadioGroupTypes::TrainRadioGroup RadioGroupTypesConversion::convertToCpp( const TA_Base_Bus::ITrainRadioGroupCorba::TrainRadioGroup& CorbaType )
    {
        RadioGroupTypes::TrainRadioGroup trainRadioGroup;

        trainRadioGroup.groupName = CorbaType.groupName.in();
        trainRadioGroup.groupTsi = CorbaType.groupTsi.in();
        trainRadioGroup.lastUpdateTime = CorbaType.lastUpdateTime;
        trainRadioGroup.locationCoverage = convertToCpp( CorbaType.locationCoverage );
        trainRadioGroup.locked = CorbaType.locked;

        return trainRadioGroup;
    }
}
