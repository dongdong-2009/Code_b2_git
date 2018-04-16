/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PAZonesStepParameter.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Represents the PaZones parameter of a Station PA step.
  *
  **/

#include "StdAfx.h"

#include "PAZonesStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameterEditDlg.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/pa/src/PaZoneAccessFactory.h"
#include "core/data_access_interface/pa/src/IPaZone.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

/////////////////////////////////////////////////////////////////////////////
// PaZonesStepParameter

PaZonesStepParameter::PaZonesStepParameter(PlanStep& parent, const std::string& name, std::vector<unsigned long>& zones) :
    PlanStepParameter(parent, name, zones)
{
    FUNCTION_ENTRY( "PaZonesStepParameter" );
    FUNCTION_EXIT;
}
PaZonesStepParameter::~PaZonesStepParameter()
{
    FUNCTION_ENTRY( "~PaZonesStepParameter" );
    FUNCTION_EXIT;
}

void PaZonesStepParameter::addPaZoneID (const unsigned long zoneId)
{
    FUNCTION_ENTRY( "addPaZoneID" );

    m_refSrcValue.push_back(zoneId);

    FUNCTION_EXIT;
}

//unsigned long PaZonesStepParameter::getPaZoneID (const int i)
//{
//    FUNCTION_ENTRY( "getPaZoneID" );
//    FUNCTION_EXIT;
//    return m_paZones [i];
//}

void PaZonesStepParameter::resetZones ()
{
    FUNCTION_ENTRY( "resetZones" );

    m_refSrcValue.clear();

    FUNCTION_EXIT;
}

IPlanStepParameterForm* PaZonesStepParameter::getEditDialog( bool canEdit )
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new CPlanStepParameterEditDlg(*this, canEdit);
}

std::vector<std::string> PaZonesStepParameter::getStepParameterValues()
{
	std::vector<std::string> stepParameterVect;
    std::map<unsigned long, std::string> mapLocationKeyToName;
    std::vector<TA_Base_Core::ILocation*> vecLocations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
    for ( std::vector<TA_Base_Core::ILocation*>::iterator itLoop = vecLocations.begin();
        vecLocations.end() != itLoop; ++itLoop )
    {
        mapLocationKeyToName[(*itLoop)->getKey()] = (*itLoop)->getDisplayName();
        delete (*itLoop);
        (*itLoop) = NULL;
    }

	for (std::vector<unsigned long>::iterator itPaZone = m_refSrcValue.begin(); itPaZone != m_refSrcValue.end(); ++itPaZone)
    {
        unsigned long zoneID = *itPaZone;

        TA_Base_Core::IPaZone* zone = NULL;
        try
        {
            zone = TA_Base_Core::PaZoneAccessFactory::getInstance().getPaZone (zoneID, false);
        }
        catch ( std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Caught unkonwn exception while getPaTrainDvaMessage" );
        }

        if ( NULL == zone )
        {
            continue;
        }

        std::string zoneName = zone->getLabel();

        std::map<unsigned long, std::string>::iterator itFound = mapLocationKeyToName.find( zone->getLocationKey() );

        delete zone;
        zone = NULL;

        if ( mapLocationKeyToName.end() == itFound )
        {
            continue;
        }

        zoneName = itFound->second + " - " + zoneName;
        stepParameterVect.push_back(zoneName);
    }

	return stepParameterVect;
}

const std::string PaZonesStepParameter::displayValue() const
{
	return "";
}