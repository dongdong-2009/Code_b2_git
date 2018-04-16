/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/system_control/system_manager/dummy_agent/src/TestAgent.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#if !defined(TESTAGENT_H)
#define TESTAGENT_H

#include "ManagedAgentCorbaDef.h"
#include "core/process_management/IDL/src/IProcessManagerCorbaDef.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/process_management/IDL/src/IManagedProcessCorbaDef.h"
#include "core/corba/src/ServantBase.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/RunParams.h"


class TestAgent :  public POA_TA_Base_Core::IManagedProcessCorbaDef,
                   public TA_Base_Core::RunParamUser,
                   public TA_Base_Core::ServantBase
{
public:
    TestAgent();
    ~TestAgent();

    void terminate();
    void setParam(const TA_Base_Core::RunParam& param);
    void poll();
    void servicesRestarted();
    TA_Base_Core::RunParamSeq* getParams();
    void onRunParamChange(const std::string& name,const std::string& value);


    ManagedAgentCorbaDef* m_managedAgent;

private:
    TA_Base_Core::IProcessManagerCorbaDef_var m_systemController;
};

#endif
