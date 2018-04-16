 /**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/MessageLibraryManager.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #9 $
  *
  * Last modification: $DateTime: 2014/08/25 17:14:07 $
  * Last modified by:  $Author: huang.wenbo $
  *
  */
#ifdef __WIN32__
#pragma warning(disable:4503)
#endif

#include "app/signs/tis_agent/src/MessageLibraryManager.h"
#include "app/signs/tis_agent/src/STISManager.h"
#include "app/signs/tis_agent/src/StatusMonitor.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryAccessFactory.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryData.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryTable.h"
#include "app/signs/tis_agent/src/Exceptions.h"
#include "core/exceptions/src/DataException.h"
#include "core/message/types/TISAlarms_MessageTypes.h"
#include "core/message/types/TISAudit_MessageTypes.h"
#include "boost/regex.hpp"
#include "core/sftp/src/SFTPManager.h"
#include "core/sftp/src/SFTPException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "app/signs/tis_agent/src/StringConverter.h"
#include "app/signs/tis_agent/src/GlobalConstant.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{	
	const std::string MessageLibraryManager::COMPRESS_TTIS_FILE_NAME = "PISOnboardDataSupply";
	void MessageLibraryManager::parseTTISMessageLibrary()
	{		
		TtisLibraryParser parser( m_libraryFileBuffer);
		unsigned long version = parser.getLibraryVersion();
		if (version <= 0)
		{
			TA_THROW( TA_IRS_Core::TISAgentException( "The version number could not be extracted from the predefined message library file" ) );
		}
		m_nextTrainLibraryVersion = version;		
		if( m_nextTrainLibraryVersion != m_indicatedVersion )
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"The version number in the predefined TTIS message library file is different to the version indicated by the CCT server.$$ The version should be [%d], version in file [%d]",
				m_indicatedVersion,  m_nextTrainLibraryVersion);
			TA_THROW( TA_IRS_Core::TISAgentException( "The version number in the predefined TTIS message library file is different to the version indicated by the CCT server" ) );
		}
		parser.parseLibrary();
		StatusMonitor::getInstance()->setNextISCSTrainLibraryVersion_central( m_nextTrainLibraryVersion );
	}
		
	void MessageLibraryManager::parseSTISMessageLibrary()
	{		
		// First check the library file buffer size, it should be a even number
		if(0 != m_libraryFileBuffer.size() % 2)
		{
            TA_THROW( TA_IRS_Core::TISAgentException( "The library file buffer size is not legal, the size should be a even number" ));
		}
		// Delete 'fdff' characters 	
		unsigned char compared = 0xff;
		for(std::vector<unsigned char>::iterator ite = m_libraryFileBuffer.begin(); ite != m_libraryFileBuffer.end();)
		{
			if((compared == *ite ) || (compared == *(ite + 1)))
			{
				ite = m_libraryFileBuffer.erase(ite);
				ite = m_libraryFileBuffer.erase(ite);
			}
			else
			{
				ite += 2;
			}
		}

		StisLibraryParser parser(m_libraryFileBuffer);		
		unsigned long version = parser.getLibraryVersion();
        if (version <= 0)
        {
            TA_THROW( TA_IRS_Core::TISAgentException( "The version number could not be extracted from the predefined message library file" ) );
        }
		m_nextStationLibraryVersion = version;
		if( m_nextStationLibraryVersion != m_indicatedVersion )
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
				"The version number in the predefined STIS message library file is different to the version indicated by the STIS server.$$ The version should be [%d], version in file [%d]",
				m_indicatedVersion,  m_nextStationLibraryVersion);
			TA_THROW( TA_IRS_Core::TISAgentException( "The version number in the predefined STIS message library file is different to the version indicated by the STIS server" ) );
		}
		parser.parseLibrary();
	    // Only set the next version now, since the new predefined message library has been added successfully 
	    // to the database.
		//We just set the version of lib version of central DB. Before, we set the occ lib version
	    StatusMonitor::getInstance()->setNextISCSStationLibraryVersion_central( m_nextStationLibraryVersion );
	}

	void MessageLibraryManager::parseMessageLibrary( EMessageLibraryType type )
	{
		TA_ASSERT( STISManager::getInstance()->getLocationType() == TA_Base_Core::ILocation::OCC,
                   "MessageLibraryManager::parseMessageLibrary() is only available at the OCC" );
		if( STATION_LIBRARY == type )
		{
			parseSTISMessageLibrary();
		}
		else
		{
			parseTTISMessageLibrary();
		}
	}  
	


	void MessageLibraryManager::processNewMessageLibrary( EMessageLibraryType type, const std::string& filename, unsigned long version  )
	{
		FUNCTION_ENTRY("processNewMessageLibrary");
		TA_Base_Core::ThreadGuard guard( m_libraryProcessLock );

		TA_ASSERT( STISManager::getInstance()->getLocationType() == TA_Base_Core::ILocation::OCC,
                   "MessageLibraryManager::processNewMessageLibrary() is only available at the OCC" );
		
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Process new message library");


		m_indicatedVersion = version;

		TA_Base_Core::DescriptionParameters desc;
		std::stringstream libraryVersionStream;
		switch(type)
		{
		case STATION_LIBRARY:
			libraryVersionStream << StatusMonitor::getInstance()->getNextSTISStationLibraryVersion();
			break;
		case TRAIN_LIBRARY:
			libraryVersionStream << StatusMonitor::getInstance()->getNextSTISTrainLibraryVersion();
		}

		TA_Base_Core::NameValuePair libraryVersion("version", libraryVersionStream.str());		
		desc.push_back(&libraryVersion);

		try
		{
			
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Retrieving and Parsing new message library");
			retrieveMessageLibrary( type, filename, version);
			parseMessageLibrary( type );

			unsigned long currentLibraryVersion;
			unsigned long parsedNextLibraryVersion;
			switch( type )
			{
			case STATION_LIBRARY:
				{
					parsedNextLibraryVersion = StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central();
					currentLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSStationLibraryVersion_central();

					if( currentLibraryVersion == parsedNextLibraryVersion )
					{
						TA_THROW( TA_IRS_Core::TISAgentException( "New parsed Station message library file has the same version number as the current message library" ) );
					}

					//TES831 Events not logged!
					TA_Base_Core::NameValuePair valid("Valid/Invalid", "Valid");
					desc.push_back(&valid);

					//TES831 Events not logged!

					// Presume that none of the stations have received this new library version yet..
					StatusMonitor::getInstance()->setStationLibrariesSynchronised( false );

					// Let the STISManager know that the new message library version is ready to go
					// so it can begin examining the stations
					STISManager::getInstance()->newMessageLibraryReady( parsedNextLibraryVersion, type );
				}
				break;

			case TRAIN_LIBRARY:
				{
					parsedNextLibraryVersion = StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central();
					currentLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSTrainLibraryVersion_central();

					if( currentLibraryVersion == parsedNextLibraryVersion )
					{
						TA_THROW( TA_IRS_Core::TISAgentException( "New parsed Train message library file has the same version number as the current message library" ) );
					}

					//TD19279
					TA_Base_Core::NameValuePair valid("Valid/Invalid", "Valid");
					desc.push_back(&valid);
					
					//TD19279

					// Presume that none of the stations have received this new library version yet..
					StatusMonitor::getInstance()->setTrainLibrariesSynchronised( false );

					// Let the STISManager know that the new message library version is ready to go
					// so it can begin examining the stations
					STISManager::getInstance()->newMessageLibraryReady( parsedNextLibraryVersion, type );
				}
				break;
			}

			//TD15198 azenitha++
			if (m_raisedMessageLibraryDownloadErrorAlarm == true)
			{
				STISManager::getInstance()->closeAlarm(TA_Base_Core::TISAlarms::MessageLibraryDownloadError);
				m_raisedMessageLibraryDownloadErrorAlarm = false;
			}
			//TD15198 ++azenitha			

		}
		catch ( const TA_IRS_Core::TISAgentException& tae  )
		{	
			TA_Base_Core::DescriptionParameters dp;
			
			//TD 15899
			//zhou yuan++
			
			switch( type )
			{
			
			case STATION_LIBRARY:
			     {

					 std::string description(tae.what());
					 TA_Base_Core::NameValuePair descVP("Description", description );
					 TA_Base_Core::NameValuePair typeVP("Type", "Station" );					 
					 dp.push_back( &typeVP );
					 dp.push_back( &descVP );

					 STISManager::getInstance()->submitAlarm( TA_Base_Core::TISAlarms::MessageLibraryDownloadError, dp );
					 m_raisedMessageLibraryDownloadErrorAlarm = true; //TD15198 azenitha

					 //TES831 Events not logged!
					 TA_Base_Core::NameValuePair valid("Valid/Invalid", "Invalid");
					 desc.push_back(&valid);
					 //TES831 Events not logged!
					 
					 break;
				 }
			
			case TRAIN_LIBRARY:
				{
					std::string description(tae.what());
					TA_Base_Core::NameValuePair descVP("Description", description );
					TA_Base_Core::NameValuePair typeVP("Type", "Train" );
					dp.push_back( &descVP );
					dp.push_back( &typeVP );
					STISManager::getInstance()->submitAlarm( TA_Base_Core::TISAlarms::MessageLibraryDownloadError, dp );
					m_raisedMessageLibraryDownloadErrorAlarm = true; //TD15198 azenitha

					//TES831 Events not logged!
					TA_Base_Core::NameValuePair valid("Valid/Invalid", "Invalid");
					desc.push_back(&valid);
					//TES831 Events not logged!
					break;
				}
			}

			throw;

			//++zhou yuan
		}
	}

	void MessageLibraryManager::retrieveMessageLibrary( EMessageLibraryType type, const std::string& filename, unsigned long version)
	{
		TA_ASSERT( STISManager::getInstance()->getLocationType() == TA_Base_Core::ILocation::OCC,
                   "MessageLibraryManager::retrieveMessageLibrary() is only available at the OCC" );
		//clear the buffer first
		m_libraryFileBuffer.clear();
        try
        {
			 switch( type )
				{
				case STATION_LIBRARY:
					{
#ifndef LOCALTEST
			            std::string stisFileLocation = SFTP_HOME + STIS_LIBRARY_DOWNLOAD_FOLDER + filename;
						TA_Base_Core::SFTPManager::getInstance().retrieveFile(m_stisSftpIPAddress, m_stisSftpUserName, m_stisSftpUserPassword, stisFileLocation, m_libraryFileBuffer, m_stisSftpPort);
#else
                        #include <iterator>
                        std::fstream f("LibrarySTIS1.txt", std::ios::binary | std::ios::in);
                        if(f.is_open())
                        {
                            std::istreambuf_iterator<char> b(f);
                            std::istreambuf_iterator<char> e;
                            std::copy(b, e, std::back_inserter(m_libraryFileBuffer));
                        }
#endif
						break;
					}

				case TRAIN_LIBRARY:
					{
						std::vector<unsigned char> tmpBuf; //used to save the compressed library file
						std::string tgzFileName = COMPRESS_TTIS_FILE_NAME + ".tgz";
						std::string tarFileName = COMPRESS_TTIS_FILE_NAME + ".tar";
#ifndef LOCALTEST
						std::string ttisFileLocation = SFTP_HOME + "/" + tgzFileName;
						TA_Base_Core::SFTPManager::getInstance().retrieveFile(m_cctSftpIPAddress, m_cctSftpUserName, m_cctSftpUserPassword, ttisFileLocation, tmpBuf, m_cctSftpPort);
#else
                        #include <iterator>
                        std::fstream f("F:\\tool\\PISOnboardDataSupply.tgz", std::ios::binary | std::ios::in);
                        if(f.is_open())
                        {
                            std::istreambuf_iterator<char> b(f);
                            std::istreambuf_iterator<char> e;
                            std::copy(b, e, std::back_inserter(tmpBuf));
                        }

#endif
						preProcessTtisLibrary(tmpBuf, tgzFileName, tarFileName);
						// Create a file to save the buffer
						//open the decompressed library file						
						std::fstream iFile;
						iFile.open(filename.c_str(), std::ios::in | std::ios::binary);						
						if( false == iFile.is_open() )
						{
							TA_THROW(TA_Base_Core::TransactiveException("Can not open library file to read"));
						}
						std::copy(std::istreambuf_iterator<char>(iFile), std::istreambuf_iterator<char>(), std::back_inserter(m_libraryFileBuffer));
					    iFile.close();

						// add the original file to the tail of m_libraryFileBuffer
						// add a mark to signature the original file start
						std::string mark = TtisLibraryParser::getComprisedFileSearchMark();
						std::copy(mark.begin(), mark.end(), std::back_inserter(m_libraryFileBuffer));
						// add the file
						std::copy(tmpBuf.begin(), tmpBuf.end(), std::back_inserter(m_libraryFileBuffer));
						break;				
					}
				}

        }
        catch ( const TA_Base_Core::SFTPException& sftpex )
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, sftpex.what() );
			// TODO - Raise an alarm to indicate that the library could not be retrievd

			TA_THROW( TA_IRS_Core::TISAgentException( "Caught an SFTPException while attempt to retrieve new message library" ) );
        }
		catch (const TA_Base_Core::TransactiveException& e)
		{
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, e.what());
			TA_THROW( TA_IRS_Core::TISAgentException( "Caught an file exception while attempt to decompress or read new message library file" ) );
		}
		catch ( ... )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Unknown error");
			TA_THROW( TA_IRS_Core::TISAgentException( "Caught an unknown exception while attempt to decompress or read new message library file" ) );
		}
	}

	MessageLibraryManager::MessageLibraryManager( const std::string& stationMessageFilename, const std::string& stationMessagePath, 
			const std::string& trainMessageFilename, const std::string& trainMessagePath,
			const std::string& stisSftpIPAddress, int stisSftpPort, const std::string& stisSftpUserName, const std::string& stisSftpPassword,
			const std::string& cctSftpIPAddress, int cctSftpPort, const std::string& cctSftpUserName, const std::string& cctSftpPassword )
	{
		m_stationMessageLibraryFilename = stationMessageFilename;
		m_stationMessageLibraryPath = stationMessagePath;
        m_trainMessageLibraryFilename = stationMessageFilename;
		m_trainMessageLibraryPath = stationMessagePath;
		
		m_stisSftpIPAddress = stisSftpIPAddress;
		m_stisSftpPort = stisSftpPort;
		m_stisSftpUserName = stisSftpUserName;
		m_stisSftpUserPassword = stisSftpPassword;

		m_cctSftpIPAddress = cctSftpIPAddress;
		m_cctSftpPort = cctSftpPort;
		m_cctSftpUserName = cctSftpUserName;
		m_cctSftpUserPassword = cctSftpPassword;
		
		m_raisedMessageLibraryDownloadErrorAlarm = false;
	}

	void MessageLibraryManager::preProcessTtisLibrary(const std::vector<unsigned char>& libraryVector,
		                                              const std::string& tgzFileName,
													  const std::string& tarFileName)
	{
		std::fstream iFile(tgzFileName.c_str(), std::ios::out | std::ios::binary);
		if(false == iFile.is_open())
		{
			TA_THROW(TA_Base_Core::TransactiveException("Create compressed file failure"));
		}
		// Save buffer to the file
		std::copy(libraryVector.begin(), libraryVector.end(), std::ostreambuf_iterator<char>(iFile));
		iFile.close();
		
		//decompress file
		//use command gzip -df & tar -xf
		std::stringstream ss;
#if !defined(WIN32)
		ss << "gzip -df ";
#else
		ss << "7z e -y "; // windows use 7z to decomprise file
#endif
		ss << tgzFileName;

		int error = -1;
		error = system(ss.str().c_str());
		if( -1 == error)
		{
			TA_THROW(TA_Base_Core::TransactiveException("Decompress tgz file failure"));
		}

		ss.clear();
		ss.str("");
#if !defined(WIN32)
		ss << "tar -xf ";
#else
		ss << "7z e -y ";
#endif
		ss << tarFileName;

		error = system(ss.str().c_str());
		if( -1 == error)
		{
			TA_THROW(TA_Base_Core::TransactiveException("Decompress tar file failure"));
		}
	}

	MessageLibraryManager::~MessageLibraryManager()
	{
	}
}
