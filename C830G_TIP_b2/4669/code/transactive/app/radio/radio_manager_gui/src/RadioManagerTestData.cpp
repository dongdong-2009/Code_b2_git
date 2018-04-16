/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/RadioManagerTestData.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * [Taken from Radio Monitor, and adapted]
  * Responsible for generating test data, for purposes of testing Radio Manager
  *
  */

#include "app/radio/radio_manager_gui/src/StdAfx.h"
#include "app/radio/radio_manager_gui/src/RadioManagerTestData.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/data_access_interface/src/PrimitiveWrapper.h"

#include <sstream>
#include <vector>

using namespace TA_Base_Core;

namespace TA_IRS_App
{
    namespace RadioManager
    {
        DWORD WINAPI RadioManagerTestData::RadioManagerTestData_Thread2(void* val)
        {
            for(;;)
            {
                Sleep(1000);
                
                SubscriberDef* currentCall = RadioManagerTestData::getInstance().m_currentCallSubscriber;

                if (0 == currentCall)
                {
                    // Exit requested
                    RadioManagerTestData::getInstance().flagThreadComplete();                   
                    ExitThread(0);      
                }
                
                // Scan for any conference calls
                const RadioManagerTestData::CallDetails* call = RadioManagerTestData::getInstance().getRandomConferenceCall();

                if (0 == call)
                {
                    // No conference calls to operate on
                    continue;
                }

                TA_Base_Bus::IRadioCorbaDef::PTTStatusNotification ptt;
                ptt.state = rand() % 2 == 0 ? TA_Base_Bus::IRadioCorbaDef::PTT_GRANTED : TA_Base_Bus::IRadioCorbaDef::PTT_CEASE;
                ptt.callID = call->callID;
                ptt.sessionRef = call->sessionRef;
                ptt.transactionID = 0;
                ptt.userITSI = CORBA::string_dup(call->callerTSI.in());
                
                ASSERT(call->participants.length() > 0);

                // Find a random participant to be the talking operator                
                ptt.tpiITSI = CORBA::string_dup(call->participants[rand() % call->participants.length()].TSI.in());

                TA_Base_Core::CommsMessageCorbaDef baseMessage;
                baseMessage.messageState <<= (new TA_Base_Bus::IRadioCorbaDef::PTTStatusNotification(ptt));
                    
                currentCall->receiveSpecialisedMessage(baseMessage);                    
            }

            return 0;
        }

        DWORD WINAPI RadioManagerTestData::RadioManagerTestData_Thread(void* val)
        {                        
            for(;;)
            {
                Sleep(10);
                
                SubscriberDef* currentCall = RadioManagerTestData::getInstance().m_currentCallSubscriber;

                if (0 == currentCall)
                {   
                    RadioManagerTestData::getInstance().flagThreadComplete();
                    ExitThread(0);                    
                }
                
                PrimitiveWrapper<CallDetails> update;
                                
                DWORD result = MessageBox(0, "Yes - add new\nNo - update existing\nCancel - remove existing", "Debug monitor subscribers", MB_YESNOCANCEL);

                if (IDYES == result)
                {
                    update.setValue(RadioManagerTestData::getInstance().generateRandomCurrentCallUpdate(RadioManagerTestData::NEW_UPDATE));
                }
                else if (IDNO == result)
                {
                    update.setValue(RadioManagerTestData::getInstance().generateRandomCurrentCallUpdate(RadioManagerTestData::UPDATE_UPDATE));
                }
                else
                {
                    update.setValue(RadioManagerTestData::getInstance().generateRandomCurrentCallUpdate(RadioManagerTestData::REMOVE_UPDATE));
                }

                if (update.hasBeenSet())
                {
                    TA_Base_Bus::IRadioCorbaDef::CallStatusEvent event;
                    event.callID = update.getValue().callID;
                    event.sessionRef = update.getValue().sessionRef;
                    event.callStatus = update.getValue().callStatus;
                    event.callCategory = update.getValue().callCategory;

                    TA_Base_Core::CommsMessageCorbaDef baseMessage;
                    baseMessage.messageState <<= (new TA_Base_Bus::IRadioCorbaDef::CallStatusEvent(event));
                    
                    currentCall->receiveSpecialisedMessage(baseMessage);                    
                }                
            }
        }


        RadioManagerTestData::RadioManagerTestData()
        :
        m_currentCallSubscriber(0),
        m_activeThreads(0)
        {
        }


        RadioManagerTestData& RadioManagerTestData::getInstance()
        {
            static RadioManagerTestData sInstance;
            return sInstance;
        }


        void RadioManagerTestData::beginThread(SubscriberDef& currentCallSubscriber, long sessionRef)                                                
        {
            m_currentCallSubscriber = &currentCallSubscriber;
            m_sessionRef = sessionRef;

            HANDLE hThread = CreateThread(0, 0, RadioManagerTestData::RadioManagerTestData_Thread, 0, 0, 0);
            m_activeThreads ++;
            CloseHandle(hThread);
        
            HANDLE hThread2 = CreateThread(0, 0, RadioManagerTestData::RadioManagerTestData_Thread2, 0, 0, 0);
            m_activeThreads ++;
            CloseHandle(hThread2);
        }
        

