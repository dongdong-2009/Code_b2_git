/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_monitor_gui/src/RadioMonitorTestData.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Responsible for generating test data, for purposes of testing Radio Monitor
  *
  */

#include "app/radio/radio_monitor_gui/src/StdAfx.h"
#include "app/radio/radio_monitor_gui/src/RadioMonitorDlg.h"
#include "app/radio/radio_monitor_gui/src/RadioMonitorTestData.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/pa/messagesubscriber/src/SimpleMessageSubscriber.h"
#include "core/data_access_interface/src/PrimitiveWrapper.h"


#include <sstream>

using namespace TA_IRS_App::RadioMonitorUtils;

namespace TA_IRS_App
{
    namespace RadioMonitor
    {
        DWORD WINAPI RadioMonitorTestData_Thread(void* val)
        {                        
            for(;;)
            {
                Sleep(10);
                
                TA_IRS_Bus::SimpleMessageSubscriber<CorbaMonitorUpdate>* pMonitor
                    = RadioMonitorTestData::getInstance().getMonitorSubscriber();

                TA_IRS_Bus::SimpleMessageSubscriber<CorbaMonitorUpdate>* pListen 
                    = RadioMonitorTestData::getInstance().getListenSubscriber();

                TA_IRS_Bus::SimpleMessageSubscriber<TA_Base_Bus::IRadioCorbaDef::SubscriberDetailsNotification>* pSearchPanel
                    = RadioMonitorTestData::getInstance().getSearchPanelSubscriber();

                if (0 == pMonitor || 0 == pListen)
                {   
                    RadioMonitorTestData::getInstance().flagThreadComplete();
                    ExitThread(0);                    
                }
                
                PrimitiveWrapper<CorbaMonitorUpdate> update;
                                
                DWORD result = MessageBox(0, "Yes - add new\nNo - update existing\nCancel - remove existing", "Debug monitor subscribers", MB_YESNOCANCEL);

                if (IDYES == result)
                {
                    update.setValue(RadioMonitorTestData::getInstance().generateRandomUpdate(RadioMonitorTestData::NEW_UPDATE));
                }
                else if (IDNO == result)
                {
                    update.setValue(RadioMonitorTestData::getInstance().generateRandomUpdate(RadioMonitorTestData::UPDATE_UPDATE));
                }
                else
                {
                    update.setValue(RadioMonitorTestData::getInstance().generateRandomUpdate(RadioMonitorTestData::REMOVE_UPDATE));
                }

                /*
                int freq = 10;      // Higher the value, less freqent the updates
                if (rand() % freq == 0)
                {
                    update.setValue(RadioMonitorTestData::getInstance().generateRandomUpdate(RadioMonitorTestData::NEW_UPDATE));
                }
                else if (rand() % freq == 0)
                {
                    update.setValue(RadioMonitorTestData::getInstance().generateRandomUpdate(RadioMonitorTestData::UPDATE_UPDATE));
                }
                else if (rand() % freq == 0)
                {
                    update.setValue(RadioMonitorTestData::getInstance().generateRandomUpdate(RadioMonitorTestData::REMOVE_UPDATE));
                }
                */
                
                if (update.hasBeenSet())
                {
                    pMonitor->DEBUG_processMessage(update.getValue());
                    pListen->DEBUG_processMessage(update.getValue());
                }
                
                static bool postToSearchPage = true;
                
                if (postToSearchPage) 
                {
                    static char* TSIs[] =
                    {
                        "485-356-364",
                        "235-362-623",
                        "987-356-323",
                        "424-356-774"
                    };
                    static char* names[] =
                    {
                        "James",
                        "Henry",
                        "Bobby",
                        "Jack"
                    };

                    TA_Base_Bus::IRadioCorbaDef::SubscriberDetailsNotification data;
                    data.notifyType = TA_Base_Bus::IRadioCorbaDef::SubscriberNotificationType(rand() % 4);

                    if (rand() % 2 == 0)
                    {
                        data.details.type = TA_Base_Bus::IRadioCorbaDef::INDIVIDUAL_SUBSCRIBER;
                    }
                    else
                    {
                        data.details.type = TA_Base_Bus::IRadioCorbaDef::GROUP_SUBSCRIBER;
                    }
                    
                    int random = rand() % 4;
                    const char* tsi = TSIs[random];
                    const char* name = names[random];

                    data.details.TSI = CORBA::string_dup(tsi);
                    data.details.subscriberName = CORBA::string_dup(name);
                    data.details.locationKey = 1 + (rand() % 5);
                    std::ostringstream ostr;
                    ostr << "LOC:" << data.details.locationKey;
                    data.details.locationName = CORBA::string_dup(ostr.str().c_str());

                    pSearchPanel->DEBUG_processMessage(data);
                }
            }
        }


