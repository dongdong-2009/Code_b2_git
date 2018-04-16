/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking_variable/src/InterlockVariableHelper.cpp $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */


#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/interlocking_variable/src/InterlockVariableHelper.h"
#include "core/data_access_interface/interlocking_variable/src/InterlockVariableAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

	InterlockVariableHelper::InterlockVariableHelper( const InterlockVariableHelper& theInterlockVariableHelper)
		: m_dllName(theInterlockVariableHelper.m_dllName),
          m_entityType(theInterlockVariableHelper.m_entityType),
          m_buttonText(""),
          m_isValidData(true)
    {	  
	}


    InterlockVariableHelper::InterlockVariableHelper(const std::string& entityType)
        : m_dllName(""),
          m_entityType(entityType),
          m_buttonText(""),
          m_isValidData(false)
    {
    }



    InterlockVariableHelper::~InterlockVariableHelper()
    {
        try
        {
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    std::string InterlockVariableHelper::getDllName()
    {
        if (!m_isValidData)
        {
            reload();
        }
        return m_dllName;
    }

    std::string InterlockVariableHelper::getEntityType()
    {
        if (!m_isValidData)
        {
            reload();
        }
        return m_entityType;
    }

    std::string InterlockVariableHelper::getButtonText()
    {
        if (!m_isValidData)
        {
            reload();
        }
        return m_buttonText;
    }



    void InterlockVariableHelper::reload()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "InterlockVariableHelper::reload" );

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // create the SQL string to retrieve the data of the operator
        // based upon the key
        // use nvl for those columns that may be null to prevent exceptions
        std::ostringstream sql;
		sql << "select DLL_NAME, BUTTON_TEXT ";
        sql << "FROM LK_VARIABLE_TYPES WHERE ENTITY_TYPE = '" << databaseConnection->escapeQueryString(m_entityType) << "'";
 
        // Set up the columnNames vector to be passed to executeQuery()
        std::string nameColumn = "DLL_NAME";
        std::string textColumn = "BUTTON_TEXT";

        std::vector<std::string> columnNames;
		columnNames.push_back(nameColumn);
		columnNames.push_back(textColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql.str(),columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "No dll name found for entity type = '" << m_entityType << "'";
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one dll name found for entity type = '" << m_entityType << "'";
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_dllName = data->getStringData( 0, nameColumn );
		m_buttonText = data->getStringData( 0, textColumn );

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        
        // Need to record that we now have valid data
        m_isValidData = true;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "OperatorHelper::reload" );
    }


} // closes TA_Base_Core
