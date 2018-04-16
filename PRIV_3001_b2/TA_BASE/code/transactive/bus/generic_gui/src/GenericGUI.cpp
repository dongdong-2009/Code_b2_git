/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/generic_gui/src/GenericGUI.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2018/03/23 17:34:15 $
  * Last modified by:  $Author: Ouyang $
  * 
  * This is the central class for GUI applications. It listens for several callbacks
  * and passes relevant messages onto the GUI application. This class also kicks off
  * ManagedProcess.
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "bus\generic_gui\src\stdafx.h"
#include "bus\generic_gui\src\GenericGUI.h"
#include "bus\generic_gui\src\IGUIApplication.h"
#include "bus\generic_gui\src\TransActiveMessage.h"
#include "bus\generic_gui\src\HelpLauncher.h"
#include "bus\response_plans\event_triggered\src\EventTriggeredPlanProcessor.h"
#include "bus\security\access_control\actions\src\AccessControlledActions.h"
#include "bus\security\rights_library\src\RightsLibrary.h"
#include "bus/event/src/SessionHelper.h"

#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/AlarmPublicationManager.h"
#include "core/alarm/src/AlarmSubscriptionManager.h"
#include "core\configuration_updates\src\OnlineUpdateListener.h"
#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"
#include "core\data_access_interface\entity_access\src\IEntityData.h"
#include "core/data_access_interface/src/IResource.h"
#include "core\exceptions\src\GenericGUIException.h"
#include "core\exceptions\src\DatabaseException.h"
#include "core\exceptions\src\DataException.h"
#include "core\exceptions\src\EntityTypeException.h"
#include "core\exceptions\src\UtilitiesException.h"
#include "core\exceptions\src\InvalidCommandLineException.h"
#include "core\exceptions\src\ProcessAlreadyRunningException.h"
#include "core\exceptions\src\ManagedProcessException.h"
#include "core\exceptions\src\InvalidCommandLineException.h"
#include "core\exceptions\src\RightsException.h"
#include "core\process_management\src\UtilityInitialiser.h"
#include "core\process_management\src\ManagedProcess.h"
#include "core\naming\src\Naming.h"
#include "core\message\src\TransactiveMessageSubscription.h"
#include "core\message\src\MessagePublicationManager.h"
#include "core\message\types\ConfigUpdate_MessageTypes.h"
#include "core\utilities\src\TAAssert.h"
#include "core/event_processing/src/RealTimeEventSender.h"
#include "core/event_processing/src/EventProcessingLibrary.h"
#include "core/event_processing/src/ISessionHelper.h"

using TA_Base_Core::GenericGUIException;
using TA_Base_Core::TransactiveException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;
using TA_Base_Core::ManagedProcessException;
using TA_Base_Core::InvalidCommandLineException;
using TA_Base_Core::EntityTypeException;
using TA_Base_Core::UtilitiesException;
using TA_Base_Core::ProcessAlreadyRunningException;
using TA_Base_Core::RightsException;

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::CorbaUtil;
using TA_Base_Core::ConfigUpdateMessageCorbaDef;
using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::ConfigUpdateDetails;
using TA_Base_Core::OnlineUpdateListener;
using TA_Base_Core::IEntityData;
using TA_Base_Bus::RightsLibrary;

namespace TA_Base_Bus
{

#if 0	//TD18355 bugfix, transferred to ConfigurationEditor.cpp
	// TD15529
	const std::string GenericGUI::CONFIGURATIONEDITOR_APPNAME = "ConfigurationEditor" ;
	const std::string GenericGUI::CONFIGCONNECTIONFILE_NAME = "ConfigConnectionStrings.csv";
#endif

	// TimerCache is used internally by GenericGUI to diagnose
	// startup delays in applications.
	class TimerCache
	{
	public:
		void start( const std::string& name );
		void stop();
		void log( const std::string& context ) const;

	private:
		clock_t m_start;
		clock_t m_stop;

		typedef std::list<std::string> NameList;
		typedef std::list<double> ElapsedTimeList;
		NameList m_name;
		ElapsedTimeList m_elapsedTime;
	};

	void TimerCache::start( const std::string& name )
	{
		m_name.push_back(name);
		m_start = ::clock();
	}

