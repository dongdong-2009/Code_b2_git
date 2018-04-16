#pragma once
#pragma warning(disable:4786)
#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

namespace TA_Base_Bus
{
    class GenericAgent;
}

struct DummyAgent : public virtual TA_Base_Bus::IGenericAgentUser
{
    DummyAgent(int argc, char* argv[]);

    virtual void checkOperationMode() {}
    virtual void agentTerminate();
    virtual bool createAllEntities(TA_Base_Core::IEntityDataPtr, const TA_Base_Core::SharedIEntityDataList&, TA_Base_Bus::IEntityList&);
    virtual void entityCreationComplete();
    virtual TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr EntityData);
    virtual void agentSetMonitor();
    virtual void agentSetControl();
    virtual void notifyGroupOffline(const std::string& group);
    virtual void notifyGroupOnline(const std::string& group);
    virtual void registerForStateUpdates();
    virtual void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message);
    virtual void processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);
    virtual void run();

    TA_Base_Bus::GenericAgent* m_genericAgent;
};
