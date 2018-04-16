/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/incidents/incident_manager/src/IncidentManagerFramework.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Provide the interface to access database
  *
  */

#if !defined(IncidentManagerFramework_D46AE2EA_E1AC_4f7a_AB4E_9BDFACD735C2__INCLUDED_)
#define IncidentManagerFramework_D46AE2EA_E1AC_4f7a_AB4E_9BDFACD735C2__INCLUDED_

#include "bus/generic_gui/src/AbstractDocumentGUI.h"
#include <string>

namespace TA_Base_Bus
{
	class ItaUserMessage;
};

namespace TA_IRS_Bus
{
	class ItaIncidentRights;
}

namespace TA_IRS_Core
{
	class ItaLocationAccessFactory;
	class ItaIncidentReportFactory;
}

using namespace TA_Base_Bus;
using namespace TA_IRS_Bus;
using namespace TA_IRS_Core;

namespace TA_IRS_App
{	
	class CtaIncidentSummary;

	class IncidentManagerFramework : public AbstractDocumentGUI 
	{
		
	public:
		IncidentManagerFramework();
		virtual ~IncidentManagerFramework();
		//TD18095, jianghp
		virtual void onInitGenericGUICompleted();		
		/**
		* loadAllUserSettings  This will load all the application's user preferences if
		* configured to do so. This should be called by the application itself in
		* OnInit() or equivalent.
		*/
		virtual void loadAllUserSettings();
		/**
		* checkEntity  This will check that the entity is valid and then store a local
		* pointer to the entity. This must be implemented in the lowest level class as
		* this will be where the GUI specific configuration details are known.
		* @param IEntityData* guiEntity - A pointer to the gui entity to check and store.
		* 
		* @param guiEntity
		* 
		*/
		virtual void checkEntity(TA_Base_Core::IEntityData* guiEntity);
		/**
		* saveAllUserSettings  This will save all the application's user preferences if
		* configured to do so. This is provided in case the GUI needs to save user
		* preferences at a predetermined time. The settings are already saved when the
		* application exits.
		*/
		virtual void saveAllUserSettings();
		/**
		* dutyChanged  This tells an application that the duties for the session in which
		* it is running have been updated. The appliation is responsible for re-checking
		* all duty-based functionality.  NB: The application should override this method
		* if they wish to be notified of duty changes.
		*/
		virtual void dutyChanged();
		/**
		* serverIsDown  This is called when the server the workstation is using goes down.
		* The GUI should indicate to the user that the server is down. It should also
		* then limit operations so that the user can only view existing data. This must
		* be implemented by the lowest level class as it knows what to do when this
		* occurs.
		*/
		virtual void serverIsDown();
		/**
		* serverIsUp  This is called when the server the workstation is using starts
		* functioning again. The GUI should indicate to the user that the server is
		* functioning. It should  then allow the user to perform all valid operations
		* again. This must be implemented by the lowest level class as it knows what to
		* do when this occurs.
		*/
		virtual void serverIsUp();
		/**
		* prepareForClose  This method is used to prepare the GUI for closing. It should
		* save the user settings and perform any other tidying up. eg Stop any processing
		* threads, ask the user if they wish to save anything etc. This should be called
		* before any close is done on the GUI whether initiated by the GUI itself or the
		* Control Station. This means it should be the first call in the  implementation
		* of terminateGUI().
		*/
		virtual void prepareForClose();
		//member function to get the location access factory object
		ItaLocationAccessFactory& getLocationAccessFactory();
		//member function to get the incideent right checker object
		ItaIncidentRights& getIncidentRightsChecker();
		//member function to get the incident report factory object
		ItaIncidentReportFactory& getIncidentReportFactory();
		//member function to get the user message object
		TA_Base_Bus::ItaUserMessage& getUserMessage();
		
		/**
		* getAlarmIdFromCommandLine
		* 
		* get the alarm id from the command line
		* 
		* @return std::string 
		* 
		* @exception <exceptions> Optional
		*/
		std::string getAlarmIdFromCommandLine();
		std::string getAlarmValueFromCommandLine();
		
	protected:
	/**
	* createApplicationObject  This method must set the m_application member variable.
	* It should create a new instance of the dialog/document/property sheet and
	* assign it to m_application
		*/
		virtual void createApplicationObject();
		
	private:
		//The three memeber virtual functions are required by the TA_Base_Bus::AbstractDocumentGUI
		virtual void checkCommandLine();
		virtual void entityChanged( const std::vector<std::string>& changes );
		virtual unsigned long getApplicationType();
		
		CtaIncidentSummary *m_CtaIncidentSummary;
		std::string m_alarmIdFormCommandLine;
		std::string m_alarmValueFormCommandLine;
		static const std::string RPARAM_REPORTTEMPLATE;
		
		
	};
}// end TA_IRS_App

#endif // !defined(IncidentManagerFramework_D46AE2EA_E1AC_4f7a_AB4E_9BDFACD735C2__INCLUDED_)
