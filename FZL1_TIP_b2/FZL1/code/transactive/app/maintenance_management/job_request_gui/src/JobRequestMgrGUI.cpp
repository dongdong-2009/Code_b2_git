/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/maintenance_management/job_request_gui/src/JobRequestMgrGUI.cpp $
  * @author:  
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/05/02 17:29:17 $
  * Last modified by:  $Author: lu.xiangmei $
  * 
  * Implimentation of JobRequestMgrGUI class
  *
  */

#include "stdafx.h"
#include <sstream>

#include "app/maintenance_management/job_request_gui/src/JobRequestMgrGUI.h"
#include "app/maintenance_management/job_request_gui/src/JobRequestMgrDlg.h"
#include "app/maintenance_management/job_request_gui/src/BadRunParamException.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/user_settings/src/SettingsMgr.h"

#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DataConversion.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define INVALID_PARAMS	TA_IRS_App::CJobRequestMgrDlg::INVALID_PARAMS
#define FROM_ALARM		TA_IRS_App::CJobRequestMgrDlg::FROM_ALARM
#define FROM_SCHEMATIC	TA_IRS_App::CJobRequestMgrDlg::FROM_SCHEMATIC


namespace TA_IRS_App
{
	const char* JobRequestMgrGUI::LAUNCH_PARAM = "LaunchCondition";

	JobRequestMgrGUI::JobRequestMgrGUI()
    :
    AbstractDialogGUI( IDD_MMS_DIALOG ),
    m_dlg( NULL )
	{   
		m_launchConditions.launchedFrom = INVALID_PARAMS;
	
	}


	JobRequestMgrGUI::~JobRequestMgrGUI()
	{

	}


