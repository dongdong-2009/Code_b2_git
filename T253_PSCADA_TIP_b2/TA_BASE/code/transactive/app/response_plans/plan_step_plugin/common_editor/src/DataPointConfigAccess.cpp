//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointConfigAccess.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// This is a utility class. It provides methods for determining a data point's type and,
// where applicable, configured states.

#include <algorithm>
#include <sstream>
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "DataPointConfigAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    struct DataPointStateComparator
    {
        bool operator()(DataPointConfigAccess::StateValue &sv1, DataPointConfigAccess::StateValue &sv2) const
        {
            return sv1.state < sv2.state;
        }
    };
}

namespace TA_Base_App
{


/////////////////////////////////////////////////////////////////////////////
// DataPointConfigAccess

DataPointConfigAccess::EDataPointType DataPointConfigAccess::getDataPointType(std::string dataPointName)
{
    FUNCTION_ENTRY( "getDataPointType" );

    try
    {
        // Get the data point entity corresponding to the specified data point entity name
        std::auto_ptr<TA_Base_Core::IEntityData> entityData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(dataPointName));
        TA_Base_Core::DataPointEntityData* dataPointEntityData = dynamic_cast<TA_Base_Core::DataPointEntityData *>(entityData.get());
        if (!dataPointEntityData)
        {
            std::ostringstream message;
            message << "Entity " << entityData->getName() << " is not a data point entity.";

            TA_THROW(TA_Base_Core::TransactiveException(message.str()));
        }

        // Extract data point type and data point data type as strings
        std::string dataPointType = dataPointEntityData->getDataPointType();
        std::string dataPointDataType = dataPointEntityData->getDataType();

        // Conver to and return as an enumeration
        if (dataPointType == TA_Base_Bus::DPT_DERIVED_TYPE_STRING)
        {
            FUNCTION_EXIT;
            return DataPointConfigAccess::DPT_DERIVED_IN;
        }

        if (dataPointType == TA_Base_Bus::DPT_COMBINED_OUTPUT_TYPE_STRING)
        {
            FUNCTION_EXIT;
            return DataPointConfigAccess::DPT_COMBINED_OUT;
        }

        if (dataPointDataType == TA_Base_Bus::DPT_CFG_UINT16)
        {
            FUNCTION_EXIT;
            return DataPointConfigAccess::DPT_UNSIGNED_INT16;
        }

        if (dataPointDataType == TA_Base_Bus::DPT_CFG_INT16)
        {
            FUNCTION_EXIT;
            return DataPointConfigAccess::DPT_SIGNED_INT16_2C;
        }

        if (dataPointDataType == TA_Base_Bus::DPT_CFG_INT16S)
        {
            FUNCTION_EXIT;
            return DataPointConfigAccess::DPT_SIGNED_INT16_SM;
        }

        if (dataPointDataType == TA_Base_Bus::DPT_CFG_UINT32)
        {
            FUNCTION_EXIT;
            return DataPointConfigAccess::DPT_UNSIGNED_INT32;
        }

        if (dataPointDataType == TA_Base_Bus::DPT_CFG_INT32)
        {
            FUNCTION_EXIT;
            return DataPointConfigAccess::DPT_SIGNED_INT32_2C;
        }

        if (dataPointDataType == TA_Base_Bus::DPT_CFG_INT32S)
        {
            FUNCTION_EXIT;
            return DataPointConfigAccess::DPT_SIGNED_INT32_SM;
        }

        if (dataPointDataType == TA_Base_Bus::DPT_CFG_IEEE32)
        {
            FUNCTION_EXIT;
            return DataPointConfigAccess::DPT_IEEE_FLOAT32;
        }

        if (dataPointDataType == TA_Base_Bus::DPT_CFG_BOOL)
        {
            FUNCTION_EXIT;
            return DataPointConfigAccess::DPT_BOOLEAN;
        }

        if (dataPointDataType == TA_Base_Bus::DPT_CFG_TEXT)
        {
            FUNCTION_EXIT;
            return DataPointConfigAccess::DPT_TEXT;
        }

        FUNCTION_EXIT;
        return DataPointConfigAccess::DPT_UNDEFINED;
    }
    catch (...)
    {
        // [ToDo] Catch exception in presentation layer
        //TA_Base_Bus::TransActiveMessage userMsg;
        //userMsg << dataPointName;
        //userMsg.showMsgBox(IDS_UE_210098);
    }

