/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/telephone/telephone_direct_lines_dai/src/TelephoneDirectLineHelper.cpp $
  * @author:  Raymond Pau
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/12/27 17:54:14 $
  * Last modified by:  $Author: weikun.lin $
  *
  * TelephoneDirectLineHelper is an object that is held by TelephoneDirectLine and ConfigTelephoneDirectLine objects.
  * It contains all data used by TelephoneDirectLines, and manipulation
  * methods for the data. It helps avoid re-writing code for both TelephoneDirectLine and ConfigTelephoneDirectLine.
  */


#pragma warning(disable:4786 4290)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include <vector>
#include <string>
#include <algorithm>
#include <ctime>

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/ValueNotSetException.h"

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/TelephoneDirectLineHelper.h"
#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/TelephoneDirectLineAccessFactory.h"
#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/DatabaseQueryHelper.h"

using namespace TA_Base_Core;
using namespace std;

namespace TA_IRS_Core
{

    const string TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_SEQ_NUM       = "TESDSE_SEQ";
    const string TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_TABLE_NAME    = "TE_SPEED_DIAL_SET";
    const string TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_PRI_KEY       = "TESDSE_ID";
    const string TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_OPERATOR_KEY  = "OPERATOR_KEY";
    const string TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_SEPROF_ID     = "SEPROF_ID";
    const string TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_SET_NAME      = "SET_NAME";
    const string TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_CREATED_BY    = "CREATED_BY";
    const string TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_DATE_CREATED  = "DATE_CREATED";
    const string TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_MODIFIED_BY   = "MODIFIED_BY";
    const string TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_DATE_MODIFIED = "DARE_MODIFIED";

    const string TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_SEQ_NUM         = "TESDBU_SEQ";
    const string TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_TABLE_NAME      = "TE_SPEED_DIAL_BUTTON";
    const string TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_PRI_KEY         = "TESDBU_ID";
    const string TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_TESDSE_ID       = "TESDSE_ID";
    const string TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_BUTTON_POSITION = "BUTTON_POSITION";
    const string TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_BUTTON_LABEL    = "BUTTON_LABEL";
    const string TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_BUTTON_TSI      = "BUTTON_TSI";
    const string TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_CREATED_BY      = "CREATED_BY";
    const string TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_DATE_CREATED    = "DATE_CREATED";
    const string TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_MODIFIED_BY     = "MODIFIED_BY";
    const string TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_DATE_MODIFIED   = "DATE_MODIFIED";



    TelephoneDirectLineHelper::TelephoneDirectLineHelper(const unsigned long btnKey)
        :m_isDialSetValidData(false),
         m_isDialBtnValidData(false),
         m_isDialBtnNew(false),
         m_isDialSetNew(false),
         m_isOperatorKeySet(false),
         m_isProfIdSet(false)
    {
        FUNCTION_ENTRY("TelephoneDirectLineHelper");
        initialize();
        m_dialBtnPrimaryKey.setValue(btnKey);
        FUNCTION_EXIT;
    }

    TelephoneDirectLineHelper::TelephoneDirectLineHelper(const TelephoneDirectLineHelper& theTelephoneDirectLineHelper)
        :m_isDialSetValidData(false),
         m_isDialBtnValidData(false),
         m_isDialBtnNew(true),
         m_isDialSetNew(false),
         m_isOperatorKeySet(true),
         m_isProfIdSet(true)
    {
        FUNCTION_ENTRY("TelephoneDirectLineHelper");
        m_dialSetPrimaryKey.setValue(theTelephoneDirectLineHelper.m_dialSetPrimaryKey.getValue());
        m_dialSetOperatorKey    = theTelephoneDirectLineHelper.m_dialSetOperatorKey;
        m_dialSetSeProfId       = theTelephoneDirectLineHelper.m_dialSetSeProfId;
  	    m_dialSetSetName        = theTelephoneDirectLineHelper.m_dialSetSetName;

        m_dialBtnPos            = theTelephoneDirectLineHelper.m_dialBtnPos;
        m_dialBtnLabel          = theTelephoneDirectLineHelper.m_dialBtnLabel;
        m_dialBtnTsi            = theTelephoneDirectLineHelper.m_dialBtnTsi;
        FUNCTION_EXIT;
    }

