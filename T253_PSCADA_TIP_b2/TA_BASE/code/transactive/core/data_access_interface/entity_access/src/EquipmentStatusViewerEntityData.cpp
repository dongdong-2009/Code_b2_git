/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/EquipmentStatusViewerEntityData.cpp $
  * @author Bart Golab
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by: $Author: CM $
  * 
  * Implement the IEntityData interface for the Equipment Status Viewer entity.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/EquipmentStatusViewerEntityData.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    const std::string EquipmentStatusViewerEntityData::ENTITY_TYPE          = "EquipmentDatapointStatusViewer";
    const std::string EquipmentStatusViewerEntityData::EXPORT_DIRECTORY     = "ExportDirectory";

    EquipmentStatusViewerEntityData::EquipmentStatusViewerEntityData() :
        EntityData()
    {
    }


    EquipmentStatusViewerEntityData::EquipmentStatusViewerEntityData(unsigned long key) :
        EntityData(key, getStaticType())
    {
    }


    EquipmentStatusViewerEntityData::~EquipmentStatusViewerEntityData()
    {
    }


    std::string EquipmentStatusViewerEntityData::getExportDirectory()
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
		return getHelper()->getParameter(EXPORT_DIRECTORY);
    }


    void EquipmentStatusViewerEntityData::invalidate()
    {
        getHelper()->invalidate();
    }


    std::string EquipmentStatusViewerEntityData::getType()
    {
		return getStaticType();
    }


    std::string EquipmentStatusViewerEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }


    IEntityData* EquipmentStatusViewerEntityData::clone(unsigned long key)
    {
        return (new EquipmentStatusViewerEntityData(key));
    }


} // namespace TA_Base_Core
