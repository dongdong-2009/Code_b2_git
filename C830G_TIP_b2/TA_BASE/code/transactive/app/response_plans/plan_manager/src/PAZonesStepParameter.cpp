/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/PAZonesStepParameter.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Represents the PaZones parameter of a Station PA step.
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_STATION_PUBLIC_ADDRESS)

#include "PAZonesStepParameter.h"
#include "PlanStepParameterEditDlg.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/pa_4669/src/PaZoneAccessFactory.h"
#include "core/data_access_interface/pa_4669/src/IPaZone.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PaZonesStepParameter

PaZonesStepParameter::PaZonesStepParameter(PlanStep& parent, const std::string& name) :
    PlanStepParameter(parent, name)
{
    FUNCTION_ENTRY( "PaZonesStepParameter" );

    m_paZones = NULL;

    FUNCTION_EXIT;
}


PaZonesStepParameter::PaZonesStepParameter(PlanStep& parent, const std::string& name, const TA_Base_Core::StationZones& zones) :
    PlanStepParameter(parent, name),
    m_paZones (zones)
{
    FUNCTION_ENTRY( "PaZonesStepParameter" );
    FUNCTION_EXIT;
}


PaZonesStepParameter::PaZonesStepParameter(PlanStep& parent, const PaZonesStepParameter& parameter) :
    PlanStepParameter(parent, parameter.getName()),
    m_paZones(parameter.getPAZones())
{
    FUNCTION_ENTRY( "PaZonesStepParameter" );
    FUNCTION_EXIT;
}


PaZonesStepParameter::PaZonesStepParameter(const PaZonesStepParameter& parameter) :
    PlanStepParameter(*parameter.getParentStep(), parameter.getName()),
    m_paZones(parameter.getPAZones())
{
    FUNCTION_ENTRY( "PaZonesStepParameter" );
    FUNCTION_EXIT;
}


PaZonesStepParameter::~PaZonesStepParameter()
{
    FUNCTION_ENTRY( "~PaZonesStepParameter" );
    FUNCTION_EXIT;
}


void PaZonesStepParameter::addToParameterList(CPlanStepParameterListCtrl &listCtrl)
{
    FUNCTION_ENTRY( "addToParameterList" );

    for (int i = 0; i < m_paZones.length (); i++)
    {
        CString zoneName;
        unsigned long zoneID = m_paZones [i];
        TA_Base_Core::IPaZone* zone = TA_Base_Core::PaZoneAccessFactory::getInstance().getPaZone (zoneID, false);
        zoneName = zone->getName().c_str ();
        listCtrl.AddParameter(getName().c_str(), zoneName.GetBuffer (1), this);
      }

    FUNCTION_EXIT;
}


void PaZonesStepParameter::setPaZones(const TA_Base_Core::StationZones& zones)
{
    FUNCTION_ENTRY( "setPaZones" );

    m_paZones = zones;

    FUNCTION_EXIT;
}


void PaZonesStepParameter::addPaZoneID (const unsigned long zoneId)
{
    FUNCTION_ENTRY( "addPaZoneID" );

    unsigned long currentLength = m_paZones.length ();
    // increase the lenght to make spac for new element
    m_paZones.length (currentLength + 1);
    m_paZones [currentLength] = zoneId;

    FUNCTION_EXIT;
}


unsigned long PaZonesStepParameter::getPaZoneID (const int i)
{
    FUNCTION_ENTRY( "getPaZoneID" );
    FUNCTION_EXIT;
    return m_paZones [i];
}


void PaZonesStepParameter::resetZones ()
{
    FUNCTION_ENTRY( "resetZones" );

    m_paZones.length (0);

    FUNCTION_EXIT;
}


TA_Base_App::CPlanStepParameterEditDlg* PaZonesStepParameter::getEditDialog(bool canEdit)
{
    FUNCTION_ENTRY( "getEditDialog" );
    FUNCTION_EXIT;
    return new TA_Base_App::CPlanStepParameterEditDlg(*this, canEdit);
}


#endif // defined (STEPTYPE_STATION_PUBLIC_ADDRESS)
