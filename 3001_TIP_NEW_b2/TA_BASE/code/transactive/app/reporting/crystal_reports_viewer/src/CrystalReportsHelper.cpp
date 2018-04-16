/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/CrystalReportsHelper.cpp $
  * @author:  B. Carbonaro
  * @version: $Revision: #22 $
  *
  * Last modification: $DateTime: 2017/12/08 17:42:23 $
  * Last modified by:  $Author: Ouyang $
  * 
  * This class is a wrapper for the Crystal Reports Report
  * Designer Components and as such it simplifies their use.
  */

#include "stdafx.h"
#include "time.h"
#include "direct.h"
#include "CrystalReportsHelper.hpp"
#include "EventLogger.hpp"
#include "resource.h"
#include "crutilDlg.h"
#include "globals.h"
#include "strings.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include "CommandLineParser.hpp"
#include "ace/OS.h"
#include "ace/SString.h"
#include "ace/Tokenizer_T.h"
#include "oleauto.h"

#include "crutil.h"
#ifdef TRANSACTIVE_BUILD


#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/exceptions/src/DatabaseException.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/types/ReportsAudit_MessageTypes.h"
#include "core/exceptions/src/ReportManagerException.h"
#include "core/database/src/SimpleDb.h"
#include "app\reporting\crystal_reports_viewer\src\DatabaseConnectionDlg.h"

const int DATABASENAME_INDEX = 1;
const int USERID_INDEX = 2;
const int PASSWORD_INDEX = 3;
using namespace TA_Base_Core;
namespace TA_Base_App
{
    namespace TA_Reporting
    {

#endif // TRANSACTIVE_BUILD

        //const long REPORT_MANAGER_ENTITY = 17;

		const char* REPORT_TEMPLATE_EXTENSION = "rpt";
		const char* REPORT_FORMAT_PDF = "pdf";
		const char* REPORT_FORMAT_WORD = "word";
		const char* REPORT_FORMAT_EXCEL = "excel";
		const char* REPORT_FORMAT_RTF = "rtf";
		const char* REPORT_FORMAT_HTML = "html";
		const char* REPORT_FORMAT_XML = "xml";
		const char* REPORT_FORMAT_TEXT = "text";

		const char* PDF_EXTENSION = ".pdf";
		const char* WORD_EXTENSION = ".doc";
		const char* EXCEL_EXTENSION = ".xls";
		const char* RTF_EXTENSION = ".rtf";
		const char* HTML_EXTENSION = ".html";
		const char* XML_EXTENSION = ".xml";
		const char* TEXT_EXTENSION = ".txt";
		const char* CRYSTALREPORT_EXTENSION = ".rpt";

		const char* DEFAULT_DLLNAME = "crdb_oracle";
		const char* DEFAULT_SERVERNAME = "tra_dev1";
		const char* DEFAULT_DATABASENAME = "tra_dev1";
		const char* DEFAULT_USERID = "its";
		const char* DEFAULT_PASSWORD = "its";

		const char* HISTORY_PATH_EXTENSION = "history";

		const char* tblMonths[] = { "January", 
							        "February", 
							        "March", 
							        "April", 
							        "May", 
							        "June", 
							        "July", 
							        "August", 
							        "September", 
							        "October", 
							        "November", 
							        "December" };


		CrystalReportsHelper::CrystalReportsHelper()
		: m_pIApplication( NULL ), m_pIReport( NULL ), m_pIReportBackup( NULL ), m_ccbstrReportSource( "" ),
		  m_pReportViewer( NULL ), m_bDisplayPrompt( false ), m_bAddTimeStamp( false ),
		  m_ccbstrPrinterDriver( "" ), m_ccbstrPrinterName( "" ), m_ccbstrPrinterPort( "" ),
		  m_cstrFormat( "" ), m_ccbstrOutputFilename( "" ), m_ccbstrOutputPath( "" ),
		  m_ccbstrDllName( DEFAULT_DLLNAME ), m_ccbstrServerName( DEFAULT_SERVERNAME ), 
		  m_vtDatabaseName( DEFAULT_DATABASENAME ), m_vtUserID( DEFAULT_USERID ), 
		  m_vtPassword( DEFAULT_PASSWORD ), m_pIDatabase( NULL ), m_pITables( NULL ),
		  m_pEventLogger( NULL ), m_crhViewingType( crhOnDemand ), m_pHistoryReportViewer(NULL ),
		  m_cx( 0 ), m_cy( 0 ), m_bEnableParameterPrompting( true ), m_pCCrutilDlg( NULL ),
		  m_cstrUseLogon( YES_STRING ), m_cstrSessionId( "" ), m_cstrOperatorName( "" )
		  // ,m_pReportViewerBackup( NULL )

		{
			m_pEventLogger = new EventLogger( APPLICATION_NAME, EXECUTABLE_NAME );
			m_pAppSettings = new AppSettings( COMPANY_NAME, APPLICATION_NAME );

			initialise();
        }


        CrystalReportsHelper::~CrystalReportsHelper()
		{
			// Clean up.
			if (m_pIReport)
			{
				m_pIReport->Release();
				m_pIReport = NULL;
			}

			if (m_pIApplication) 
			{
				m_pIApplication->Release();
				m_pIApplication = NULL;
			}

			if (m_pEventLogger)
			{
				delete m_pEventLogger;
				m_pEventLogger = NULL;
			}

			if (m_pAppSettings)
			{
				delete m_pAppSettings;
				m_pAppSettings = NULL;
			}
		}


        bool CrystalReportsHelper::initialise()
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Begin CrystalReportsHelper::initialise");

			bool bResult = false;

            // The following code is necessary so because this Crystal Reports Viewer
            // component does not currently support a close report method. Consequently,
            // if you don't release the current instance, every time you open a new 
            // report, it consumes more memory. Hence, by placing the instance release 
            // code here, every call to this method ensures that before creating 
            // an instance, if one already exists, then release it first which releases
            // the memory that was used when the last report was opened.
			if (m_pIReport)
			{
				m_pIReport->Release();
				m_pIReport = NULL;
			}

			if (m_pIApplication) 
			{
				m_pIApplication->Release();
				m_pIApplication = NULL;
			}
           
            // Crystal Reports Viewer Initialisation.
			if (m_pIApplication == NULL)
			{
				HRESULT hr = CoCreateInstance(CLSID_Application, NULL, CLSCTX_ALL, IID_IApplication, (void **) &m_pIApplication);
				if (SUCCEEDED(hr))
				{}
				else
				{
                    TA_Base_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_140032);
				}
			}
			if ((m_pIApplication) && (m_pIReport == NULL))
			{
				HRESULT hr = CoCreateInstance(CLSID_Report, NULL, CLSCTX_ALL, IID_IReport, (void **) &m_pIReport);
				if (SUCCEEDED(hr))
				{
					m_pIReportBackup = m_pIReport;
					// Success.
					setupDefaults();
					bResult = true;
				}
				else
				{
                    TA_Base_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_140032);
				}
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End CrystalReportsHelper::initialise");

