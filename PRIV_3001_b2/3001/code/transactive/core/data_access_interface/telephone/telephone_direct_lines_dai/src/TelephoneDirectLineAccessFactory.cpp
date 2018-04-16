/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/telephone/telephone_direct_lines_dai/src/TelephoneDirectLineAccessFactory.cpp $
  * @author Raymond Pau
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * TelephoneDirectLineAccessFactory is a singleton that is used to retrieve TelephoneDirectLine objects either from the
  * database or newly created. All TelephoneDirectLine objects returned will adhere to the ITelephoneDirectLine interface.
  */
#pragma warning(disable:4786 4290)

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/TelephoneDirectLineAccessFactory.h"
#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/TelephoneDirectLine.h"
#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/ConfigTelephoneDirectLine.h"

using namespace std;
using namespace TA_Base_Core;

namespace TA_IRS_Core
{

    TelephoneDirectLineAccessFactory* TelephoneDirectLineAccessFactory::m_instance = NULL;


    TelephoneDirectLineAccessFactory& TelephoneDirectLineAccessFactory::getInstance()
    {
        FUNCTION_ENTRY("getInstance");
        if (m_instance == NULL)
        {
            m_instance = new TelephoneDirectLineAccessFactory();
        }
        FUNCTION_EXIT;
        return *m_instance;
    }


    void TelephoneDirectLineAccessFactory::removeInstance()
    {
        FUNCTION_ENTRY("removeInstance");
        if (m_instance != NULL)
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
        FUNCTION_EXIT;
    }

    ITelephoneDirectLine* TelephoneDirectLineAccessFactory::getButtonDetails(const long operatorKey, 
                                                                             const long profileId, 
                                                                             const std::string setName, 
                                                                             const int btnPos, 
                                                                             bool readWrite)
    {
        FUNCTION_ENTRY("getButtonDetails");
        // get a connection to the database
        IDatabase* databaseConnection = 
                DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Read);

        if (databaseConnection == NULL)
        {
            FUNCTION_EXIT;
            TA_THROW(DatabaseException("Cannot connect to database"));
        }