    TelephoneDirectLineHelper::TelephoneDirectLineHelper(unsigned long row, TA_Base_Core::IData& data)
        :m_isDialSetValidData(true),
         m_isDialBtnValidData(true),
         m_isDialBtnNew(false),
         m_isDialSetNew(false),
         m_isOperatorKeySet(true),
         m_isProfIdSet(true)
    {
        FUNCTION_ENTRY("TelephoneDirectLineHelper");
        reloadUsing(row, data);
        FUNCTION_EXIT;
    }

    TelephoneDirectLineHelper::TelephoneDirectLineHelper(unsigned long setPrimaryKey, long operaterKey, long profId, const std::string& setName)
        :m_isDialSetValidData(false),
         m_isDialBtnValidData(false),
         m_isDialBtnNew(true),
         m_isDialSetNew(false),
         m_isOperatorKeySet(true),
         m_isProfIdSet(true)
    {
        m_dialSetPrimaryKey.setValue(setPrimaryKey);
        m_dialSetOperatorKey    = operaterKey;
        m_dialSetSeProfId       = profId;
  	    m_dialSetSetName        = setName;
    }

    TelephoneDirectLineHelper::TelephoneDirectLineHelper()
        :m_isDialSetValidData(false),
         m_isDialBtnValidData(false),
         m_isDialBtnNew(true),
         m_isDialSetNew(true),
         m_isOperatorKeySet(false),
         m_isProfIdSet(false)
    {
        FUNCTION_ENTRY("TelephoneDirectLineHelper");
        initialize();
        FUNCTION_EXIT;
    }

    TelephoneDirectLineHelper::~TelephoneDirectLineHelper()
    {
        FUNCTION_ENTRY("~TelephoneDirectLineHelper");
        FUNCTION_EXIT;
    }

    void TelephoneDirectLineHelper::initialize()
    {
        FUNCTION_ENTRY("initialize");
        m_dialSetPrimaryKey.setValue(DatabaseKey::getInvalidKey());
        m_dialSetOperatorKey    = 0;
        m_dialSetSeProfId       = 0;
  	    m_dialSetSetName        = "";

        m_dialBtnPrimaryKey.setValue(DatabaseKey::getInvalidKey());
        m_dialBtnPos            = 0;
        m_dialBtnLabel          = "";
        m_dialBtnTsi            = "";
        FUNCTION_EXIT;
    }

    const DatabaseQueryHelper::QueryData
        TelephoneDirectLineHelper::getBasicQueryData()
    {
        FUNCTION_ENTRY("DatabaseQueryHelper::QueryData getBasicQueryData");
        DatabaseQueryHelper::QueryData result;

        result.primaryKeyCol    = TE_SPEED_DIAL_BTN_PRI_KEY;
	    result.tableName        = TE_SPEED_DIAL_BTN_TABLE_NAME;

        result.columnNames.push_back(TE_SPEED_DIAL_BTN_PRI_KEY);
        result.columnNames.push_back(TE_SPEED_DIAL_BTN_TESDSE_ID);
        result.columnNames.push_back(TE_SPEED_DIAL_BTN_BUTTON_POSITION);
	    result.columnNames.push_back(TE_SPEED_DIAL_BTN_BUTTON_LABEL);
        result.columnNames.push_back(TE_SPEED_DIAL_BTN_BUTTON_TSI);

        DatabaseQueryHelper::createBasicSelectStatement(result.sqlStatement,
															result.columnNames, 
                                                            result.tableName,
                                                            false);

        FUNCTION_EXIT;
        return result;
    }


