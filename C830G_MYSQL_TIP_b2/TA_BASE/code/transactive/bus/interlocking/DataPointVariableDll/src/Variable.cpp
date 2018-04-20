/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/DataPointVariableDll/src/Variable.cpp $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This is the 'mainline' of the DLL. This is the entry point and is run when the 
  * Interlocking Rule Trigger Editor loads the DLL.
  *
  */

#include "stdafx.h"

#include "bus/interlocking/InterlockingConfig/src/IAddVariable.h"
#include "core/utilities/src/DllHandler.h"
#include "bus/interlocking/interlockingConfig/src/SingletonHelper.h"
#include "core/utilities/src/DebugUtil.h"

#include "VariableSelector.h"
#include "Variable.h"
#include "resource.h"


namespace TA_Base_Bus
{
    /**
     * Every variable type supported must instantiate this interface.
     * 
     * Typically it will be done in a dll that is loaded on demand..
     * 
     * This interface works through a callback that is passed in to the constructor.
     * Thus no other functions are required.
     * @version 1.0
     * @created 08-Feb-2005 02:20:36 PM
     */
	/**
	 * @param variableType
	 * @param adder
	 * 
	 */
    Variable::Variable(IAddVariable* adder)
        :m_adder(adder),
         m_selector(NULL)
    {
    }

    Variable::~Variable()
    {
    }

    /**
     * showSelector
     *
     * Display the variable selector dialog
     */
    void Variable::showSelector()
    {

        if ( m_selector != NULL )
        {
			m_selector->ShowWindow(SW_SHOW);
            m_selector->SetFocus();
            return;
        }

		try
		{
			// create main dialog screen
			m_selector = new VariableSelector();
			// copy rule information to main dialog
			m_selector->m_adder = m_adder;
			m_selector->m_variable = this;

			// display main dialog
			m_selector->Create(IDD_VariableSelector_DIALOG);
			m_selector->ShowWindow(SW_SHOW);
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", "Error caught: Variable_showSelector. Exception in creating VariableSelectorDialog." );
			if ( m_selector != NULL )
			{
				m_selector->ShowWindow(SW_SHOW);
				m_selector->SetFocus();
				return;
			}
			return;
		}
    }

    /**
     * destroySelector
     *
     * Destroys the variable selector dialog
     */
    void Variable::destroySelector()
    {
        if ( m_selector != NULL )
        {
			m_selector->cleanScadaTree();
			m_selector->DestroyWindow();

			delete m_selector;
			m_selector = NULL;
        }
    }

    /** 
     * setUpSingletons
     *
     * This will set up the singletons in the DLL to match those in the main application.
     *
     * @param IDllHandler& - This is a handler from the main application that can be used to
     *                       retrieve the details of it's singletons. This can then be used to
     *                       set up the singletons in the DLL.
     */
    void Variable::setUpSingletons(const TA_Base_Core::IDllHandler& dllHandler)
    {
        // initialise CORBA
        TA_Base_Core::CorbaUtil::setInstance( dllHandler.getCorbaUtil() );

        // initialise debug util
                // Set the debug level
        std::string param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVEL);
        if (!param.empty())
        {
            TA_Base_Core::DebugUtil::getInstance().setLevel( TA_Base_Core::DebugUtil::getDebugLevelFromString(param.c_str()));
        }

        // Set the maximum size of a debug file in bytes
        param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGFILEMAXSIZE);
        if (!param.empty())
        {
            TA_Base_Core::DebugUtil::getInstance().setMaxSize(atoi(param.c_str()));
        }

        // Set the maximum number of log files
        param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGMAXFILES);
        if (!param.empty())
        {
            TA_Base_Core::DebugUtil::getInstance().setMaxFiles(atoi(param.c_str()));
        }
        // end initialise debug util


        // Pick a random runparam that always gets set and see if it is already set
        if (TA_Base_Core::RunParams::getInstance().isSet( RPARAM_APPNAME ))
        {
            // We have already initialised the singletons so don't do it again
            return;
        }

        // Now initialise all the singletons inside the DLLs
        TA_Base_Core::RunParams& dllRunParams = TA_Base_Core::RunParams::getInstance();
        TA_Base_Core::RunParams::ParamVector mainAppRunParams(dllHandler.getRunParams());
        TA_Base_Core::RunParams::ParamVector::iterator it = 0;
        for( it = mainAppRunParams.begin(); it != mainAppRunParams.end(); it++ )
        {
            if( !dllRunParams.isSet( it->name.c_str() ) )
            {
                dllRunParams.set( it->name.c_str(), it->value.c_str() );
            }
        }
    }

}//namespace
