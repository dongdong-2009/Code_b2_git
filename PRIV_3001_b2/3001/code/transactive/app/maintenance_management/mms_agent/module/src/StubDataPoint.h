/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_ModuleTest/CODE_4669_T00271301/transactive/app/maintenance_management/mms_agent/test/module/src/StubDataPoint.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2007/10/25 11:49:33 $
  * Last modified by:  $Author: grace.koh $
  * 
  */
#ifndef STUBDATAPOINT_H_
#define STUBDATAPOINT_H_

#include "bus/scada/datapoint_library/src/DataPoint.h "
//#include "core/data_access_interface/entity_access/src/ScadaRootEntityData.h"

namespace TA_IRS_App_Test 
{
	class StubDataPoint : public TA_Base_Bus::DataPoint
	{
		public:
		
		StubDataPoint( TA_Base_Core::DataPointEntityDataPtr			dataPointEntityAccess,
				   	   TA_Base_Bus::EDataPointType			        dpType,
				       TA_Base_Bus::EDataPointDataType		        dpDataType,
                       TA_Base_Bus::ScadaPersistencePtr             persistence,
                       TA_Base_Core::ScadaRootEntityData&           scadaRootEntityData,    // Raymond Pau++ TD 13367
                       TA_Base_Bus::MmsScadaUtility & mmsScadaUtility,
                       TA_Base_Bus::CommandProcessor& commandProcessor   ) 
        :
        DataPoint( dataPointEntityAccess,dpType,dpDataType,persistence,scadaRootEntityData,mmsScadaUtility,commandProcessor) 
        {
        	
        }

		virtual ~StubDataPoint()
		{
		
		}
		
		void setAlarmProperties( const std::string& sessionId, const TA_Base_Bus::AlarmProperty & newAlarmProperties)
		{
			
		}
		
		void getEngineeringUnits(TA_Base_Bus::DataPointEngineeringUnits & eng )
		{
		
		}
		
		TA_Base_Bus::AlarmPropertyVect getAlarmPropertiesVect()
		{
			::TA_Base_Bus::AlarmProperty  _AlarmProperty_0 ;
     		_AlarmProperty_0.type = ::TA_Base_Bus::DptNoAlarm;
     		_AlarmProperty_0.enabled = true ;
     		_AlarmProperty_0.threshold = 123456 ;
    		_AlarmProperty_0.enumValue = 123;
     		std::vector<::TA_Base_Bus::AlarmProperty > _AlarmPropertyVect;
     		_AlarmPropertyVect.push_back(_AlarmProperty_0);
     		
     		return  _AlarmPropertyVect;
		}
		
		std::string getDataPointValueAsString( const TA_Base_Bus::DpValue & dpValue )
		{
			return "Test";
		}
		
		void updateSynAlarmProperties( const TA_Base_Bus::AlarmProperty & newAlarmProperties)
		{
			
		}
	};
}

#endif /*STUBDATAPOINT_H_*/
