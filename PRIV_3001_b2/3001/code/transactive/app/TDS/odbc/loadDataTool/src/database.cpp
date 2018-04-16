#include "DataBase.h"
#include "SimpleLog.h"

DataBase::DataBase(std::string user, std::string password, std::string server)
    :
    m_user(user),
    m_passwd(password),
    m_server(server),
    m_session(NULL)
{
    m_session = new DB_SESSION();    
    std::string msg = "user: " + m_user + " , server: " + m_server;
    simpleLog__(__FILE__, __LINE__, msg.c_str());
}

DataBase::~DataBase()
{
    if (m_session->hStmt)
        SQLFreeHandle(SQL_HANDLE_STMT, m_session->hStmt);

    if (m_session->hDbc)
    {
        SQLDisconnect(m_session->hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, m_session->hDbc);
    }
    
    if (m_session->hEnv)
        SQLFreeHandle(SQL_HANDLE_ENV, m_session->hEnv);
    
    delete m_session;
}

int DataBase::_DB_Connect()
{
    if (!(m_session->init()))
    {
        simpleLog__(__FILE__, __LINE__, "Error: DB session init failed.");
        return DB_FAILURE;
    }
    
    SQLCHAR *server = (SQLCHAR*)(m_server.c_str());
    SQLCHAR *user = (SQLCHAR*)(m_user.c_str());
    SQLCHAR *passwd = (SQLCHAR*)(m_passwd.c_str());
    if (SQL_SUCCESS != SQLConnect(m_session->hDbc, server, (SQLSMALLINT)lstrlen((char*)server),
                                  user, (SQLSMALLINT)lstrlen((char*)user),
                                  passwd, (SQLSMALLINT)lstrlen((char*)passwd)))
    {
        simpleLog__(__FILE__, __LINE__, "Error: DB connect error.");
        return DB_FAILURE;
    }

    return DB_SUCCESS;
}

int DataBase::executeSql_(std::string sSql)
{
    if (NULL == m_session->hStmt)
    {
        SQLRETURN ret = (SQLAllocHandle(SQL_HANDLE_STMT, m_session->hDbc, &(m_session->hStmt)));
        if (SQL_SUCCESS != ret)
        {
            simpleLog__(__FILE__, __LINE__, "Error: SQL allocate handle for hStmt is failed");
            return DB_FAILURE;
        }
    }

    SQLCHAR *sql = (SQLCHAR*)(sSql.c_str());    
    if (!(SQL_SUCCESS == SQLExecDirect(m_session->hStmt, sql, SQL_NTS)))
    {
        simpleLog__(__FILE__, __LINE__, "Error: SQL execute failed");
        return DB_FAILURE;
    }
    
    return DB_SUCCESS;
}

int DataBase::recordSet_(RecordCollection & recd)
{
    int flag = DB_SUCCESS;
    std::vector<COL_BINDING*> vColumns; // all columns for one sql statement will storen this vector
    SQLSMALLINT column_number = 0;
    SQLNumResultCols(m_session->hStmt, &column_number); // get detailed column numbers
    recd.column = column_number;

    allocatBindings(m_session->hStmt, column_number, vColumns); // allocate memory for column value
    while (true)
    {
        SQLRETURN ret = SQLFetch(m_session->hStmt); // try to get one row data
        if (SQL_SUCCESS == ret) // if found a row data
        {
            recd.rownum++;
            std::vector<std::string> row_record;
            for (unsigned int i = 0; i < vColumns.size(); i++)
            {
                COL_BINDING *binding = vColumns[i];
                
                if (SQL_NULL_DATA != binding->indPtr)
                {
                    row_record.push_back(std::string(binding->buffer));
                }
                else // if the column has no value, we pass a " "
                {
                    row_record.push_back(" ");
                }
            }

            // push a row data to the vector
            recd.Records.push_back(row_record);
        }
        else if (SQL_NO_DATA_FOUND == ret || SQL_NO_DATA == ret) // it means all data has been returned
        {
            simpleLog__(__FILE__, __LINE__, "finished data found.");
            break;
        }
        else if (SQL_ERROR == ret)
        {
            simpleLog__(__FILE__, __LINE__, "sql statement executed failed.");
            flag = DB_FAILURE;
            break;
        }
    }

    // free memory
    for (unsigned int i = 0; i < vColumns.size(); i++)
    {
        free(vColumns[i]->buffer);
        free(vColumns[i]);
    }
    return flag;
}

// allocate memory for column data
void DataBase::allocatBindings(SQLHSTMT hStmt, SQLSMALLINT cols, std::vector<COL_BINDING*>& vColumns)
{
    SQLSMALLINT iCol;
    COL_BINDING *pThisBinding;
    SQLLEN ssType;
    
    for (iCol = 1; iCol <= cols; iCol++)
    {
        pThisBinding = (COL_BINDING *)(malloc(sizeof(COL_BINDING)));
        if (!(pThisBinding))
        {            
            simpleLog__(__FILE__, __LINE__, "allocate memory failed, program will exit...");
            exit(-100);
        }

        vColumns.push_back(pThisBinding);

        SQLColAttribute(hStmt, iCol, SQL_DESC_CONCISE_TYPE, NULL, 0, NULL, &ssType);
        pThisBinding->isChar = (ssType == SQL_CHAR || ssType == SQL_VARCHAR || ssType == SQL_LONGVARCHAR);        

        pThisBinding->buffer = (char*)malloc(DB_MAX_FIELD_LEN * sizeof(char));
        if (!(pThisBinding->buffer))
        {
            simpleLog__(__FILE__, __LINE__, "allocate memory failed, program will exit...");
            exit(-100);
        }
        
        if (SQL_SUCCESS != SQLBindCol(hStmt, iCol, SQL_C_CHAR, (SQLPOINTER)pThisBinding->buffer, DB_MAX_FIELD_LEN * sizeof(char), &pThisBinding->indPtr))
        {
            simpleLog__(__FILE__, __LINE__, "allocate memory for column failed, program will exit...");
            exit(-100);
        }
    }
}

_db_session::_db_session()
    :
    hDbc(NULL),
    hEnv(NULL),
    hStmt(NULL)
{
    
}

bool _db_session::init()
{
    bool isSuccessful = false;
    isSuccessful = ((SQL_SUCCESS == SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv))
                    && (SQL_SUCCESS == SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER))
                    && (SQL_SUCCESS == SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc)));

    return isSuccessful;
}

void RecordsResult::reset()
{
    column = 0;
    rownum = 0;
    Records.clear();
}
