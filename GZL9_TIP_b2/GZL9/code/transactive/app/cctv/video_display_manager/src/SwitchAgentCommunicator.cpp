/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/SwitchAgentCommunicator.cpp $
  * @author:  Robert Young
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/11/18 18:38:22 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Singleton to provide access to Agent data and functions
  *
  */

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif // defined( WIN32 )

#include "StdAfx.h"
#include "app/cctv/video_display_manager/src/SwitchAgentCommunicator.h"
#include "app/cctv/video_display_manager/src/DisplayItemManager.h"
#include "app/cctv/video_display_manager/src/VideoInput.h"
#include "app/cctv/video_display_manager/src/Camera.h"
#include "app/cctv/video_display_manager/src/Quad.h"
#include "app/cctv/video_display_manager/src/Sequence.h"
#include "app/cctv/video_display_manager/src/VideoOutput.h"
#include "app/cctv/video_display_manager/src/Monitor.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/exceptions/src/TransActiveException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "bus/generic_gui/src/TransActiveMessage.h"

using CORBA::String_var;
using TA_Base_Core::TransactiveException;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;

// using TA_Base_Bus_GZL9::VideoSwitchAgentFactory::VideoOutputNamedObject;
// using TA_Base_Bus_GZL9::VideoSwitchAgentFactory::VideoMonitorNamedObject;
// using TA_Base_Bus_GZL9::VideoSwitchAgentFactory::QuadNamedObject;
// using TA_Base_Bus_GZL9::VideoSwitchAgentFactory::CameraNamedObject;
// using TA_Base_Bus_GZL9::VideoSwitchAgentFactory::SequenceNamedObject;
// using TA_Base_Bus_GZL9::VideoSwitchAgentFactory:: BVSStageNamedObject;
typedef TA_Base_Bus_GZL9::VideoSwitchAgentFactory::VideoOutputNamedObject VideoOutputNamedObject;
typedef TA_Base_Bus_GZL9::VideoSwitchAgentFactory::VideoMonitorNamedObject VideoMonitorNamedObject;
typedef TA_Base_Bus_GZL9::VideoSwitchAgentFactory::QuadNamedObject QuadNamedObject;
typedef TA_Base_Bus_GZL9::VideoSwitchAgentFactory::CameraNamedObject CameraNamedObject;
typedef TA_Base_Bus_GZL9::VideoSwitchAgentFactory::SequenceNamedObject SequenceNamedObject;
//typedef TA_Base_Bus_GZL9::VideoSwitchAgentFactory::BVSStageNamedObject BVSStageNamedObject;


namespace TA_IRS_App
{
    // Initialise singleton and threadlock Statics
    SwitchAgentCommunicator* SwitchAgentCommunicator::m_theInstance = 0;
    TA_Base_Core::ReEntrantThreadLockable SwitchAgentCommunicator::m_singletonLock;

    //
    // getInstance
    //
    SwitchAgentCommunicator& SwitchAgentCommunicator::getInstance()
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        if ( m_theInstance == 0 )
        {
            m_theInstance = new SwitchAgentCommunicator();
        }

