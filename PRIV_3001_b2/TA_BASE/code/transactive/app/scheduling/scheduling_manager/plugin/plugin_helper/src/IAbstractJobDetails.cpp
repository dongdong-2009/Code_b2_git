/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/scheduling/scheduling_manager/plugin/plugin_helper/src/IAbstractJobDetails.cpp $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This interface must be implemented by each plugin component library. The Scheduling Manager uses
  * this interface to retrieve information from each component and also to pass other information.
  * 
  */

#include "app/scheduling/scheduling_manager/plugin/plugin_helper/src/IAbstractJobDetails.h"
#include "app/scheduling/scheduling_manager/plugin/plugin_helper/src/SingletonHelper.h"
#include "core/utilities/src/IDllHandler.h"

namespace TA_Base_App
{
    void IAbstractJobDetails::setUpSingletons(const TA_Base_Core::IDllHandler& dllHandler, const std::string& name)
    {
        SingletonHelper helper;
        helper.initialiseUtilities(dllHandler, name);
    }


	void IAbstractJobDetails::setData(const JobData& data)
    {
        m_jobData.name = data.name;
        m_jobData.description = data.description;
        m_jobData.parameters = data.parameters;
    }

}


