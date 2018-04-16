/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking/src/InterlockAlias.cpp $
  * @author:  Ross Tucker
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

#include "core/data_access_interface/interlocking/src/InterlockAlias.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{


    InterlockAlias::InterlockAlias(const unsigned long ruleId)
        : m_ruleId(ruleId),
          m_alias(""),
          m_variableType(""),
          m_variableName(""),
          m_isNew(true),
          m_isValidData(false)
    {
    }


    InterlockAlias::~InterlockAlias()
    {
        try
        {
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }

    std::string InterlockAlias::getAlias()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_alias;
    }

    std::string InterlockAlias::getVariableName()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_variableName;
    }

    std::string InterlockAlias::getVariableType()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_variableType;
    }


    void InterlockAlias::setRuleId(const unsigned int ruleId)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "InterlockAlias::setRuleId" );
        if (m_ruleId != 0)
        {
            TA_ASSERT(false, "setRuleId can only be called on new aliases");
        }
        m_ruleId = ruleId;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "InterlockAlias::setAlias" );
    }


    void InterlockAlias::setAlias(const std::string& alias)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "InterlockAlias::setAlias" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_alias = alias;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "InterlockAlias::setAlias" );
    }


    void InterlockAlias::setVariableName(const std::string& variableName)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "InterlockAlias::setVariableName" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_variableName = variableName;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "InterlockAlias::setVariableName" );
    }


    void InterlockAlias::setVariableType(const std::string& variableType)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "InterlockAlias::setVariableType" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_variableType = variableType;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "InterlockAlias::setVariableType" );
    }


    void InterlockAlias::reload()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "InterlockAlias::reload" );
        // Ensure the rule has already been written to the database. It doesn't make
        // any sense to reload an rule that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This rule does not yet exist in the database. Therefore it cannot be reloaded.");
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // create the SQL string to retrieve the data of the operator
        // based upon the key
        // use nvl for those columns that may be null to prevent exceptions
        std::ostringstream sql;
		sql << "select ALIAS, ";
        sql <<        "VARIABLE_TYPE ";
        sql << "FROM LK_ALIAS WHERE LKIRUL_ID = " << m_ruleId;
        sql <<                "AND VARIABLE_NAME = " << databaseConnection->escapeQueryString(m_variableName); 

        // Set up the columnNames vector to be passed to executeQuery()
        std::string aliasColumn = "NAME";
        std::string variableTypeColumn = "VARIABLE_TYPE";

        std::vector<std::string> columnNames;
		columnNames.push_back(aliasColumn);
		columnNames.push_back(variableTypeColumn);

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
			reasonMessage << "No alias found for pkey = " << m_ruleId;
            reasonMessage << " and variable name = " << m_variableName;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one alias found for pkey = " << m_ruleId;
            reasonMessage << " and variable name = " << m_variableName;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_alias = data->getStringData( 0, aliasColumn );
        m_variableType = data->getStringData(0, variableTypeColumn);

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        

        // Need to record that we now have valid data
        m_isValidData = true;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "InterlockAlias::reload" );
    }


    void InterlockAlias::save()
    {       
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "InterlockAlias::writeInterlockAliasData" );     

        // Check all required values exist. If not add them to the dataConfiguraitonException
        std::vector<std::string> fieldNames;
        if ( m_alias.empty() )
        {
            fieldNames.push_back("Alias");
        }
        if (m_ruleId == 0)
        {
            fieldNames.push_back("Rule ID");
        }
        if ( m_variableName.empty() )
        {
            fieldNames.push_back("Variable Name");
        }
        if (m_variableType.empty())
        {
            fieldNames.push_back("Variable Type");
        }
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("Interlocking alias data not fully specified. Interlocking alias cannot be written to database",
                                                              fieldNames));
        }

        // Write the Alias to the database

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
        
        // Need to check if this is a new or existing rule - 
        // as a new rules will be inserted, while an existing rules will be updated
        if (m_isNew) // This is a new rule
        {
            addNewInterlockAlias();
        }
        else // This is an existing Operator
        {
            modifyExistingInterlockAlias();
        }
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "InterlockAlias::writeInterlockRuleData" );
    }


    void InterlockAlias::erase()
    {
    }


    void InterlockAlias::modifyExistingInterlockAlias()
    {
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "Interlocking Alias %s already exists. It's data will be updated.",m_alias.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

        std::ostringstream sql;
        sql << "update LK_ALIAS set ";
        sql <<         "ALIAS = '"         << (databaseConnection->escapeInsertString(m_alias)).c_str() << "', ";
        sql <<         "VARIABLE_TYPE = '" << (databaseConnection->escapeInsertString(m_variableType)).c_str() << "' ";
        sql << " where LKIRUL_ID = " << m_ruleId;
        sql <<    "and VARIABLE_NAME = '" << (databaseConnection->escapeInsertString(m_variableName)).c_str() << "' ";

        databaseConnection->executeModification(sql.str().c_str());

        // nothing more to be done here
    }


    void InterlockAlias::addNewInterlockAlias()
    {

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Interlocking rule %s is a new rule. It will be added to the database.",m_alias.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

        // insert new record
        std::ostringstream sql;
        sql << "insert into LK_ALIAS (LKIRUL_ID, ALIAS, VARIABLE_NAME, VARIABLE_TYPE) ";
        sql << "values (";
        sql <<          m_ruleId << ",";
        sql <<   "'" << (databaseConnection->escapeInsertString(m_alias)).c_str() << "', ";
        sql <<   "'" << (databaseConnection->escapeInsertString(m_variableName)).c_str() << "', ";
        sql <<   "'" << (databaseConnection->escapeInsertString(m_variableType)).c_str() << "' ";
        sql << ")";

        databaseConnection->executeModification(sql.str());


    }


} // closes TA_Base_Core
