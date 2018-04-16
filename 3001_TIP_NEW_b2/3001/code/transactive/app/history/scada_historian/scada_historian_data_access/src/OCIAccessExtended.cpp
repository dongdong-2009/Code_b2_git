/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/scada_historian/scada_historian_data_access/src/OCIAccessExtended.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#include "app/history/scada_historian/scada_historian_data_access/src/OCIAccessExtended.h"
#include "app/history/scada_historian/scada_historian_data_access/src/InsertDBError.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Core;


namespace TA_App
{
    
        OCIAccessExtended::OCIAccessExtended(const char* hostName, 
									         const char* databaseName,
									         const char* userName, 
									         const char* password, 
									         int max_iterations)
	        : OCIInsertAccess(hostName, databaseName, userName, password), 
	        m_maxIterations(max_iterations) 
        {
        }

        bool OCIAccessExtended::execArrayInsert(const std::string& command, long &retCode, 
	        BindVars *bindVector, int bindType, int commit)
        {
	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "OCIAccessExtended::execArrayInsert()");

	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Statement: %s", command.c_str());
	        
	        int mode = OCI_DEFAULT;
	        int attempts = 0;

	        if (commit != 0)
	        {
		        mode |= OCI_COMMIT_ON_SUCCESS;
	        }

	        do
	        {
		        try
		        {
			        if (m_statement != 0)
			        {
				        OCIHandleFree(m_statement, OCI_HTYPE_STMT);
			        }

			        if ((m_LastStatus = OCIHandleAlloc(	m_env, (void**)&m_statement, 
															        OCI_HTYPE_STMT, 0, 0) )
					        != OCI_SUCCESS)
			        {
				        EvaluateLastStatus(__FILE__, __LINE__);
			        }

			        // First prepare the statement.
			        if ((m_LastStatus = OCIStmtPrepare( m_statement, m_error, 
															        (unsigned char *) command.c_str(), 
															        command.length(),
															        OCI_NTV_SYNTAX, mode ) )
					        != OCI_SUCCESS )
			        {
				        EvaluateLastStatus(__FILE__, __LINE__);
			        }

			        // Bind variable names to database values if requested.
			        if ( bindVector != 0 )
			        {
				        bindVector->begin();

				        int		pos	= 1;
				        BindVar	*var	= 0;
				        while ((var = bindVector->next()) != 0)
				        {
					        // Translate the generic variable type	into something OCI specific.
					        int theType;
					        switch(var->type())
					        {
					        case DBA_STRING:			theType = SQLT_STR; break;
					        //case DBA_STRING:			theType = SQLT_CHR; break;
					        case DBA_INT:				theType = SQLT_INT; break;
					        case DBA_NUM:				theType = SQLT_FLT; break; // AL: Changed this from SQLT_NUM to SQLT_FLT
					        case DBA_DATE:				theType = SQLT_DATE; break;
					        case DBA_TIME:				theType = SQLT_TIME; break;
					        case DBA_TIME_TZ:			theType = SQLT_TIME_TZ; break;
					        case DBA_TIMESTAMP:		theType = SQLT_TIMESTAMP; break;
					        case DBA_TIMESTAMP_TZ:	theType = SQLT_TIMESTAMP_TZ; break;

					        default:
						        // Whatever datatype it is, it's not supported. Force an OCI error.
						        theType = -1;
					        }

					        // The bindPtr MUST be null on each Oracle bind call.
					        // It's never used since the bind handle will be released implicit.
					        OCIBind		*bindPtr		= 0;
					        OCIDefine	*DefinePtr	= 0;

					        switch (bindType)
					        {
					        case BY_DEFINE_POS:
						        m_LastStatus = 
							        OCIDefineByPos ( m_statement, 
												          &DefinePtr,
												          m_error,
												          pos++,
												          const_cast<void *>(var->addr()),
												          var->len(),
												          theType,
												          0,
												          0,
												          0,
												          OCI_DEFAULT );
						        break;

					        case BY_NAME:
						        {
							        int varLen = var->len();
							        if(var->type() == DBA_STRING)
								        --varLen;

							        m_LastStatus = 
								        OCIBindByName(	m_statement, 
													        &bindPtr,
													        m_error,
													        (unsigned char *) const_cast<char *>(var->name()),
													        -1,
													        const_cast<void *>(var->addr()),
													        varLen,
													        theType,
													        0,
													        0,
													        0,
													        0,
													        0, 
													        OCI_DEFAULT  );
						        }
						        break; // AL added 11/7/02
					        case BY_POSITION:
						        m_LastStatus = 
							        OCIBindByPos(	m_statement, 
												        &bindPtr,
												        m_error,
												        pos++,
												        const_cast<void *>(var->addr()),
												        var->len(),
												        theType,
												        0,
												        0,
												        0,
												        0,
												        0, 
												        OCI_DEFAULT  );
					        default:
						        break;
					        }
					        
					        if (m_LastStatus != OCI_SUCCESS )
					        {
						        EvaluateLastStatus(__FILE__, __LINE__);
						        break;
					        }

					        m_LastStatus = OCIBindArrayOfStruct(bindPtr, m_error, var->len(), 0, 0, 0);
					        if(m_LastStatus != OCI_SUCCESS)
					        {
						        EvaluateLastStatus(__FILE__, __LINE__);
					        }

				        } // end while
			        }

			        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Executing...");

			        // At last execute the statement.
			        if ((m_LastStatus = OCIStmtExecute( m_context, m_statement, m_error, 
															        m_maxIterations, 0, 0, 0, OCI_DEFAULT ) ) 
					        != OCI_SUCCESS )
			        {
				        EvaluateLastStatus(__FILE__, __LINE__);
			        }

			        // The flag 'OCI_COMMIT_ON_SUCCESS' doesn't seem to work...
			        // Do it manually.
			        if (commit != 0)
			        {
				        StatementCommit();
			        }
		        }

		        catch(InsertDBError& e)
		        {
			        // Within this method, this error should only occur during the 
			        // OCIStmtExecute.
			        if( 3113 == e.status() )
			        {
				        if( RetryStatement() == false )
					        break;
				        // Force the retry
				        m_LastStatus = OCI_ERROR;
			        }
			        else
			        {
				        break;
			        }
		        }

		        // Increment the number of events
		        attempts++;
	        } while ( m_LastStatus != OCI_SUCCESS && attempts <= 2 );

	        retCode = m_LastStatus;
	        return (m_LastStatus == OCI_SUCCESS);
        }

    
} // TA_App


