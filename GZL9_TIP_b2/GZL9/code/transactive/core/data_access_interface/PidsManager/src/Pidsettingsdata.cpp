/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/TA_BASE/transactive/core/data_access_interface/user_settings/src/PidSettingsData.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/10/31 12:12:40 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  *
  */
#pragma warning(disable:4786)

#include <string>
#include "PidSettingsData.h"
#include "core\data_access_interface\src\IDatabase.h"
#include "core\data_access_interface\src\IData.h"
#include "core\data_access_interface\src\DatabaseFactory.h"
#include "core\exceptions\src\DataUserSettingException.h"
#include "core\utilities\src\DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/SQLCode.h"

using namespace TA_Base_Core;
using TA_Base_Core::DataUserSettingException;
using TA_Base_Core::DebugUtil;
namespace 
{
	std::string WChar2Ansi(LPCWSTR pwszSrc)
	{
		int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
		if (nLen<= 0) return std::string("");
		char* pszDst = new char[nLen];
		if (NULL == pszDst) return std::string("");
		WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
		pszDst[nLen -1] = 0;
		std::string strTemp(pszDst);
		delete [] pszDst;
		return strTemp;
	}
	//string ws2s(wstring& inputws){ return WChar2Ansi(inputws.c_str()); }
	//Converting a Ansi string to WChar string
	std::wstring Ansi2WChar(LPCSTR pszSrc, int nLen)
	{
		int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
		if(nSize <= 0) return NULL;
		WCHAR *pwszDst = new WCHAR[nSize+1];
		if( NULL == pwszDst) return NULL;
		MultiByteToWideChar(CP_ACP, 0,(LPCSTR)pszSrc, nLen, pwszDst, nSize);
		pwszDst[nSize] = 0;
		if( pwszDst[0] == 0xFEFF) // skip Oxfeff
		for(int i = 0; i < nSize; i ++)
		pwszDst[i] = pwszDst[i+1];
		wstring wcharString(pwszDst);
		delete pwszDst;
		return wcharString;
	}
}
namespace TA_IRS_Core
{
	const std::string ID_COLUMN = "MSG_ID";
    const std::string NAME_COLUMN = "MSG_DESCRIPTION";
    const std::string VALUE_COLUMN = "MSG_CONTENT";

    PidSettingsData::PidSettingsData(std::string sessionId, unsigned long appType)
        : m_sessionId(sessionId), m_applicationType(appType), m_isValidData(false)
    {

    }


    PidSettingsData::~PidSettingsData()
    {

    }




