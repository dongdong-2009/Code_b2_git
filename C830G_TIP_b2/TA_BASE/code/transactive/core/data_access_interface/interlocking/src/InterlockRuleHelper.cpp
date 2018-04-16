/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking/src/InterlockRuleHelper.cpp $
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
#include <ctime>

#include "core/data_access_interface/interlocking/src/InterlockRuleHelper.h"
#include "core/data_access_interface/interlocking/src/InterlockRuleAccessFactory.h"
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
    const unsigned int InterlockRuleHelper::MAX_RULES(60000);
    const std::string InterlockRuleHelper::DELETED_FLAG("#");
    const std::string InterlockRuleHelper::STRING_TRUE  = "T";
    const std::string InterlockRuleHelper::STRING_FALSE = "F";
    const std::string InterlockRuleHelper::PARAMETER_SEPARATOR = "|||";

	InterlockRuleHelper::InterlockRuleHelper( const InterlockRuleHelper& theInterlockRuleHelper)
		: m_key(0), 
          m_name(theInterlockRuleHelper.m_name),
          m_description(""),
          m_entityKey(theInterlockRuleHelper.m_entityKey),
          m_entityType(""),
          m_desiredState(theInterlockRuleHelper.m_desiredState),
          m_parameters(0),
          m_condition(theInterlockRuleHelper.m_condition),
          m_preCondition(true),
          m_enabled(true),
          m_aliases(0),
          m_isValidData(false),
          m_isNew(true)
    {	  
	}


    InterlockRuleHelper::InterlockRuleHelper(const unsigned long key)
        : m_key(key),
          m_name(""),
          m_description(""),
          m_entityKey(0),
          m_entityType(""),
          m_desiredState(""),
          m_parameters(0),
          m_condition(""),
          m_preCondition(true),
          m_enabled(true),
          m_aliases(0),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    InterlockRuleHelper::InterlockRuleHelper()
        : m_key(0),
          m_name(""),
          m_description(""),
          m_entityKey(0),
          m_entityType(""),
          m_desiredState(""),
          m_parameters(0),
          m_condition(""),
          m_preCondition(true),
          m_enabled(true),
          m_aliases(0),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    InterlockRuleHelper::~InterlockRuleHelper()
    {
        try
        {
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    unsigned long InterlockRuleHelper::getKey()
    {
        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"The data must be written to the database before the key can be retrieved");
        
        return m_key;
    }


    std::string InterlockRuleHelper::getName()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_name;
    }


    void InterlockRuleHelper::setName(const std::string& name)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "InterlockRuleHelper::setName" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_name = name;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "InterlockRuleHelper::setName" );
    }


    std::string InterlockRuleHelper::getDescription()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_description;
    }


    void InterlockRuleHelper::setDescription(const std::string& description)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "InterlockRuleHelper::setDescription" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_description = description;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "InterlockRuleHelper::setDescription" );
    }


    std::string InterlockRuleHelper::getDesiredState()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_desiredState;
    }


    void InterlockRuleHelper::setDesiredState(const std::string& desiredState)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "InterlockRuleHelper::setDesiredState" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_desiredState = desiredState;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "InterlockRuleHelper::setDesiredState" );
    }


    std::string InterlockRuleHelper::getCondition()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_condition;
    }

    void InterlockRuleHelper::setCondition(const std::string& expression)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "InterlockRuleHelper::setCondition" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_condition = expression;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "InterlockRuleHelper::setCondition" );
    }


    std::vector<std::string> InterlockRuleHelper::getParameters()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_parameters;
    }

    void InterlockRuleHelper::setParameters(const std::vector<std::string>& parameters)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "InterlockRuleHelper::setParameters" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        
        // clear out old parameters and replace with new
        m_parameters.clear();
        m_parameters = parameters;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "InterlockRuleHelper::setParameter" );
    }


    unsigned long InterlockRuleHelper::getEntityKey()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_entityKey;
    }


    void InterlockRuleHelper::setEntityKey(unsigned long entityKey)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "InterlockRuleHelper::setEntityKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_entityKey = entityKey;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "InterlockRuleHelper::setEntityKey" );
    }


    std::string InterlockRuleHelper::getEntityType()
    { 
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_entityType;
    }

    void InterlockRuleHelper::setEntityType( const std::string& entityType)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "InterlockRuleHelper::setEntityType" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_entityType = entityType;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "InterlockRuleHelper::setEntityType" );
    }

    
    bool InterlockRuleHelper::getEnabled()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_enabled;
    }


    void InterlockRuleHelper::setEnabled(bool enabled)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "InterlockRuleHelper::setEnabled" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_enabled = enabled;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "InterlockRuleHelper::setEnabled" );
    }


    bool InterlockRuleHelper::getPreCondition()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_preCondition;
    }


    void InterlockRuleHelper::setPreCondition(bool preCondition)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "InterlockRuleHelper::setPreCondition" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_preCondition = preCondition;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "InterlockRuleHelper::setPreCondition" );
    }


    IInterlockAliasVector InterlockRuleHelper::getAliases()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_aliases;
    }


    void InterlockRuleHelper::setAliases(const IInterlockAliasVector& aliases)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "InterlockRuleHelper::setAliases" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        
        // clear out old aliases and replace with new
        m_aliases.clear();
        m_aliases = aliases;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "InterlockRuleHelper::setParameter" );
    }


    void InterlockRuleHelper::invalidate()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "InterlockRuleHelper::invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new rule");

        m_isValidData = false;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "InterlockRuleHelper::invalidate" );
    }


    void InterlockRuleHelper::reload()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "InterlockRuleHelper::reload" );
        // Ensure the rule has already been written to the database. It doesn't make
        // any sense to reload an rule that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This rule does not yet exist in the database. Therefore it cannot be reloaded.");
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // create the SQL string to retrieve the data of the operator
        // based upon the key
        // use nvl for those columns that may be null to prevent exceptions
        std::ostringstream sql;
		sql << "select NAME, ";
        sql <<        "ENTITY_KEY, ";
        sql <<        "ENTITY_TYPE, ";
        sql <<        "DESIRED_STATE, ";
        sql <<        "nvl(PARAMETERS, ' ') as PARAMETERS, ";
        sql <<        "PRECONDITION, ";
        sql <<        "CONDITION, ";
        sql <<        "nvl(DESCRIPTION, ' ') as DESCRIPTION, ";
        sql <<        "ENABLED ";
        sql << "FROM LK_INTERLOCKING_RULES WHERE LKIRUL_ID = " << m_key;
 
        // Set up the columnNames vector to be passed to executeQuery()
        std::string nameColumn = "NAME";
        std::string entityKeyColumn = "ENTITY_KEY";
        std::string entityTypeColumn = "ENTITY_TYPE";
        std::string desiredStateColumn = "DESIRED_STATE";
        std::string parametersColumn = "PARAMETERS";
        std::string preconditionColumn = "PRECONDITION";
		std::string conditionColumn = "CONDITION";
        std::string descriptionColumn = "DESCRIPTION";
        std::string enabledColumn = "ENABLED";

        std::vector<std::string> columnNames;
		columnNames.push_back(nameColumn);
        columnNames.push_back(entityKeyColumn);
		columnNames.push_back(entityTypeColumn);
        columnNames.push_back(desiredStateColumn);
		columnNames.push_back(parametersColumn);
		columnNames.push_back(preconditionColumn);
		columnNames.push_back(conditionColumn);
		columnNames.push_back(descriptionColumn);
		columnNames.push_back(enabledColumn);

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
			reasonMessage << "No interlocking rule found for pkey = " << m_key;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "More than one interlocking rule found for pkey = " << m_key;
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        try
        {
		    m_name = data->getStringData( 0, nameColumn );
            m_entityKey = data->getUnsignedLongData(0,entityKeyColumn);
		    m_entityType = data->getStringData( 0, entityTypeColumn );
            m_desiredState = data->getStringData(0,desiredStateColumn);
		    m_parameters = getParametersAsVector(data->getStringData( 0, parametersColumn ));
            m_preCondition = std::string(STRING_FALSE).compare(data->getStringData( 0, preconditionColumn )) == 0 ? false : true;
		    m_condition = data->getStringData( 0, conditionColumn );
		    m_description = data->getStringData( 0, descriptionColumn );
            m_enabled = std::string(STRING_FALSE).compare(data->getStringData( 0, enabledColumn )) == 0 ? false : true;
        }
        catch(...)
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;
        
        // load alias data
        reloadAliases();

        // Need to record that we now have valid data
        m_isValidData = true;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "OperatorHelper::reload" );
    }

    void InterlockRuleHelper::reloadAliases()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "InterlockRuleHelper::reloadAliases" );

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
        sql <<        "VARIABLE_NAME, ";
        sql <<        "VARIABLE_TYPE ";
        sql << "FROM LK_ALIAS WHERE LKIRUL_ID = " << m_key;
 
        // Set up the columnNames vector to be passed to executeQuery()
        std::string aliasColumn = "ALIAS";
        std::string variableNameColumn = "VARIABLE_NAME";
        std::string variableTypeColumn = "VARIABLE_TYPE";

        std::vector<std::string> columnNames;
		columnNames.push_back(aliasColumn);
        columnNames.push_back(variableNameColumn);
		columnNames.push_back(variableTypeColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql.str(),columnNames);

        // remove old stuff and add new
        m_aliases.clear();
        try
        {
            do
            {
                // Create a new Alias object for each row of data.
                for(unsigned int i = 0; i < data->getNumRows(); i ++)
                {
                    boost::shared_ptr<InterlockAlias> alias(new InterlockAlias(m_key));
                    alias->setAlias(data->getStringData(i, aliasColumn));
                    alias->setVariableName(data->getStringData(i, variableNameColumn));
                    alias->setVariableType(data->getStringData(i, variableTypeColumn));
                    m_aliases.push_back(alias);
                }

                // Delete the data object.
                delete data;
                data = NULL;

            }
            while( databaseConnection->moreData(data) );

        }

        // Ensure any memory gets cleaned up if there is an exception.
        catch(...)
        {
            delete data;
            data = NULL;

            throw;
        }
    }
    void InterlockRuleHelper::writeInterlockRuleData()
    {       
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "InterlockRuleHelper::writeInterlockRuleData" );     

        // Check all required values exist. If not add them to the dataConfiguraitonException
        std::vector<std::string> fieldNames;
        if ( m_name.empty() )
        {
            fieldNames.push_back("Name");
        }
        if (m_entityKey == 0)
        {
            fieldNames.push_back("Entity Key");
        }
        if ( m_entityType.empty() )
        {
            fieldNames.push_back("Entity Type");
        }
        if (m_desiredState.empty())
        {
            fieldNames.push_back("Desired State");
        }
        if ( m_condition.empty() )
        {
            fieldNames.push_back("Condition");
        }
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("Interlocking rule data not fully specified. Interlocking rule cannot be written to database",
                                                         fieldNames));
        }

        // Write the Operator to the database

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);
        
        // Need to check if this is a new or existing rule - 
        // as a new rules will be inserted, while an existing rules will be updated
        if (m_isNew) // This is a new rule
        {
            addNewInterlockRule();
        }
        else // This is an existing Operator
        {
            modifyExistingInterlockRule();
        }
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "InterlockRuleHelper::writeInterlockRuleData" );
    }


    void InterlockRuleHelper::deleteInterlockRule()
    {
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "About to delete Interlocking Rule %s .",m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

        // must delete aliases first
        std::ostringstream sql;
        sql << "delete from LK_ALIAS where LKIRUL_ID = " << m_key;
        databaseConnection->executeModification(sql.str().c_str());

        sql.str("");
        sql << "delete from LK_INTERLOCKING_RULES where LKIRUL_ID = " << m_key;
        databaseConnection->executeModification(sql.str().c_str());

        // nothing more to be done here

    }


    void InterlockRuleHelper::modifyExistingInterlockRule()
    {
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "Interlocking Rule %s already exists. It's data will be updated.",m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Write);

        std::ostringstream sql;
        sql << "update LK_INTERLOCKING_RULES set ";
        sql <<         "NAME = '"          << (databaseConnection->escapeInsertString(m_name)).c_str() << "', ";
        sql <<         "ENTITY_KEY = "     << m_entityKey    << ", ";
        sql <<         "ENTITY_TYPE = '"   << m_entityType   << "', ";
        sql <<         "DESIRED_STATE = '" << (databaseConnection->escapeInsertString(m_desiredState)).c_str() << "', ";
        sql <<         "PARAMETERS = '"    << getParametersAsString(*databaseConnection)                    << "', ";
        sql <<         "PRECONDITION = '"  << (m_preCondition ? STRING_TRUE : STRING_FALSE) << "', ";
        sql <<         "CONDITION = '"     << (databaseConnection->escapeInsertString(m_condition)).c_str()    << "', ";
        sql <<         "DESCRIPTION = '"   << (databaseConnection->escapeInsertString(m_description)).c_str()  << "', ";
        sql <<         "ENABLED = '"       << (m_enabled ? STRING_TRUE : STRING_FALSE)     << "' ";
        sql << " where LKIRUL_ID = " << m_key;

        databaseConnection->executeModification(sql.str().c_str());

        // replace the aliases
        sql.str("");
        sql << "delete from LK_ALIAS where LKIRUL_ID = " << m_key;
        databaseConnection->executeModification(sql.str().c_str());

        IInterlockAliasVector::iterator alias = m_aliases.begin();
        while ( alias != m_aliases.end() )
        {
            (*alias)->save();
            alias++;
        }

        // nothing more to be done here
    }


    void InterlockRuleHelper::addNewInterlockRule()
    {

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Interlocking rule %s is a new rule. It will be added to the database.",m_name.c_str());

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // do not exceed max rules (60000)
        std::ostringstream sql;
        sql << ("select count(LKIRUL_ID) c from LK_INTERLOCKING_RULES");

        std::string countColumn = "c";
        std::vector<std::string> columnNames;
        columnNames.push_back(countColumn);

        IData* data = databaseConnection->executeQuery(sql.str(),columnNames);

        try
        {
            int records = data->getUnsignedLongData(0,countColumn);
            if ( records >= MAX_RULES )
            {
                TA_THROW(DataException("Too many rules",DataException::NO_VALUE,""));
            }
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // get a new LKIRUL_ID 
        sql.str("");
        sql << "select LKIRUL_SEQ.NEXTVAL from DUAL";

        std::string keyColumn = "LKIRUL_ID";
        columnNames.clear();
        columnNames.push_back(keyColumn);

        data = databaseConnection->executeQuery(sql.str(),columnNames);

        try
        {
            m_key = data->getUnsignedLongData(0,keyColumn);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // now we can insert our record with our new LKIRUL_ID
        sql.str("");
        sql << "insert into LK_INTERLOCKING_RULES (LKIRUL_ID, NAME, ENTITY_KEY, ENTITY_TYPE, DESIRED_STATE, "
                                                  "PARAMETERS, PRECONDITION, CONDITION, DESCRIPTION, ENABLED) ";
        sql << "values (";
        sql <<          m_key << ",";
        sql <<   "'" << m_name << "', ";
        sql <<          m_entityKey << ", ";
        sql <<   "'" << m_entityType << "', ";
        sql <<   "'" << (databaseConnection->escapeInsertString(m_desiredState)).c_str() << "', ";
        sql <<   "'" << getParametersAsString(*databaseConnection) << "', ";
        sql <<   "'" << (m_preCondition ? STRING_TRUE : STRING_FALSE) << "', ";
		sql <<   "'" << (databaseConnection->escapeInsertString(m_condition)).c_str() << "', ";
		sql <<   "'" << (databaseConnection->escapeInsertString(m_description)).c_str() << "', ";
        sql <<   "'" << (m_enabled ? STRING_TRUE : STRING_FALSE) << "' ";
        sql << ")";

        databaseConnection->executeModification(sql.str());

        // save the aliases
        IInterlockAliasVector::iterator alias = m_aliases.begin();
        while ( alias != m_aliases.end() )
        {
            (*alias)->setRuleId( m_key );
            (*alias)->save();
            alias++;
        }
    }

    std::string InterlockRuleHelper::getParametersAsString(IDatabase& databaseConnection)
    {
        std::string parametersString = "";

        for(std::vector<std::string>::iterator it = m_parameters.begin();
            it != m_parameters.end();
            it ++)
        {
            parametersString += databaseConnection.escapeInsertString(*it);
            parametersString += PARAMETER_SEPARATOR;
        }

        return parametersString;
    }


    std::vector<std::string> InterlockRuleHelper::getParametersAsVector(
        const std::string& parametersString)
    {
        FUNCTION_ENTRY("getParametersAsVector");

        //
        // Tokenise the string, splitting it on PARAMETER_SEPARATOR.
        //
        std::vector< std::string > values;

        const std::string::size_type SEPARATOR_LENGTH =
            PARAMETER_SEPARATOR.length();

        std::string::size_type nvBegin = 0;
        std::string::size_type nvEnd   = parametersString.find(PARAMETER_SEPARATOR);

        while(nvBegin != std::string::npos)
        {
            std::string::size_type nvLength = nvEnd - nvBegin;
            std::string value(parametersString, nvBegin, nvLength);

            if (value != "")
            {
                values.push_back(value);
            }

            nvBegin = nvEnd;
            if (nvEnd != std::string::npos)
            {
                nvBegin += SEPARATOR_LENGTH;
                nvEnd   = parametersString.find(PARAMETER_SEPARATOR, nvEnd + SEPARATOR_LENGTH);
            }
        }

        return values;
    }



} // closes TA_Base_Core
