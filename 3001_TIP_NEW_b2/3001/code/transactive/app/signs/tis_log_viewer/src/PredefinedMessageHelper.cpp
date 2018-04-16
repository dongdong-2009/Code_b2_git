/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_log_viewer/src/PredefinedMessageHelper.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This is used to load the current TTIS pre-defined messages.
  * Messages are organised into normal and emergency priorities, and
  * indexed by name and Id.
  *
  */
#include "boost/shared_ptr.hpp"

#include "app/signs/tis_log_viewer/src/PredefinedMessageHelper.h"
#include "app/signs/tis_log_viewer/src/UserMessages.h"


#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryAccessFactory.h"
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"

#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"


#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/ScadaProxyException.h"

#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"


using TA_Base_Core::ILocation;
using TA_Base_Core::LocationAccessFactory;

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;

using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;

using TA_Base_Bus::TISAgentAccessFactory;

#pragma warning ( disable : 4786 )
#pragma warning ( disable : 4290 )

namespace
{
    const std::string CURRENT_STATION_LIBRARY_VERSION_DP_NAME(".TIS.ISCS.ISCS.aiiTISC-CurrentSTISLibraryVersion");
    const std::string CURRENT_TRAIN_LIBRARY_VERSION_DP_NAME(".TIS.ISCS.ISCS.aiiTISC-CurrentTTISLibraryVersion");
}


namespace TA_IRS_App
{
    
    PredefinedMessageHelper* PredefinedMessageHelper::m_theClassInstance;
    TA_Base_Core::ReEntrantThreadLockable PredefinedMessageHelper::m_singletonLock;


    PredefinedMessageHelper& PredefinedMessageHelper::getInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

	    // If guard acquired then make sure the singleton is still NULL.

	
	    if ( m_theClassInstance == NULL )
	    {
		    // Create the one & only object
            m_theClassInstance = new PredefinedMessageHelper();
	    }
	    