    void PidSettingsData::setSetting(const int id, const std::wstring& name,const std::wstring& value)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "setSetting");

        updateSettingInDatabase(id, name,value);
		reload();
        LOG( SourceInfo, DebugUtil::FunctionExit, "setSetting");
    }


    
    void PidSettingsData::addSetting(const int id, const std::wstring& name, const std::wstring& value)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "addSetting");


        insertSettingIntoDatabase(id, name,value);

		reload();

        LOG( SourceInfo, DebugUtil::FunctionExit, "addSetting");
    }

  
    void PidSettingsData::reload()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "reload");

		m_pidMsgs.clear();
		
   //     std::string sql("select MSG_ID, MSG_DESCRIPTION, MSG_CONTENT from PID_MESSAGES ");
      /*  sql += m_sessionId;
        sql += "' and U.AP_TYPE_KEY = ";
        sql += unsignedLongToString(m_applicationType);
		*/
   //     sql += " order by MSG_ID";
		
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		columnNames.push_back(ID_COLUMN);
        columnNames.push_back(NAME_COLUMN);
        columnNames.push_back(VALUE_COLUMN);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IDatabase *databaseConnection = DatabaseFactory::getInstance().getDatabase(Application_Cd, Read);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, PID_MESSAGES_SELECT_94001);
        IData* data = databaseConnection->executeQuery(sql,columnNames);

        do // loop for each lot of IData that has been retrieved
        {

            for (unsigned long row = 0; row < data->getNumRows(); ++row)
            {
                // Retrieve the setting name
				PidMessage pidMsg;

				std::string id = data->getStringData(row,ID_COLUMN);
                std::string t_name = data->getStringData(row,NAME_COLUMN);
				
				std::wstring name, value;
				name = Ansi2WChar(t_name.c_str(), t_name.length());
                
				pidMsg.setId(::atoi(id.c_str()));
				pidMsg.setMsgDescription(name);
				// Retrieve the setting value
                std::string t_value = data->getStringData(row,VALUE_COLUMN);
				if(t_value != "")
				{
					value = Ansi2WChar(t_value.c_str(), t_value.length());
					pidMsg.setMsgContent(value);
				}				
				
                // Add the setting to the map.
                //m_settings.insert(PidSettingsData::Settings::value_type(name, value));
				m_pidMsgs.push_back(pidMsg);
                LOGMORE( SourceInfo, "Name: %s Setting: %s",name.c_str(), value.c_str() );
            }

            // Delete ready to retrieve the next lot of data
            if (data != NULL)
            {
                delete data;
                data = NULL;
            }
        }while ( databaseConnection->moreData(data) );

        LOG( SourceInfo, DebugUtil::FunctionExit, "reload");
    }


    void PidSettingsData::updateSettingInDatabase(const int id, const std::wstring& name, const std::wstring& value)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "updateSettingInDatabase");
		char tmp[9];
		sprintf(tmp, "%d", id);
		std::string t_name, t_value;
		t_name = WChar2Ansi(name.c_str());
		t_value = WChar2Ansi(value.c_str());

        IDatabase *databaseConnection = DatabaseFactory::getInstance().getDatabase(Application_Cd, Write);
       /* std::string sql("update PID_MESSAGES set MSG_DESCRIPTION = '");
		sql += databaseConnection->escapeInsertString(t_name);
		sql +="', MSG_CONTENT = '";
        sql += databaseConnection->escapeInsertString(t_value);
        sql += "' where MSG_ID = ";
		sql += tmp;*/

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, PID_MESSAGES_UPDATE_94002, databaseConnection->escapeInsertString(t_name),
			databaseConnection->escapeInsertString(t_value), tmp);

        // Execute the modification. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        databaseConnection->executeModification(sql);

        LOG( SourceInfo, DebugUtil::FunctionExit, "updateSettingInDatabase");
    }

	
    void PidSettingsData::insertSettingIntoDatabase(const int id, const std::wstring& name, const std::wstring& value)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "insertSettingIntoDatabase");
		 
		char tmp[9];
		sprintf(tmp, "%d", id);
		std::string t_name, t_value;
		t_name = WChar2Ansi(name.c_str());
		t_value = WChar2Ansi(value.c_str());
        IDatabase *databaseConnection = DatabaseFactory::getInstance().getDatabase(Application_Cd, Write);
       /* std::string sql("insert into PID_MESSAGES(MSG_ID, MSG_DESCRIPTION,MSG_CONTENT) ");
        sql += "values(";
		sql += tmp;
        
        sql += ",'";
        sql += databaseConnection->escapeInsertString(t_name);
        sql += "','";
        sql += databaseConnection->escapeInsertString(t_value);
        sql += "')";*/

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, PID_MESSAGES_INSERT_94003, tmp, databaseConnection->escapeInsertString(t_name),
			databaseConnection->escapeInsertString(t_value));

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        databaseConnection->executeModification(sql);

        LOG( SourceInfo, DebugUtil::FunctionExit, "insertSettingIntoDatabase");
    }


	std::string PidSettingsData::getLocalDatabaseName()
    {
        if (m_localDatabase.empty())
        {
            // have to find out what is the local db name
            // this should be the first in the connection list so grab it
            if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
            {
                throw DatabaseException("db-connection-file not set");
            }
            try
            {
                DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
                DataConnections connections = connectionStrings.getConnectionList(Application_Cd, Write);
                m_localDatabase = connections[0].first;  
            }
            catch(...) // most likely a file not found error
            {
                throw DatabaseException("Unable to find database connection infomation");
            }

        }
        return m_localDatabase;
    }
	
	PidMessages& PidSettingsData::getPidMessages()
	{
		reload();

		return m_pidMsgs;
	}
} //end namespace TA_Base_Core