        return *m_theInstance;
    }


    //
    // removeInstance
    //
    void SwitchAgentCommunicator::removeInstance()
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        if ( m_theInstance != 0 )
        {
            delete m_theInstance;
            m_theInstance = 0;
        }
    }


    //
    // Constructor
    //
    SwitchAgentCommunicator::SwitchAgentCommunicator()
    {
//        try
//        {
//            loadBVSNamedObjects();
//            loadCameraNamedObjects();
//            loadQuadNamedObjects();
//            loadSequenceNamedObjects();
//            loadMonitorNamedObjects();
//        }
//        catch ( ... )
//        {
//        }
    }


    SwitchAgentCommunicator::~SwitchAgentCommunicator()
    {
        try
        {
            CamToNamedObject::iterator camIt = m_camToNamedObject.begin();

            for (; camIt != m_camToNamedObject.end(); camIt++ )
            {
                delete ( *camIt ).second;
            }

            QuadToNamedObject::iterator quadIt = m_quadToNamedObject.begin();

            for (; quadIt != m_quadToNamedObject.end(); quadIt++ )
            {
                delete ( *quadIt ).second;
            }

            SequenceToNamedObject::iterator seqIt = m_sequenceToNamedObject.begin();

            for (; seqIt != m_sequenceToNamedObject.end(); seqIt++ )
            {
                delete ( *seqIt ).second;
            }

            MonitorToNamedObject::iterator monIt = m_consoleMonitorToNamedObject.begin();

            for (; monIt != m_consoleMonitorToNamedObject.end(); monIt++ )
            {
                delete ( *monIt ).second;
            }

            //luobihui

            //   OutputToNamedObject::iterator voIt = m_commonMonitorToNamedObject.begin();
            MonitorToNamedObject::iterator voIt = m_commonMonitorToNamedObject.begin();

            for (; voIt != m_commonMonitorToNamedObject.end(); voIt++ )
            {
                delete ( *voIt ).second;
            }
        }
        catch ( const TA_Base_Core::ObjectResolutionException& ore )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", ore.what() );

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << ore.what();
            UINT selectedButton = userMsg.showMsgBox( IDS_UE_090087 );
            //std::ostringstream err;
            //err << "Error loading display devices from Video Switch Agent: " << std::endl;
            //err << ore.what();

            //AfxMessageBox(err.str().c_str());
        }
        catch ( const TA_Base_Core::TransactiveException& te )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", te.what() );

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << te.what();
            UINT selectedButton = userMsg.showMsgBox( IDS_UE_090087 );
            //std::ostringstream err;
            //err << "Error loading display devices from Video Switch Agent: " << std::endl;
            //err << te.what();

            //AfxMessageBox(err.str().c_str());
        }
        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while calling submitAdHocRequest: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA", exceptionMsg.c_str() );

            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox( IDS_UE_090088 );
            //std::ostringstream err;
            //err << "Error loading display devices from Video Switch Agent";

            //AfxMessageBox(err.str().c_str());
        }
    }



    void SwitchAgentCommunicator::assignMonitor( Monitor* monitor, VideoInput* source  )
    {
        // Guard the method to ensure access by client code is synchronous
        FUNCTION_ENTRY( "assignMonitor" );
        ThreadGuard guard( m_singletonLock );

        TA_ASSERT( monitor != NULL, "Destination Monitor is NULL" );
        TA_ASSERT( source != NULL, "Video Input is NULL" );

        // no need to get the named object for the source - the key is sufficient
        // Camera* camera =dynamic_cast<Camera*>(source);
        // bool isMovementSupported=camera->getIsMovementSupported();
        // make the call
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "assignMonitor( Monitor = %ld, VideoInput = %ld)",
                     monitor->getKey() , source->getKey()  );

        try
        {
            std::string session = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

            if ( monitor->isConsoleMonitor() )
            {
                VideoMonitorNamedObject* monObj = NULL;
                monObj = getConsoleMonitor( monitor->getName() );
                //    TA_ASSERT(monObj != NULL, "Destination Monitor named object is NULL - when there is a local monitor object for it");

                if ( monObj==NULL )
                {
                    AfxMessageBox( "远程监视器对象不存在" );
                    throw( "远程监视器对象不存在" );
                    return;
                }

                CORBA_CALL( ( *monObj ), requestSwitchOfInput, ( source->getKey(), session.c_str(), true ) );//limin
            }
            else
            {
                // VideoOutputNamedObject* monObj = NULL;
                VideoMonitorNamedObject * monObj;
                monObj = getCommonMonitor( monitor->getName() );
                //    TA_ASSERT(monObj != NULL, "Destination Monitor named object is NULL - when there is a local monitor object for it");

                if ( monObj==NULL )
                {
                    AfxMessageBox( "远程监视器对象不存在" );
                    throw ( "远程监视器对象不存在" );
                    return;
                }

                CORBA_CALL( ( *monObj ), requestSwitchOfInput, ( source->getKey(), session.c_str(), true ) );//limin
            }
        }
        catch ( TransactiveException& te )
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );

            throw;
        }
        catch ( TA_Base_Bus_GZL9::VideoSwitchAgentException& ex )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "VideoSwitchAgentException", ex.what.in() );

            //TA_THROW( TransactiveException(ex.what.in()) )
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( ex.what.in() );
        }

        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while assigning monitor: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

            // TD 16879
            // TA_THROW( TransactiveException("Could not assign device to monitor.") )
            TA_THROW( TransactiveException( "UE-090037" ) )
            // TD 16879
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown" );

            TA_THROW( TransactiveException( "Could not assign device to monitor." ) )
        }

        // update the state objects
        monitor->assignInput( source );

        // tell the GUI to update its display
        DisplayItemManager::getInstance().notifyGUIofDisplayItemUpdate( monitor );

        FUNCTION_EXIT;
    }


	void SwitchAgentCommunicator::assignMonitor( Monitor* monitor, VideoInput* videoInputTopRight,
		VideoInput* videoInputTopLeft,
		VideoInput* videoInputBotRight,
		VideoInput* videoInputBotLeft )
	{
        // Guard the method to ensure access by client code is synchronous
        FUNCTION_ENTRY( "assignMonitor" );
        ThreadGuard guard( m_singletonLock );

        TA_ASSERT( monitor != NULL, "Destination Monitor is NULL" );

        // no need to get the named object for the source - the key is sufficient
        // Camera* camera =dynamic_cast<Camera*>(source);
        // bool isMovementSupported=camera->getIsMovementSupported();
        // make the call
		unsigned long videoInputKeyTopRight = 0, videoInputKeyTopLeft = 0, videoInputKeyBotRight = 0, videoInputKeyBotLeft = 0;
		Sequence* seq = NULL;
		
		if(videoInputTopRight)
		{
			videoInputKeyTopRight = videoInputTopRight->getKey();
			if((seq = dynamic_cast<Sequence*>(videoInputTopRight)) && seq->getCameras().size() > 10)
			{
                AfxMessageBox( "四画面人工序列超长" );
                throw( "四画面人工序列超长" );
			}
		}
		if(videoInputTopLeft)
		{
			videoInputKeyTopLeft = videoInputTopLeft->getKey();
			if((seq = dynamic_cast<Sequence*>(videoInputTopLeft)) && seq->getCameras().size() > 10)
			{
                AfxMessageBox( "四画面人工序列超长" );
                throw( "四画面人工序列超长" );
			}
		}
		if(videoInputBotRight)
		{
			videoInputKeyBotRight = videoInputBotRight->getKey();
			if((seq = dynamic_cast<Sequence*>(videoInputBotRight)) && seq->getCameras().size() > 10)
			{
                AfxMessageBox( "四画面人工序列超长" );
                throw( "四画面人工序列超长" );
			}
		}
		if(videoInputBotLeft)
		{
			videoInputKeyBotLeft = videoInputBotLeft->getKey();
			if((seq = dynamic_cast<Sequence*>(videoInputBotLeft)) && seq->getCameras().size() > 10)
			{
                AfxMessageBox( "四画面人工序列超长" );
                throw( "四画面人工序列超长" );
			}
		}


        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "assignMonitor( Monitor = %ld, VideoInputTopRight = %ld, VideoInputTopLeft = %ld, VideoInputBotRight = %ld, VideoInputBotLeft = %ld)",
                     monitor->getKey() , videoInputKeyTopRight, videoInputKeyTopLeft, videoInputKeyBotRight, videoInputKeyBotLeft);

        try
        {
            std::string session = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

			/*
			TA_ASSERT(monitor->isConsoleMonitor(), "");
			if(!monitor->isConsoleMonitor())
			{
                AfxMessageBox( "监视器配置错误" );
                throw( "监视器配置错误" );
			}
			*/
            VideoMonitorNamedObject* monObj = NULL;
            monObj = getConsoleMonitor( monitor->getName() );
			if(monObj == NULL)
			{
				monObj = getCommonMonitor(monitor->getName());
			}
            //    TA_ASSERT(monObj != NULL, "Destination Monitor named object is NULL - when there is a local monitor object for it");

            if ( monObj==NULL )
            {
                AfxMessageBox( "远程监视器对象不存在" );
                throw( "远程监视器对象不存在" );
            }

            CORBA_CALL( ( *monObj ), requestSwitchOfInputs, ( videoInputKeyTopRight, videoInputKeyTopLeft, videoInputKeyBotRight, videoInputKeyBotLeft, session.c_str(), true ) );//limin
        }
        catch ( TransactiveException& te )
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );

            throw;
        }
        catch ( TA_Base_Bus_GZL9::VideoSwitchAgentException& ex )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "VideoSwitchAgentException", ex.what.in() );

            //TA_THROW( TransactiveException(ex.what.in()) )
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( ex.what.in() );
        }

        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while assigning monitor: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

            // TD 16879
            // TA_THROW( TransactiveException("Could not assign device to monitor.") )
            TA_THROW( TransactiveException( "UE-090037" ) )
            // TD 16879
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown" );

            TA_THROW( TransactiveException( "Could not assign device to monitor." ) )
        }

        FUNCTION_EXIT;
	}

	void SwitchAgentCommunicator::assignQuad( Quad* quad, Camera* topLeftCam, Camera* topRightCam,
                                              Camera* bottomLeftCam, Camera* bottomRightCam )
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        TA_ASSERT( quad != NULL, "Destination Quad is NULL" );

        // get the quad corba object
        QuadNamedObject* quadObj = NULL;
        quadObj = getQuad( quad->getName() );
        TA_ASSERT( quadObj != NULL, "Quad named object is NULL - when there is a local quad object for it" );

        // if the camera is not null - get the key, if it is, then set the key to 0
        unsigned long topLeft = ( topLeftCam != NULL ) ? topLeftCam->getKey() : 0;
        unsigned long topRight= ( topRightCam != NULL ) ? topRightCam->getKey() : 0;
        unsigned long bottomLeft = ( bottomLeftCam != NULL ) ? bottomLeftCam->getKey() : 0;
        unsigned long bottomRight = ( bottomRightCam != NULL ) ? bottomRightCam->getKey() : 0;

        try
        {
            std::string session = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

            CORBA_CALL( ( *quadObj ), requestSwitchOfInputs, ( topLeft, topRight, bottomLeft, bottomRight, session.c_str(), true ) );//limin
        }
        catch ( TransactiveException& te )
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );

            throw;
        }
        catch ( TA_Base_Bus_GZL9::VideoSwitchAgentException& ex )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus_GZL9::VideoSwitchAgentException", ex.what.in() );

            TA_THROW( TransactiveException( ex.what.in() ) )
        }
        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while assigning quad: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

            TA_THROW( TransactiveException( "Could not assign new inputs to quad." ) )
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown" );

            TA_THROW( TransactiveException( "Could not assign new inputs to quad." ) )
        }

        // update the state objects
        quad->setTopLeft( topLeftCam );

        quad->setTopRight( topRightCam );

        quad->setBottomLeft( bottomLeftCam );

        quad->setBottomRight( bottomRightCam );

        // tell the GUI to update its display
        DisplayItemManager::getInstance().notifyGUIofDisplayItemUpdate( quad );
    }


    void SwitchAgentCommunicator::setSequenceConfig( Sequence* sequence,
                                                     unsigned short dwellTime,
                                                     const std::list< Camera* > cameraInputs,bool con )
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        TA_ASSERT( sequence != NULL, "Destination Sequence is NULL" );

        // get the quad corba object
        SequenceNamedObject* seqObj = NULL;
        seqObj = getSequence( sequence->getName() );
        TA_ASSERT( seqObj != NULL, "Sequence named object is NULL - when there is a local sequence object for it" );

        try
        {
            std::string session = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

            TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig sequenceConfig;
            sequenceConfig.dwellTime = dwellTime;
            sequenceConfig.inputKeySequence.length( cameraInputs.size() );

            int index = 0;

            for ( std::list< Camera* >::const_iterator iter = cameraInputs.begin();
                    iter != cameraInputs.end(); iter++, index++ )
            {
                TA_ASSERT( *iter != NULL, "New Camera list contains a NULL camera" );

                sequenceConfig.inputKeySequence[index] = ( *iter )->getKey();
            }

            CORBA_CALL( ( *seqObj ), setSequenceConfig, ( sequenceConfig, session.c_str(),con ) );//limin
        }
        catch ( TransactiveException& te )
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );

            throw;
        }
        catch ( TA_Base_Bus_GZL9::VideoSwitchAgentException& ex )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus_GZL9::VideoSwitchAgentException", ex.what.in() );

            TA_THROW( TransactiveException( ex.what.in() ) )
        }
        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while updating sequence: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

            TA_THROW( TransactiveException( "Could not save new sequence configuration." ) )
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown" );

            TA_THROW( TransactiveException( "Could not save new sequence configuration." ) )
        }

		/* comment these statement, it done by VideoDisplayManagerPresenter
        // update the local state
        sequence->setDwellTime( dwellTime );
        sequence->setCameras( cameraInputs );
        DisplayItemManager::getInstance().notifyGUIofDisplayItemUpdate( sequence );
		*/
    }


    bool SwitchAgentCommunicator::connectUserDefinedSeqToMonitor( Monitor* monitor,Sequence* sequence )
    {
        // Guard the method to ensure access by client code is synchronous
        FUNCTION_ENTRY( "connectUserDefinedSeqToMonitor" );
        ThreadGuard guard( m_singletonLock );

        TA_ASSERT( sequence != NULL, "Destination Sequence is NULL" );

        // get the quad corba object
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "connectUserDefinedSeqToMonitor( Monitor = %ld, Sequence = %ld)",
                     monitor->getKey() , sequence->getKey()  );
        std::list< Camera* > cameraInputs = sequence->getCameras();

        try
        {
            std::string session = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

            TA_Base_Bus_GZL9::VideoOutputCorbaDef::UserDefinedSequenceConfig sequenceConfig;
            sequenceConfig.dwellTime = sequence->getDwellTime();
            sequenceConfig.inputKeySequence.length( cameraInputs.size() );

            int index = 0;

            for ( std::list< Camera* >::const_iterator iter = cameraInputs.begin();
                    iter != cameraInputs.end() && index < MAX_VIDEO_SOURCES; iter++, index++ )
            {
                TA_ASSERT( *iter != NULL, "New Camera list contains a NULL camera" );

                sequenceConfig.inputKeySequence[index] = ( *iter )->getKey();
            }

            if ( monitor->isConsoleMonitor() )
            {
                VideoMonitorNamedObject* monObj = NULL;
                monObj = getConsoleMonitor( monitor->getName() );
                //     TA_ASSERT(monObj != NULL, "Destination Monitor named object is NULL - when there is a local monitor object for it");

                if ( monObj == NULL )
                {
                    AfxMessageBox( "远程监视器对象不存在" );
                    return false;
                }

                CORBA_CALL( ( *monObj ), connectSequence, ( sequenceConfig, session.c_str() ) );//limin
            }
            else
            {
                VideoMonitorNamedObject* monObj = NULL;
                monObj = getCommonMonitor( monitor->getName() );
                //     TA_ASSERT(monObj != NULL, "Destination Monitor named object is NULL - when there is a local monitor object for it");

                if ( monObj == NULL )
                {
                    AfxMessageBox( "远程监视器对象不存在" );
                    return false;
                }

                CORBA_CALL( ( *monObj ),connectSequence, ( sequenceConfig, session.c_str() ) );//limin
            }
        }
        catch ( TransactiveException& te )
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );

            throw;

        }
        catch ( TA_Base_Bus_GZL9::VideoSwitchAgentException& ex )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus_GZL9::VideoSwitchAgentException", ex.what.in() );

            // TA_THROW( TransactiveException(ex.what.in()) )
            throw ex;

        }

        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while updating sequence: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

            TA_THROW( TransactiveException( "Could not save new sequence configuration." ) )

        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown" );

            TA_THROW( TransactiveException( "Could not save new sequence configuration." ) )

        }

        // update the local state

        // tell the GUI to update
        //DisplayItemManager::getInstance().saveSequencesRelationship(sequence->getKey() );
        DisplayItemManager::getInstance().notifyGUIofDisplayItemUpdate( sequence );

        FUNCTION_EXIT;

        return true;
    }

    bool SwitchAgentCommunicator::disconnectUserDefinedSeqToMonitor( Monitor* monitor )
    {
        // Guard the method to ensure access by client code is synchronous
        FUNCTION_ENTRY( "disconnectUserDefinedSeqToMonitor" );
        ThreadGuard guard( m_singletonLock );

        TA_ASSERT( monitor != NULL, "Monitor is NULL" );

        // get the quad corba object
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "disconnectUserDefinedSeqToMonitor( Monitor = %ld)",
                     monitor->getKey()  );

        try
        {
            std::string session = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

            if ( monitor->isConsoleMonitor() )
            {
                VideoMonitorNamedObject* monObj = NULL;
                monObj = getConsoleMonitor( monitor->getName() );
                //     TA_ASSERT(monObj != NULL, "Destination Monitor named object is NULL - when there is a local monitor object for it");

                if ( monObj == NULL )
                {
                    AfxMessageBox( "远程监视器对象不存在" );
                    return false;
                }

                CORBA_CALL( ( *monObj ), disconnectSequence, ( session.c_str() ) );//limin
            }
            else
            {
                VideoMonitorNamedObject* monObj = NULL;
                monObj = getCommonMonitor( monitor->getName() );
                //   TA_ASSERT(monObj != NULL, "Destination Monitor named object is NULL - when there is a local monitor object for it");

                if ( monObj == NULL )
                {
                    AfxMessageBox( "远程监视器对象不存在" );
                    return false;
                }

                CORBA_CALL( ( *monObj ),disconnectSequence, ( session.c_str() ) );//limin
            }
        }
        catch ( TransactiveException& te )
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );

            throw;
        }
        catch ( TA_Base_Bus_GZL9::VideoSwitchAgentException& ex )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus_GZL9::VideoSwitchAgentException", ex.what.in() );

            // TA_THROW( TransactiveException(ex.what.in()) )
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( ex.what.in() );
        }
        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while updating sequence: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

            TA_THROW( TransactiveException( "Could not save new sequence configuration." ) )
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown" );

            TA_THROW( TransactiveException( "Could not save new sequence configuration." ) )
        }

        // update the local state

        // tell the GUI to update
        DisplayItemManager::getInstance().notifyGUIofDisplayItemUpdate( monitor );

        FUNCTION_EXIT;

        return true;
    }

    void SwitchAgentCommunicator::setSequenceDescription( Sequence* sequence,
                                                          const std::string& newDescription )
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        TA_ASSERT( sequence != NULL, "Destination Sequence is NULL" );

        // get the quad corba object
        SequenceNamedObject* seqObj = NULL;
        seqObj = getSequence( sequence->getName() );
        TA_ASSERT( seqObj != NULL, "Sequence named object is NULL - when there is a local sequence object for it" );

        try
        {
            std::string session = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

            CORBA_CALL( ( *seqObj ), setSequenceDescription, ( newDescription.c_str(), session.c_str() ) );//limin
        }
        catch ( TransactiveException& te )
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );

            throw;
        }
        catch ( TA_Base_Bus_GZL9::VideoSwitchAgentException& ex )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus_GZL9::VideoSwitchAgentException", ex.what.in() );

            // TA_THROW( TransactiveException(ex.what.in()) )
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( ex.what.in() );
        }
        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while updating sequence description: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

            TA_THROW( TransactiveException( "Could not save new sequence description" ) )
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown" );

            TA_THROW( TransactiveException( "Could not save new sequence description" ) )
        }

		/* comment these statements, it done by VideoDisplayManagerPresenter
        // update the local state
        sequence->setSequenceDescription( newDescription ); // TD16691
        // tell the GUI to update
        DisplayItemManager::getInstance().notifyGUIofDisplayItemUpdate( sequence );
		*/
    }


    void SwitchAgentCommunicator::clearMonitor( Monitor* monitor )
    {
        // Guard the method to ensure access by client code is synchronous
        FUNCTION_ENTRY( "clearMonitor" );
        ThreadGuard guard( m_singletonLock );

        TA_ASSERT( monitor != NULL, "Destination Monitor is NULL" );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "clearMonitor( Monitor = %ld)",
                     monitor->getKey()  );

        try
        {
            std::string session = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

            if ( monitor->isConsoleMonitor() )
            {
                VideoMonitorNamedObject* monObj = NULL;
                monObj = getConsoleMonitor( monitor->getName() );
                //    TA_ASSERT(monObj != NULL, "Destination Monitor named object is NULL - when there is a local monitor object for it");

                if ( monObj == NULL )
                {
                    AfxMessageBox( "远程监视器对象不存在" );
                    throw( "远程监视器对象不存在" );
                    return;
                }

                CORBA_CALL( ( *monObj ), requestSwitchOfInput, ( 0, session.c_str(), true ) );//limin

            }
            else
            {
                VideoMonitorNamedObject* monObj = NULL;
                monObj = getCommonMonitor( monitor->getName() );
                //   TA_ASSERT(monObj != NULL, "Destination Monitor named object is NULL - when there is a local monitor object for it");

                if ( monObj == NULL )
                {
                    AfxMessageBox( "远程监视器对象不存在" );
                    throw( "远程监视器对象不存在" );
                }

                CORBA_CALL( ( *monObj ), requestSwitchOfInput, ( 0, session.c_str(), true ) );//limin
            }
        }
        catch ( TransactiveException& te )
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );

            throw;
        }
        catch ( TA_Base_Bus_GZL9::VideoSwitchAgentException& ex )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus_GZL9::VideoSwitchAgentException", ex.what.in() );

            // TA_THROW( TransactiveException(ex.what.in()) )
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( ex.what.in() );
        }
        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while clearing monitor: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

            // TD16879
            // TA_THROW( TransactiveException("Could not clear monitor.") )
            TA_THROW( TransactiveException( "UE-090037" ) )
            // TD16879
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown" );

            TA_THROW( TransactiveException( "Could not clear monitor." ) )
        }

        // update the state objects
        monitor->assignInput( NULL );

        // tell the GUI to update its display
        DisplayItemManager::getInstance().notifyGUIofDisplayItemUpdate( monitor );

        FUNCTION_EXIT;
    }


    void SwitchAgentCommunicator::controlSequencePlayback( Monitor* monitor,
                                                           ESequencePlaybackCommandType playbackCmd )
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        TA_ASSERT( monitor != NULL, "Destination Monitor is NULL" );

        try
        {
            std::string session = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

            if ( monitor->isConsoleMonitor() )
            {
                VideoMonitorNamedObject* monObj = NULL;
                monObj = getConsoleMonitor( monitor->getName() );
                TA_ASSERT( monObj != NULL, "Monitor named object is NULL - when there is a local sequence object for it" );

                switch ( playbackCmd )
                {

                    case SEQUENCE_HOLD:
                        CORBA_CALL( ( *monObj ), pause, ( session.c_str() ) );//limin
                        break;

                    case SEQUENCE_SKIP_FORWARD:
                        CORBA_CALL( ( *monObj ), cycleToNextVideoInput, ( session.c_str() ) );//limin
                        break;

                    case SEQUENCE_SKIP_BACKWARD:
                        CORBA_CALL( ( *monObj ), cycleToPreviousVideoInput, ( session.c_str() ) );//limin
                        break;

                    case SEQUENCE_PLAY:
                        CORBA_CALL( ( *monObj ), play, ( session.c_str() ) );//limin
                        break;

                    default:
                        break; // do nothing
                }
            }
            else
            {
                VideoMonitorNamedObject* monObj = NULL;
                monObj = getCommonMonitor( monitor->getName() );
                TA_ASSERT( monObj != NULL, "Monitor named object is NULL - when there is a local sequence object for it" );

                switch ( playbackCmd )
                {

                    case SEQUENCE_HOLD:
                        CORBA_CALL( ( *monObj ), pause, ( session.c_str() ) );//limin
                        break;

                    case SEQUENCE_SKIP_FORWARD:
                        CORBA_CALL( ( *monObj ), cycleToNextVideoInput, ( session.c_str() ) );//limin
                        break;

                    case SEQUENCE_SKIP_BACKWARD:
                        CORBA_CALL( ( *monObj ), cycleToPreviousVideoInput, ( session.c_str() ) );//limin
                        break;

                    case SEQUENCE_PLAY:
                        CORBA_CALL( ( *monObj ), play, ( session.c_str() ) );//limin
                        break;

                    default:
                        break; // do nothing
                }
            }

        }
        catch ( TransactiveException& te )
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );

            throw;
        }
        catch ( TA_Base_Bus_GZL9::VideoSwitchAgentException& ex )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus_GZL9::VideoSwitchAgentException", ex.what.in() );

            // TA_THROW( TransactiveException(ex.what.in()) )
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( ex.what.in() );
        }
        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while controlling sequence: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

            // TD16879
            // TA_THROW( TransactiveException("Could not control sequence playback.") )
            TA_THROW( TransactiveException( "UE-090037" ) )
            // TD16879
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown" );

            TA_THROW( TransactiveException( "Could not control sequence playback." ) )
        }

        // tell the GUI to update
        //DisplayItemManager::getInstance().notifyGUIofDisplayItemUpdate(sequence);
    }


    /*   void SwitchAgentCommunicator::setActiveTrainID( BvsStage* bvsStage, unsigned short trainID )
       {
           // Guard the method to ensure access by client code is synchronous
           ThreadGuard guard( m_singletonLock );

           TA_ASSERT(bvsStage != NULL, "Destination Stage is NULL");

           // get the stage corba object
           BVSStageNamedObject* stageObj = NULL;
           stageObj = getStage(bvsStage->getName());
           TA_ASSERT(stageObj != NULL, "Stage named object is NULL - when there is a local stage object for it");

           try
           {
               CORBA_CALL( ( *stageObj ), setActiveTrain, ( static_cast< unsigned char >( trainID ) ) );//limin
           }
           catch (TransactiveException& te)
           {
               // object resolution etc
               LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );

               throw;
           }
           catch (TA_Base_Bus_GZL9::VideoSwitchAgentException& ex)
           {
               LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus_GZL9::VideoSwitchAgentException", ex.what.in() );

               TA_THROW( TransactiveException(ex.what.in()) )
           }
           catch( const CORBA::Exception& cex )
           {
            std::string exceptionMsg( "Exception thrown while setting active train: " );
             exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

               TA_THROW( TransactiveException("Unable to associate active train with BVS Stage.") )
           }
           catch (...)
           {
               LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown" );

               TA_THROW( TransactiveException("Unable to associate active train with BVS Stage.") )
           }

           // update the local state
           bvsStage->setActiveTrain(trainID);

           // tell the GUI to update
           DisplayItemManager::getInstance().notifyGUIofDisplayItemUpdate(bvsStage);
       }
    */
    /*
        void SwitchAgentCommunicator::clearStages( const std::vector<BvsStage*>& stages )
        {
            for (std::vector<BvsStage*>::const_iterator iter = stages.begin(); iter != stages.end(); ++iter)
            {
                setActiveTrainID(*iter, 0);
            }
        }
    */
    /* bool SwitchAgentCommunicator::isBvsInAlarmStack(BvsStage *bvsStage)
     {
      ThreadGuard guard( m_singletonLock );

            TA_ASSERT(bvsStage != NULL, "Destination Stage is NULL");

            // get the stage corba object
            BVSStageNamedObject* stageObj = NULL;
            stageObj = getStage(bvsStage->getName());
            TA_ASSERT(stageObj != NULL, "Stage named object is NULL - when there is a local stage object for it");

            try
            {
                CORBA::Boolean ret;//limin
                CORBA_CALL_RETURN( ret, ( *stageObj ), isInAnAlarmStack, () );//limin
                return static_cast< bool >( ret );//limin
            }
            catch (TransactiveException& te)
            {
                // object resolution etc
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );

                return false;
            }
            catch (TA_Base_Bus_GZL9::VideoSwitchAgentException& ex)
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus_GZL9::VideoSwitchAgentException", ex.what.in() );

                return false;
            }
            catch( const CORBA::Exception& cex )
            {
             std::string exceptionMsg( "Exception thrown while setting active train: " );
              exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

             LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

                return false;
            }
            catch (...)
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown" );

                return false;
            }
        }



        void SwitchAgentCommunicator::loadBVSNamedObjects()
        {
            TA_Base_Bus_GZL9::VideoSwitchAgentFactory::BVSStageVector bvsStageVec = TA_Base_Bus_GZL9::VideoSwitchAgentFactory::getInstance().getBVSStages();
            TA_Base_Bus_GZL9::VideoSwitchAgentFactory::BVSStageVector::iterator bvsStageVecIt = bvsStageVec.begin();
            for (; bvsStageVecIt != bvsStageVec.end(); bvsStageVecIt++ )
            {
                String_var address;
                CORBA_CALL_RETURN( address, ( *( *bvsStageVecIt ) ), getAddress, () );//limin // Stage ID
                unsigned short stageID = static_cast< unsigned short >( strtoul( address.in(), NULL, 10 ) );
                if ( 0 != stageID )
                {
                    String_var name;
                    CORBA_CALL_RETURN( name, ( *( *bvsStageVecIt ) ), getName, () );//limin
                    m_bvsStageToNamedObject[ name.in() ]  = ( *bvsStageVecIt );
                }
            }
        }
    */

    void SwitchAgentCommunicator::loadCameraNamedObjects()
    {
        TA_Base_Bus_GZL9::VideoSwitchAgentFactory::CameraVector camVec = TA_Base_Bus_GZL9::VideoSwitchAgentFactory::getInstance().getCameras();
        for(TA_Base_Bus_GZL9::VideoSwitchAgentFactory::CameraVector::iterator camVecIt = camVec.begin(); camVecIt != camVec.end(); ++camVecIt)
        {
            String_var name;
            CORBA_CALL_RETURN( name, ( *( *camVecIt ) ), getName, () );//limin
            m_camToNamedObject[ name.in() ] = ( *camVecIt );
        }
    }


    void SwitchAgentCommunicator::loadQuadNamedObjects()
    {
        TA_Base_Bus_GZL9::VideoSwitchAgentFactory::QuadVector quadVec = TA_Base_Bus_GZL9::VideoSwitchAgentFactory::getInstance().getQuads();
        TA_Base_Bus_GZL9::VideoSwitchAgentFactory::QuadVector::iterator quadVecIt = quadVec.begin();

        for (; quadVecIt != quadVec.end(); quadVecIt++ )
        {
            String_var name;
            CORBA_CALL_RETURN( name, ( *( *quadVecIt ) ), getName, () );//limin
            m_quadToNamedObject[ name.in() ] = ( *quadVecIt );
        }
    }


    void SwitchAgentCommunicator::loadSequenceNamedObjects()
    {
        TA_Base_Bus_GZL9::VideoSwitchAgentFactory::SequenceVector seqVec = TA_Base_Bus_GZL9::VideoSwitchAgentFactory::getInstance().getSequences();
        TA_Base_Bus_GZL9::VideoSwitchAgentFactory::SequenceVector::iterator seqVecIt = seqVec.begin();
        std::vector< unsigned long > sequenceKeys;

        for (; seqVecIt != seqVec.end(); seqVecIt++ )
        {
            String_var name;
            CORBA_CALL_RETURN( name, ( *( *seqVecIt ) ), getName, () );//limin
            m_sequenceToNamedObject[ name.in() ] = ( *seqVecIt );
            unsigned long key;
            CORBA_CALL_RETURN( key, ( *( *seqVecIt ) ), getKey, () );//limin
            sequenceKeys.push_back( key );
        }
    }


    void SwitchAgentCommunicator::loadMonitorNamedObjects()
    {
        // build the map of console monitors
        //bihui modified
        TA_Base_Bus_GZL9::VideoSwitchAgentFactory::VideoMonitorVector videoMonVec =
            TA_Base_Bus_GZL9::VideoSwitchAgentFactory::getInstance().getVideoMonitors();

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "loadMonitorNamedObjects: consoleMonVec is %s",videoMonVec.empty()?"Empty":"Not empty"  );


        for ( TA_Base_Bus_GZL9::VideoSwitchAgentFactory::VideoMonitorVector::iterator commonIt = videoMonVec.begin();
                commonIt != videoMonVec.end(); commonIt++ )
        {
            String_var name;
            unsigned long consoleKey ;
            CORBA_CALL_RETURN( name, ( *( *commonIt ) ), getName, () );//limin

            CORBA_CALL_RETURN( consoleKey, ( *( *commonIt ) ), getConsoleKey, () );//limin
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Loaded MonitorNamedObject:%s",name );
            // m_commonMonitorToNamedObject[ name.in() ] = ( *commonIt );
            // VideoMonitorNamedObject* monobj=dynamic_cast<VideoMonitorNamedObject*> (*commonIt);

            if ( consoleKey > 0 )
            {

                m_consoleMonitorToNamedObject[ name.in() ] = ( *commonIt );
            }
            else
            {

                //     VideoOutputNamedObject* monobj=dynamic_cast<VideoOutputNamedObject*> (*commonIt);
                m_commonMonitorToNamedObject[ name.in() ] = ( *commonIt  );
            }
        }

        /*
              TA_Base_Bus_GZL9::VideoSwitchAgentFactory::VideoMonitorVector consoleMonVec =
                  TA_Base_Bus_GZL9::VideoSwitchAgentFactory::getInstance().getLocalConsoleMonitors();
        LOG( SourceInfo, DebugUtil::ExceptionCatch , "loadMonitorNamedObjects: consoleMonVec is ",consoleMonVec.empty()?"Empty":"Not empty" );
              int i(0);
        for (TA_Base_Bus_GZL9::VideoSwitchAgentFactory::VideoMonitorVector::iterator it = consoleMonVec.begin();
              it != consoleMonVec.end(); it++)
              {
                  String_var name;
                  CORBA_CALL_RETURN( name, ( *( *it ) ), getName, () );//limin
         LOG( SourceInfo, DebugUtil::ExceptionCatch, "loadMonitorNamedObjects: name: ",name);
                  m_consoleMonitorToNamedObject[ name.in() ] = ( *it );
              }

              // build the map of other monitors

              TA_Base_Bus_GZL9::VideoSwitchAgentFactory::VideoOutputGroupVector groups =
                  TA_Base_Bus_GZL9::VideoSwitchAgentFactory::getInstance().getVideoOutputGroups();
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "loadMonitorNamedObjects: VideoOutputGroupVector groups ",groups.empty()?"Empty": "Not Empty" );
        i=0;
              TA_Base_Bus_GZL9::VideoSwitchAgentFactory::VideoOutputGroupVector::iterator grpsIt = groups.begin();
              for (; grpsIt != groups.end(); grpsIt++ )
              {
                  unsigned long key;

         CORBA_CALL_RETURN( key, ( *( *grpsIt ) ), getKey, () );//limin
        // LOG( SourceInfo, DebugUtil::ExceptionCatch, "loadMonitorNamedObjects: key[%d]:[%ld]",i++,key);
                  delete ( *grpsIt );
                  TA_Base_Bus_GZL9::VideoSwitchAgentFactory::VideoOutputVector mons =
                      TA_Base_Bus_GZL9::VideoSwitchAgentFactory::getInstance().getVideoOutputsForGroup( key );
         int j=0;
                  for (TA_Base_Bus_GZL9::VideoSwitchAgentFactory::VideoOutputVector::iterator commonIt = mons.begin();
                  commonIt != mons.end(); commonIt++)
                  {
                      String_var name;
                      CORBA_CALL_RETURN( name, ( *( *commonIt ) ), getName, () );//limin
          LOG( SourceInfo, DebugUtil::ExceptionCatch, "loadMonitorNamedObjects: name",name);
                      m_commonMonitorToNamedObject[ name.in() ] = ( *commonIt );
                  }

              }*/
    }


    SwitchAgentCommunicator::CamToNamedObject SwitchAgentCommunicator::getCameraObjects()
    {
        if ( m_camToNamedObject.size() == 0 )
        {
            loadCameraNamedObjects();
        }

        return m_camToNamedObject;
    }

    SwitchAgentCommunicator::QuadToNamedObject SwitchAgentCommunicator::getQuadObjects()
    {
        if ( m_quadToNamedObject.size() == 0 )
        {
            loadQuadNamedObjects();
        }

        return m_quadToNamedObject;
    }

    SwitchAgentCommunicator::SequenceToNamedObject SwitchAgentCommunicator::getSequenceObjects()
    {
        if ( m_sequenceToNamedObject.size() == 0 )
        {
            loadSequenceNamedObjects();
        }

        return m_sequenceToNamedObject;
    }

    /*
        SwitchAgentCommunicator::BVSStageToNamedObject SwitchAgentCommunicator::getStageObjects()
        {
            if (m_bvsStageToNamedObject.size() == 0)
            {
          loadBVSNamedObjects();
            }

            return m_bvsStageToNamedObject;
        }
    */
    SwitchAgentCommunicator::MonitorToNamedObject SwitchAgentCommunicator::getConsoleMonitorObjects()
    {
        if ( m_consoleMonitorToNamedObject.size() == 0 )
        {
            loadMonitorNamedObjects();
        }

        return m_consoleMonitorToNamedObject;
    }

    SwitchAgentCommunicator::MonitorToNamedObject SwitchAgentCommunicator::getCommonMonitorObjects()
    {
        if ( m_commonMonitorToNamedObject.size() == 0 )
        {
            loadMonitorNamedObjects();
        }

        return m_commonMonitorToNamedObject;
    }

    void SwitchAgentCommunicator::updateSequence( Sequence* sequence )
    {
        try
        {
            if ( m_sequenceToNamedObject.size() == 0 )
            {
                try
                {
                    loadSequenceNamedObjects();
                }
                catch ( ... )
                {
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unknown error when loading sequence name object." );
                }
            }

            SequenceToNamedObject::iterator iter = m_sequenceToNamedObject.find( sequence->getName() );

            TA_ASSERT( iter != m_sequenceToNamedObject.end(), "Sequence cannot be null - there is a corresponding state object for it" );

            // the only 2 things that can change are the description and the read only flag

            // TD16691
            String_var description;
            CORBA_CALL_RETURN( description, ( *( iter->second ) ), getSequenceDescription, () );//limin
            sequence->setSequenceDescription( description.in() );
            // TD16691

            CORBA::Boolean ret;
            CORBA_CALL_RETURN( ret, ( *( iter->second ) ), isReadOnly, () );//limin
            sequence->setReadOnly( ret );
        }
        catch ( TransactiveException& te )
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );
        }
        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while updating sequence: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );
        }
    }


    VideoMonitorNamedObject* SwitchAgentCommunicator::getCommonMonitor( const std::string name )
    {
        if ( m_commonMonitorToNamedObject.size() == 0 )
        {
            try
            {
                loadMonitorNamedObjects();
            }
            catch ( ... )
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unknown error when loading monitor name object." );
            }
        }

        MonitorToNamedObject::iterator it = m_commonMonitorToNamedObject.find( name );

        if ( it != m_commonMonitorToNamedObject.end() )
        {
            return it->second;
        }

        return NULL;
    }


    VideoMonitorNamedObject* SwitchAgentCommunicator::getConsoleMonitor( const std::string name )
    {
        if ( m_consoleMonitorToNamedObject.size() == 0 )
        {
            try
            {
                loadMonitorNamedObjects();
            }
            catch ( ... )
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unknown error when loading monitor name object." );

            }
        }

        MonitorToNamedObject::iterator it = m_consoleMonitorToNamedObject.find( name );

        if ( it != m_consoleMonitorToNamedObject.end() )
        {
            return it->second;
        }

        return NULL;
    }


    QuadNamedObject* SwitchAgentCommunicator::getQuad( const std::string name )
    {
        if ( m_quadToNamedObject.size() == 0 )
        {
            try
            {
                loadQuadNamedObjects();
            }
            catch ( ... )
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unknown error when loading Quad name object." );
            }
        }

        QuadToNamedObject::iterator it = m_quadToNamedObject.find( name );

        if ( it != m_quadToNamedObject.end() )
        {
            return it->second;
        }

        return NULL;
    }


    CameraNamedObject* SwitchAgentCommunicator::getCamera( const std::string name )
    {
        CamToNamedObject::iterator it = m_camToNamedObject.find( name );

        if ( it != m_camToNamedObject.end() )
        {
            return it->second;
        }

        return NULL;
    }




    SequenceNamedObject* SwitchAgentCommunicator::getSequence( const std::string name )
    {
        if ( m_sequenceToNamedObject.size() == 0 )
        {
            try
            {
                loadSequenceNamedObjects();
            }
            catch ( ... )
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unknown error when loading sequence name object." );
            }
        }

        SequenceToNamedObject::iterator it = m_sequenceToNamedObject.find( name );

        if ( it != m_sequenceToNamedObject.end() )
        {
            return it->second;
        }

        return NULL;
    }

    /*
        BVSStageNamedObject* SwitchAgentCommunicator::getStage(const std::string name)
        {
            if (m_bvsStageToNamedObject.size() == 0)
            {
       try
       {
        loadBVSNamedObjects();
       }
       catch (...)
       {
        LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Unknown error when loading bvs name object.");
       }
            }

      BVSStageToNamedObject::iterator it = m_bvsStageToNamedObject.find( name );
            if ( it != m_bvsStageToNamedObject.end() )
            {
                return it->second;
            }

            return NULL;
        }
    */

    void SwitchAgentCommunicator::zoomCamera ( Camera* camera, unsigned short zoomDirection  )
    {
        FUNCTION_ENTRY( "zoomCamera" );
        CameraNamedObject* cameraObj= NULL;
        getCameraObjects();
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "zoomCamera( Camera = %ld,zoomDirection=%d)",
                     camera->getKey() ,zoomDirection );

        try
        {
            cameraObj = getCamera( camera->getName() );

            if ( cameraObj!=NULL )
            {
                CORBA_CALL( ( *cameraObj ), CameraZoom, ( zoomDirection ) );//limin
            }
            else
            {
                AfxMessageBox( "摄像机对象不存在" );
                return;
            }
        }
        catch ( TransactiveException& te )
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );

            throw;
        }
        catch ( TA_Base_Bus_GZL9::VideoSwitchAgentException& ex )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus_GZL9::VideoSwitchAgentException", ex.what.in() );

            // TA_THROW( TransactiveException(ex.what.in()) )
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( ex.what.in() );
        }
        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while setting active train: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

            return;
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown" );

            return;
        }

        FUNCTION_EXIT;
    }


    void SwitchAgentCommunicator::panCamera ( Camera* camera, unsigned short panDirection , unsigned short speed )
    {
        FUNCTION_ENTRY( "panCamera" );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "panCamera( Camera = %ld,panDirection=%d,speed = %d)",
                     camera->getKey() ,panDirection, speed );
        CameraNamedObject* cameraObj= NULL;

        try
        {
            getCameraObjects();
            cameraObj = getCamera( camera->getName() );

            if ( cameraObj!=NULL )
            {
                CORBA_CALL( ( *cameraObj ), CameraPan, ( panDirection ,speed ) );//limin
            }
            else
            {
                AfxMessageBox( "摄像机对象不存在" );
                return;
            }
        }
        catch ( TransactiveException& te )
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );

            throw;
        }
        catch ( TA_Base_Bus_GZL9::VideoSwitchAgentException& ex )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus_GZL9::VideoSwitchAgentException", ex.what.in() );

            // TA_THROW( TransactiveException(ex.what.in()) )
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( ex.what.in() );
        }
        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while setting active train: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

            return;
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown" );

            return;
        }

        FUNCTION_EXIT;
    }


    void SwitchAgentCommunicator::tiltCamera ( Camera* camera, unsigned short tiltDirection, unsigned short speed )
    {
        FUNCTION_ENTRY( "tiltCamera" );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "tiltCamera( Camera = %ld,tiltDirection=%d,speed = %d)",
                     camera->getKey() ,tiltDirection, speed );
        CameraNamedObject* cameraObj= NULL;

        try
        {
            getCameraObjects();
            cameraObj = getCamera( camera->getName() );

            if ( cameraObj!=NULL )
            {
                CORBA_CALL( ( *cameraObj ), CameraTilt, ( tiltDirection ,speed ) );//limin
            }
            else
            {
                AfxMessageBox( "摄像机对象不存在" );
                return;
            }
        }
        catch ( TransactiveException& te )
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );

            throw;
        }
        catch ( TA_Base_Bus_GZL9::VideoSwitchAgentException& ex )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus_GZL9::VideoSwitchAgentException", ex.what.in() );

            // TA_THROW( TransactiveException(ex.what.in()) )
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( ex.what.in() );
        }
        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while setting active train: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

            return;
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown" );

            return;
        }

        FUNCTION_EXIT;
    }

    void SwitchAgentCommunicator::panAndTiltCamera ( Camera* camera, unsigned short panDirection, unsigned short tiltDirection, unsigned short panSpeed , unsigned short tiltSpeed )
    {
        FUNCTION_ENTRY( "panAndTiltCamera" );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "tiltCamera( Camera = %ld,panDirection=%d,tiltDirection = %d,panSpeed,tiltSpeed =%d)",
                     camera->getKey() ,panDirection,tiltDirection, panSpeed,tiltSpeed );
        CameraNamedObject* cameraObj= NULL;

        try
        {
            getCameraObjects();
            cameraObj = getCamera( camera->getName() );

            if ( cameraObj!=NULL )
            {
                CORBA_CALL( ( *cameraObj ), CameraPanAndTilt, ( panDirection , panSpeed ,tiltDirection ,tiltSpeed ) );//limin
            }
            else
            {
                AfxMessageBox( "摄像机对象不存在" );
                return;
            }
        }
        catch ( TransactiveException& te )
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );

            throw;
        }
        catch ( TA_Base_Bus_GZL9::VideoSwitchAgentException& ex )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus_GZL9::VideoSwitchAgentException", ex.what.in() );

            // TA_THROW( TransactiveException(ex.what.in()) )
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( ex.what.in() );
        }
        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while setting active train: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

            return;
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown" );

            return;
        }

        FUNCTION_EXIT;
    }








} // namespace TA_IRS_App