    FUNCTION_EXIT;
    return DataPointConfigAccess::DPT_UNDEFINED;
}


DataPointConfigAccess::StateValueList DataPointConfigAccess::getDataPointStates(std::string dataPointName)
{
    FUNCTION_ENTRY( "getDataPointStates" );

    StateValueList dataPointStates;

    try
    {
        // Get the data point entity corresponding to the specified data point entity name
        std::auto_ptr<TA_Base_Core::IEntityData> entityData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(dataPointName));
        TA_Base_Core::DataPointEntityData* dataPointEntityData = dynamic_cast<TA_Base_Core::DataPointEntityData *>(entityData.get());
        if (!dataPointEntityData)
        {
            std::ostringstream message;
            message << "Entity " << entityData->getName() << " is not a data point entity.";

            TA_THROW(TA_Base_Core::TransactiveException(message.str()));
        }

        // Get the data point type
        std::string dataPointType = dataPointEntityData->getDataPointType();

        // Must be a derived input or a combined output data point
        if (( dataPointType == TA_Base_Bus::DPT_DERIVED_TYPE_STRING ) || ( dataPointType == TA_Base_Bus::DPT_COMBINED_OUTPUT_TYPE_STRING ))
        {
            // Extract derived states for the specified data point
            TA_Base_Core::DerivedDataPointHelper derivedDataPointHelper;
            TA_Base_Core::DerivedStateList derivedStates(derivedDataPointHelper.getDerivedStates(dataPointEntityData->getKey()));

            dataPointStates.reserve(derivedStates.size());

            // Iterate over the extracted states and add them to the state list
            for (TA_Base_Core::DerivedStateList::const_iterator iter(derivedStates.begin()); iter != derivedStates.end(); iter++)
            {
                StateValue stateValue;
                stateValue.state = iter->stateValue;
                stateValue.description = iter->stateDescription;

                dataPointStates.push_back(stateValue);
            }

            // Sort the states by STATE_VALUE
            std::sort(dataPointStates.begin(), dataPointStates.end(), DataPointStateComparator());
        }
        else if ( dataPointType == TA_Base_Bus::DPT_BOOLEAN_TYPE_STRING )
        {
            StateValue stateValue;

            dataPointStates.push_back(stateValue);
            dataPointStates.back().state = 0;
            dataPointStates.back().description = dataPointEntityData->getFalseLabel();

            dataPointStates.push_back(stateValue);
            dataPointStates.back().state = 1;
            dataPointStates.back().description = dataPointEntityData->getTrueLabel();
        }
    }
    catch (...)
    {
    }

    FUNCTION_EXIT;
    return dataPointStates;
}

DataPointConfigAccess::StateValueList DataPointConfigAccess::getDataPointStates(const unsigned long dataPointKey)
{
    FUNCTION_ENTRY("getDataPointStates");

    StateValueList dataPointStates;

    try
    {
        // Extract derived states for the specified data point
        TA_Base_Core::DerivedDataPointHelper derivedDataPointHelper;
        TA_Base_Core::DerivedStateList derivedStates(derivedDataPointHelper.getDerivedStates(dataPointKey));

        dataPointStates.reserve(derivedStates.size());

        // Iterate over the extracted states and add them to the state list
        for (TA_Base_Core::DerivedStateList::const_iterator iter(derivedStates.begin()); iter != derivedStates.end(); iter++)
        {
            StateValue stateValue;
            stateValue.state = iter->stateValue;
            stateValue.description = iter->stateDescription;

            dataPointStates.push_back(stateValue);
        }

        // Sort the states by STATE_VALUE
        std::sort(dataPointStates.begin(), dataPointStates.end(), DataPointStateComparator());
    }
    catch (...)
    {
    }

    FUNCTION_EXIT;
    return dataPointStates;
}


}
