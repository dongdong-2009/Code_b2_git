/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_mft_agent_icdv4/src/RadioResourceConfigFacade.h $
 * @author:  Glen Kidd
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 11:47:25 $
 * Last modified by:  $Author: builder $
 * 
 * MFT Agent facade to underlying Resource Config Protocol implementation.
 * 
 * @note The ResourceConfig class is pretty well a facade in inself, providing an
 * extra level of abstraction doesn't really add any value, in this case. 
 */

#ifndef RADIO_RESOURCE_CONFIG_FACADE_H_INCLUDED
#define RADIO_RESOURCE_CONFIG_FACADE_H_INCLUDED

#include "ace/Singleton.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/MDB.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/IMcdbObserver.h"
#include "app/radio/radio_mft_agent_icdv4/src/IMcdbRefreshCallback.h"
//class RadioMFTAgentEventImpl;

class RadioResourceConfigFacade : public IMcdbObserver
{
public:
	RadioResourceConfigFacade();
	virtual ~RadioResourceConfigFacade();

    void setEventImpl(ICDV4::IMcdbRefreshCallback* pImpl);
    void startRetrieveDB();
void setIAPIRequest_ptr(MCC7500SDK::IAPIRequest_ptr &ptr);
    bool getRadioResource(std::vector<RadioResource>& radioResource);

    /**
     * IMcdbObserver implementation
     */
    virtual void notify();
    
private:
    //Disabled Methods
    RadioResourceConfigFacade(const RadioResourceConfigFacade&);
    RadioResourceConfigFacade& operator=(const RadioResourceConfigFacade&);

private:
    MDB *m_MCDB;

	ICDV4::IMcdbRefreshCallback* m_eventImpl;

	bool m_retrievedDatabase;
};

typedef ACE_Singleton<RadioResourceConfigFacade, ACE_Null_Mutex> The_ResourceConfig;

#endif 
