#ifndef MESSAGELIBRARYMANAGER_H_INCLUDED
#define MESSAGELIBRARYMANAGER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/MessageLibraryManager.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/ftp/src/FTPClient.h"
#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"

#include <string>

namespace TA_IRS_App
{

	enum EMessageLibraryType
    {
        STATION_LIBRARY,
        TRAIN_LIBRARY
    };

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
			const std::string& trainMessageFilename, const std::string& trainMessagePath, const std::string& serverIP  );

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
        void processNewMessageLibrary( EMessageLibraryType type, const std::string& filename, int version );

	private:

		// Retrieve the file from the remote server via ftp
		void retrieveMessageLibrary( EMessageLibraryType type, const std::string& filename );

		// read the retrieved message library file, parse it, and
		// store to the ISCS database
		void parseMessageLibrary( EMessageLibraryType type, const std::string& filename );

        // read a "tag = value" line
        // returns an empty string on failure
        std::string getValue( const std::string& file, 
                              const std::string& tag,
                              std::string::size_type findFrom = 0 ) const;

        // get a number from a string
        // returns -1 on failure
        int getNumber( const std::string& numberStr ) const;
        int getHexNumber( const std::string& numberStr ) const;



        /** 
          * parseMessage
          *
          * Parses the next message in the predefined message library
          *
          * @param file the message library content
          * @param librarySection the message library section the next message belongs to
          * @param type the message library type
          * @param currentPosition the current position in the message library
          *                        on return this will be the position of the next message
          *
          * @return A new predfined message
          *
          * @exception TisAgentException if there is an error in the format of the message
          */
        TA_Base_Core::PredefinedMessage* parseMessage( const std::string& file,
                                                      TA_Base_Core::ELibrarySection librarySection,
                                                      EMessageLibraryType type,
                                                      std::string::size_type& currentPosition );


        /** 
          * parseStisLedAttributeString
          *
          * Given the string containing the STIS LED display attributes, parse them into the required structure
          *
          * @param attributeString The string containing the attributes
          *
          * @return The structure containing the attributes
          *
          * @exception TISAgentException if the format is incorrect or the values are out of range
          */
        TA_Base_Core::STISLedAttributes parseStisLedAttributeString(const std::string& attributeString);


        /** 
          * parseTtisLedAttributeString
          *
          * Given the string containing the TTIS LED display attributes, parse them into the required structure
          *
          * @param attributeString The string containing the attributes
          *
          * @return The structure containing the attributes
          *
          * @exception TISAgentException if the format is incorrect or the values are out of range
          */
        TA_Base_Core::TTISLedAttributes parseTtisLedAttributeString(const std::string& attributeString);


        /** 
          * parseStisPlasmaAttributeString
          *
          * Given the string containing the STIS plasma display attributes, parse them into the required structure
          *
          * @param attributeString The string containing the attributes
          *
          * @return The structure containing the attributes
          *
          * @exception TISAgentException if the format is incorrect or the values are out of range
          */
        TA_Base_Core::STISPlasmaAttributes parseStisPlasmaAttributeString(const std::string& attributeString);


		std::string m_trainMessageLibraryFilename;

		std::string m_trainMessageLibraryPath;

		std::string m_stationMessageLibraryFilename;

		std::string m_stationMessageLibraryPath;

		std::string m_serverIPAddress;

		int m_nextStationLibraryVersion;
		int m_nextTrainLibraryVersion;

		// library version that the STIS server has told us is the next version
		int m_indicatedVersion;


        TA_Base_Core::FTPClient m_ftpClient;

		// Ensure that only one library is processed at the same time
		TA_Base_Core::ReEntrantThreadLockable m_libraryProcessLock;

		// Temporary storage for the file once it is retrieved
		std::vector<unsigned char> m_libraryFileBuffer;

		bool m_raisedMessageLibraryDownloadErrorAlarm; //TD15198 azenitha
	};

}

#endif // MESSAGELIBRARYMANAGER_H_INCLUDED