        void RadioManagerTestData::cancelThread()
        {
            m_currentCallSubscriber  = 0;
            
            while (m_activeThreads > 0)
            {
                Sleep(500);
            }
        }

    
        const RadioManagerTestData::CallDetails 
        RadioManagerTestData::generateRandomCurrentCallUpdate(EUpdateType type)
        {
            TA_Base_Core::ThreadGuard guard(m_currentCallLock);

            if (0 == m_currentCallUpdates.size())
            {
                type = NEW_UPDATE;
            }
            
            int randomIndex = 0;
            
            if (m_currentCallUpdates.size() > 0)
            {
                randomIndex = rand() % m_currentCallUpdates.size();
            }

            std::list<CallDetails>::iterator existingItem = m_currentCallUpdates.begin();

            for (int i = 0; i < randomIndex; i ++)
            {
                existingItem ++;
            }
                
            switch (type)
            {
            case NEW_UPDATE:
                {
                    CallDetails update = generateCurrentCallUpdate(0);
                    m_currentCallUpdates.push_back(update);
                    return m_currentCallUpdates.back();
                }
            case UPDATE_UPDATE:        
                {        
                    CallDetails update = generateCurrentCallUpdate(&(*existingItem));
    
                    // Make sure the update retains the properties that make it
                    // unique
                    *existingItem = update;
    
                    return *existingItem;
                }
            case REMOVE_UPDATE:
                {   
                    CallDetails result = *existingItem;
                    result.callStatus = TA_Base_Bus::IRadioCorbaDef::Deleted;

                    // Remove from internal list, so not present in later calls
                    m_currentCallUpdates.erase(existingItem);
                    return result;
                }
            default:
                TA_ASSERT(FALSE, "Unrecognized type");
            }

            TA_THROW(TA_Base_Core::TransactiveException("RadioManagerTestData: Invalid type"));
        }


        const RadioManagerTestData::CallDetails 
        RadioManagerTestData::generateCurrentCallUpdate(const CallDetails* existingItem)
        {
            CallDetails result;
            
            static long callId = 0;
            callId ++;

            result.callID = callId;
            result.sessionRef = m_sessionRef;
                      
            time_t nowTime;
            time(&nowTime);
            enum { BUF_MAX = 256 };
            static char buf[BUF_MAX];
            const std::string format("%d-%b %H:%M:%S");        
            result.time = nowTime;

            strftime(buf, BUF_MAX, format.c_str(), localtime(&nowTime));

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
               
            result.callTime = CORBA::string_dup(buf);
            result.callerName   = CORBA::string_dup(sourceName);
            result.callerTSI    = CORBA::string_dup(sourceTSI);
            result.callerLocation = CORBA::string_dup(sourceLoc);

            result.calledName   = CORBA::string_dup(destName);
            result.calledTSI    = CORBA::string_dup(destTSI);
            result.calledLocation = CORBA::string_dup(destLoc);
            
            // Should always be connected if 'current call'
            result.callStatus = TA_Base_Bus::IRadioCorbaDef::Connected;
            
            int participants = 2 + (rand() % 4);

            if (rand() % 2 == 0)
            {
                // Make into full duplex call
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

                participant.name = CORBA::string_dup(pName);
                participant.TSI = CORBA::string_dup(pTSI);
                participant.location = CORBA::string_dup(pLoc);

                participant.isAcknowledged = false;
                // participant.pttState no state initially
            }

            // Category dependent on participant count
            result.callCategory = result.participants.length() == 0 ? 
                            TA_Base_Bus::IRadioCorbaDef::FullDuplex : 
                            TA_Base_Bus::IRadioCorbaDef::HalfDuplex;       
                            
            result.callSource = rand() % 2 == 0 ? TA_Base_Bus::IRadioCorbaDef::IncomingCall : TA_Base_Bus::IRadioCorbaDef::OutgoingCall;
            result.isEmergency = rand() % 2 == 0;
            
            if (0 != existingItem)
            {
                result.callID = existingItem->callID;
                result.callerTSI = existingItem->callerTSI;
                result.calledTSI = existingItem->calledTSI;
            }

            return result;
        }
        

        RadioManagerTestData::CallDetails* RadioManagerTestData::getCopyCurrentCallDetails(long callID)
        {   
            TA_Base_Core::ThreadGuard guard(m_currentCallLock);

            for (std::list<CallDetails>::const_iterator it = m_currentCallUpdates.begin(); 
                    it != m_currentCallUpdates.end(); ++it )
            {                
                if (it->callID == callID)
                {
                    return new CallDetails(*it);
                }
            }
            
            ASSERT(FALSE);
            return 0;
        }

        const RadioManagerTestData::CallDetails* RadioManagerTestData::getRandomConferenceCall()
        {
            TA_Base_Core::ThreadGuard guard(m_currentCallLock);

            std::vector<const RadioManagerTestData::CallDetails*> callPool;

            for (std::list<CallDetails>::const_iterator it = m_currentCallUpdates.begin(); 
                    it != m_currentCallUpdates.end(); ++it )
            {                
                if (TA_Base_Bus::IRadioCorbaDef::HalfDuplex == it->callCategory)
                {
                    callPool.push_back(&(*it));
                }
            }

            if (callPool.size() > 0)
            {
                return callPool.at(rand() % callPool.size());
            }
            
            // No conference calls found
            return 0;
        }

    }   // end namespace RadioManager  
}   // end namespace TA_IRS_App