       /* std::ostringstream sql;
	    sql << "select "
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_PRI_KEY
            << ","
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_TESDSE_ID
            << ","
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_BUTTON_POSITION
            << ","
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_BUTTON_LABEL
            << ","
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_BUTTON_TSI
            << ","
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_SET_NAME
            << ","
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_OPERATOR_KEY
            << ","
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_SEPROF_ID
            << " from " 
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_TABLE_NAME
            << " INNER JOIN "
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_TABLE_NAME
            << " USING (TESDSE_ID) where "
            <<      TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_OPERATOR_KEY << " = " << operatorKey
            <<      " AND "
            <<      TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_SEPROF_ID << "=" << profileId
            <<      " AND "
            <<      TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_SET_NAME << "='" 
            <<      databaseConnection->escapeInsertString(setName).c_str() << "'"
            <<      " AND "
            <<      TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_BUTTON_POSITION << "=" << btnPos ;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TE_SPEED_DIAL_BUTTON_STD_SELECT_56501,operatorKey,
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_SPEED_DIAL_BUTTON_SELECT_56501,operatorKey,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_SPEED_DIAL_BUTTON_SELECT_56501,operatorKey,
			profileId, databaseConnection->escapeInsertString(setName), btnPos);

        // Set up the columnNames vector to be passed to executeQuery()
        vector<string> columnNames;
        columnNames.push_back(TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_PRI_KEY);
        columnNames.push_back(TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_TESDSE_ID);
        columnNames.push_back(TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_BUTTON_POSITION);
	    columnNames.push_back(TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_BUTTON_LABEL);
        columnNames.push_back(TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_BUTTON_TSI);

	    //columnNames.push_back(TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_PRI_KEY);
	    columnNames.push_back(TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_SET_NAME);
	    columnNames.push_back(TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_OPERATOR_KEY);
	    columnNames.push_back(TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_SEPROF_ID);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);
        TA_ASSERT(data != NULL, "NULL item returned after executeQuery");

        if(data->getNumRows() == 0 )
        {
            delete data;
            data = NULL;
        
            FUNCTION_EXIT;
            std::ostringstream reason;
            reason << "There are no data for setName [" << setName 
                   << "] and button Position [" << btnPos << "]"
                   << "for current profile and operator";
            TA_THROW(DataException(reason.str().c_str(), DataException::NO_VALUE, ""));
        }

        ITelephoneDirectLine* pTelDl;
        if(readWrite)
            pTelDl = new ConfigTelephoneDirectLine(0, *data);
        else
            pTelDl = new TelephoneDirectLine(0, *data);

        delete data;
        data = NULL;

        FUNCTION_EXIT;
        return pTelDl;
    }


    ITelephoneDirectLines TelephoneDirectLineAccessFactory::getButtonSet(const long operatorKey, const long profileId, const std::string setName, bool readWrite)
    {
        FUNCTION_ENTRY("getButtonSet");
        // get a connection to the database
        IDatabase* databaseConnection = 
                DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Read);

        if (databaseConnection == NULL)
        {
            FUNCTION_EXIT;
            TA_THROW(DatabaseException("Cannot connect to database"));
        }

       /* std::ostringstream sql;
	    sql << "select "
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_PRI_KEY
            << ","
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_TESDSE_ID
            << ","
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_BUTTON_POSITION
            << ","
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_BUTTON_LABEL
            << ","
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_BUTTON_TSI
            << ","
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_SET_NAME
            << ","
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_OPERATOR_KEY
            << ","
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_SEPROF_ID
            << " from " 
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_TABLE_NAME
            << " INNER JOIN "
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_TABLE_NAME
            << " USING (TESDSE_ID) where "
            <<      TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_OPERATOR_KEY << "=" << operatorKey
            <<      " AND "
            <<      TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_SEPROF_ID << "=" << profileId
            <<      " AND "
            <<      TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_SET_NAME << "='" 
            <<      databaseConnection->escapeInsertString(setName).c_str() << "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(),TE_SPEED_DIAL_BUTTON_STD_SELECT_56502, operatorKey,
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_SPEED_DIAL_BUTTON_SELECT_56502, operatorKey,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_SPEED_DIAL_BUTTON_SELECT_56502, operatorKey,
			profileId, databaseConnection->escapeInsertString(setName));

        // Set up the columnNames vector to be passed to executeQuery()
        vector<string> columnNames;
        columnNames.push_back(TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_PRI_KEY);
        columnNames.push_back(TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_TESDSE_ID);
        columnNames.push_back(TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_BUTTON_POSITION);
	    columnNames.push_back(TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_BUTTON_LABEL);
        columnNames.push_back(TelephoneDirectLineHelper::TE_SPEED_DIAL_BTN_BUTTON_TSI);

        columnNames.push_back(TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_SET_NAME);
	    columnNames.push_back(TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_OPERATOR_KEY);
	    columnNames.push_back(TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_SEPROF_ID);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);
        TA_ASSERT(data != NULL, "NULL item returned after executeQuery");    

        ITelephoneDirectLines btnList;
        if(data->getNumRows() == 0 )
        {
            delete data;
            data = NULL;
            
            FUNCTION_EXIT;
            return btnList;
        }

        do 
        {
            ITelephoneDirectLine* pTelDl = NULL;
            for(unsigned int i = 0; i < data->getNumRows(); i++)
            {
                if(readWrite)
                    pTelDl = new ConfigTelephoneDirectLine(i, *data);
                else
                    pTelDl = new TelephoneDirectLine(i, *data);

                btnList.push_back(pTelDl);
            }
        	
            delete data;
            data = NULL;

        } while(databaseConnection->moreData(data) == true);

        FUNCTION_EXIT;
        return btnList;
    }


    ITelephoneDirectLine* TelephoneDirectLineAccessFactory::getSetDetails(const long operatorKey, 
                                                                         const long profileId, 
                                                                         const std::string setName, 
                                                                         bool readWrite)
    {
        FUNCTION_ENTRY("getButtonDetails");
        // get a connection to the database
        IDatabase* databaseConnection = 
                DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Telephone_Cd, TA_Base_Core::Read);

        if (databaseConnection == NULL)
        {
            FUNCTION_EXIT;
            TA_THROW(DatabaseException("Cannot connect to database"));
        }

      /*  std::ostringstream sql;
	    sql << "select "
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_PRI_KEY
            << " from " 
            << TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_TABLE_NAME
            << " WHERE " 
            <<      TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_OPERATOR_KEY << "=" << operatorKey
            <<      " AND "
            <<      TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_SEPROF_ID << "=" << profileId
            <<      " AND "
            <<      TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_SET_NAME << "='" 
            <<      databaseConnection->escapeInsertString(setName).c_str() << "'";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TE_SPEED_DIAL_BUTTON_STD_SELECT_56503, operatorKey,
//		std::string strSql  = databaseConnection->prepareSQLStatement(TE_SPEED_DIAL_BUTTON_SELECT_56503, operatorKey,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TE_SPEED_DIAL_BUTTON_SELECT_56503, operatorKey,
			profileId, databaseConnection->escapeInsertString(setName));

        // Set up the columnNames vector to be passed to executeQuery()
        vector<string> columnNames;
        columnNames.push_back(TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_PRI_KEY);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);
        TA_ASSERT(data != NULL, "NULL item returned after executeQuery"); 

        if(data->getNumRows() == 0 )
        {
            delete data;
            data = NULL;
        
            FUNCTION_EXIT;
            std::ostringstream reason;
            reason << "There are no data with setName [" << setName 
                   << "for current profile and operator";
            TA_THROW(DataException(reason.str().c_str(), DataException::NO_VALUE, ""));
        }

        if(data->getNumRows() > 1)
        {
            delete data;
            data = NULL;
        
            FUNCTION_EXIT;
            TA_THROW(DataException("There exist more than one TE_SPEED_DIAL_SET with the same name", DataException::NOT_UNIQUE, ""));
        }

        unsigned long setKey = data->getUnsignedLongData(0, TelephoneDirectLineHelper::TE_SPEED_DIAL_SET_PRI_KEY);

        ITelephoneDirectLine* pTelDl;
        if(readWrite)
        {
            pTelDl = new ConfigTelephoneDirectLine(setKey, operatorKey, profileId, setName);
        
        }
        else
        {
            pTelDl = new TelephoneDirectLine(setKey, operatorKey, profileId, setName);
        }

        delete data;
        data = NULL;

        FUNCTION_EXIT;
        return pTelDl;
    }

    ITelephoneDirectLine* TelephoneDirectLineAccessFactory::createNewButtonSetRecord(bool readWrite)
    {
        FUNCTION_ENTRY("createNewButtonSetRecord");

        ITelephoneDirectLine* pTelDl;
        if(readWrite)
        {
            pTelDl = new ConfigTelephoneDirectLine();
        
        }
        else
        {
            pTelDl = new TelephoneDirectLine();
        }

        FUNCTION_EXIT;
        return pTelDl;
    }

} // close TA_IRS_Core