	    return *m_theClassInstance;
    }


    PredefinedMessageHelper::PredefinedMessageHelper()
        : m_ttisNormalMessages(),
          m_ttisEmergencyMessages(),
          m_stisNormalMessages(),
          m_stisEmergencyMessages(),
          m_messagesLoaded(false),
          m_tTisVersion(0),
          m_tSisVersion(0)
    {
        // register for library version changes
        registerForLibraryChanges();
		
    }


    unsigned short PredefinedMessageHelper::getTtisLibraryVersion()
    {
        // load if not already loaded
        if (!m_messagesLoaded)
        {
            loadPredefinedMessages();
        }

        return m_tTisVersion;
    }


    unsigned short PredefinedMessageHelper::getStisLibraryVersion()
    {
        // load if not already loaded
        if (!m_messagesLoaded)
        {
            loadPredefinedMessages();
        }

        return m_tSisVersion;
    }


    std::string PredefinedMessageHelper::getTtisMessage(TA_Base_Core::ELibrarySection librarySection, int messageId)
    {
        if ( librarySection == TA_Base_Core::EMERGENCY_SECTION )
        {
            return getMessage(m_ttisEmergencyMessages, messageId);
        }
        else if ( librarySection == TA_Base_Core::NORMAL_SECTION )
        {
            return getMessage(m_ttisNormalMessages, messageId);
        }

        return "";
    }


    std::string PredefinedMessageHelper::getStisMessage(TA_Base_Core::ELibrarySection librarySection, int messageId)
    {
        if ( librarySection == TA_Base_Core::EMERGENCY_SECTION )
        {
            return getMessage(m_stisEmergencyMessages, messageId);
        }
        else if ( librarySection == TA_Base_Core::NORMAL_SECTION )
        {
            return getMessage(m_stisNormalMessages, messageId);
        }

        return "";
    }


    std::string PredefinedMessageHelper::getMessage(std::map<unsigned short, std::string>& messageMap, int messageId)
    {
        // load if not already loaded
        if (!m_messagesLoaded)
        {
            loadPredefinedMessages();
        }

        std::map<unsigned short, std::string>::iterator findIter = messageMap.find(messageId);

        // if its found
        if ( findIter != messageMap.end() )
        {
            // return the text
            return findIter->second;
        }
        // otherwise, an empty string
        else
        {
            return std::string("");
        }
    }


    void PredefinedMessageHelper::loadPredefinedMessages()
    {
        bool errorsEncountered = false;
        std::string errorMessages;

        try
        {
            TA_Base_Core::IPredefinedMessageLibrary* ttisMessageLibrary = NULL;

            try
            {
                // Establish the current predefined message library versions
                CORBA_CALL_RETURN( m_tTisVersion, 
                    TISAgentAccessFactory::getInstance().getSTISAgent(), getCurrentTTISMessageLibraryVersion, () );//limin

                // get the TTIS message libraries
                TA_Base_Core::IPredefinedMessageLibrary* ttisMessageLibrary = NULL;
                ttisMessageLibrary = 
                    TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary(m_tTisVersion, "TTIS");

                // a vector of pointers to pre-defined message structures
                TA_Base_Core::IPredefinedMessageLibrary::PredefinedMessageList ttisPredefinedMessages
                    = ttisMessageLibrary->getMessages();

                // build the pre-defined message maps
                // clear them first
                m_ttisNormalMessages.clear();
                m_ttisEmergencyMessages.clear();

                for (TA_Base_Core::IPredefinedMessageLibrary::PredefinedMessageList::iterator ttisIter = ttisPredefinedMessages.begin();
                     ttisIter != ttisPredefinedMessages.end(); ttisIter++)
                {
                    // if the current message is normal
                    if ( (*ttisIter)->librarySection == TA_Base_Core::NORMAL_SECTION )
                    {
                        m_ttisNormalMessages[ (*ttisIter)->messageTag ] = (*ttisIter)->message;
                    }
                    // otherwise if its emergency
                    else if ( (*ttisIter)->librarySection == TA_Base_Core::EMERGENCY_SECTION )
                    {
                        m_ttisEmergencyMessages[ (*ttisIter)->messageTag ] = (*ttisIter)->message;
                    }
                    // otherwise its unknown
                    else
                    {
                        LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                            "TTIS Predefined message with tag %d is from an unknown library section %d.",
                            (*ttisIter)->messageTag, (*ttisIter)->librarySection );
                    }
                }


                if ( ttisMessageLibrary != NULL )
                {
                    delete ttisMessageLibrary;
                    ttisMessageLibrary = NULL;
                }

            }
            catch (...)
            {
                if ( ttisMessageLibrary != NULL )
                {
                    delete ttisMessageLibrary;
                    ttisMessageLibrary = NULL;
                }

                errorsEncountered = true;

                throw;
            }
        }
        catch ( const DataException& de)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_IRS_Core::DataException", de.what() );

            // this means there is an error with the data
            errorMessages.append( "Error while loading TTIS predefined messages.\n"
                                  "TTIS Predefined message text will not be shown." );

        }
        catch ( const DatabaseException& dbe)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_IRS_Core::DatabaseException", dbe.what() );

            // this means that there is a problem with the database
            errorMessages.append( "Error while loading TTIS predefined messages.\n"
                                  "TTIS Predefined message text will not be shown." );
        }
        catch ( const TA_Base_Core::ObjectResolutionException& ore )
	    {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::ObjectResolutionException", ore.what() );

            errorMessages.append( "Error connecting to the TIS agent while loading TTIS predefined messages.\n"
                                  "TTIS Predefined message text will not be shown." );
	    }
        catch ( const CORBA::Exception& cex )
        {
	        std::string exceptionMsg( "Exception thrown while calling submitClearRequest: " );
		        exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );
	        LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

            errorMessages.append( "Error connecting to the TIS agent while loading TTIS predefined messages.\n"
                                  "TTIS Predefined message text will not be shown." );
        }


        try
        {
            TA_Base_Core::IPredefinedMessageLibrary* stisMessageLibrary = NULL;

            try
            {
                // Establish the current predefined message library versions
                CORBA_CALL_RETURN( m_tSisVersion, 
                    TISAgentAccessFactory::getInstance().getSTISAgent(), getCurrentSTISMessageLibraryVersion, () );//limin

                // get the STIS message libraries
                TA_Base_Core::IPredefinedMessageLibrary* stisMessageLibrary = NULL;
                stisMessageLibrary = 
                    TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary(m_tSisVersion, "STIS");


                // a vector of pointers to pre-defined message structures
                TA_Base_Core::IPredefinedMessageLibrary::PredefinedMessageList stisPredefinedMessages
                    = stisMessageLibrary->getMessages();

                // if execution got here, all libraries are loaded.
                // no more exceptions can be thrown


                // build the pre-defined message maps
                // clear them first
                m_stisNormalMessages.clear();
                m_stisEmergencyMessages.clear();

                for (TA_Base_Core::IPredefinedMessageLibrary::PredefinedMessageList::iterator stisIter = stisPredefinedMessages.begin();
                stisIter != stisPredefinedMessages.end(); stisIter++)
                {
                    // if the current message is normal
                    if ( (*stisIter)->librarySection == TA_Base_Core::NORMAL_SECTION )
                    {
                        m_stisNormalMessages[ (*stisIter)->messageTag ] = (*stisIter)->message;
                    }
                    // otherwise if its emergency
                    else if ( (*stisIter)->librarySection == TA_Base_Core::EMERGENCY_SECTION )
                    {
                        m_stisEmergencyMessages[ (*stisIter)->messageTag ] = (*stisIter)->message;
                    }
                    // otherwise its unknown
                    else
                    {
                        LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                            "STIS Predefined message with tag %d is from an unknown library section %d.",
                            (*stisIter)->messageTag, (*stisIter)->librarySection );
                    }
                }

                if ( stisMessageLibrary != NULL )
                {
                    delete stisMessageLibrary;
                    stisMessageLibrary = NULL;
                }
            }
            catch (...)
            {
                if ( stisMessageLibrary != NULL )
                {
                    delete stisMessageLibrary;
                    stisMessageLibrary = NULL;
                }

                // ther is previous error message text
                if (errorsEncountered)
                {
                    // insert a new line
                    errorMessages.append( "\n" );
                }

                errorsEncountered = true;

                throw;
            }
        }
        catch ( const DataException& de)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_IRS_Core::DataException", de.what() );

            // this means there is an error with the data
            errorMessages.append( "Error while loading STIS predefined messages.\n"
                                  "STIS Predefined message text will not be shown." );

        }
        catch ( const DatabaseException& dbe)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_IRS_Core::DatabaseException", dbe.what() );

            // this means that there is a problem with the database
            errorMessages.append( "Error while loading STIS predefined messages.\n"
                                  "STIS Predefined message text will not be shown." );
        }
        catch ( const TA_Base_Core::ObjectResolutionException& ore )
	    {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::ObjectResolutionException", ore.what() );

            errorMessages.append( "Error connecting to the TIS agent while loading STIS predefined messages.\n"
                                  "STIS Predefined message text will not be shown." );
	    }
        catch ( const CORBA::Exception& cex )
        {
	        std::string exceptionMsg( "Exception thrown while calling submitClearRequest: " );
		        exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );
	        LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

            errorMessages.append( "Error connecting to the TIS agent while loading STIS predefined messages.\n"
                                  "STIS Predefined message text will not be shown." );
        }


        // pop up a message box with accumulated errors
        if (errorsEncountered)
        {
            UserMessages::getInstance().displayError( errorMessages.c_str() );
        }

        // even if it failed, set this to true otherwise 
        // it will try for every entry in the list of display requests
        // this can be changed later if refreshing is to be made dynamic
        m_messagesLoaded = true;
    }


    void PredefinedMessageHelper::registerForLibraryChanges()
    {
        std::string locationName("OCC");
        ILocation* location = NULL;

        try
		{
            // get the location name
            std::stringstream locationKeyStream;
            locationKeyStream << TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
            unsigned long locationKey = 0;
            locationKeyStream >> locationKey;

            ILocation* location = LocationAccessFactory::getInstance().getLocationByKey(locationKey);

            // insert it into the map
            locationName = location->getName();
		}
		catch( ... )
		{}

        // clean up
        if (location != NULL)
        {
            delete location;
            location = NULL;
        }

        try
        {
			// 2008-07-18 to solve the scadaproxyfactory singleton usage
			m_proxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());
			TA_ASSERT ( ( 0 != m_proxyFactory ), "SCADA Proxy Factory not created");

            // build the datapoint names
            std::string stationLibraryDpName = locationName + CURRENT_STATION_LIBRARY_VERSION_DP_NAME;
            std::string trainLibraryDpName = locationName + CURRENT_TRAIN_LIBRARY_VERSION_DP_NAME;

            // create data point proxies
			TA_Base_Core::IEntityData* entityDate = NULL;
			entityDate = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(stationLibraryDpName);
			boost::shared_ptr<TA_Base_Core::DataPointEntityData> stationLibraryDp(dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityDate));
			TA_ASSERT(stationLibraryDp.get(), "can't create boost::shared_ptr<TA_Base_Core::DataPointEntityData> befroe create proxy.");

            m_proxyFactory->createDataPointProxy(stationLibraryDp, *this, m_currentStationLibraryDp);

			entityDate = NULL;
			entityDate = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(trainLibraryDpName);
			boost::shared_ptr<TA_Base_Core::DataPointEntityData> trainLibraryDp(dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityDate));
			TA_ASSERT(trainLibraryDp.get(), "can't create boost::shared_ptr<TA_Base_Core::DataPointEntityData> befroe create proxy.");

            m_proxyFactory->createDataPointProxy(trainLibraryDp, *this, m_currentTrainLibraryDp);

            TA_Base_Bus::ScadaProxyFactory::getInstance().setProxiesToControlMode();
        }
        catch (const DataException& de)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_IRS_Core::DataException", de.what() );
        }
        catch (const DatabaseException& dbe)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_IRS_Core::DatabaseException", dbe.what() );
        }
        catch (...)
        {
            // data point proxy creation error
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Error creating datapoint proxies" );
        }
    }


    void PredefinedMessageHelper::processEntityUpdateEvent(unsigned long entityKey,
                                                          TA_Base_Bus::ScadaEntityUpdateType updateType)
    {
        // this means the current version has changed

        // check the type of update
        if (updateType == TA_Base_Bus::ValueStateUpdate)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "A predefined message library was updated, invalidating the old one" );


            // a datapoint was updated - just set the message libraries to not loaded
            // so that next time they are asked for they will be loaded

            m_messagesLoaded = false;
        }
    }

    std::string PredefinedMessageHelper::getPredefinedMessageForTag(unsigned short messageId,
                                                               std::string messageLibrary,
                                                               int librarySection,
                                                               unsigned short libraryVersion)
    {
        // if its TTIS
        if (messageLibrary == "TTIS")
        {
            // check the version first
            // if the version is 0, it couldnt be loaded
            if (PredefinedMessageHelper::getInstance().getTtisLibraryVersion() == 0)
            {
                return "Error loading TTIS Predefined Message Library, message text cannot be determined.";
            }

            // if it was made with an old version of the message library
            if ( libraryVersion != PredefinedMessageHelper::getInstance().getTtisLibraryVersion() )
            {
                return "This request used an old version of the TTIS Predefined Message Library. The message text cannot be determined.";
            }

            // get the message
            std::string message = 
                PredefinedMessageHelper::getInstance().getTtisMessage( 
                static_cast<TA_Base_Core::ELibrarySection>(librarySection), messageId );
        
            if (message.length() == 0)
            {
                // if its not found
                std::stringstream error;
                error << "This request used an unknown TTIS pre-defined message tag: " << messageId
                    << " from library section " << librarySection;

                return error.str().c_str();
            }

            return message;
        }
        // otherwise if its STIS
        else if (messageLibrary == "STIS")
        {
            // check the version first
            // if the version is 0, it couldnt be loaded
            if (PredefinedMessageHelper::getInstance().getStisLibraryVersion() == 0)
            {
                return "Error loading STIS Predefined Message Library, message text cannot be determined.";
            }

            // if it was made with an old version of the message library
            if ( libraryVersion != PredefinedMessageHelper::getInstance().getStisLibraryVersion() )
            {
                return "This request used an old version of the STIS Predefined Message Library. The message text cannot be determined.";
            }

            // get the message
            std::string message = 
                PredefinedMessageHelper::getInstance().getStisMessage( 
                    static_cast<TA_Base_Core::ELibrarySection>(librarySection), messageId );
        
            if (message.length() == 0)
            {
                // if its not found
                std::stringstream error;
                error << "This request used an unknown STIS pre-defined message tag: " << messageId
                    << " from library section " << librarySection;

                return error.str().c_str();
            }

            return message;
        }
        // unknown library
        else
        {
        }

        return std::string("Unable to determine message text. See log for details.");
    }

    

    std::string PredefinedMessageHelper::getMessageString(TA_IRS_Core::IDisplayRequest* request)
    {
        // all messages (even predefined) should have the message text set in the adhoc content field
        std::string message = request->getAdhocMessageContent();

        // on the odd chance this is empty, and this is a predefined message,
        // then get the message text from the library

        if ( (message == "") && (request->getMessageType() == TA_IRS_Core::IDisplayRequest::PREDEFINED) )
        {
            message = getPredefinedMessageForTag( request->getMessageTag(),
                                                  request->getDestinationSystem(),
                                                  request->getLibrarySection(),
                                                  request->getLibraryVersion());

            // set the adhoc content - for the text box at the bottom
            request->setAdhocMessageContent(message);
        }

        return message;
    }
} // end namespace TA_App
