/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/tis_agent_4669/src/ConfigPredefinedMessageLibraryData.cpp $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class contains the database retrievals and writes for Predefined Message Libraries
  * for the Traveller Information System (TIS) Agent
  * It implements the IConfigPredefinedMessageLibraryData interface.
  */

#pragma warning( disable : 4786 )

#include <string>
#include <sstream>

#include "core/data_access_interface/tis_agent_4669/src/ConfigPredefinedMessageLibraryData.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{

    //
    // CameraPresetsData
    //
    ConfigPredefinedMessageLibraryData::ConfigPredefinedMessageLibraryData()
        : m_isValidData( false )
    {
        m_messageLibraryHelper = new PredefinedMessageLibraryHelper();
    }  


	ConfigPredefinedMessageLibraryData::ConfigPredefinedMessageLibraryData( unsigned short version, const std::string& libraryType,
		std::vector<unsigned char>& libraryFileContent ) : m_isValidData(true)
	{
		m_messageLibraryHelper = new PredefinedMessageLibraryHelper( version, libraryType, libraryFileContent );

	}


	ConfigPredefinedMessageLibraryData::~ConfigPredefinedMessageLibraryData()
	{
		if( m_messageLibraryHelper != NULL )
		{
			delete m_messageLibraryHelper;
			m_messageLibraryHelper = NULL;
		}

	}
	

	unsigned long ConfigPredefinedMessageLibraryData::getKey()
	{
		TA_ASSERT(m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null.");

		return m_messageLibraryHelper->getKey();
	}


	unsigned short ConfigPredefinedMessageLibraryData::getVersion()
	{
		TA_ASSERT(m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null.");
	
		return m_messageLibraryHelper->getVersion();
	}

	
    std::string ConfigPredefinedMessageLibraryData::getLibraryType()
	{
		TA_ASSERT(m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null.");

		return m_messageLibraryHelper->getLibraryType();
	}


	void ConfigPredefinedMessageLibraryData::getLibraryFileContent( std::vector<unsigned char>& fileContent )
	{
		TA_ASSERT(m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null.");

		m_messageLibraryHelper->getLibraryFileContent( fileContent );
	}


	void ConfigPredefinedMessageLibraryData::invalidate()
	{
        TA_ASSERT(m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null.");
        m_messageLibraryHelper->invalidate();
	}

	
    void ConfigPredefinedMessageLibraryData::addMessage( const PredefinedMessage& theMessage )
	{
		TA_ASSERT(m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null.");

		m_messageLibraryHelper->addMessage( theMessage );
	}

	IPredefinedMessageLibrary::PredefinedMessageList ConfigPredefinedMessageLibraryData::getMessages()
	{
		TA_ASSERT(m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null.");
		
		return m_messageLibraryHelper->getMessages();
	}

    PredefinedMessage* ConfigPredefinedMessageLibraryData::getMessage(int librarySection, unsigned short messageTag)
    {
		TA_ASSERT(m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null.");
		
		return m_messageLibraryHelper->getMessage(librarySection, messageTag);
	}

    STISLedAttributes ConfigPredefinedMessageLibraryData::getDefaultSTISLedAttributes()
    {
        TA_ASSERT(m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null.");
		
		return m_messageLibraryHelper->getDefaultSTISLedAttributes();
    }


    STISPlasmaAttributes ConfigPredefinedMessageLibraryData::getDefaultSTISPlasmaAttributes()
    {
        TA_ASSERT(m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null.");
		
		return m_messageLibraryHelper->getDefaultSTISPlasmaAttributes();
    }


    TTISLedAttributes ConfigPredefinedMessageLibraryData::getDefaultTTISLedAttributes()
    {
        TA_ASSERT(m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null.");
		
		return m_messageLibraryHelper->getDefaultTTISLedAttributes();
    }


    void ConfigPredefinedMessageLibraryData::setDefaultDisplayAttributes(
                                                const STISLedAttributes& stisLedAttributes,
                                                const STISPlasmaAttributes& stisPlasmaAttributes,
                                                const TTISLedAttributes& ttisLedAttributes )
    {
        TA_ASSERT(m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null.");

		m_messageLibraryHelper->setDefaultDisplayAttributes( stisLedAttributes,
                                                             stisPlasmaAttributes,
                                                             ttisLedAttributes);
    }


	void ConfigPredefinedMessageLibraryData::deleteThisLibrary()
	{
		TA_ASSERT(m_messageLibraryHelper != NULL, "The MessageLibraryHelper pointer is NULL");

		m_messageLibraryHelper->deleteThisLibrary();
	}


	bool ConfigPredefinedMessageLibraryData::isNew()
    {
        TA_ASSERT(m_messageLibraryHelper != NULL, "The MessageLibraryHelper pointer is NULL");

        return m_messageLibraryHelper->isNew();
    }


    void ConfigPredefinedMessageLibraryData::applyChanges()
    {
        TA_ASSERT(m_messageLibraryHelper != NULL, "The MessageLibraryHelper pointer is NULL");

//        m_messageLibraryHelper->applyChanges();
        m_messageLibraryDataChanges.clear();
    }
   


} //end namespace TA_Base_Core
