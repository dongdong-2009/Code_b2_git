/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/entity_access/src/EquipmentStatusViewerEntityData.cpp $
  * @author Bart Golab
  * @version $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/04/10 17:41:00 $
  * Last modified by: $Author: haijun.li $
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
