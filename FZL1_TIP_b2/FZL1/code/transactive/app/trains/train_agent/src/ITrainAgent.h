/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/trains/train_agent/src/ITrainAgent.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Call back interface used by TrainManager to access TrainAgent functionality.
  *
  */

///////////////////////////////////////////////////////////
//  ITrainAgent.h
//  Implementation of the Interface ITrainAgent
//  Created on:      28-Nov-2003 08:09:13 PM
///////////////////////////////////////////////////////////

#if !defined(ITrainAgent_B2738E6D_8B4B_44e4_B858_F691DDE44061__INCLUDED_)
#define ITrainAgent_B2738E6D_8B4B_44e4_B858_F691DDE44061__INCLUDED_

#include <string>

#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/TrainSettingsEntityData.h"

/**
 * Call back interface used by TrainManager to access TrainAgent functionality.
 */
namespace TA_IRS_App
{
	class ITrainAgent
	{
		public:
			virtual void sendMessageToTrain(std::string trainTSI,const std::vector<unsigned char> messageData) =0;
			virtual TA_Base_Core::TrainSettingsEntityDataPtr getGlobalTrainAgentEntity() = 0;
			virtual TA_Base_Core::TrainAgentEntityDataPtr getLocalTrainAgentEntity() = 0;
	};
};
#endif // !defined(ITrainAgent_B2738E6D_8B4B_44e4_B858_F691DDE44061__INCLUDED_)