	void TimerCache::stop()
	{
		m_stop = ::clock();
		m_elapsedTime.push_back(static_cast<double>((m_stop - m_start)/CLOCKS_PER_SEC));
	}

	void TimerCache::log( const std::string& context ) const
	{
		NameList::const_iterator nameIt;
		ElapsedTimeList::const_iterator timeIt;

		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "TimerCache for %s", context.c_str() );
		for( nameIt = m_name.begin(), timeIt = m_elapsedTime.begin(); nameIt != m_name.end(); nameIt++, timeIt++ )
		{
			LOGMORE( SourceInfo, "%s: %.1f", nameIt->c_str(), *timeIt );
		}
	}


    GenericGUI::GenericGUI(IGUIApplication& guiApp, const std::string& commandLine)
        : m_guiApplication(guiApp),
          m_managedProcess(NULL),
          m_guiEntity(NULL),
          m_terminateCode(TA_Base_Core::UserExit)/*, // We assume user exit unless this is changed
          m_rightsMgr(NULL)*/
    {
        // Note: This log will not appear in the log file as it has not been set yet.
        LOG( SourceInfo, DebugUtil::FunctionEntry, "Constructor" );

        if ( commandLine.empty() )
        {
            TA_THROW ( GenericGUIException( GenericGUIException::COMMAND_LINE_INVALID ) );
        }

        setUpApplicationType();
        //
        // Register to be notified of changes to the Debug parameters.
        // It is possible these could change when Managed Process is set up.
        //
        RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGFILE);
        RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGLEVEL);
        RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGFILEMAXSIZE);
        RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGMAXFILES);
		RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGPIDFILENAMES);
		RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGLEVELOFF);
		RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGLEVELFILE);

        // We must set the OperationMode to be Control so that the rebinding of names works.
        // All GUI's are always in Control Mode. Only agents will have different modes.
        RunParams::getInstance().set(RPARAM_OPERATIONMODE, RPARAM_CONTROL);
        try
        {
            setUpManagedProcess(commandLine);
/*//TD18095, jianghp, to fix the performance of showing manager application
			char tApp1[128];
			_strtime(tApp1);
            checkCommandLine();

			
			            setUpGUIEntity();
						
						  if (m_guiEntity == NULL)  // TD1141
						  {
						  TA_THROW (GenericGUIException( GenericGUIException::INITIALISATION_ERRORS ) );
						  }
						  
							char tApp25[128];
							_strtime(tApp25);
							
							  TA_Base_Core::RunParams::getInstance().get( RPARAM_RIGHTCHECKING );
							  
								if(TA_Base_Core::RunParams::getInstance().isSet( RPARAM_RIGHTCHECKING ))
								{
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
								"ignore to check the rights of lanching manager.");
								}
								else
								{
								// Initialising rights library
								TA_Base_Bus::RightsLibraryFactory rightsFactory;
								m_rightsMgr = rightsFactory.buildRightsLibrary();
								
								  m_resourceId = m_guiEntity->getKey();
								  
									checkPermission();
		}*/
			
            activateServant();

/*//TD18095, jianghp, to fix the performance of showing manager application
            // TD8684: Ensure plans are triggered from events.
            EventTriggeredPlanProcessor::registerWithProcessingLibrary();
*/
        }
        catch ( const ProcessAlreadyRunningException& )
        {
            m_terminateCode = TA_Base_Core::RequestedTerminate;
            // In case the servant was activated we need to deactivate it
			if (m_managedProcess != NULL)
			{
	            deactivateServant();
			}
            shutdown();
            throw;
        }
        catch ( ... )
        {
            // If any error has occurred we need to set the termination code and tell
            // ManagedProcess we are terminating before rethrowing the exception
            m_terminateCode = TA_Base_Core::InitError;

            // In case the servant was activated we need to deactivate it
			if (m_managedProcess != NULL)
			{
	            deactivateServant();
			}
            shutdown();
            throw;
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "Constructor" );
    }


	bool GenericGUI::init()
	{
		checkCommandLine();
		
		setUpGUIEntity();
		
		if (m_guiEntity == NULL)  // TD1141
		{
			TA_THROW (GenericGUIException( GenericGUIException::INITIALISATION_ERRORS ) );
		}
		
		/*TA_Base_Core::RunParams::getInstance().get( RPARAM_RIGHTCHECKING );
		
		if(TA_Base_Core::RunParams::getInstance().isSet( RPARAM_RIGHTCHECKING ))
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"ignore to check the rights of lanching manager.");
		}
		else
		{
			// Initialising rights library
			TA_Base_Bus::RightsLibraryFactory rightsFactory;
			m_rightsMgr = rightsFactory.buildRightsLibrary();
			
			m_resourceId = m_guiEntity->getKey();
			
			if ( ! checkPermission())
			{
				return false;
			}
		}*/

		// TD8684: Ensure plans are triggered from events.
		EventTriggeredPlanProcessor::registerWithProcessingLibrary();
		
		//set SessionHelper for RealtimeEventSender for sending real time events.
		TA_Base_Core::ISessionHelper * sessionHelper = new TA_Base_Bus::SessionHelper();
		TA_Base_Core::RealTimeEventSenderInstance::instance()->setSessionHelper(sessionHelper);

		return true;
	}

    GenericGUI::~GenericGUI()
    {
        FUNCTION_ENTRY("~GenericGUI");
        try
        {
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "deleting m_managedProcess");
            if (m_managedProcess != NULL)
            {
                // In case shutdown() wasn't called then we'll deactivate and delete
                // again here.
                m_managedProcess->deactivateAndDeleteServant();
            }

			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "deleting m_guiEntity");
            if (m_guiEntity != NULL)
            {
                delete m_guiEntity;
                m_guiEntity = NULL;
            }

			/*// The rights manager should be deleted in the shutdown method already... but just in case
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "deleting m_rightsMgr");
            if (m_rightsMgr != NULL)
            {
                delete m_rightsMgr;
                m_rightsMgr = NULL;
            }*/


            // Not our responsibility to delete the GUI Application
        }
        catch( ... )
        {
            // Catch all exceptions so we don't get errors filtering out
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor of GenericGUI");
        }

        FUNCTION_EXIT;
    }