			return( bResult );
		}


        void CrystalReportsHelper::setupDefaults()
		{
			char* pszValue;

			// Use Logon.
			pszValue = m_pAppSettings->getStringValue( REG_USELOGON_LABEL );
			if (pszValue)
			{
				CString cstrTemp = pszValue;
				if (cstrTemp.CompareNoCase( YES_STRING ) == 0)
				{
					m_cstrUseLogon = YES_STRING;
				}
				else
				{
					m_cstrUseLogon = NO_STRING;
				}
			}
			else
			{
				m_cstrUseLogon = YES_STRING;
			}

			// Server Name
			pszValue = m_pAppSettings->getStringValue( REG_SERVERNAME_LABEL );
			if (pszValue)
			{
				m_ccbstrServerName = pszValue;
			}
			else
			{
				m_ccbstrServerName = "";
			}

			// Database Name
			pszValue = m_pAppSettings->getStringValue( REG_DATABASENAME_LABEL );
			if (pszValue)
			{
				m_vtDatabaseName = pszValue;
			}
			else
			{
				m_vtDatabaseName = "";
			}

			// UserID
			pszValue = m_pAppSettings->getStringValue( REG_USERID_LABEL );
			if (pszValue)
			{
				m_vtUserID = pszValue;
			}
			else
			{
				m_vtUserID = "";
			}

			// Password
			pszValue = m_pAppSettings->getStringValue( REG_PASSWORD_LABEL );
			if (pszValue)
			{
				m_vtPassword = pszValue;
			}
			else
			{
				m_vtPassword = "";
			}
		}


		bool CrystalReportsHelper::isReady()
		{
			if ((m_pIApplication ) && (m_pIReport))
			{
				return( true );
			}
			else
			{
				return( false );
			}
		}


        void CrystalReportsHelper::setReportName( char* p_szReportName )
		{
			//if the report path is given in a string with '/', change to '\\'
			std::replace(p_szReportName,p_szReportName+strlen(p_szReportName),'/','\\');
			m_ccbstrReportSource =  p_szReportName;
		}


        void CrystalReportsHelper::setViewerInstance( CCrystalReportViewer9* p_pReportViewer )
		{
			m_pReportViewer = p_pReportViewer;
		}


        void CrystalReportsHelper::setBrowserInstance( CWebBrowser2* p_pHistoryReportViewer )
		{
			m_pHistoryReportViewer = p_pHistoryReportViewer;
		}


		void  CrystalReportsHelper::setDialogInstance( CCrutilDlg* p_pCCrutilDlg )
		{
			m_pCCrutilDlg = p_pCCrutilDlg;
		}


        void CrystalReportsHelper::setFormat( char* p_szFormat )
		{
			m_cstrFormat = p_szFormat;
		}


        void CrystalReportsHelper::setOutputFilename( char* p_szOutFile )
		{
			m_ccbstrOutputFilename = p_szOutFile;
		}


        void CrystalReportsHelper::setOutputPath( char* p_szOutPath )
		{
			m_ccbstrOutputPath = p_szOutPath;
		}


        void CrystalReportsHelper::enablePrompt( bool p_bPrompt )
		{
			m_bDisplayPrompt = p_bPrompt;
		}


        void CrystalReportsHelper::enableTimeStamping( bool p_bTimeStamping )
		{
			m_bAddTimeStamp = p_bTimeStamping;
		}


        void CrystalReportsHelper::enableParameterPrompting( bool p_bEnableParameterPrompting)
		{
			m_bEnableParameterPrompting = static_cast<VARIANT_BOOL>(p_bEnableParameterPrompting);
		}


        void CrystalReportsHelper::setDLLName( char* p_szDLLName )
		{
			m_ccbstrDllName = p_szDLLName;
			m_pAppSettings->setStringValue( REG_DLLNAME_LABEL, p_szDLLName );
		}


        void CrystalReportsHelper::setServerName( char* p_szServerName )
		{
			m_ccbstrServerName = p_szServerName;
			m_pAppSettings->setStringValue( REG_SERVERNAME_LABEL, p_szServerName );
		}


        void CrystalReportsHelper::setDatabaseName( char* p_szDatabaseName )
		{
			m_vtDatabaseName = p_szDatabaseName;
			m_pAppSettings->setStringValue( REG_DATABASENAME_LABEL, p_szDatabaseName );
		}


        void CrystalReportsHelper::setUserID( char* p_szUserID )
		{
			m_vtUserID = p_szUserID;
			m_pAppSettings->setStringValue( REG_USERID_LABEL, p_szUserID );
		}


        void CrystalReportsHelper::setPassword( char* p_szPassword )
		{
			m_vtPassword = p_szPassword;
			m_pAppSettings->setStringValue( REG_PASSWORD_LABEL, p_szPassword );
		}


        void CrystalReportsHelper::setViewingType( crhViewingType p_crhViewingType )
		{
			m_crhViewingType = p_crhViewingType;
		}

		
        void CrystalReportsHelper::setSessionID( char* p_szSessionID )
        {
            m_cstrSessionId = p_szSessionID;
        }

		
        void CrystalReportsHelper::setOperatorName( char* p_szOperatorName )
        {
            m_cstrOperatorName = p_szOperatorName;
        }


        CrystalReportsHelper::crhViewingType CrystalReportsHelper::getViewingType( void )
		{
			return( m_crhViewingType );
		}


        void CrystalReportsHelper::setWindowWidth( int p_cx )
		{
			m_cx = p_cx;
		}

		int CrystalReportsHelper::getWindowWidth()
		{
			return m_cx;
		}

        void CrystalReportsHelper::setWindowHeight( int p_cy )
		{
			m_cy = p_cy;
		}

		int CrystalReportsHelper::getWindowHeight()
		{
			return m_cy;
		}

		bool CrystalReportsHelper::openReport( bool isATS)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Begin CrystalReportsHelper::openReport");

			bool bResult = false;

			// If report template doesn't exist, exit immediately.
			if (reportTemplateExists() == false ) 
			{
				OPEN_REPORT_FILE_RATE = OPEN_REPORT_RATE_MILESTONE_TWO;
				return( bResult );
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, 1...");

			// Continue with the opening the report template.
			HRESULT hr = m_pIApplication->OpenReport( m_ccbstrReportSource, m_vtValue, &m_pIReport );
			if (SUCCEEDED(hr))
			{
				m_pIReportBackup = m_pIReport;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, 2...");

				// The report has been opened successfully.
				bResult = true;

				// Save for future use.
				m_ccbstrReportTemplate = m_ccbstrReportSource;

				// Now lets setup the database logon information.
				if (m_cstrUseLogon.CompareNoCase( YES_STRING ) == 0)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, 3...");

					hr = m_pIReport->get_Database( &m_pIDatabase );
					if (SUCCEEDED(hr))
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, 4...");
						USES_CONVERSION;

						hr = m_pIDatabase->get_Tables( &m_pITables );
						if (SUCCEEDED(hr))
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, 5...");

							IDatabaseTable* pITable;
							long lIndex;
							hr = m_pITables->get_Count( &lIndex );
							if (SUCCEEDED(hr))
							{
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, 6...");
								DatabaseConnectionDlg dbconDlg;

								std::string userName;	
								std::string passwordStr;
								if(isATS)
								{
									int nResponse = dbconDlg.DoModal();
									if(nResponse == IDOK)
									{
										LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::database dialog successfully");

										userName = dbconDlg.m_userName.GetBuffer(0);	
										passwordStr = dbconDlg.m_password.GetBuffer(0);

									}
									else
									{
										LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::database password dialog is canceled");
									}
								}

								long i;
								for (i = 1; i <= lIndex; i++)
								{
									hr = m_pITables->get_Item( i, &pITable );
									if (SUCCEEDED(hr))
									{
										LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, 7...");
										CComBSTR dllBstr;
										hr = pITable->get_DllName( &dllBstr );
										std::string dllName = W2CA(dllBstr);
										LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::database library used is: %s",dllName.c_str());
										CComBSTR logDatabaseNameBstr;
										hr = pITable->get_LogOnDatabaseName( &logDatabaseNameBstr );
										std::string logDatabaseName = W2CA(logDatabaseNameBstr);
										LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::database source used is: %s",logDatabaseName.c_str());
										CComBSTR logServerNameBstr;
										hr = pITable->get_LogOnServerName( &logServerNameBstr );
										std::string logServerName = W2CA(logServerNameBstr);
										LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::database server used is: %s",logServerName.c_str());
										
										if(logDatabaseName.empty())
											logDatabaseName = logServerName;
										
										////////////for log
										CComBSTR rightLog;
										hr = pITable->get_Name( &rightLog );
										std::string logDetails = W2CA(rightLog);
										LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::get_Name is: %s",logDetails.c_str());

										hr = pITable->get_Location( &rightLog );
										logDetails = W2CA(rightLog);
										LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::get_Location: %s",logDetails.c_str());

										hr = pITable->get_DllName( &rightLog );
										logDetails = W2CA(rightLog);
										LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::get_DllName is: %s",logDetails.c_str());

										hr = pITable->get_DecriptiveName( &rightLog );
										logDetails = W2CA(rightLog);
										LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::get_DecriptiveName is: %s",logDetails.c_str());

										hr = pITable->get_LogOnServerName( &rightLog );
										logDetails = W2CA(rightLog);
										LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::get_LogOnServerName is: %s",logDetails.c_str());

										hr = pITable->get_LogOnDatabaseName( &rightLog );
										logDetails = W2CA(rightLog);
										LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::get_LogOnDatabaseName is: %s",logDetails.c_str());

										hr = pITable->get_LogOnUserID( &rightLog );
										logDetails = W2CA(rightLog);
										LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::get_LogOnUserID is: %s",logDetails.c_str());

										hr = pITable->get_SessionUserID( &rightLog );
										logDetails = W2CA(rightLog);
										LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::get_SessionUserID is: %s",logDetails.c_str());

										hr = pITable->get_SubLocation( &rightLog );
										logDetails = W2CA(rightLog);
										LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::get_SubLocation is: %s",logDetails.c_str());

										hr = pITable->get_ConnectBufferString( &rightLog );
										logDetails = W2CA(rightLog);
										LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::get_ConnectBufferString is: %s",logDetails.c_str());
										///////////log end
										CComBSTR logUserNameBstr;
										hr = pITable->get_Name( &logUserNameBstr );
										std::string logUserName = W2CA(logUserNameBstr);
										LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::database user used is: %s",logUserName.c_str());
										if(isATS)
										{
											CComBSTR serverBstr;
											hr = pITable->get_LogOnServerName( &serverBstr );
											std::string serverName = W2CA(serverBstr);
											setServerName( const_cast<char*>(serverName.c_str()) );

												/*
												CComBSTR databaseBstr;
																								hr = pITable->get_LogOnDatabaseName( &databaseBstr );
																								std::string databaseName = W2CA(databaseBstr);*/
												
												
												//setDatabaseName( const_cast<char*>(databaseName.c_str()) );

												//std::string databaseStr = dbconDlg.m_userName.GetBuffer(0);	
												/*
												setServerName( const_cast<char*>(databaseName.c_str()) );
																								setDatabaseName( const_cast<char*>(databaseName.c_str()) );*/
												
												//setDatabaseName( const_cast<char*>(serverName.c_str()) );
												setDatabaseName( const_cast<char*>(logDatabaseName.c_str()) );
												
												setUserID( const_cast<char*>(userName.c_str()) );

												setPassword( const_cast<char*>(passwordStr.c_str()) );
												LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::Login Details: %s, %s,%s",serverName.c_str(),userName.c_str(),passwordStr.c_str());
												hr = pITable->SetLogOnInfo( m_ccbstrServerName, 
													m_vtDatabaseName,
													m_vtUserID,
													m_vtPassword );
												if (SUCCEEDED(hr))
												{
													LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, 10...");

													CComBSTR ccbstrTableName;
													hr = pITable->get_Name( &ccbstrTableName );
													if (SUCCEEDED(hr))
													{
														hr = pITable->put_Location( ccbstrTableName );
														if (SUCCEEDED(hr))
														{}

														INameValuePairs* pNameValuepair;
														hr = pITable->get_ConnectionProperties(&pNameValuepair);
														
														/*
														_bstr_t serverNameBStrt = "Provider";
														BSTR serverNameBStr = serverNameBStrt.copy();
														//1  _bstr_t serverNameValStr = "VFPOLEDB.1";
														//_bstr_t serverNameValStr = "OraOLEDB.Oracle.1";
														_bstr_t serverNameValStr = "crdb_ado.Oracle";
														VARIANT serverNameVal ;
														serverNameVal.vt = VT_BSTR;
														serverNameVal.bstrVal = serverNameValStr.copy();
														pNameValuepair->Add(serverNameBStr,serverNameVal);*/
														
														
														_bstr_t integratedBStrt = "Integrated Security";
														BSTR integratedBSTR = integratedBStrt.copy();
														_bstr_t integratedValStr = "SSPI";
														VARIANT integratedVal ;
														integratedVal.vt = VT_BSTR;
														integratedVal.bstrVal = integratedValStr.copy();
														pNameValuepair->Add(integratedBSTR,integratedVal);

														_bstr_t initialCatalogBStrt = "Initial Catalog";
														BSTR initialCatalogBStr = initialCatalogBStrt.copy();
														_bstr_t initialCatalogValStr = logServerName.c_str();
														VARIANT initialCatalogVal ;
														initialCatalogVal.vt = VT_BSTR;
														initialCatalogVal.bstrVal = initialCatalogValStr.copy();
														pNameValuepair->Add(initialCatalogBStr,initialCatalogVal);
														LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CrystalReportLoginInfo:Initial Catalog=>%s",logServerName.c_str());

														_bstr_t datasourceBStrt = "Data source";
														BSTR datasourceBStr = datasourceBStrt.copy();
														//_bstr_t datasourceValStr = logServerName.c_str();
														_bstr_t datasourceValStr = logDatabaseName.c_str();
														VARIANT datasourceVal ;
														datasourceVal.vt = VT_BSTR;
														datasourceVal.bstrVal = datasourceValStr.copy();
														pNameValuepair->Add(datasourceBStr,datasourceVal);
														LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CrystalReportLoginInfo:Data source=>%s",logServerName.c_str());

														_bstr_t dnsBStrt = "DSN";
														BSTR dnsBStr= dnsBStrt.copy();
														//_bstr_t dnsValStr = logServerName.c_str();
														_bstr_t dnsValStr = logDatabaseName.c_str();
														VARIANT dnsVal ;
														dnsVal.vt = VT_BSTR;
														dnsVal.bstrVal = dnsValStr.copy();
														pNameValuepair->Add(dnsBStr,dnsVal);
														LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CrystalReportLoginInfo:DSN=>%s",logServerName.c_str());

														_bstr_t DatabaseBStrt = "Database";
														BSTR DatabaseBStr= DatabaseBStrt.copy();
														//_bstr_t DatabaseValStr = logServerName.c_str();
														_bstr_t DatabaseValStr = logDatabaseName.c_str();
														VARIANT DatabaseVal ;
														DatabaseVal.vt = VT_BSTR;
														DatabaseVal.bstrVal = DatabaseValStr.copy();
														pNameValuepair->Add(DatabaseBStr,DatabaseVal);
														LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CrystalReportLoginInfo:Database=>%s",logServerName.c_str());

														_bstr_t userNameBStrt = "User ID";
														BSTR userNameBSTR = userNameBStrt.copy();
														_bstr_t userNameValStr = userName.c_str();
														VARIANT userNameVal ;
														userNameVal.vt = VT_BSTR;
														userNameVal.bstrVal = userNameValStr.copy();
														pNameValuepair->Add(userNameBSTR,userNameVal);
														LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CrystalReportLoginInfo:User ID=>%s",userName.c_str());
														
														_bstr_t passwordBStrt = "Password";
														BSTR passwordBStr = passwordBStrt.copy();
														_bstr_t passwordValStr =  passwordStr.c_str();
														VARIANT upasswordVal ;
														upasswordVal.vt = VT_BSTR;
														upasswordVal.bstrVal = passwordValStr.copy();
														pNameValuepair->Add(passwordBStr,upasswordVal);
														LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CrystalReportLoginInfo:Password=>%s",passwordStr.c_str());



														pNameValuepair->Release();

													}

												}
											
										}
										else
										{
											std::string dataBaseName = "";
											DataConnections connections ;
											try
											{
												//DbConnectionStrings connectionStrings(TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));

												// jiangshengguang++ 08-11-06 IncidentReport print button crash
												//std::string filename = "C:\\transActive\\config\\database\\OCCConnectionStrings.CSV";
												/*std::string filename =TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);
												if (filename.empty())
												{
												return false;
												}*/

												//libo++
												std::string filename=""; 
#ifdef TRANSACTIVE_BUILD
												TA_Reporting::CommandLineParser objArgv;
												//[[xinyang++ CL-20758			
												if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE))
												{
													std::string value = TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);
													filename = value;
												}
												else
												{
													//set parameters from start.ini file from the computer with the report
													CommandLineParser objArgv;
													std::string runParameterFile="--run-param-file=";
													if (objArgv.parameterExists( REPORTNAME_LABEL)) 
													{
														std::vector<std::string> values = objArgv.getParameterValue(REPORTNAME_LABEL);
														filename = values[0];

														//if the report path is given in a string with '/', change to '\\'
														std::replace(filename.begin(),filename.end(),'/','\\');
														char szTemp[1000];
														char *token;
														char seps[] = "\\";
														strcpy( szTemp, filename.c_str());
														stripOffExtension( szTemp );


														std::string::size_type fileNamePos = filename.find('$');
														std::string remotePath = "";
														//if remote computer, the file path is like this:\\mft-occ-01\c$\transactive\start.ini
														if(fileNamePos != std::string::npos)
														{
															token = strtok( szTemp, seps );
															runParameterFile +="\\\\" ;
															runParameterFile += token;

															remotePath +="\\\\" ;
															remotePath += token;
															remotePath +="\\";

															runParameterFile += "\\";
															token = strtok( NULL, seps );
															runParameterFile +=token;
															runParameterFile += "\\transActive\\start.ini";
															TA_Base_Core::RunParams::getInstance().parseCmdLine(runParameterFile.c_str());
															std::string value = TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);
															fileNamePos = value.find(':');
															//the configuration fine name is like this:\\mft-occ-01\c$\transactive\config\database\DT18ConnectionString.csv
															if(fileNamePos != std::string::npos)
															{
																filename = remotePath + value.replace(fileNamePos,1,1,'$');
															}
														}
														// if it is the local computer, the file path is like this: c:\transactive\start.ini
														else
														{
															token = strtok( szTemp, seps );
															runParameterFile +=token;
															runParameterFile += "\\transActive\\start.ini";
															TA_Base_Core::RunParams::getInstance().parseCmdLine(runParameterFile.c_str());
															std::string value = TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);
															filename = value;
														}

													}
												}
												//]]xinyang++ CL-20758
#else
												CommandLineParser objArgv;

												if (objArgv.parameterExists( RPARAM_DBCONNECTIONFILE)) 
												{
													std::vector<std::string> value = objArgv.getParameterValue(RPARAM_DBCONNECTIONFILE);
													filename = value[0].c_str();
												}

#endif // TRANSACTIVE_BUILD

												if(filename.empty())
												{
													//[[xinyang++ CL-20758
													TA_Base_Core::DatabaseException("Unable find Local DbConnectionfile!");
													//]]xinyang++ CL-20758
													return false;
												}
												//++libo

												DbConnectionStrings connectionStrings(filename);
												// jiangshengguang++ 08-11-06 IncidentReport print button crash
												connections = connectionStrings.getConnectionList(TA_Base_Core::Archiving_Cd, TA_Base_Core::Write);											
												dataBaseName = connections[0].first;
											}
											catch (...) 
											{
												throw TA_Base_Core::DatabaseException("Unable to find database connection infomation");
											}

											LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, 8...");

											//std::string connectionString = connections[0].second;
											//TD 13191
											std::string connectionString ;
											if(connections[0].second.length() != 0)
												connectionString = connections[0].second;
											else
												connectionString= TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTION);

											std::vector<std::string> dbConnData;
											SimpleDb::ParseConnectStr(connectionString, dbConnData);										
											if (dbConnData.size() >= 5)
											{
												LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, 9...");

												dbConnData[DATABASENAME_INDEX]=dataBaseName;											
												m_vtDatabaseName = const_cast<char*>(dbConnData[DATABASENAME_INDEX].c_str());
												m_vtUserID = const_cast<char*>(dbConnData[USERID_INDEX].c_str());
												m_vtPassword = const_cast<char*>(dbConnData[PASSWORD_INDEX].c_str());										
												m_ccbstrServerName = const_cast<char*>(dbConnData[DATABASENAME_INDEX].c_str());

												//TD 13180
												setDatabaseName( const_cast<char*>(dbConnData[DATABASENAME_INDEX].c_str()) );
												setUserID( const_cast<char*>(dbConnData[USERID_INDEX].c_str()) );
												setPassword( const_cast<char*>(dbConnData[PASSWORD_INDEX].c_str()) );
												setServerName( const_cast<char*>(dbConnData[DATABASENAME_INDEX].c_str()) );
												//TD 13180
											}
											//TD 13191
											hr = pITable->SetLogOnInfo( m_ccbstrServerName, 
												m_vtDatabaseName,
												m_vtUserID,
												m_vtPassword );
											if (SUCCEEDED(hr))
											{
												LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, 10...");

												CComBSTR ccbstrTableName;
												hr = pITable->get_Name( &ccbstrTableName );
												if (SUCCEEDED(hr))
												{
													hr = pITable->put_Location( ccbstrTableName );
													if (SUCCEEDED(hr))
													{}
												}
											}
										}
										pITable->Release();
									}
								}
							}
							m_pITables->Release();
						}
						m_pIDatabase->Release();
					}
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, 11...");

				}
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, <1>...");

				initialise();
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End CrystalReportsHelper::openReport");

			OPEN_REPORT_FILE_RATE = OPEN_REPORT_RATE_MILESTONE_TWO;
			return( bResult );
		}


        bool CrystalReportsHelper::viewReport(std::vector<std::string>& parameterValuePairs , bool isATS)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Begin CrystalReportsHelper::viewReport");

			bool bResult = false;

			//if ( (m_pReportViewer == NULL) && (m_pReportViewerBackup != m_pReportViewer) )
			//{
			//	setViewerInstance(m_pReportViewerBackup);
			//}

			// Initially set the Window's caption to just the application title.
			if (m_pCCrutilDlg) m_pCCrutilDlg->SetWindowText( CAPTION_TITLE );

			// If viewing type is On Demand then use the RDC component
			// else use the Web Browser control to view an historical report.
			if (m_crhViewingType == crhOnDemand)
			{
				// Ensure that we have gain access to the crystal reports COM objects.
				if ((m_pIApplication ) && (m_pIReport))
				{
					// Ensure that there is a valid pointer to an instance of the 
					// ActiveX Report Viewer control.
					if (m_pReportViewer)
					{
						// Ensure that we have a report template to display.
						CString cstrTemp = m_ccbstrReportSource;
						if (cstrTemp != "")
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::viewReport, 1...");

							OPEN_REPORT_FILE_RATE = OPEN_REPORT_RATE_MILESTONE_ONE;
							// Open the report template.
							if (openReport( isATS) == true )
							{
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::viewReport, 2...");

								m_pHistoryReportViewer->MoveWindow(0, 0, 0, 0);
								m_pReportViewer->MoveWindow(0, 0, m_cx, m_cy);

                                setupParameters(parameterValuePairs);
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::viewReport, 2.1...");

								m_pIReport->put_EnableParameterPrompting( m_bEnableParameterPrompting );
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::viewReport, 2.2...");

								m_pReportViewer->SetReportSource( m_pIReport );
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::viewReport, 2.3...");

								OPEN_REPORT_FILE_RATE = OPEN_REPORT_RATE_MILESTONE_THREE;

								m_pReportViewer->SetEnablePrintButton( true );
								m_pReportViewer->SetEnableExportButton( true );
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::viewReport, 2.4...");

								setupGroupButton();
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::viewReport, 2.5...");

								m_pReportViewer->ViewReport();

								OPEN_REPORT_FILE_RATE = OPEN_REPORT_RATE_MILESTONE_FOUR;

								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::viewReport, 3...");

								// Log this event.
								CString cstrLogMsg = "Report ";
								cstrLogMsg += getReportName();
                                #ifdef TRANSACTIVE_BUILD
								cstrLogMsg += " was viewed by ";
								cstrLogMsg += m_cstrOperatorName;
                                #else
								cstrLogMsg += " was opened for viewing";
                                #endif // TRANSACTIVE_BUILD
								cstrLogMsg += ".";

                                #ifdef TRANSACTIVE_BUILD
					            sendAuditMessage( cstrLogMsg.GetBuffer(cstrLogMsg.GetLength()) );
                                #else
					            m_pEventLogger->logInformation( cstrLogMsg.GetBuffer(cstrLogMsg.GetLength()) );
                                #endif // TRANSACTIVE_BUILD

								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::viewReport, 4...");

                                // Indicate success.
								bResult = true;
							}
						}
					}
				}
			}
			else
			{
				OPEN_REPORT_FILE_RATE = OPEN_REPORT_RATE_MILESTONE_ONE;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::viewReport, <1>...");

				m_pReportViewer->MoveWindow(0, 0, 0, 0);	
				m_pHistoryReportViewer->MoveWindow(0, 0, m_cx, m_cy);
				if (m_pHistoryReportViewer)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::viewReport, <2>...");

					OPEN_REPORT_FILE_RATE = OPEN_REPORT_RATE_MILESTONE_TWO;
					CComVariant vtFile;
					vtFile = m_ccbstrReportSource;
					m_pHistoryReportViewer->Navigate2( &vtFile, NULL, NULL, NULL, NULL );

					OPEN_REPORT_FILE_RATE = OPEN_REPORT_RATE_MILESTONE_THREE;
					// Log this event.
					CString cstrLogMsg = "Historical Report ";
					cstrLogMsg += getHistoryReportName();
                    #ifdef TRANSACTIVE_BUILD
					cstrLogMsg += " was viewed by ";
					cstrLogMsg += m_cstrOperatorName;
                    #else
					cstrLogMsg += " was opened for viewing";
                    #endif // TRANSACTIVE_BUILD
					cstrLogMsg += ".";

                    #ifdef TRANSACTIVE_BUILD
					sendAuditMessage( cstrLogMsg.GetBuffer(cstrLogMsg.GetLength()) );
                    #else
					m_pEventLogger->logInformation( cstrLogMsg.GetBuffer(cstrLogMsg.GetLength()) );
                    #endif // TRANSACTIVE_BUILD

					OPEN_REPORT_FILE_RATE = OPEN_REPORT_RATE_MILESTONE_FOUR;
					// Indicate success.
					bResult = true;
				}
			}

			// If a report has been successfully displayed, update the caption accordingly.
			if (bResult)
			{
				CString cstrTemp = CAPTION_TITLE;
				cstrTemp += " - ";
				cstrTemp += getReportFileName();
				if (m_pCCrutilDlg) m_pCCrutilDlg->SetWindowText( cstrTemp );
			}
			//OPEN_REPORT_FILE_RATE = 1000;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End CrystalReportsHelper::viewReport");

			return( bResult );
		}

		bool CrystalReportsHelper::closeReport()
		{
			bool bResult = false;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Begin CrystalReportsHelper::closeReport");

			// Ensure that we have gain access to the crystal reports COM objects.
			if ((m_pIApplication ) && (m_pIReport))
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::closeReport, 1...");

				// Ensure that there is a valid pointer to an instance of the 
				// ActiveX Report Viewer control.
				if (m_pReportViewer)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::closeReport, 2...");

					if ( m_pReportViewer->GetIsBusy() == FALSE)
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::closeReport, ActiveX Report Viewer control is not busy, so available");

						bResult = true;

						initialise();
					}
				}
			}

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End CrystalReportsHelper::closeReport");

			return ( bResult );
		}

		void CrystalReportsHelper::setDisplayBackgroundEdge( BOOL displayBackgroundEdge )
		{
			m_pReportViewer->SetDisplayBackgroundEdge( displayBackgroundEdge );
		}

		void CrystalReportsHelper::setDisplayBorder( BOOL displayBorder)
		{
			m_pReportViewer->SetDisplayBorder( displayBorder );
		}

		bool CrystalReportsHelper::setShowToolBar( bool showToolBar )
		{
			bool bResult = false;

			// Ensure that we have gain access to the crystal reports COM objects.
			if ((m_pIApplication ) && (m_pIReport))
			{
				// Ensure that there is a valid pointer to an instance of the 
				// ActiveX Report Viewer control.
				if (m_pReportViewer)
				{
					bResult = true;
					m_pReportViewer->SetEnableToolbar( showToolBar );
					m_pReportViewer->SetEnableCloseButton( showToolBar );
				}
			}

			return ( bResult );
		}

		bool CrystalReportsHelper::setShowStatusBar( bool showStatusBar )
		{
			bool bResult = false;

			// Ensure that we have gain access to the crystal reports COM objects.
			if ((m_pIApplication ) && (m_pIReport))
			{
				// Ensure that there is a valid pointer to an instance of the 
				// ActiveX Report Viewer control.
				if (m_pReportViewer)
				{
					bResult = true;
					m_pReportViewer->SetEnableProgressControl( showStatusBar );
				}
			}

			return ( bResult );
		}

        bool CrystalReportsHelper::printReport(std::string incidentid)
        {
			bool bResult = false;
			
			// Ensure that we have gain access to the crystal reports COM objects.
			if ((m_pIApplication ) && (m_pIReport ))
			{
				// Open the report template.
				if (openReport() == true )
				{
                    // Create an instance of the Command Line Parser object.
                    std::ostringstream resultstream;
                    CommandLineParser objArgv;
                    // Configure the Database connection parameters if the parameter 
                    // has been supplied.
                    //Mintao ++
                    //Bug 643
            
                    if (objArgv.parameterExists( RPARAM_DBCONNECTION )) 
            					{

                                    std::vector<std::string> value = objArgv.getParameterValue( RPARAM_DBCONNECTION );
                                    std::string connectionString = value[0];
                                    std::vector<std::string> dbConnData;
                                    SimpleDb::ParseConnectStr(connectionString, dbConnData);
									CString dbType = dbConnData[0].c_str();
                                    if (dbConnData.size() >=3 && (dbType.CompareNoCase("ORACLE") ==0))
									{
                				        setServerName( const_cast<char*>(dbConnData[DATABASENAME_INDEX].c_str()) );
                				        setDatabaseName( const_cast<char*>(dbConnData[DATABASENAME_INDEX].c_str()) );
                				        setUserID( const_cast<char*>(dbConnData[USERID_INDEX].c_str()) );
            	    			        setPassword( const_cast<char*>(dbConnData[PASSWORD_INDEX].c_str()) );
                                    }
                                }
            
					setupincidentId(incidentid);
					// And print.    
                  
/*
                      long retVal;
                                               m_pIReport->PrinterSetupEx(0, &retVal);
                                               if (retVal == 0)//user choose to print the report
                           					{
                 								m_pIReport->put_EnableParameterPrompting( false );
                                                 m_pIReport->put_DisplayProgressDialog(false);
                           					    VARIANT vtbPrompt;
                           					    vtbPrompt.vt = VT_BOOL;
                           					    vtbPrompt.boolVal = false;
                           					    m_pIReport->PrintOut( vtbPrompt );
                                               }
                 */
                 
                 							  m_pIReport->put_EnableParameterPrompting( m_bEnableParameterPrompting );
                 							  m_pIReport->put_DisplayProgressDialog( false );
                 							  VARIANT vtbPrompt;
                 							  vtbPrompt.vt = VT_BOOL;
                 							  vtbPrompt.boolVal = m_bDisplayPrompt;
                 							  m_pIReport->PrintOut( vtbPrompt );
                 

                    //Mintao ++
                    //Bug 643
					// Indicate success.
					bResult = true;
				}
			}
			return( bResult );

        }

        bool CrystalReportsHelper::setupincidentId(std::string incidentid)
        {
			bool bResult = false;
			if(openReport()==true)
			{
				IParameterFieldDefinitions* pIParameterFieldDefinitions = NULL;
				HRESULT hr = m_pIReport->get_ParameterFields( &pIParameterFieldDefinitions );
				if (SUCCEEDED( hr ))
				{
					IParameterFieldDefinition* pIParameterFieldDefinition = NULL;
					std::string incidentIdLabel="ININCI_ID";
					CComBSTR ccbstrLabel = incidentIdLabel.c_str();

					CComVariant vtSubReport = "";
					hr = pIParameterFieldDefinitions->GetItemByName( ccbstrLabel, vtSubReport, &pIParameterFieldDefinition );
					if (SUCCEEDED( hr ))
					{
						CComVariant vtValue = incidentid.c_str();
						hr = pIParameterFieldDefinition->SetCurrentValue(vtValue);
						

					}
					else
					{
						// Error.
					}
				}
				else
				{
					// Error.
				}
			}

			return( bResult );
        }
        bool CrystalReportsHelper::printReport(std::vector<std::string>& parameterValuePairs)
		{
			bool bResult = false;

			// Ensure that we have gain access to the crystal reports COM objects.
			if ((m_pIApplication ) && (m_pIReport ))
			{
				// Open the report template.
				if (openReport() == true )
				{
					// And print.
                    setupParameters(parameterValuePairs);
					m_pIReport->put_EnableParameterPrompting( m_bEnableParameterPrompting );
					m_pIReport->put_DisplayProgressDialog( false );
					VARIANT vtbPrompt;
					vtbPrompt.vt = VT_BOOL;
					vtbPrompt.boolVal = m_bDisplayPrompt;
					m_pIReport->PrintOut( vtbPrompt );

					// Log this event.
					CString cstrLogMsg = "Report ";
					cstrLogMsg += getReportName();
					cstrLogMsg += " was printed.";

                    #ifdef TRANSACTIVE_BUILD
                    // Disabled to address the following TransActive defect.
                    // Defect #2868 - Report Manager: Running Report via the command line/Windows Scheduler.
                    // The following line has been temporarily commented out due to the 
                    // fact that an exception thrown when called. The exception is thrown
                    // because the app is run as a console application and as such a
                    // TransActive operator login has not been performed. Because a operator
                    // login has not been performed, we don't have a session ID and without this
                    // sending an audit message fails. When a fix is found, then this auditing
                    // can be re-introduced.
					//sendAuditMessage( cstrLogMsg.GetBuffer(cstrLogMsg.GetLength()) );
                    #else
					m_pEventLogger->logInformation( cstrLogMsg.GetBuffer(cstrLogMsg.GetLength()) );
                    #endif // TRANSACTIVE_BUILD

					// Indicate success.
					bResult = true;
				}
			}

			return( bResult );
		}


        bool CrystalReportsHelper::exportReport(std::vector<std::string>& parameterValuePairs)
		{
			bool bResult = false;

			// Ensure that we have gain access to the crystal reports COM objects.
			if ((m_pIApplication ) && (m_pIReport ))
			{
				// Open the report template.
				if (openReport() == true )
				{
					CComBSTR bstrFileName;
					if (m_bDisplayPrompt == false)
					{
						// Setup export options.	
						HRESULT hresult = m_pIReport->get_ExportOptions( &m_pIExportOptions );
						hresult = m_pIExportOptions->put_DestinationType( crEDTDiskFile );
						hresult = m_pIExportOptions->put_FormatType( getValueForFormat() );
						std::string exportFileName = getExportFileName();	
						//if filename contains '$', then the file is considered to save to remote mft
						std::string::size_type exportFileNamePos = exportFileName.find("$");
						if(exportFileNamePos != std::string::npos)
						{
							//there are 2 cases that filename would contains '$', 
							//case 1: --out-path is given while the file is a local file. In this case, it is unnecessary to add "\\\\" in front of file name
							//case 2: --out-path is not given and the file is a remote file. In this case,"\\\\" should be insert in front of the file name
							std::string::size_type subscribePos=exportFileName.find("\\\\");
							if(subscribePos != std::string::npos)
							{

							}
							else
								exportFileName.insert(0,"\\\\");
						}
						bstrFileName = exportFileName.c_str();
						//bstrFileName = getExportFileName();	
						hresult = m_pIExportOptions->put_DiskFileName( bstrFileName );
					}

					// And export.
                    setupParameters(parameterValuePairs);
					m_pIReport->put_EnableParameterPrompting( m_bEnableParameterPrompting );
					_mkdir( getHistoryReportPath() );

					m_pIReport->put_DisplayProgressDialog( false );
				/*
					if (m_cstrFormat.CompareNoCase( REPORT_TEMPLATE_EXTENSION ) == 0)
									{
										CString reportSource (m_ccbstrReportSource.m_str);
										CString reportDestination(bstrFileName.m_str);
										std::string deleteFolder = "cmd.exe /c copy \""+(std::string)(reportSource.GetBuffer(0)) + "\" \"" + (std::string)(reportDestination.GetBuffer(0)) + "\"";
										char *deleteName = (char*)deleteFolder.c_str();
										PROCESS_INFORMATION ProcessInfo2;
										STARTUPINFOA StartupInfo2; //This is an [in] parameter
										ZeroMemory(&StartupInfo2, sizeof(StartupInfo2));
										StartupInfo2.wShowWindow=SW_HIDE;
										if(CreateProcessA(NULL,deleteName ,NULL,NULL,false,CREATE_NO_WINDOW ,NULL,NULL,&StartupInfo2,&ProcessInfo2))
										{
											WaitForSingleObject(ProcessInfo2.hProcess,INFINITE);
											CloseHandle(ProcessInfo2.hThread);
											CloseHandle(ProcessInfo2.hProcess);
										}
									}
									else
									{*/
				
						VARIANT vtbPrompt;
						vtbPrompt.vt = VT_BOOL;
						vtbPrompt.boolVal = m_bDisplayPrompt;
						m_pIReport->Export( vtbPrompt );
					//}

					// Log this event.
					CString cstrLogMsg = "Report ";
					cstrLogMsg += getReportName();
					cstrLogMsg += " was exported";

                    #ifdef TRANSACTIVE_BUILD
                    // Disabled to address the following TransActive defect.
                    // Defect #2868 - Report Manager: Running Report via the command line/Windows Scheduler.
                    // The following line has been temporarily commented out due to the 
                    // fact that an exception thrown when called. The exception is thrown
                    // because the app is run as a console application and as such a
                    // TransActive operator login has not been performed. Because a operator
                    // login has not been performed, we don't have a session ID and without this
                    // sending an audit message fails. When a fix is found, then this auditing
                    // can be re-introduced.
					//sendAuditMessage( cstrLogMsg.GetBuffer(cstrLogMsg.GetLength()) );
                    #else
					m_pEventLogger->logInformation( cstrLogMsg.GetBuffer(cstrLogMsg.GetLength()) );
                    #endif // TRANSACTIVE_BUILD

					// Indicate success.
					bResult = true;
				}
			}

			return( bResult );
		}


        void CrystalReportsHelper::refreshReport()
		{
			//if ( (m_pReportViewer == NULL) && (m_pReportViewerBackup != m_pReportViewer) )
			//{
			//	setViewerInstance(m_pReportViewerBackup);
			//}

			if (m_crhViewingType == crhOnDemand)
			{
				m_pReportViewer->RefreshEx(true);//cl-18123--lkm
			//	m_pReportViewer->Refresh();
			}
			else
			{
				m_pHistoryReportViewer->Refresh();
			}
		}


        bool CrystalReportsHelper::stripOffExtension( char* p_szFullReportName )
		{
			bool bResult = false;

			CString cstrReportName = p_szFullReportName;
			CString cstrTemp = cstrReportName.Right(3);
			if (cstrTemp == REPORT_TEMPLATE_EXTENSION)
			{
				cstrTemp = cstrReportName.Left(cstrReportName.GetLength() - sizeof(REPORT_TEMPLATE_EXTENSION));
				int iBufferLength = strlen( p_szFullReportName );
				strcpy( p_szFullReportName, cstrTemp.GetBuffer( iBufferLength ) );
				bResult = true;
			}

			return( bResult );
		}


        enum CRExportFormatType CrystalReportsHelper::getValueForFormat()
		{
			enum CRExportFormatType iValue = crEFTNoFormat;

			if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_PDF ) == 0)
			{
				iValue = crEFTPortableDocFormat;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_TEMPLATE_EXTENSION ) == 0)
			{
				iValue = crEFTCrystalReport;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_WORD ) == 0)
			{
				iValue = crEFTWordForWindows;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_EXCEL ) == 0)
			{
				iValue = crEFTExcel97;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_RTF ) == 0)
			{
				iValue = crEFTRichText;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_HTML ) == 0)
			{
				iValue = crEFTHTML32Standard;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_XML ) == 0)
			{
				iValue = crEFTXML;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_TEXT ) == 0)
			{
				iValue = crEFTText;
			}
				
			return( iValue );
		}


        char* CrystalReportsHelper::getExtensionForFormat()
		{
			if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_PDF ) == 0)
			{
				m_cstrTemp = PDF_EXTENSION;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_TEMPLATE_EXTENSION ) == 0)
			{
				m_cstrTemp = CRYSTALREPORT_EXTENSION;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_WORD ) == 0)
			{
				m_cstrTemp = WORD_EXTENSION;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_EXCEL ) == 0)
			{
				m_cstrTemp = EXCEL_EXTENSION;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_RTF ) == 0)
			{
				m_cstrTemp = RTF_EXTENSION;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_HTML ) == 0)
			{
				m_cstrTemp = HTML_EXTENSION;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_XML ) == 0)
			{
				m_cstrTemp = XML_EXTENSION;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_TEXT ) == 0)
			{
				m_cstrTemp = TEXT_EXTENSION;
			}
			else
			{
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_140030);
            }
				
			return( m_cstrTemp.GetBuffer(m_cstrTemp.GetLength()) );
		}

		char* CrystalReportsHelper::getExportFileName()
		{
			CString cstrTemp("");
			
            // Set the path
            if(m_ccbstrOutputPath.Length() == 0)
            {
				cstrTemp = getHistoryReportPath();
            }
            else
            {
                cstrTemp = m_ccbstrOutputPath;
                
                // Append a '\' if one hasn't been added
                if(cstrTemp.GetAt(cstrTemp.GetLength() - 1) != '\\')
                {
                    cstrTemp += "\\";
                }
            }

            // Add the timestamp if required
            if (m_bAddTimeStamp)
            {
                cstrTemp += generateTimestamp();
            }

            // Set the filename
            if(m_ccbstrOutputFilename.Length() == 0)
            {
                cstrTemp += getReportName();
            }
            else
            {
                cstrTemp += (CString) m_ccbstrOutputFilename;
            }
				
            // Set the extension
            cstrTemp += getExtensionForFormat();
				
            m_cstrTemp = cstrTemp;
			
            return( m_cstrTemp.GetBuffer(m_cstrTemp.GetLength()) );
		}


        char* CrystalReportsHelper::getReportFileName()
		{
			m_cstrTemp = m_ccbstrReportSource.m_str;
			return( m_cstrTemp.GetBuffer(m_cstrTemp.GetLength()) );
		}


        char* CrystalReportsHelper::getReportName()
		{
			char szTemp[500];

			// Strip of extension.
			m_cstrTemp = m_ccbstrReportTemplate;
			strcpy( szTemp, m_cstrTemp.GetBuffer(500) );
			stripOffExtension( szTemp );

			// And get the string after the last backslash.
            char buf[500];
            ACE_OS::strcpy( buf, szTemp );
            ACE_Tokenizer tok( buf );
            tok.delimiter_replace( '\\', 0 );
            for( char* p = tok.next();
                 p;
                 p = tok.next() )
            {
                if ( NULL != p ) m_cstrTemp = p;
            }

			// Return the report name.
			return( m_cstrTemp.GetBuffer(m_cstrTemp.GetLength()) );
		}


        char* CrystalReportsHelper::getHistoryReportName()
		{
			char szTemp[500];

			// Strip of extension.
			m_cstrTemp = m_ccbstrReportSource;
			strcpy( szTemp, m_cstrTemp.GetBuffer(500) );
			stripOffExtension( szTemp );

			// And get the string after the last backslash.
			char *token;
			char seps[] = "\\";
			token = strtok( szTemp, seps );
			while( token != NULL )
			{
			   token = strtok( NULL, seps );
			   if (token) m_cstrTemp = token;
			}

			// Return the report name.
			return( m_cstrTemp.GetBuffer(m_cstrTemp.GetLength()) );
		}


        char* CrystalReportsHelper::getReportPath()
		{
			char szTemp[500];

			// Strip of extension.
			m_cstrTemp = m_ccbstrReportTemplate;
			strcpy( szTemp, m_cstrTemp.GetBuffer(500) );
			stripOffExtension( szTemp );

			// Count up the number of tokens.
			char *token;
			char seps[] = "\\";
			int iTokenCount = 0;
			token = strtok( szTemp, seps );
			while( token != NULL )
			{
				token = strtok( NULL, seps );
				if (token) iTokenCount++;
			}

			// Create a string that only contains the report's path.
			int i = 0;
			strcpy( szTemp, m_cstrTemp.GetBuffer(500) );
			token = strtok( szTemp, seps );
			m_cstrTemp = token;
			m_cstrTemp += "\\";
			while( token != NULL )
			{
				token = strtok( NULL, seps );
				if (i++ < iTokenCount-1) 
				{
					m_cstrTemp += token;
   					m_cstrTemp += "\\";
				}
			}
			int moneySymbolpos = m_cstrTemp.Find(_T('$'));
			if ( moneySymbolpos != -1)
				m_cstrTemp.Insert(0,_T("\\\\"));
			// And return the report's path.
			return( m_cstrTemp.GetBuffer(m_cstrTemp.GetLength()) );
		}
		
		//TD18040
		//xxs add
		CString CrystalReportsHelper::getHistoryReportPath( bool useCmdlineParam )
		{
			CString tempPath = m_cstrHistoryReportPath;
			if ( tempPath.IsEmpty() )
			{
				tempPath = getHistoryReportPath();
				//because the getReportPath will omit string "\\\\"
				//so i add two replace operation
				//tempPath.Replace( "\\\\adm-occ-1", "\\\\sta-occ-1" );
				//tempPath.Replace( "adm-occ-1", "\\\\sta-occ-1" );

				//use hard code
				tempPath = "\\\\adm-occ-1\\C$\\transactive\\reports\\R-Report\\historical Report\\";
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "use template file name" );
			}
			ASSERT( tempPath.GetLength() > 0 );
			if (tempPath[tempPath.GetLength()-1] != '\\')
			{
				tempPath += "\\";
			}

			//store the path
			m_cstrHistoryReportPath = tempPath;

			return m_cstrHistoryReportPath;
		}
		//end xxs add 


        char* CrystalReportsHelper::getHistoryReportPath()
		{
			// Add the History subdirectory to the report path.
			//xxs add for test
			CString tempStr = "report file full name = ";
			tempStr += m_ccbstrReportTemplate.m_str;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, tempStr );
			//end xxs for test

			getReportPath();

			//xxs add for test
			tempStr = "report file path=";
			tempStr += m_cstrTemp;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, tempStr );
			//end xxs for test

			m_cstrTemp += HISTORY_PATH_EXTENSION;
			m_cstrTemp += "\\";

			// And return the history report path.
			return( m_cstrTemp.GetBuffer(m_cstrTemp.GetLength()) );
		}


        bool CrystalReportsHelper::reportTemplateExists()
		{
			bool bResult = false;
			FILE* fin = NULL;

			m_cstrTemp = m_ccbstrReportSource.m_str;

            // Defect #3023 - Assertion when no file given in --file parameter
            // The following IF statement checks that the file name supplied
            // is not an empty string. If so, it informs the operator that 
            // a report template filename needs to be supplied.
            if (m_cstrTemp.Compare( "" ) != 0)
            {
                // The fopen function is used to ensure that the actual report template 
                // file exists. If not, it informs the operator that the report template
                // file could not be found.
			    fin = fopen( m_cstrTemp.GetBuffer(m_cstrTemp.GetLength()), "rb" );
			    if (fin != NULL)
			    {
				    bResult = true;
				    fclose( fin );
			    }
			    else
			    {
                    TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg << m_cstrTemp;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_140029);
			    }
            }
            else
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_140031);
            }

			return( bResult );
		}


        char* CrystalReportsHelper::generateTimestamp()
		{
			struct tm newtime; 
			time_t aclock;
            time(&aclock);    
            
            if(ACE_OS::localtime_r(&aclock, &newtime) != NULL)
            {
			    char szAMPM[10];
			    int iHour;
			    if (newtime.tm_hour > 12)
			    {
				    iHour = newtime.tm_hour - 12;
				    strcpy( szAMPM, "PM" );
			    }
			    else
			    {
				    iHour = newtime.tm_hour;
				    strcpy( szAMPM, "AM" );
			    }

			    char szTemp[200] = {0};
			    sprintf( szTemp, "%02d %s, %d %02d.%02d%s - ", newtime.tm_mday, 
									  					       tblMonths[newtime.tm_mon], 
														       newtime.tm_year+1900,
														       iHour,
														       newtime.tm_min,
														       szAMPM );
 			    m_cstrTimestamp = szTemp;

                // And return the timestamp string.
			    return( m_cstrTimestamp.GetBuffer(m_cstrTimestamp.GetLength()) );
            }

			return NULL;
		}


        void CrystalReportsHelper::setupGroupButton()
		{
			if (isReady() && m_pReportViewer)
			{
				long lNoOfGroups = 0;
				HRESULT hr = m_pIReport->get_NumberOfGroup( & lNoOfGroups );
				if (SUCCEEDED( hr ))
				{
					if (lNoOfGroups > 0)
					{
						m_pReportViewer->SetDisplayGroupTree( true );
					}
					else
					{
						m_pReportViewer->SetDisplayGroupTree( false );
					}
				}
				else
				{
					// Error.
				}

			}
			else
			{
				// Error.
			}
		}


        void CrystalReportsHelper::setupParameters(std::vector<std::string>& parameterValuePairs)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin CrystalReportsHelper::setupParameters");

            using TA_Base_Core::DebugUtil;

            if(parameterValuePairs.empty())
            {
                return;
            }

			bool bResult = false;
            bool bInvalidParams = false;

            // Report parameters will be specified in the following format:
            // --report-parameters=<parametername>:<value>,<parametername>:<value>, ...
            // eg. --report-parameters=p_planName:PlanA,p_group:OCC

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Parsing Report ParameterValue pairs");
            
            std::map<std::string, std::string> parameterValueMap;
            std::vector<std::string>::iterator iter = parameterValuePairs.begin();
            for(; iter != parameterValuePairs.end(); ++iter)
            {
                std::string parameterValuePair = *iter;
                int colonPos = parameterValuePair.find(":");
                if(colonPos != std::string::npos)
                {
                    std::string parameter = parameterValuePair.substr(0, colonPos);
                    std::string value = parameterValuePair.substr(colonPos+1, parameterValuePair.size()-(colonPos+1));
                    parameterValueMap[parameter] = value;
                }
                else
                {
                    // Error
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "ReportParameters - %s. Missing colon.", parameterValuePair.c_str());
                    bInvalidParams = true;
                }
            }

            // Open the report template.
			if (openReport() == true )
			{
                // Add the valid parameters to Crystal Reports
                IParameterFieldDefinitions* pIParameterFieldDefinitions = NULL;
			    HRESULT hr = m_pIReport->get_ParameterFields( &pIParameterFieldDefinitions );
			    if (SUCCEEDED( hr ))
			    {
                    std::map<std::string,std::string>::iterator iter = parameterValueMap.begin();
                    for(; iter != parameterValueMap.end(); ++iter)
                    {
				        IParameterFieldDefinition* pIParameterFieldDefinition = NULL;
				        CComBSTR ccbstrLabel = iter->first.c_str();
						CComVariant vtSubReport = "";
						hr = pIParameterFieldDefinitions->GetItemByName( ccbstrLabel, vtSubReport, &pIParameterFieldDefinition );
						//hr = pIParameterFieldDefinitions->get_Item( 1, &pIParameterFieldDefinition );
				        if (SUCCEEDED( hr ))
				        {
								CRFieldValueType fieldType;
														pIParameterFieldDefinition->get_ValueType(&fieldType);
														if(fieldType == crStringField)
														{
															//CComVariant vtValue =atoi( iter->second.c_str());
															_variant_t v1;
															v1.vt = VT_BSTR;     
															v1.bstrVal = (_bstr_t)iter->second.c_str();
							
															CComVariant vtValue = CComVariant(iter->second.c_str());
							
															hr = pIParameterFieldDefinition->SetCurrentValue(vtValue);
															//hr = pIParameterFieldDefinition->SetCurrentValue(v1);
							
															if (SUCCEEDED( hr ))
															{
																LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Added report parameter/value: %s/%s", iter->first.c_str(), iter->second.c_str());
							
															}
															else
															{
																LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Invalid value: %s for parameter: %s", iter->second.c_str(), iter->first.c_str());
																break;
															}
														}
														else if (((int)fieldType)<=7 )
														{
															
															CComVariant vtValue = CComVariant(atoi( iter->second.c_str()));
															hr = pIParameterFieldDefinition->SetCurrentValue(vtValue);
							
															if (SUCCEEDED( hr ))
															{
																LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Added report parameter/value: %s/%s", iter->first.c_str(), iter->second.c_str());
							
															}
															else
															{
																LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Invalid value: %s for parameter: %s", iter->second.c_str(), iter->first.c_str());
																break;
															}
														}
														else if (fieldType==crDateTimeField)
														{
															COleDateTime dateTest;
															std::string parameterValue = iter->second;
															if(parameterValue.length()!=14)
																LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Invalid datetime value, should be like 12032012211021, ddmmyyyyHHMISS");
															else
															{
																int v_Date,v_Month,v_Year,v_HH,v_Mi,v_SS;
																v_Date = atoi(parameterValue.substr(0,2).c_str());
																v_Month = atoi(parameterValue.substr(2,2).c_str());
																v_Year = atoi(parameterValue.substr(4,4).c_str());
																v_HH = atoi(parameterValue.substr(8,2).c_str());
																v_Mi = atoi(parameterValue.substr(10,2).c_str());
																v_SS = atoi(parameterValue.substr(12,2).c_str());
																dateTest.SetDateTime(v_Year,v_Month, v_Date,v_HH,v_Mi,v_SS);
																VARIANT vtStart;
																vtStart.vt = VT_DATE;
																vtStart.date = dateTest;
																hr = pIParameterFieldDefinition->SetCurrentValue(vtStart);
							
																if (SUCCEEDED( hr ))
																{
																	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Added report parameter/value: %s/%s", iter->first.c_str(), iter->second.c_str());
							
																}
																else
																{
																	LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Invalid value: %s for parameter: %s", iter->second.c_str(), iter->first.c_str());
																	break;
																}
															}
														}
														else if (fieldType==crDateField)
														{
															COleDateTime dateTest;
															std::string parameterValue = iter->second;
															if(parameterValue.length()!=8)
																LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Invalid datetime value, should be like 12032012211021, ddmmyyyyHHMISS");
															else
															{
																int v_Date,v_Month,v_Year,v_HH,v_Mi,v_SS;
																v_Date = atoi(parameterValue.substr(0,2).c_str());
																v_Month = atoi(parameterValue.substr(2,2).c_str());
																v_Year = atoi(parameterValue.substr(4,4).c_str());
																dateTest.SetDate(v_Year,v_Month, v_Date);
																VARIANT vtStart;
																vtStart.vt = VT_DATE;
																vtStart.date = dateTest;
																hr = pIParameterFieldDefinition->SetCurrentValue(vtStart);
							
																if (SUCCEEDED( hr ))
																{
																	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Added report parameter/value: %s/%s", iter->first.c_str(), iter->second.c_str());
							
																}
																else
																{
																	LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Invalid value: %s for parameter: %s", iter->second.c_str(), iter->first.c_str());
																	break;
																}
															}
														}
					    }
				        else
				        {
                            // This parameter doesn't exist in this template
                            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Invalid parameter: %s", iter->first.c_str());
                            break;
				        }
                    }

                    if(iter == parameterValueMap.end())
                    {
                        if(!bInvalidParams)
                        {
                            bResult = true;
                        }
                    }
                }
                else
                {
                    // Error
                }
            }

            // Have kept the following code here in case there is a need for passing in a time parameter

            /*
			ccbstrLabel = "p_interval";
			vtSubReport = "";
			hr = pIParameterFieldDefinitions->GetItemByName( ccbstrLabel, vtSubReport, &pIParameterFieldDefinition );
			if (SUCCEEDED( hr ))
			{
				COleDateTime dateTest;
				dateTest.SetDate(2003,10, 1);
				VARIANT vtStart;
				vtStart.vt = VT_DATE;
				vtStart.date = dateTest;
				dateTest.SetDate(2003,12,8);
				VARIANT vtEnd;
				vtEnd.vt = VT_DATE;
				vtEnd.date = dateTest;
				enum CRRangeInfo crRangeInfo;
				crRangeInfo = (enum CRRangeInfo)(crRangeIncludeLowerBound | crRangeIncludeUpperBound);
				hr = pIParameterFieldDefinition->AddCurrentRange(vtStart, vtEnd, crRangeInfo );
				if (SUCCEEDED( hr ))
				{
					bResult = true;
				}
				else
				{
					// Error.
				}
			}
            */
				
			if(!bResult)
            {
                TA_THROW(TA_Base_Core::ReportManagerException("Some or all Report Parameters are invalid"));
            }

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "End CrystalReportsHelper::setupParameters");
		}


        bool CrystalReportsHelper::sendAuditMessage( char* p_szAuditMessage )
		{
			bool bResult = false;

            #ifdef TRANSACTIVE_BUILD

    	    // Obtain a pointer to the audit message sender instance.
            TA_Base_Core::AuditMessageSender* sender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::ReportsAudit::Context );
		    
		    // Send AuditMessage
	        TA_Base_Core::NameValuePair description1( "AuditMessage", p_szAuditMessage );
	        TA_Base_Core::DescriptionParameters description;
	        description.push_back( &description1);

            // TD4213: Retrieve the correct entity key to pass as a parameter
            unsigned long lEntity = strtoul(TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYKEY).c_str(), NULL, 10);
		    
            sender->sendAuditMessage( TA_Base_Core::ReportsAudit::Reports, 
								      lEntity, 
								      description, 
								      "", 
                                      m_cstrSessionId.GetBuffer( m_cstrSessionId.GetLength() ),
								      "", 
								      "", 
								      "" );

		    delete sender;

            // And return success.
            bResult = true;

            #else

            // Just return success.
            bResult = true;

            #endif // TRANSACTIVE_BUILD

			return( bResult );
		}

		//get history report path
		void CrystalReportsHelper::setHistoryReportPathFromCmdLine( const char * pathName )
		{
			m_cstrHistoryReportPath = pathName;
		}


#ifdef TRANSACTIVE_BUILD
	} // TA_Reporting
} // TA_Base_App
#endif // TRANSACTIVE_BUILD
