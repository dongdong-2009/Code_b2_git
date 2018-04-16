/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/entity_access/src/PlanManagerEntityData.cpp $
  * @author Bart Golab
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Implementataion of PlanManagerEntityData.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/PlanManagerEntityData.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    const std::string PlanManagerEntityData::ENTITY_TYPE        = "PlanManager";
    const std::string PlanManagerEntityData::FLOWCHART_DIR      = "FlowchartDir";
    //TD 13265
    const std::string PlanManagerEntityData::REPORT_TEMPLATE    = "ReportTemplate";

	//TD17888 marc_bugfix
	const std::string PlanManagerEntityData::EXPORT_DIRECTORY   = "ExportDirectory";

    PlanManagerEntityData::PlanManagerEntityData() :
        EntityData()
        //TD 13265
    {
    }


    PlanManagerEntityData::PlanManagerEntityData(unsigned long key) :
        EntityData(key, getStaticType())
    {
    }


    PlanManagerEntityData::~PlanManagerEntityData()
    {
    }


    std::string PlanManagerEntityData::getFlowchartDirectory()
    {
		return getHelper()->getParameter(FLOWCHART_DIR);
   }

    //TD 13265
    std::string PlanManagerEntityData::getReportTemplate()
    {
		return getHelper()->getParameter(REPORT_TEMPLATE);
    }

	//TD17888 marc_bugfix
	std::string PlanManagerEntityData::getExportDirectory()
	{
		return getHelper()->getParameter(EXPORT_DIRECTORY);
	}

    void PlanManagerEntityData::invalidate()
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
        getHelper()->invalidate();
    }

    std::string PlanManagerEntityData::getType()
    {
		return getStaticType();
    }

    
    std::string PlanManagerEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }


    IEntityData* PlanManagerEntityData::clone(unsigned long key)
    {
        return (new PlanManagerEntityData(key));
    }

} // namespace TA_Base_Core
