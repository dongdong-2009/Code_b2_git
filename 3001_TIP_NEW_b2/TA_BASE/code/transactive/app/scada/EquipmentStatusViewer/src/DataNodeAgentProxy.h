/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/DataNodeAgentProxy.h $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/12/06 15:44:36 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// DataNodeAgentProxy.h: interface for the DataNodeAgentProxy class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DATANODE_AGENT_PROXY_H
#define DATANODE_AGENT_PROXY_H

#include <set>
#include <string>
#include <sys/timeb.h>
#include "app/scada/EquipmentStatusViewer/src/EquipmentStatusViewerDlg.h"
#include "core/naming/src/NamedObject.h"
#include "core/synchronisation/src/Condition.h"
#include "core/timers/src/ITimeoutCallback.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include "bus/scada/common_library/IDL/src/IDataNodeAgentCorbaDef.h"
#include "bus/generic_gui/src/TransActiveDialog.h"

namespace TA_Base_App
{
    class DataNodeAgentProxy : public TA_Base_Core::Thread      
    {
    public:
        typedef TA_Base_Core::NamedObject< TA_Base_Bus::IDataNodeAgentCorbaDef,
                                           TA_Base_Bus::IDataNodeAgentCorbaDef_ptr,
                                           TA_Base_Bus::IDataNodeAgentCorbaDef_var >   DataNodeAgentInterface;

		DataNodeAgentProxy(std::string & agentName, CEquipmentStatusViewerDlg* dlg);

	    virtual ~DataNodeAgentProxy();

        virtual void run();
        
        virtual void terminate();

		void pausePoll();

		void resumePoll();

		void setToBadNotConnectedQuality();
                
    private:
		volatile bool							m_pollSwitchOn;
        volatile bool							m_toBeTerminated;
		DataNodeAgentInterface*                 m_agentDnInterface;
		unsigned long							m_observerId;
		std::set<unsigned long>					m_storeOITPDn;
		CEquipmentStatusViewerDlg*				m_dlg;
    };
}
#endif // DATANODE_AGENT_PROXY_H