/////////////////////////////////////////////////////
// Private Helper Methods
/////////////////////////////////////////////////////

    void GenericGUI::setUpManagedProcess(const std::string& commandLine)
    {
        // Note: This log will not appear in the log file as it has not been set yet.
        LOG( SourceInfo, DebugUtil::FunctionEntry, "setUpManagedProcess" );

		TimerCache timer;
		timer.start( "setUpManagedProcess(begin)->initialiseUtilities" );
		
        try
        {
            // First initialise all utilities ready for managed process.
            // Among other things, the log file is set in here If this is successful then
            // all future LOG statements will be output to a file (if configured to do so).
			timer.stop();
			timer.start( "initialiseUtilities->registerManagedApplication" );
            TA_Base_Core::UtilityInitialiser::initialiseUtilities(commandLine);

			// If the version information is requested, open the about box.
			if (RunParams::getInstance().isSet(RPARAM_VERSION))
			{
				HelpLauncher::getInstance().displayAboutBox();

				LOG( SourceInfo, DebugUtil::ExceptionCatch,"InvalidCommandLineException", "Will change this exception into a ProcessAlreadyRunningException shut down quietly after version info" );
				TA_THROW ( ProcessAlreadyRunningException( "Version info only" ) );	
			}

            m_managedProcess = new TA_Base_Core::ManagedProcess( this );
            if (m_managedProcess == NULL)
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "new on ManagedProcess returned NULL");
                TA_THROW (GenericGUIException( GenericGUIException::INITIALISATION_ERRORS ));
            }
            // Register GenericGUI as a ManagedApplication since it implements the IManagedGUICorbaDef
            // interface.
			timer.stop();
			timer.start( "registerManagedApplication->setUpManagedProcess(end)" );
            m_managedProcess->registerManagedApplication( this->_this() );
        }
        catch ( const InvalidCommandLineException& )
        {
			LOG( SourceInfo, DebugUtil::ExceptionCatch,"InvalidCommandLineException", "Will change this exception into a GenericGUIException." );
			TA_THROW ( GenericGUIException( GenericGUIException::COMMAND_LINE_INVALID ) );
        }
        catch ( const ManagedProcessException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch,"ManagedProcessException", "Will change this exception into a GenericGUIException." );
            TA_THROW ( GenericGUIException( GenericGUIException::COMMUNICATION_ERROR_WITH_CONTROL_STATION ) );
        }
        catch ( const UtilitiesException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch,"UtilitiesException","Will change this exception into a GenericGUIException.");
            TA_THROW (GenericGUIException( GenericGUIException::INITIALISATION_ERRORS ));
        }
        catch ( const ProcessAlreadyRunningException& )
        {
            // This indicates the application is already running so we must quietly terminate
            throw;
        }
        catch ( const GenericGUIException& )
        {
            // Catch and rethrow so this doesn't get caught in the catch block below.
            throw;
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch,"Unknown", "This exception has been thrown because CorbaUtil initialise or activate did not work. Will change this exception into a GenericGUIException." );
                TA_THROW (GenericGUIException( GenericGUIException::INITIALISATION_ERRORS ) );
        }

		timer.stop();
		timer.log( "setUpManagedProcess" );
        LOG( SourceInfo, DebugUtil::FunctionExit, "setUpManagedProcess" );
    }


    void GenericGUI::setUpApplicationType()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "setUpApplicationType" );

        //
        // Retrieve the application type and set it in RunParams.
        //
        std::ostringstream appType;
        appType << m_guiApplication.getApplicationType();
        TA_Base_Core::RunParams::getInstance().set( RPARAM_APPTYPE, appType.str().c_str() );
        LOG( SourceInfo, DebugUtil::FunctionExit, "setUpApplicationType" );
    }


    void GenericGUI::checkCommandLine()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "checkCommandLine" );

        //
        // Check the command line passed was correct. We will check entity separately so
        // this is just for any extra params. This may throw an exception. This should be
        // thrown straight up the the calling class as we can't deal with it.
        //
        m_guiApplication.checkCommandLine();

        LOG( SourceInfo, DebugUtil::FunctionExit, "checkCommandLine" );
    }


    void GenericGUI::setUpGUIEntity()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "setUpGUIEntity" );

        //
        // Retrieve the entity name from RunParams to ensure it was on the command line.
        //
        std::string entityName = TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME );
        if ( entityName.empty() )
        {
                TA_THROW ( GenericGUIException( "No entity specified on command line (ie --entity-name not found)", GenericGUIException::COMMAND_LINE_INVALID ) );
        }

		// TD15529