        RadioMonitorTestData::RadioMonitorTestData()
        :
        m_pMonitor(0),
        m_pListen(0),
        m_pSearchPanel(0),
        m_threadActive(false)
        {
        }


        RadioMonitorTestData& RadioMonitorTestData::getInstance()
        {
            static RadioMonitorTestData sInstance;
            return sInstance;
        }


        void RadioMonitorTestData::beginThread(TA_IRS_Bus::SimpleMessageSubscriber<CorbaMonitorUpdate>* pMonitor,
                                                TA_IRS_Bus::SimpleMessageSubscriber<CorbaMonitorUpdate>* pListen,                                                
                                                TA_IRS_Bus::SimpleMessageSubscriber<SubscriberNotification>* pSearchPanel)
        {
            if (0 != m_pMonitor || 0 != m_pListen || 0 != m_pSearchPanel)
            {
                TA_THROW(TA_Base_Core::TransactiveException("beginThread has already been called"));
            }
                   
            ////////////////////////////////////
            // Populate selected list with some items
            /*
            RadioMonitorDlg* dlg = static_cast<RadioMonitorDlg*>(AfxGetMainWnd());
            std::vector<RadioMonitorDlg::SelectedEntity> data;
            
            for (int i = 0; i < 5; i ++)
            {
                RadioMonitorDlg::SelectedEntity ent;
                ent.id = "ID";
                ent.location = "DBG";
                std::ostringstream ostr;
                ostr << i;
                ent.tsi = ostr.str();
                data.push_back(ent);
            }
            
            dlg->addToSelectionList(data);
            */
            ////////////////////////////////////

            m_pMonitor = pMonitor;
            m_pListen = pListen;
            m_pSearchPanel = pSearchPanel;

            HANDLE hThread = CreateThread(0, 0, RadioMonitorTestData_Thread, 0, 0, 0);
            m_threadActive = true;
            CloseHandle(hThread);
        }
        

        void RadioMonitorTestData::cancelThread()
        {
            m_pMonitor  = 0;
            m_pListen   = 0;
            m_pSearchPanel = 0;
            
            while (m_threadActive)
            {
                Sleep(500);
            }
        }

    
        const CorbaMonitorUpdate RadioMonitorTestData::generateRandomUpdate(EUpdateType type)
        {
            if (0 == m_updates.size())
            {
                type = NEW_UPDATE;
            }
            
            int randomIndex = 0;
            
            if (m_updates.size() > 0)
            {
                randomIndex = rand() % m_updates.size();
            }

            std::list<TA_IRS_App::RadioMonitorUtils::CorbaMonitorUpdate>::iterator existingItem = m_updates.begin();

            for (int i = 0; i < randomIndex; i ++)
            {
                existingItem ++;
            }
                
            switch (type)
            {
            case NEW_UPDATE:
                {
                    CorbaMonitorUpdate update = generateUpdate(0);
                    m_updates.push_back(update);
                    return m_updates.back();
                }
            case UPDATE_UPDATE:        
                {        
                    CorbaMonitorUpdate update = generateUpdate(&(*existingItem));
    
                    // Make sure the update retains the properties that make it
                    // unique
                    *existingItem = update;
    
                    return *existingItem;
                }
            case REMOVE_UPDATE:
                {   
                    CorbaMonitorUpdate result = *existingItem;
                    result.callStatus = TA_Base_Bus::IRadioCorbaDef::Stopped;

                    // Remove from internal list, so not present in later calls
                    m_updates.erase(existingItem);
                    return result;
                }
            default:
                TA_ASSERT(FALSE, "Unrecognized type");
            }

            TA_THROW(TA_Base_Core::TransactiveException("RadioMonitorTestData: Invalid type"));
        }