    void TelephoneDirectLineHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");
        m_dialBtnPrimaryKey.setValue(data.getUnsignedLongData(row, TE_SPEED_DIAL_BTN_PRI_KEY));
        m_dialBtnPos = data.getIntegerData(row, TE_SPEED_DIAL_BTN_BUTTON_POSITION);
        m_dialBtnLabel = data.getStringData(row, TE_SPEED_DIAL_BTN_BUTTON_LABEL);
        m_dialBtnTsi = data.getStringData(row, TE_SPEED_DIAL_BTN_BUTTON_TSI);

        m_dialSetPrimaryKey.setValue(data.getUnsignedLongData(row, TE_SPEED_DIAL_BTN_TESDSE_ID));
        m_dialSetOperatorKey = data.getIntegerData(row, TE_SPEED_DIAL_SET_OPERATOR_KEY);
        m_dialSetSeProfId = data.getIntegerData(row, TE_SPEED_DIAL_SET_SEPROF_ID);
        m_dialSetSetName = data.getStringData(row, TE_SPEED_DIAL_SET_SET_NAME);

        m_isDialSetValidData = true;
        m_isDialBtnValidData = true;
        FUNCTION_EXIT;
    }

    unsigned long TelephoneDirectLineHelper::getKeyDirect() const
    {
        FUNCTION_ENTRY("getKeyDirect");
        try
        {
            if (m_isDialBtnNew && !m_dialBtnPrimaryKey.hasBeenSet())
            {
                FUNCTION_EXIT;
                return TA_Base_Core::DatabaseKey::getInvalidKey();
            }

            FUNCTION_EXIT;
            return m_dialBtnPrimaryKey.getValue();
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            FUNCTION_EXIT;
            // reload() should have either thrown exception or set this value
            TA_ASSERT(false, "No key data available");
            throw;
        }
        FUNCTION_EXIT;
    }

    void TelephoneDirectLineHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate");
        m_isDialSetValidData = false;
        m_isDialBtnValidData = false;
        FUNCTION_EXIT;
    }

    void TelephoneDirectLineHelper::writeBtnData()
    {
        FUNCTION_ENTRY("writeBtnData");
        std::vector<std::string> fieldsNames;

        if(m_dialSetPrimaryKey.hasBeenSet() == false)
        {
            fieldsNames.push_back(TE_SPEED_DIAL_BTN_TESDSE_ID);
        }

        if(fieldsNames.empty() == false)
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("Telephone Sub Directory data not completely specified. Data cannot be written to database", fieldsNames));
        }

        //try
        //{
            if (m_isDialBtnNew) // This is a new record
            {
                addNewBtnRecord();
            }
            else // This is an existing record
            {
                modifyExistingBtnRecord();
            }


        //}
        //catch (DatabaseException& e)
        //{
        //    FUNCTION_EXIT;
        //    throw e;
        //}
        FUNCTION_EXIT;

    }

    void TelephoneDirectLineHelper::addNewBtnRecord()
    {
        FUNCTION_ENTRY("addNewBtnRecord");
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
            FUNCTION_EXIT;
            TA_THROW(DatabaseException("Cannot connect to database"));
        }

        validateSpeedDialSetExistance();

        // Get a suitable primary key generated for this new entry
        m_dialBtnPrimaryKey = DatabaseQueryHelper::getNextSequenceNumber(
                                                                databaseConnection,
                                                                TE_SPEED_DIAL_BTN_SEQ_NUM);

        // create the SQL string to retrieve the data of the IncidentReportData
       /* std::ostringstream sql;
        sql << "insert into " << TE_SPEED_DIAL_BTN_TABLE_NAME
            << " (" << TE_SPEED_DIAL_BTN_PRI_KEY << ","
            << TE_SPEED_DIAL_BTN_TESDSE_ID << ","
            << TE_SPEED_DIAL_BTN_BUTTON_POSITION << ","
            << TE_SPEED_DIAL_BTN_BUTTON_LABEL << ","
            << TE_SPEED_DIAL_BTN_BUTTON_TSI << ") values ("
            << m_dialBtnPrimaryKey.getValue() << ","
            << m_dialSetPrimaryKey.getValue() << ","
            << m_dialBtnPos << ",'"
            << databaseConnection->escapeInsertString(m_dialBtnLabel).c_str()
            << "','"
            << databaseConnection->escapeInsertString(m_dialBtnTsi).c_str()
            << "')";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TE_SPEED_DIAL_BUTTON_STD_INSERT_56801,m_dialBtnPrimaryKey.getValue(),
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_SPEED_DIAL_BUTTON_INSERT_56801,m_dialBtnPrimaryKey.getValue(),
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_SPEED_DIAL_BUTTON_INSERT_56801,m_dialBtnPrimaryKey.getValue(),
			m_dialSetPrimaryKey.getValue(),m_dialBtnPos,databaseConnection->escapeInsertString(m_dialBtnLabel),databaseConnection->escapeInsertString(m_dialBtnTsi));

        databaseConnection->executeModification(strSql);
        m_isDialBtnNew = false;
        m_isDialBtnValidData = true;
        FUNCTION_EXIT;
    }

    void TelephoneDirectLineHelper::modifyExistingBtnRecord()
    {
        FUNCTION_ENTRY("modifyExistingBtnRecord");
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
            FUNCTION_EXIT;
            TA_THROW(DatabaseException("Cannot connect to database"));
        }

        validateSpeedDialSetExistance();

      /*  ostringstream sql;
        sql.str("");
        sql << "update " << TE_SPEED_DIAL_BTN_TABLE_NAME << " set "
            << TE_SPEED_DIAL_BTN_BUTTON_LABEL << " = '" << databaseConnection->escapeInsertString(m_dialBtnLabel).c_str() << "',"
            << TE_SPEED_DIAL_BTN_BUTTON_TSI  << "= '" << databaseConnection->escapeInsertString(m_dialBtnTsi).c_str()  << "'"
            << " where "
            << TE_SPEED_DIAL_BTN_TESDSE_ID << "=" << m_dialSetPrimaryKey.getValue() << " AND "
            << TE_SPEED_DIAL_BTN_BUTTON_POSITION << "=" << m_dialBtnPos;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TE_SPEED_DIAL_BUTTON_STD_UPDATE_56651,
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_SPEED_DIAL_BUTTON_UPDATE_56651,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_SPEED_DIAL_BUTTON_UPDATE_56651,
			databaseConnection->escapeInsertString(m_dialBtnLabel),databaseConnection->escapeInsertString(m_dialBtnTsi),
			m_dialSetPrimaryKey.getValue(), m_dialBtnPos);

        databaseConnection->executeModification(strSql);

        m_isDialBtnValidData = true;
        FUNCTION_EXIT;
    }

    void TelephoneDirectLineHelper::writeSetData()
    {
        FUNCTION_ENTRY("writeSetData");

        std::vector<std::string> fieldsNames;
        if(m_dialSetSetName.empty())
        {
            fieldsNames.push_back(TE_SPEED_DIAL_SET_SET_NAME);
        }
        if(m_isOperatorKeySet== false)
        {
            fieldsNames.push_back(TE_SPEED_DIAL_SET_OPERATOR_KEY);
        }
        if(m_isProfIdSet== false)
        {
            fieldsNames.push_back(TE_SPEED_DIAL_SET_SEPROF_ID);
        }
        if(fieldsNames.empty() == false)
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("Telephone Sub Directory data not completely specified. Data cannot be written to database", fieldsNames));
        }

        //try
        //{
            // Check to see if TE_SPEED_DIAL_SET has this set record
            if(m_isDialSetNew)
            {
                addNewSetRecord();
            }
            else
            {
                modifyExistingSetRecord();
            }
        //}
        //catch (DatabaseException& e)
        //{
        //    FUNCTION_EXIT;
        //    throw e;
        //}
        FUNCTION_EXIT;
    }

    void TelephoneDirectLineHelper::addNewSetRecord()
    {
        FUNCTION_ENTRY("addNewSetRecord");
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
            FUNCTION_EXIT;
            TA_THROW(DatabaseException("Cannot connect to database"));
        }

        // Get a suitable primary key generated for this new entry
        m_dialSetPrimaryKey = DatabaseQueryHelper::getNextSequenceNumber(
                                                        databaseConnection,
                                                        TE_SPEED_DIAL_SET_SEQ_NUM);
        //m_dialBtnDialSetId = m_dialSetPrimaryKey.getValue();

        // create the SQL string to retrieve the data of the IncidentReportData
     /*   std::ostringstream sql;
        sql << "insert into " << TE_SPEED_DIAL_SET_TABLE_NAME
            << " (" << TE_SPEED_DIAL_SET_PRI_KEY << ","
            << TE_SPEED_DIAL_SET_OPERATOR_KEY << ","
            << TE_SPEED_DIAL_SET_SEPROF_ID << ","
            << TE_SPEED_DIAL_SET_SET_NAME << ") values ("
            << m_dialSetPrimaryKey.getValue() << ","
            << m_dialSetOperatorKey << ","
            << m_dialSetSeProfId << ",'"
            << databaseConnection->escapeInsertString(m_dialSetSetName).c_str()
            << "')";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TE_SPEED_DIAL_BUTTON_STD_INSERT_56802,
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_SPEED_DIAL_BUTTON_INSERT_56802,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_SPEED_DIAL_BUTTON_INSERT_56802,
			m_dialSetPrimaryKey.getValue(), m_dialSetOperatorKey, m_dialSetSeProfId,databaseConnection->escapeInsertString(m_dialSetSetName));

        databaseConnection->executeModification(strSql);
        m_isDialSetNew = false;
        m_isDialSetValidData = true;
        FUNCTION_EXIT;
    }

    void TelephoneDirectLineHelper::modifyExistingSetRecord()
    {
        FUNCTION_ENTRY("modifyExistingSetRecord");
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
            FUNCTION_EXIT;
            TA_THROW(DatabaseException("Cannot connect to database"));
        }

        std::vector<std::string> fieldsNames;
        if (m_dialSetPrimaryKey.hasBeenSet() == false)
        {
            fieldsNames.push_back(TE_SPEED_DIAL_SET_PRI_KEY);
        }
        if(fieldsNames.empty() == false)
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("Telephone Sub Directory data not completely specified. Data cannot be written to database", fieldsNames));
        }

       /* ostringstream sql;

        sql.str("");
        sql << "update " << TE_SPEED_DIAL_SET_TABLE_NAME << " set "
            << TE_SPEED_DIAL_SET_OPERATOR_KEY << " = " << m_dialSetOperatorKey << ","
            << TE_SPEED_DIAL_SET_SEPROF_ID << " = " << m_dialSetSeProfId << ","
            << TE_SPEED_DIAL_SET_SET_NAME << " = '" << databaseConnection->escapeInsertString(m_dialSetSetName).c_str() << "'"
            << " where "
            << TE_SPEED_DIAL_SET_PRI_KEY << "=" << m_dialSetPrimaryKey.getValue();*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TE_SPEED_DIAL_BUTTON_STD_UPDATE_56652,
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_SPEED_DIAL_BUTTON_UPDATE_56652,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_SPEED_DIAL_BUTTON_UPDATE_56652,
			m_dialSetOperatorKey, m_dialSetSeProfId, databaseConnection->escapeInsertString(m_dialSetSetName),
			 m_dialSetPrimaryKey.getValue());

        databaseConnection->executeModification(strSql);
        m_isDialSetValidData = true;
        FUNCTION_EXIT;
    }

    void TelephoneDirectLineHelper::deleteTeSpeedDialSetRecord(bool cascade)
    {
        FUNCTION_ENTRY("deleteTeSpeedDialSetRecord");
        if(m_isDialSetNew || !m_isDialSetValidData)  // No need to remove a new record
            return;

        // Calculate required sequence number (primary key) for insertion
        TA_Base_Core::IDatabase* databaseConnection =
                    TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Read);

        if (databaseConnection == NULL)
        {
            FUNCTION_EXIT;
            TA_THROW(DatabaseException("Cannot connect to database"));
        }

        if (!cascade)
        {
           /* std::ostringstream sql;
	        sql << "select "
                << TE_SPEED_DIAL_BTN_PRI_KEY
                << " from "
                << TE_SPEED_DIAL_BTN_TABLE_NAME
                << " INNER JOIN "
                << TE_SPEED_DIAL_SET_TABLE_NAME
                << " USING (TESDSE_UD) where "
                <<      TE_SPEED_DIAL_SET_PRI_KEY << "=" << m_dialSetPrimaryKey.getValue();*/
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TE_SPEED_DIAL_BUTTON_STD_SELECT_56504,
//			std::string strSql  = databaseConnection->prepareSQLStatement(TE_SPEED_DIAL_BUTTON_SELECT_56504,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_SPEED_DIAL_BUTTON_SELECT_56504,
				m_dialSetPrimaryKey.getValue());

            // Set up the columnNames vector to be passed to executeQuery()
            vector<string> columnNames;
            columnNames.push_back(TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_PRI_KEY);

            // Execute the query. The method can throw a DatabaseException.
            // This is documented in the comment of this method.
            // We are responsible for deleting the returned IData object when we're done with it
            IData* data = databaseConnection->executeQuery(strSql,columnNames);
            TA_ASSERT(data != NULL, "NULL item returned after executeQuery");

            if(0 < data->getNumRows())
            {
                delete data;
                data = NULL;

                FUNCTION_EXIT;

                TA_THROW(TA_Base_Core::DataException("A reference to this speed dial set exists in the TE_SPEED_DIAL_BUTTON table",
                            TA_Base_Core::DataException::CANNOT_DELETE,"TE_SPEED_DIAL_SET"));
            }

            delete data;
            data = NULL;
        }


     /*   std::ostringstream sql;

        sql << "delete " << TE_SPEED_DIAL_SET_TABLE_NAME
            << " where "
            <<      TE_SPEED_DIAL_SET_PRI_KEY << " = " << m_dialSetPrimaryKey.getValue()
            <<      " AND "
            <<      TE_SPEED_DIAL_SET_OPERATOR_KEY << " = " << m_dialSetOperatorKey << ","
            <<      " AND "
            <<      TE_SPEED_DIAL_SET_SEPROF_ID << " = " << m_dialSetSeProfId << ","
            <<      " AND "
            <<      TE_SPEED_DIAL_SET_SET_NAME << " = '"
            <<      databaseConnection->escapeInsertString(m_dialSetSetName).c_str() << "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TE_SPEED_DIAL_BUTTON_STD_DELETE_56901,
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_SPEED_DIAL_BUTTON_DELETE_56901,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_SPEED_DIAL_BUTTON_DELETE_56901,
			m_dialSetPrimaryKey.getValue(),m_dialSetOperatorKey,m_dialSetSeProfId,databaseConnection->escapeInsertString(m_dialSetSetName));

        databaseConnection->executeModification(strSql);
        FUNCTION_EXIT;
    }

    void TelephoneDirectLineHelper::deleteTeSpeedDialButtonRecord()
    {
        FUNCTION_ENTRY("deleteTeSpeedDialButtonRecord");
        if(m_isDialBtnNew || !m_isDialBtnValidData)  // No need to remove a new record
            return;

        // Calculate required sequence number (primary key) for insertion
        TA_Base_Core::IDatabase* databaseConnection =
                    TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Write);

        if (databaseConnection == NULL)
        {
            FUNCTION_EXIT;
            TA_THROW(DatabaseException("Cannot connect to database"));
        }

       /* std::ostringstream sql;

        sql << "delete " << TE_SPEED_DIAL_BTN_TABLE_NAME
            << " where "
            <<      TE_SPEED_DIAL_BTN_PRI_KEY << " = " << m_dialBtnPrimaryKey.getValue() ;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TE_SPEED_DIAL_BUTTON_STD_DELETE_56902,
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_SPEED_DIAL_BUTTON_DELETE_56902,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_SPEED_DIAL_BUTTON_DELETE_56902,
			m_dialBtnPrimaryKey.getValue());

        databaseConnection->executeModification(strSql);
        FUNCTION_EXIT;
    }

    void TelephoneDirectLineHelper::validateSpeedDialSetExistance()
    {
        FUNCTION_ENTRY("validateSpeedDialSetExistance");

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Read);
        if (databaseConnection == NULL)
        {
            FUNCTION_EXIT;
            TA_THROW(DatabaseException("Cannot connect to database"));
        }

       /* ostringstream sql;
        sql.str("");
        sql << "select "
            << TE_SPEED_DIAL_SET_PRI_KEY
            << " from "
            << TE_SPEED_DIAL_SET_TABLE_NAME
            << " where "
            <<      TE_SPEED_DIAL_SET_PRI_KEY << "=" << m_dialSetPrimaryKey.getValue();*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TE_SPEED_DIAL_BUTTON_STD_SELECT_56505,
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_SPEED_DIAL_BUTTON_SELECT_56505,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_SPEED_DIAL_BUTTON_SELECT_56505,
			m_dialSetPrimaryKey.getValue());

        std::vector<std::string> columnNames;
        columnNames.push_back(TE_SPEED_DIAL_SET_PRI_KEY);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows())
        {
            // record not found

            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No speed dial set record of key " << m_dialSetPrimaryKey.getValue() << " can be found";

            FUNCTION_EXIT;
            TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"Speed Dial Set Record" ) );
        }

        delete data;
        data = NULL;
        
        FUNCTION_EXIT;
    }

    unsigned long TelephoneDirectLineHelper::getDialSetPrimaryKey()
    {
        FUNCTION_ENTRY("getDialSetPrimaryKey");
        if (!m_isDialSetValidData && !m_isDialSetNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_dialSetPrimaryKey.getValue();
    }

    void TelephoneDirectLineHelper::setDialSetPrimaryKey(unsigned long key)
    {
        FUNCTION_ENTRY("setDialSetPrimaryKey");
        m_dialSetPrimaryKey = key;
        FUNCTION_EXIT;
    }

    long TelephoneDirectLineHelper::getDialSetOperatorKey()
    {
        FUNCTION_ENTRY("getDialSetOperatorKey");
        if (!m_isDialSetValidData && !m_isDialSetNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_dialSetOperatorKey;
    }

    void TelephoneDirectLineHelper::setDialSetOperatorKey(long key)
    {
        FUNCTION_ENTRY("setDialSetOperatorKey");
        m_dialSetOperatorKey = key;
        m_isOperatorKeySet = true;
        FUNCTION_EXIT;
    }

    long TelephoneDirectLineHelper::getDialSetSeProfId()
    {
        FUNCTION_ENTRY("getDialSetSeProfId");
        if (!m_isDialSetValidData && !m_isDialSetNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_dialSetSeProfId;
    }

    void TelephoneDirectLineHelper::setDialSetSeProfId(long id)
    {
        FUNCTION_ENTRY("setDialSetSeProfId");
        m_dialSetSeProfId = id;
        m_isProfIdSet = true;
        FUNCTION_EXIT;
    }

    std::string TelephoneDirectLineHelper::getDialSetSetName()
    {
        FUNCTION_ENTRY("string TelephoneDirectLineHelper::getDialSetSetName");
        if (!m_isDialSetValidData && !m_isDialSetNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_dialSetSetName;
    }

    void TelephoneDirectLineHelper::setDialSetSetName(const std::string& name)
    {
        FUNCTION_ENTRY("setDialSetSetName");
        m_dialSetSetName = name;
        FUNCTION_EXIT;
    }

    bool TelephoneDirectLineHelper::getIsDialSetValid()
    {
        FUNCTION_ENTRY("getIsDialSetValid");
        FUNCTION_EXIT;
        return m_isDialSetValidData;
    }

    void TelephoneDirectLineHelper::setIsDialSetValid(bool state)
    {
        FUNCTION_ENTRY("setIsBialSetValid");
        m_isDialSetValidData = state;
        FUNCTION_EXIT;
    }

    bool TelephoneDirectLineHelper::getIsDialSetNew()
    {
        FUNCTION_ENTRY("getIsDialSetNew");
        FUNCTION_EXIT;
        return m_isDialSetNew;
    }

    void TelephoneDirectLineHelper::setIsBialSetNew(bool state)
    {
        FUNCTION_ENTRY("setIsBialSetNew");
        m_isDialSetNew = state;
        FUNCTION_EXIT;
    }

    unsigned long TelephoneDirectLineHelper::getDialBtnPrimaryKey()
    {
        FUNCTION_ENTRY("getDialBtnPrimaryKey");
        if (!m_isDialBtnValidData && !m_isDialBtnNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_dialBtnPrimaryKey.getValue();
    }

    void TelephoneDirectLineHelper::setDialBtnPrimaryKey(unsigned long key)
    {
        FUNCTION_ENTRY("setDialBtnPrimaryKey");
        m_dialBtnPrimaryKey = key;
        FUNCTION_EXIT;
    }

    unsigned long TelephoneDirectLineHelper::getDialBtnDialSetId()
    {
        FUNCTION_ENTRY("getDialBtnDialSetId");
        if (!m_isDialBtnValidData && !m_isDialBtnNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_dialSetPrimaryKey.getValue();
    }

    void TelephoneDirectLineHelper::setDialBtnDialSetId(unsigned long id)
    {
        FUNCTION_ENTRY("setDialBtnDialSetId");
        m_dialSetPrimaryKey.setValue(id);
        FUNCTION_EXIT;
    }


    int TelephoneDirectLineHelper::getDialBtnPos()
    {
        FUNCTION_ENTRY("getDialBtnPos");
        if (!m_isDialBtnValidData && !m_isDialBtnNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_dialBtnPos;
    }

    void TelephoneDirectLineHelper::setDialBtnPos(int btnPos)
    {
        FUNCTION_ENTRY("setDialBtnPos");
        m_dialBtnPos = btnPos;
        FUNCTION_EXIT;
    }

    std::string TelephoneDirectLineHelper::getDialBtnLabel()
    {
        FUNCTION_ENTRY("string TelephoneDirectLineHelper::getDialBtnLabel");
        if (!m_isDialBtnValidData && !m_isDialBtnNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_dialBtnLabel;
    }

    std::string TelephoneDirectLineHelper::getDialBtnTsi()
    {
        FUNCTION_ENTRY("string TelephoneDirectLineHelper::getDialBtnTsi");
        if (!m_isDialBtnValidData && !m_isDialBtnNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_dialBtnTsi;
    }

    void TelephoneDirectLineHelper::setDialBtnTsi(const std::string& tsi)
    {
        FUNCTION_ENTRY("setDialBtnTsi");
        m_dialBtnTsi = tsi;
        FUNCTION_EXIT;
    }

    void TelephoneDirectLineHelper::setDialBtnLabel(const std::string& label)
    {
        FUNCTION_ENTRY("setDialBtnLabel");
        m_dialBtnLabel = label;
        FUNCTION_EXIT;
    }

    bool TelephoneDirectLineHelper::getIsDialBtnValidData()
    {
        FUNCTION_ENTRY("getIsDialBtnValidData");
        FUNCTION_EXIT;
        return m_isDialBtnValidData;
    }

    void TelephoneDirectLineHelper::setIsDialBtnValidData(bool state)
    {
        FUNCTION_ENTRY("setIsDialBtnValidData");
        m_isDialBtnValidData = state;
        FUNCTION_EXIT;
    }

    bool TelephoneDirectLineHelper::getIsDialBtnNew()
    {
        FUNCTION_ENTRY("getIsDialBtnNew");
        FUNCTION_EXIT;
        return m_isDialBtnNew;
    }

    void TelephoneDirectLineHelper::setIsDialBtnNew(bool state)
    {
        FUNCTION_ENTRY("setIsDialBtnNew");
        m_isDialBtnNew = state;
        FUNCTION_EXIT;
    }
} // close TA_IRS_Core