#if 0	//TD18355 bugfix, transferred to ConfigurationEditor.cpp
		if( entityName == CONFIGURATIONEDITOR_APPNAME )
		{
			LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Try to Set the RunParam RPARAM_DBCONNECTIONFILE");
			if( RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE))
			{
				std::string localDbConnectionFile = RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);
				LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "The current RunParam RPARAM_DBCONNECTIONFILE is %s", localDbConnectionFile.c_str() );
				int i = localDbConnectionFile.rfind("\\");
				if( i != std::string::npos )
				{
					// The run parameter has both the file name and path
					std::string configDbConnectionFile;
					configDbConnectionFile.assign(localDbConnectionFile, 0, i+1 );
					configDbConnectionFile += CONFIGCONNECTIONFILE_NAME;
					RunParams::getInstance().set(RPARAM_DBCONNECTIONFILE, configDbConnectionFile.c_str());
					LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Setting the RunParam RPARAM_DBCONNECTIONFILE as %s", RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE).c_str());
				}
				else
				{
					// The run parameter has only the file name, no path
					RunParams::getInstance().set(RPARAM_DBCONNECTIONFILE, CONFIGCONNECTIONFILE_NAME.c_str());
					LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Setting the RunParam RPARAM_DBCONNECTIONFILE as %s", RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE).c_str());
				}	
			}
		}