        const CorbaMonitorUpdate RadioMonitorTestData:: generateUpdate(const CorbaMonitorUpdate* existingItem)
        {
            CorbaMonitorUpdate result;
            std::ostringstream ostr;
            
            static long callId = 0;
            callId ++;

            result.callID = callId;
            result.sessionRef = 0;
            result.monitorID = 0;

            static char* TSIs[] =
            {
                "485-356-364",
                "235-362-623",
                "987-356-323",
                "424-356-774"
            };
            static char* names[] =
            {
                "James",
                "Henry",
                "Bobby",
                "Jack"
            };
            static char* locations[] =
            {
                "CVC",
                "OCC",
                "DBG",
                "MSM"
            };
            int random = rand() % 4;
            const char* sourceTSI = TSIs[random];
            const char* sourceName = names[random];
            const char* sourceLoc = locations[random];
            random ++;
            if (random >= 4)
            {
                random = 0;
            }
            const char* destTSI = TSIs[random];
            const char* destName = names[random];
            const char* destLoc = locations[random];

            time_t nowTime;
            time(&nowTime);
            enum { BUF_MAX = 256 };
            static char buf[BUF_MAX];
            const std::string format("%d/%m %H:%M:%S");        
            strftime(buf, BUF_MAX, format.c_str(), localtime(&nowTime));
               
            result.callTime = CORBA::string_dup(buf);

            ostr.str("");
            ostr << "Caller " << callId;
            result.callerName   = CORBA::string_dup(sourceName/*ostr.str().c_str()*/);

            ostr.str("");
            ostr << "TSI(s) " << callId;
            result.callerTSI    = CORBA::string_dup(sourceTSI/*ostr.str().c_str()*/);

            ostr.str("");
            ostr << "Src Location " << callId;
            result.callerLocation = CORBA::string_dup(sourceLoc/*ostr.str().c_str()*/);

            ostr.str("");
            ostr << "Called " << callId;
            result.calledName   = CORBA::string_dup(destName/*ostr.str().c_str()*/);

            ostr.str("");
            ostr << "TSI(d) " << callId;
            result.calledTSI    = CORBA::string_dup(destTSI/*ostr.str().c_str()*/);

            ostr.str("");
            ostr << "Dest Location " << callId;
            result.calledLocation = CORBA::string_dup(destLoc/*ostr.str().c_str()*/);
            
            if (rand() % 2 == 0)
            {
                result.callStatus = TA_Base_Bus::IRadioCorbaDef::Monitoring;
            }
            else
            {   
                result.callStatus = TA_Base_Bus::IRadioCorbaDef::Listening;
            }
            
            int participants = rand() % 10 - 5;
            if (participants <= 0)
            {
                participants = 0;
            }

            result.participants.length(participants);

            for (int i = 0; i < participants; i ++)
            {
                TA_Base_Bus::IRadioCorbaDef::CallParticipant& participant = result.participants[i];

                int irandom = rand() % 4;
                const char* pTSI = TSIs[irandom];
                const char* pName = names[irandom];
                const char* pLoc = locations[irandom];

                ostr.str("");
                ostr << "Participant " << i;
                participant.name = CORBA::string_dup(pName/*ostr.str().c_str()*/);

                ostr.str("");
                ostr << "P.TSI " << i;
                participant.TSI = CORBA::string_dup(pTSI/*ostr.str().c_str()*/);

                ostr.str("");
                ostr << "P.Loc " << i;
                participant.location = CORBA::string_dup(pLoc/*ostr.str().c_str()*/);
            }

            // Category dependent on participant count
            result.callCategory = result.participants.length() == 0 ? 
                            TA_Base_Bus::IRadioCorbaDef::FullDuplex : 
                            TA_Base_Bus::IRadioCorbaDef::HalfDuplex;            
            
            if (0 != existingItem)
            {
                result.callID = existingItem->callID;
                result.callerTSI = existingItem->callerTSI;
                result.calledTSI = existingItem->calledTSI;
            }

            return result;
        }


    }   // end namespace RadioMonitor  
}   // end namespace TA_IRS_App
