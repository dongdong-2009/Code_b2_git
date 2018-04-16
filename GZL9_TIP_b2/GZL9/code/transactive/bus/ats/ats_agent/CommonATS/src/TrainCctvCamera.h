/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_Review_Branch/4669/transactive/app/ats/AtsAgent/src/TrainCctvCamera.h $
  * @author  J. Welton
  * @version $Revision: #3 $
  * Last modification : $DateTime: 2010/03/30 11:17:43 $
  * Last modified by : $Author: grace.koh $
  *
  *
  *	TrainCctvCamera is an abstract base class representing
  * a single value of any supported type. It maintains
  * a set of rules determining whether or not the value
  * can be updated, as well as a set of alarm conditions.
  * TrainCctvCamera also realises the TrainCctvCameraCorbaDef interface,
  * exposing the TrainCctvCamera's configuration to remote 
  * clients.
  *
  *
  * Modified: Daniel Hoey : 11/11/02 : Added isVirtual() method
  */

#ifndef TRAINCCTVCAMERA_H
#define TRAINCCTVCAMERA_H

#include <string>
#include <vector>

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/data_access_interface/entity_access/src/TrainCctvCameraEntityData.h"

#include "core/exceptions/src/MathematicalEvaluationException.h"

#include "bus/generic_agent/src/IEntity.h"
#include "bus/ats/ats_agent/IDL/src/IAtsCorbaDef.h"

namespace TA_Base_Core
{
	class AlarmHelper;
    class AlarmModificationHelper;
};

namespace TA_Base_Bus
{
	class ExpressionEvaluator;
};

namespace TA_IRS_Bus
{
	class CctvBooleanContext;
	class CctvRealNumberContext;
	class AtsCctvData;

	class TrainCctvCamera : public TA_Base_Bus::IEntity
	{
	
	public:

		/**
  		 * TrainCctvCamera::TrainCctvCamera()
  		 *
  		 * TrainCctvCamera, initialising all basic configuration items.
		 *
		 * @param	TrainCctvCameraEntityAccess	Reference to db entity access
		 *
  		 */
		TrainCctvCamera( TA_Base_Core::TrainCctvCameraEntityDataPtr	TrainCctvCameraEntityAccess );

		virtual ~TrainCctvCamera();	

		// ========================================================================
		// IEntity methods - see IEntity.h 
		//
        bool isValid();
		void start();
		void stop();
		void update(const TA_Base_Core::ConfigUpdateDetails& updateEvent);
		void remove();
		void setToControlMode();
		void setToMonitorMode();

		// ========================================================================
		// TrainCctvCamera specific methods 
		//

		TA_Base_Core::EOperationMode getOperationMode();
		bool changeOperationMode(TA_Base_Core::EOperationMode operationMode);
		void checkOperationMode();


		void	addExpressionData(AtsCctvData* cctvData);
		void	updateCamera(bool isAlarmAgentAvailable=true);
		void	clearCameraAlarmState(bool isAlarmAgentAvailable=true);

		void	setAgentAssetName(std::string assetName) { m_agentAssetName = assetName; };

		const unsigned char getPhysicalTrainNumber();

	protected:

		void readConfiguration( );
		void updateConfiguration( );
		std::string getEvents();
		bool		updateEvents();
		std::string getCameras();
		std::string getCameraAlarmExpression();

		void setCameraAlarmState(bool isInAlarm);

	private:

		//
		// Declare the copy constructor as private 
		// to prevent pass-by-value of this monolithic object
		//
		TrainCctvCamera( const TrainCctvCamera & );

        void closeAlarm();
        void openAlarm();
		void replaceChars(std::string& modifyMe,  std::string findMe, std::string newChars);


	protected:

		TA_Base_Bus::ExpressionEvaluator*			m_expressionEvaluator;
		
		CctvBooleanContext*							m_booleanContext;
		CctvRealNumberContext*						m_realNumberContext;
		AtsCctvData*								m_cctvData;

		bool										m_alarmGenerated;
		/*/TD18093, jianghp
		TA_Base_Core::AlarmHelper&					m_alarmHelper;
        TA_Base_Core::AlarmModificationHelper&		m_alarmModHelper;
		//TD18093, jianghp*/
		TA_Base_Core::TrainCctvCameraEntityDataPtr	m_entityData;

		TA_Base_Core::EOperationMode				m_operationMode;
		TA_Base_Core::ReEntrantThreadLockable		m_lock;

		std::string									m_cameras;
		std::string									m_cameraAlarmExpression;

		std::string									m_agentAssetName; // for alarm submission

		struct VariableState
		{
			std::string variable;
			bool		value;
			bool		matches;
		};

		std::vector<VariableState>					m_variableStates;
	};
}
#endif