#endif
        //
        // Load the entity and check the configuration is correct
        //
        try
        {
            m_guiEntity = EntityAccessFactory::getInstance().getEntity( entityName );
            TA_ASSERT ( m_guiEntity != NULL, "EntityAccessFactory returned a NULL entity and yet did not throw an exception." );
            m_guiApplication.checkEntity( m_guiEntity );
        }
        catch ( const DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch,"DatabaseException", "Will change this exception into a GenericGUIException. We don't care what happened just that we can't connect to the database" );
                TA_THROW ( GenericGUIException( GenericGUIException::NO_DATABASE_CONNECTION ) );
        }
        catch ( const EntityTypeException& )
        {
            LOG(SourceInfo,DebugUtil::ExceptionCatch,"EntityTypeException","Will change this exception into a GenericGUIException.");
                TA_THROW ( GenericGUIException( GenericGUIException::ENTITY_CONFIGURATION_INVALID ) );
        }
        catch ( const DataException& ex )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch,"DataException", "Will change this exception into a GenericGUIException." );
            switch ( ex.getFailType() )
            {
                case ( DataException::WRONG_TYPE ):
                        TA_THROW ( GenericGUIException( GenericGUIException::ENTITY_CONFIGURATION_INVALID ) );
                    break;
                case ( DataException::NO_VALUE ):
                        TA_THROW ( GenericGUIException( GenericGUIException::ENTITY_DOES_NOT_EXIST ) );
                    break;
                case ( DataException::NOT_UNIQUE ):
                        TA_THROW ( GenericGUIException( GenericGUIException::ENTITY_NOT_UNIQUE ) );
                    break;
                default:
                    TA_ASSERT(false, "Do not recognise the DataException type");
            }
        }

        //
        // Register interest for online updates of the GUI entity
        // 
		OnlineUpdateListener::getInstance().registerInterest(TA_Base_Core::ENTITY_OWNER, *this, m_guiEntity->getKey());

        LOG( SourceInfo, DebugUtil::FunctionExit, "setUpGUIEntity" );
    }