	void JobRequestMgrGUI::createApplicationObject()
	{
		try
		{
			//This method validates the params and sets the local launch
			//condition members to appropriate values, even if the call throws
			setLaunchConditionsFromParams();
		}
		catch(TA_IRS_App::BadRunParamException)
		{
			//already logged
		}

		m_dlg = new CJobRequestMgrDlg(*this, m_launchConditions);
		setApplicationWnd(*m_dlg);

        TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, LAUNCH_PARAM);	
	}


	void JobRequestMgrGUI::prepareForClose()
	{
		//TODO: This provides a standard method for each GUI to use to clean itself 
		//up ready to exit. This will be called whenever the application exits whether 
		//it is closed by the Control Station or by the user. This should perform 
		//tasks such as saving user settings, stopping any threads and asking the user 
		//if they want to save their settings. When this method is completed the GUI 
		//should be in a safe state to be closed.

		try
		{
			saveAllUserSettings();
		}
		catch ( TA_Base_Core::UserSettingsException&)
		{
			// Handle here with appropriate errror message
		}

		// TODO: Perform any shutdown tasks here
	}


	void JobRequestMgrGUI::saveAllUserSettings()
	{
		// First check if we should be saving the user settings.
		if ( TA_Base_Core::RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
		{
			return;
		}

		if (getUserSetting() == NULL)
		{
			TA_Base_Bus::SettingsMgr* userSettings = new TA_Base_Bus::SettingsMgr(getSessionId(), getApplicationType());

			if (userSettings == NULL)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                    "new on SettingsMgr returned NULL. Cannot load user preferences");
			}

			setUserSetting( *userSettings );            
		}
    
		savePositionSettings();
	}


	void JobRequestMgrGUI::loadAllUserSettings()
	{
		// First check if we should be loading the user settings.
		if ( TA_Base_Core::RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
		{
		   setWindowPosition();    
		   return;
		}

		if (getUserSetting() == NULL)
		{
			TA_Base_Bus::SettingsMgr* userSettings = new TA_Base_Bus::SettingsMgr(getSessionId(), getApplicationType());

			if (userSettings == NULL)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                    "new on SettingsMgr returned NULL. Cannot load user preferences");
			}

			setUserSetting( *userSettings );            
		}

		loadPositionSettings();
	}


	void JobRequestMgrGUI::serverIsDown()
	{
		// TODO: This method is used to ensure the GUI can handle when the server goes
		//up or down. The GUI should be updated to indicate the network status.
		//If the server has gone down it should enter some sort of read-only mode and
		//if the server comes back up it can restore to the correct operating mode.
	}


	void JobRequestMgrGUI::serverIsUp()
	{
		// TODO: This method is used to ensure the GUI can handle when the server goes
		//up or down. The GUI should be updated to indicate the network status.
		//If the server has gone down it should enter some sort of read-only mode and
		//if the server comes back up it can restore to the correct operating mode.
	}


	void JobRequestMgrGUI::entityChanged(const std::vector<std::string>& changes)
	{
		//TODO: This will be called by GenericGUI when it receives a callback
		//indicating that the GUI entity has been modified. GenericGUI will have
		//invalidated the entity database object and will tell the GUI which
		//items were modified. This method just has to retrieve the details out
		//of it and update accordingly. If the details in it are invalid then this
		//should print a warning message to the user and continue to use the old settings (ie do not update the GUI).
	}


	void JobRequestMgrGUI::checkCommandLine()
	{  
		FUNCTION_ENTRY("checkCommandLine()");

		try
		{
			//This method validates the params
			setLaunchConditionsFromParams();
		}
		catch(TA_IRS_App::BadRunParamException& ex)
		{
			std::string errorMsg = ex.what();
			TA_THROW(TA_Base_Core::GenericGUIException(errorMsg.c_str(),TA_Base_Core::GenericGUIException::COMMAND_LINE_INVALID));
		}

		FUNCTION_EXIT;
	}


	void JobRequestMgrGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
	{
		setGuiEntity(*guiEntity);
	}


	unsigned long JobRequestMgrGUI::getApplicationType()
	{
		//return a value from apptypes.h
		return JOB_REQUEST_MANAGER;
	}


	void JobRequestMgrGUI::onRunParamChange(const std::string& name, const std::string& value)
	{
		FUNCTION_ENTRY ("onRunParamChange");

		//if the parameter change is not related to new launch conditions return
		if (name != LAUNCH_PARAM) 
		{
			FUNCTION_EXIT;
			return;
		}

		try
		{
			//This method validates the params and sets the local launch
			//condition members to appropriate values, even if the call throws
			setLaunchConditionsFromParams();
		}
		catch(TA_IRS_App::BadRunParamException)
		{
			//already logged
		}

		/**
		*
		* updates the dialog based on the new launch conditions. All
		* prior changes by user will be lost. 
		* This method generates a windows event to notify. Hence
		* it uses the message Q to ensure thread saftey
		*/
		m_dlg->updateLaunchConditions(m_launchConditions);

		FUNCTION_EXIT;
	}


	void JobRequestMgrGUI::setLaunchConditionsFromParams()
	{
		FUNCTION_ENTRY("setLaunchConditionsFromParams");
		
		//This is all wrapped in a try so that we can be sure to set m_client = INVALID_PARAM
		//if something is wrong with the parameters
		try
		{
			//if the required param is not set throw
			if ( !( TA_Base_Core::RunParams::getInstance().isSet(LAUNCH_PARAM)) )
			{
				std::ostringstream  details;
				details << "The required runparam " << LAUNCH_PARAM << " is not set.";
				std::string str = details.str();
				TA_THROW(TA_IRS_App::BadRunParamException(str.c_str() ) );
			}

			//Get the paramValue
			std::string paramValue = TA_Base_Core::RunParams::getInstance().get(LAUNCH_PARAM);

			//throw if the param value is empty
			if ( paramValue.empty()) 
			{
				std::ostringstream  details;
				details << "The required runparam " << LAUNCH_PARAM << " value is blank.";
				std::string str = details.str();
				TA_THROW(TA_IRS_App::BadRunParamException(str.c_str() ) );
			}

			//if the param value starts with "alarm"
			if(0 == paramValue.find("alarm"))
			{
				m_launchConditions.launchedFrom = FROM_ALARM;
			}
			//else if the param value starts with "schematic"
			else if(0 == paramValue.find("schematic"))
			{
				m_launchConditions.launchedFrom = FROM_SCHEMATIC;
			}
			//else the runparam value does not have the required format
			else
			{
				std::ostringstream  details;
				details <<  "The required runparam " << LAUNCH_PARAM << " value "
                    << "must start with \"alarm\" or \"schematic\".";
				std::string str = details.str();
				TA_THROW(TA_IRS_App::BadRunParamException(str.c_str() ) );
			}

			if(m_launchConditions.launchedFrom == FROM_ALARM)
			{
				//Use the same error message if any of the param value is invalid:
				std::string str = "When launching from an alarm the runparam must have the form:/n";
                str += "alarm:<location key>:<alarmId>[:<ats alarm flag>], where <location key> and <ats alarm flag> are uint";

				//the param value must have the form"
				//alarm:<location key>:<alarmId>

				//so find the first colon
				std::string::size_type locFirstCol = paramValue.find(":");

				if(std::string::npos == locFirstCol)
				{
					TA_THROW(TA_IRS_App::BadRunParamException(str.c_str() ) );
				}

				//find the second colon
				std::string::size_type locSecondCol = paramValue.find(":", locFirstCol + 1);

				if(std::string::npos == locSecondCol)
				{
					TA_THROW(TA_IRS_App::BadRunParamException(str.c_str() ) );
				}

				//get the location key:
				std::string locKeyStr(&paramValue[locFirstCol + 1], &paramValue[locSecondCol]);

				//find the third colon
				std::string::size_type locThirdCol = paramValue.find(":", locSecondCol + 1);
				if (std::string::npos == locThirdCol)
				{
					TA_THROW(TA_IRS_App::BadRunParamException(str.c_str() ) );
				}
				//get the alarmId:
				m_launchConditions.alarmId = std::string(&paramValue[locSecondCol + 1], &paramValue[locThirdCol]);

				//convert to location key to uint
				try
				{
                    m_launchConditions.alarmLocationKey = TA_Base_Core::DataConversion::toUnsignedInteger(locKeyStr);
                }
                catch(...)
                {
                    TA_THROW(TA_IRS_App::BadRunParamException(str.c_str() ) );				
                }

                //find the fourth colon
                std::string::size_type locFourthCol = paramValue.find(":", locThirdCol + 1);

                if (( std::string::npos != locFourthCol ) && (( locFourthCol + 1 ) < paramValue.length() ))
                {
                    //get isATSAlarm
                    std::string isATSAlarmStr = paramValue.substr(locFourthCol + 1);

                    try
                    {
                        m_launchConditions.isATSAlarm = ( TA_Base_Core::DataConversion::toUnsignedInteger(isATSAlarmStr) != 0 );
                    }
                    catch(...)
                    {
                        TA_THROW(TA_IRS_App::BadRunParamException(str.c_str() ) );				
                    }
                }
			}
			else if(m_launchConditions.launchedFrom == FROM_SCHEMATIC)
			{
				//Use the same error message if any of the param value is invalid:
				std::string str = "When launching from a schematic the runparam must have the form:/n";
				str += "--launch-condition=schematic:<entity name>, where <entity name> can be any non-empty string";

				//the param value must have the form"
				//schematic:<entity name>

				//so find the first colon
				std::string::size_type locFirstCol = paramValue.find(":");

				if(std::string::npos == locFirstCol)
				{
					TA_THROW(TA_IRS_App::BadRunParamException(str.c_str() ) );
				}

				//get the entity name:
				std::string entityNameStr(paramValue.begin() + (locFirstCol + 1), paramValue.end());

				//throw if the string is empty or blank
				if(entityNameStr.empty())
				{
					TA_THROW(TA_IRS_App::BadRunParamException(str.c_str() ) );				
				}
				if(std::string::npos == entityNameStr.find_first_not_of(" ") )
				{
					TA_THROW(TA_IRS_App::BadRunParamException(str.c_str() ) );				
				}

				m_launchConditions.equipmentName = entityNameStr;
				m_launchConditions.isATSAlarm = false;
			}
			else
			{
				TA_ASSERT(false, "This code should not be reachable");
			}
		}
		catch(TA_IRS_App::BadRunParamException& ex)
		{
			m_launchConditions.launchedFrom = INVALID_PARAMS;
			throw ex;
		}
		catch(...)
		{
			m_launchConditions.launchedFrom = INVALID_PARAMS;
			std::string errorMsg = "Unknown exception occurred while parsing runparams.";
			TA_THROW(TA_IRS_App::BadRunParamException(errorMsg.c_str() ) );
		}
		FUNCTION_EXIT;
	}

	//TD18095, jianghp
	void JobRequestMgrGUI::onInitGenericGUICompleted()
	{
		CJobRequestMgrDlg * pDlg = dynamic_cast<CJobRequestMgrDlg * >(getApplicationWnd());
		TA_ASSERT(pDlg != NULL, "the main dialog is NULL!");
		//Hongzhi,CL20343,TES-SDV-LBD-289 Submit job request settings not correct
		//during the lock status, the window status change will be no use, so the interface will be confuse when unlock
//		pDlg->LockWindowUpdate();
		pDlg->init();
//		pDlg->UnlockWindowUpdate();
		if ( ::IsWindow(pDlg->GetSafeHwnd()) )
		{
			pDlg->UpdateWindow();
		}
	};
} //end namespace TA_IRS_App
