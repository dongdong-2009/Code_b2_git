/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/bus/trainborne_cctv/trainborne_cctv_access_library/src/TrainborneCCTVAccessLibrary.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This library provides a layer of abstraction for activating a train's CCTV
  * camera and displaying it in a GUI.
  */


// Disable "identifier truncated" warning
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // _MSC_VER

#include <afxwin.h>         // MFC core and standard components

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/exceptions/src/DataException.h"
#include "bus/trainborne_cctv/trainborne_cctv_access_library/src/TrainborneCCTVAccessLibrary.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DataException.h"
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 
using TA_Base_Core::DebugUtil;
using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::IEntityData;
using TA_Base_Core::TrainAgentEntityData;
using TA_Base_Core::DataException;



namespace TA_IRS_Bus
{

    TrainborneCCTVAccessLibrary::TrainborneCCTVAccessLibrary(
        unsigned long locationKey)
    {
        initialise(locationKey);
    }


    void TrainborneCCTVAccessLibrary::initialise(unsigned long locationKey)
    {
        
		
		std::string sessionID = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );
        m_console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession( sessionID );
		
		//
        // Get the train agent's entity data from its type and location.
        //
        std::vector<IEntityData*> entities =
            EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(
                TrainAgentEntityData::getStaticType(), locationKey);
        
        if(entities.size() != 1)
        {
            const char* msg = "There is no train agent at this location.";
            const char* fld = "Train Agent";
            TA_THROW(DataException(msg, DataException::MISSING_MANDATORY, fld));
        }


        std::string trainEntityName = "";

        try
        {
            trainEntityName = entities[0]->getName();
        }
        catch(...)
        {
            delete entities[0];
            entities[0] = NULL;
            throw;
        }
        delete entities[0];
        entities[0] = NULL;


        //
        // Set the entity name of the Train Agent CORBA interface
        //
        m_trainAgent.setEntityName(trainEntityName,true);

		// 
		// Sets the train agent object timeout
		// Temporary fix for VTT
		// 
		if (TA_Base_Core::RunParams::getInstance().isSet("TrainCorbaTimeout"))
		{
			std::stringstream timeoutStr;
			timeoutStr << TA_Base_Core::RunParams::getInstance().get("TrainCorbaTimeout");
			unsigned short timeout = 0;
			timeoutStr >> timeout;
			
			if (timeout > 0)
			{
				m_trainAgent.setObjectTimeout(timeout);
			}
		}
    }


    TrainborneCCTVAccessLibrary::~TrainborneCCTVAccessLibrary()
    {
    }


    TrainborneCCTVAccessLibrary::activatePECCamera(
        unsigned short trainId,
        unsigned short cameraId,
        const std::string& sessionId)
    {
		unsigned long location = 0;

        TA_Base_Bus::ICCTVTrainCommandsCorbaDef::TrainCameraSequence trainAgentCamList;
        TA_Base_Bus::SwitchManagerCorbaDef::TrainCameraSequence switchAgentCamList;

        trainAgentCamList.length(1);
        switchAgentCamList.length(1);
        trainAgentCamList[0] = static_cast<unsigned char>(cameraId);
        switchAgentCamList[0] = static_cast<unsigned long>(cameraId);

		try
		{
			TA_Base_Bus::ITrainInformationCorbaDef::TrainDetailsList* trains = NULL;
            CORBA_CALL_RETURN( trains, m_trainAgent, getTrainList, () );//limin
			
			for(int i=0; i<trains->length(); i++)
			{
				if((*trains)[i].trainID)
				{
					location = (*trains)[i].currentLocation;
					break;
				}
			}

            CORBA_CALL( m_trainAgent, CCTVAddAlarmImage, ( static_cast< unsigned char >( trainId ), trainAgentCamList, sessionId.c_str() ) );//limin

            // allow switch exceptions to propagate
			CORBA_CALL( TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getSwitchManager(), acknowledgeTrainCameraAlarm, ( location, trainId, switchAgentCamList, m_console->getKey() ) );//limin
		}
		catch(TA_Base_Bus::ICCTVTrainCommandsCorbaDef::TrainCCTVConflictException& e)
		{
			// TD14164 ++
			std::string message = e.what;
			/*message += " \r\n Do you wish to deactivate the owning train and display the alarm image?";*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << message;

			/*if(IDOK == AfxMessageBox( message.c_str(), MB_ICONWARNING|MB_OKCANCEL ))*/
			if(IDOK == userMsg.showMsgBox(IDS_UW_650001))
			// ++ TD14164 
			{
				CORBA_CALL( m_trainAgent, CCTVCarrier, ( e.owningTrain, false, sessionId.c_str() ) );//limin
				CORBA_CALL( m_trainAgent, CCTVAddAlarmImage, ( static_cast<unsigned char>(trainId), trainAgentCamList, TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ).c_str() ) );//limin
				CORBA_CALL( TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getSwitchManager(), acknowledgeTrainCameraAlarm, ( location, trainId, switchAgentCamList, m_console->getKey() ) );//limin
			}
		}
    }



}// TA_IRS_Bus