/////////////////////////////////////////////////////
// Public Member Methods
/////////////////////////////////////////////////////

    void GenericGUI::run()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "Run" );

        m_managedProcess->startMonitoringThread();

		//TD18095 jianghp
		//m_guiApplication.displayApplication();
		m_guiApplication.setGenericGUIStartCompleted(true);
		m_guiApplication.onInitGenericGUICompleted();
		//TD18095 jianghp

        LOG( SourceInfo, DebugUtil::FunctionExit, "Run" );
    }


    void GenericGUI::shutdown()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "shutdown" );

        if (m_managedProcess != NULL)
        {
            std::string message;
            switch(m_terminateCode)
            {
                case(TA_Base_Core::RequestedTerminate):
                    message = "Control Station told application to terminate";
                    break;

                case(TA_Base_Core::InitError):
                    message = "Errors encountered while GenericGUI was being constructed";
                    break;

                case(TA_Base_Core::UserExit):
                    message = "The user closed the application";
                    break;

                case(TA_Base_Core::FatalError):
                    message = "An unhandled exception was caught by the application";
                    break;

                case(TA_Base_Core::AccessDenied):
                    message = "User does not have access permission to run application";
                    break;

                case(TA_Base_Core::NoActiveSession):
                    message = "No active session for the application to run";
                    break;

                case(TA_Base_Core::RightsError):
                    message = "Unable to determine the rights - either it's not configured properly or rights agent is not running properly";
                    break;

                case(TA_Base_Core::CommsError):
                case(TA_Base_Core::DependencyFailure):
                default:
                    // This should never happen but just in case . . .
                    message = "Unknown reason for closing";
            }
        
            m_managedProcess->notifyTerminating(m_terminateCode,message.c_str());

            // Deactivate the managed process servant
            m_managedProcess->deactivateAndDeleteServant();
            m_managedProcess = NULL;
        }

		/*// clean up the rights manager
		if (m_rightsMgr != NULL)
        {
            delete m_rightsMgr;
            m_rightsMgr = NULL;
        }*/

        // Deregister from the OnlineUpdateListener
        OnlineUpdateListener::cleanUp();
		
		// Clean up the alarm subsystem
		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();	
		
		//Clean up Real Time Event Sender Quene
		TA_Base_Core::RealTimeEventSenderInstance::instance()->cleanUp();

		//Clean up EventProcessLibray (for insert event into DB)
		TA_Base_Core::EventProcessingLibrary::instance()->cleanUp();
        // Deregister from RunParams
        RunParams::getInstance().deregisterRunParamUser(this);

        LOG( SourceInfo, DebugUtil::FunctionExit, "shutdown" );
    }

    
    void GenericGUI::onTerminate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "onTerminate" );

        m_terminateCode = TA_Base_Core::RequestedTerminate;
        m_guiApplication.terminateFromControlStation();

        LOG( SourceInfo, DebugUtil::FunctionExit, "onTerminate" );
    }


    void GenericGUI::serverStateChange(bool isServerUp)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "serverStateChange" );

        if (isServerUp)
        {
            m_guiApplication.serverIsUp();
        }
        else
        {
            m_guiApplication.serverIsDown();
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "serverStateChange" );
    }


    void GenericGUI::changeFocus(TA_Base_Core::EFocusType focusType)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "changeFocus" );

        try
        {
            m_guiApplication.changeFocus(focusType);
        }
        catch (...)
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Caught an exception when trying to set focus - most likely because m_guiApplication has not been instantiated yet" );
        }
        LOG( SourceInfo, DebugUtil::FunctionExit, "changeFocus" );
    }


    void GenericGUI::dutyChanged()
    {
        FUNCTION_ENTRY("dutyChanged");

        try
        {
            m_guiApplication.dutyChanged();
        }
        catch(...)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Caught an exception when trying to notify duty change");
        }

        FUNCTION_EXIT;
    }


    void GenericGUI::setWindowPosition(unsigned long posFlag,
                                       unsigned long alignFlag,
                                       const TA_Base_Core::IManagedGUICorbaDef::RECT& objectDim,
                                       const TA_Base_Core::IManagedGUICorbaDef::RECT& boundaryDim)

    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "setWindowPosition" );

        // convert from the corba RECT to our normal windows rect
        RECT obj = {objectDim.left, objectDim.top, objectDim.right, objectDim.bottom};
        RECT bound = {boundaryDim.left, boundaryDim.top, boundaryDim.right, boundaryDim.bottom};

        try
        {
            m_guiApplication.setPosition(posFlag, alignFlag, obj, bound);
        }
        catch (...)
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Caught an exception when trying to setWindowPosition - most likely because m_guiApplication has not been instantiated yet" );
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "setWindowPosition" );
    }


    TA_Base_Core::IManagedGUICorbaDef::RECT GenericGUI::getWindowPosition()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "getWindowPosition" );
        RECT rect = m_guiApplication.getWindowCoordinates();
        TA_Base_Core::IManagedGUICorbaDef::RECT retval = {rect.left, rect.top, rect.right, rect.bottom};

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Window position (top, left, bottom, right) = (%d, %d, %d, %d)", rect.top, rect.left, rect.bottom, rect.right);
        LOG( SourceInfo, DebugUtil::FunctionExit, "getWindowPosition" );
        return retval;
    }


    void GenericGUI::processUpdate(const ConfigUpdateDetails& updateEvent)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "processUpdate" );

        TA_ASSERT( m_guiEntity != NULL,      "Could not update entity as GUI entity is NULL");
    
        if ( updateEvent.getKey() != m_guiEntity->getKey() )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Key of configuration update does not match the update we are interested in. Ignoring" );
            LOG( SourceInfo, DebugUtil::FunctionExit, "processUpdate" );
            return;
        }
        if ( updateEvent.getType() != TA_Base_Core::ENTITY_OWNER )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Type of configuration update does not match the update we are interested in. Ignoring" );
            LOG( SourceInfo, DebugUtil::FunctionExit, "processUpdate" );
            return;
        }
        if ( updateEvent.getModifications() == TA_Base_Core::Create )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Received a 'Create' update for a key that should already exist. Ignoring" );
            LOG( SourceInfo, DebugUtil::FunctionExit, "processUpdate" );
            return;
        }
        if ( updateEvent.getModifications() == TA_Base_Core::Delete )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Received a 'Delete' update for a key that we are currently using. Reporting to the user and then ignoring." );
            LOG( SourceInfo, DebugUtil::FunctionExit, "processUpdate" );
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UW_020003);
            return;
        }
        
        if ( updateEvent.getChangedParams().empty() )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Received a configuration update with an empty list of changes. Ignoring" );
            LOG( SourceInfo, DebugUtil::FunctionExit, "processUpdate" );
            return;
        }

        //
        // If we get to here then we have a valid update so invalidate the entity and then
        // pass it on for processing
        //
        m_guiEntity->invalidate();
        m_guiApplication.entityChanged(updateEvent.getChangedParams());

        LOG( SourceInfo, DebugUtil::FunctionExit, "processUpdate" );
    }


    void GenericGUI::onRunParamChange(const std::string& name, const std::string& value)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "onRunParamChange" );

		TimerCache timer;
		timer.start( "onRunParamChange(begin)->onRunParamChange(end)" );

		// New debug file name
        if ( 0 == name.compare(RPARAM_DEBUGFILE) )
        {
            DebugUtil::getInstance().setFile(value.c_str(), TA_Base_Core::getRunParamValue(RPARAM_DEBUGFILEMAXSIZE, 0));
        }
        // New debug level
        else if ( 0 == name.compare(RPARAM_DEBUGLEVEL) )
        {
            DebugUtil::getInstance().setLevel( DebugUtil::getDebugLevelFromString(value.c_str()));
        }
        // New max file size
        else if ( 0 == name.compare(RPARAM_DEBUGFILEMAXSIZE) )
        {
            std::istringstream strValue(value);
            int maxSize;
            strValue >> maxSize;
            DebugUtil::getInstance().setMaxSize(maxSize);
        }
        // New max number of debug files
        else if ( 0 == name.compare(RPARAM_DEBUGMAXFILES) )
        {
            std::istringstream strValue(value);
            int maxFiles;
            strValue >> maxFiles;
            DebugUtil::getInstance().setMaxFiles(maxFiles);
        }
		// Use pid in filenames
		else if ( 0 == name.compare(RPARAM_DEBUGPIDFILENAMES) )
		{
			DebugUtil::getInstance().encryptPidInFilenames(value);
		}
		// Turn off logging for these debug levels
		else if ( 0 == name.compare(RPARAM_DEBUGLEVELOFF) )
		{
			DebugUtil::getInstance().decodeDisabledLevels(value);
		}
		// Filenames for debug levels
		else if ( 0 == name.compare(RPARAM_DEBUGLEVELFILE) )
		{
			DebugUtil::getInstance().decodeLevelFilenames(value);
		}
		timer.stop();
		timer.log( "onRunParamChange" );
        LOG( SourceInfo, DebugUtil::FunctionExit, "onRunParamChange" );
    }


    /*bool GenericGUI::checkPermission()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "checkPermission" );

        bool permitted = false;
        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        std::string reason;

        std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

        try
        {
            permitted = m_rightsMgr->isActionPermittedOnResource(
                sessionId, 
                m_resourceId, 
                TA_Base_Bus::aca_RUN_APPLICATION, 
                reason, 
                decisionModule);
			//td13694 bgn
            if (!permitted)
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                    "Cannot run this application: %s", reason.c_str());
                TA_THROW (GenericGUIException( GenericGUIException::ACCESS_DENIED ) );
            }
			return permitted;
			//td13694 end
        }
        catch (GenericGUIException&)
        {
            // just rethrow this
            throw;
        }
        catch (...)
        {
            // throw an exception if there is no active session
            if (sessionId.empty())
            {
                //TD1141
                TA_THROW (GenericGUIException(GenericGUIException::NO_ACTIVE_SESSION));
            }

            // We'll just convert all exceptions thrown by the rights library to generic gui exception
            LOG( SourceInfo, DebugUtil::ExceptionCatch,"Exception thrown by RightsLibrary", "Will change this exception into a GenericGUIException." );
            TA_THROW (GenericGUIException(GenericGUIException::UNKNOWN_ACCESS_RIGHTS));
        }
        LOG( SourceInfo, DebugUtil::FunctionExit, "checkPermission" );
		return permitted;
    }*/

    unsigned long GenericGUI::getEntityKey()
    {
        FUNCTION_ENTRY("getEntityKey");

        if (m_guiEntity != NULL)
        {
            return m_guiEntity->getKey();
        }
        else
        {
            return 0;
        }

        FUNCTION_EXIT;
    }
}

