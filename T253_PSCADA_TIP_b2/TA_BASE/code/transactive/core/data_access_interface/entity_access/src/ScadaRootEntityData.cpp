/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/ScadaRootEntityData.cpp $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  */

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/entity_access/src/ScadaRootEntityData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    const std::string ScadaRootEntityData::ENTITY_TYPE                      = "SCADAROOT";
    const std::string ScadaRootEntityData::ACK_CLOSE_ALARM_ON_INHIBIT = "AckCloseAlarmOnInhibit";
    const std::string ScadaRootEntityData::ACK_ANALOG_ALARM_ON_TRANSITION = "AckAnalogAlarmOnTransition"; //TD13619

    ScadaRootEntityData::ScadaRootEntityData()
	{

	}


	ScadaRootEntityData::ScadaRootEntityData( unsigned long key ) :
		EntityData( key, getStaticType() )
    {
    }


	ScadaRootEntityData::~ScadaRootEntityData()
	{

	}


	void ScadaRootEntityData::invalidate()
    {
        getHelper()->invalidate();
    }


    std::string ScadaRootEntityData::getType()
    {
        return getStaticType();
    }


	std::string ScadaRootEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }


    TA_Base_Core::IEntityData* ScadaRootEntityData::clone(unsigned long key)
    {
        return( new ScadaRootEntityData(key));
    }


	bool ScadaRootEntityData::getAckCloseAlarmOnInhibit()
    {
		std::string value = getHelper()->getParameter(ACK_CLOSE_ALARM_ON_INHIBIT);
		bool ackCloseAlarmOnInhibit = false; 
		if (!value.empty())
		{
			ackCloseAlarmOnInhibit = EntityHelper::getBooleanData(value);
		}

		return ackCloseAlarmOnInhibit;
    }

	//TD13619
	bool ScadaRootEntityData::getAckAnalogAlarmOnTransition()
    {
		std::string value = getHelper()->getParameter(ACK_ANALOG_ALARM_ON_TRANSITION);
		bool ackAnalogAlarmOnTransition = false; 
		if (!value.empty())
		{
			ackAnalogAlarmOnTransition = EntityHelper::getBooleanData(value);
		}

		return ackAnalogAlarmOnTransition;
    }
	
}

