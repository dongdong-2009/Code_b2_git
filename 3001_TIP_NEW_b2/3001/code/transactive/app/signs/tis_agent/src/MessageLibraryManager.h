#ifndef MESSAGELIBRARYMANAGER_H_INCLUDED
#define MESSAGELIBRARYMANAGER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/MessageLibraryManager.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/03/28 15:52:59 $
  * Last modified by:  $Author: huang.wenbo $
  *
  */

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/src/DataTypeEnums.h"
#include "core/data_access_interface/tis_agent_4669/src/IConfigPredefinedMessageLibrary.h"
#include "MessageLibraryParser.h"
#include <string>

namespace TA_IRS_App
{	
	class MessageLibraryManager
	{
	public:

		/**
		  * constructor
		  *
		  * @return     nothing.
		  *
		  */
		MessageLibraryManager( const std::string& stationMessageFilename, const std::string& stationMessagePath, 
			const std::string& trainMessageFilename, const std::string& trainMessagePath,
			const std::string& stisSftpIPAddress, int stisSftpPort, const std::string& stisSftpUserName, const std::string& stisSftpPassword,
			const std::string& cctSftpIPAddress, int cctSftpPort, const std::string& cctSftpUserName, const std::string& cctSftpPassword );

	    /**
		  * StatusMonitor
		  *
		  * Destructor
		  *
		  * @return     none.
		  *
		  */
		virtual ~MessageLibraryManager();



		// Other methods


		// initiates the retrieval of a new message library file
        void processNewMessageLibrary( EMessageLibraryType type, const std::string& filename, unsigned long version );

	private:	
		// Retrieve the file from the remote server via ftp
		void retrieveMessageLibrary( EMessageLibraryType type, const std::string& filename, unsigned long version);

		void preProcessTtisLibrary(const std::vector<unsigned char>& libraryVector,
			                       const std::string& tgzFileName,
			                       const std::string& tarFileName);

		// read the retrieved message library file, parse it, and
		// store to the ISCS database
		void parseMessageLibrary( EMessageLibraryType type );

		void parseTTISMessageLibrary();
		
		void parseSTISMessageLibrary();

		std::string m_trainMessageLibraryFilename;

		std::string m_trainMessageLibraryPath;

		std::string m_stationMessageLibraryFilename;

		std::string m_stationMessageLibraryPath;

		//sftp related
		std::string m_stisSftpIPAddress;
		int m_stisSftpPort;
		std::string m_stisSftpUserName;
		std::string m_stisSftpUserPassword;

		std::string m_cctSftpIPAddress;
		int m_cctSftpPort;
		std::string m_cctSftpUserName;
		std::string m_cctSftpUserPassword;

		unsigned long m_nextStationLibraryVersion;
		unsigned long m_nextTrainLibraryVersion;

		// library version that the STIS server has told us is the next version
		unsigned long m_indicatedVersion;

		// Ensure that only one library is processed at the same time
		TA_Base_Core::ReEntrantThreadLockable m_libraryProcessLock;

		// Temporary storage for the file once it is retrieved
		std::vector<unsigned char> m_libraryFileBuffer;

		bool m_raisedMessageLibraryDownloadErrorAlarm; //TD15198 azenitha

		static const std::string COMPRESS_TTIS_FILE_NAME;
	};
}

#endif // MESSAGELIBRARYMANAGER_H_INCLUDED
