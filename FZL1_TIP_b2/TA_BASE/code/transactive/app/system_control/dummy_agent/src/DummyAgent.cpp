#pragma warning(disable:4786 4503)
#include "StdAfx.h"
#include "DummyAgent.h"
#include "test/TestNaming.h"
#include "bus/generic_agent/src/IEntity.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "core/utilities/src/RunParams.h"

using namespace TA_Base_Core;

DummyAgent::DummyAgent(int argc, char* argv[])
    : m_genericAgent(NULL)
{
    m_genericAgent = new TA_Base_Bus::GenericAgent(argc, argv, this);
}

void DummyAgent::agentTerminate()
{
    if (RunParams::getInstance().isSet("TerminateTimeout"))
    {
        boost::this_thread::sleep_for(boost::chrono::seconds(getRunParamValue("TerminateTimeout", 0)));
    }

    ::exit(0);
}

bool DummyAgent::createAllEntities(TA_Base_Core::IEntityDataPtr, const TA_Base_Core::SharedIEntityDataList&, TA_Base_Bus::IEntityList&)
{
    return true;
}

void DummyAgent::entityCreationComplete()
{
}

TA_Base_Bus::IEntity* DummyAgent::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
{
    return NULL;
}

void DummyAgent::agentSetMonitor()
{
    if (RunParams::getInstance().isSet("MonitorTimeout"))
    {
        boost::this_thread::sleep_for(boost::chrono::seconds(getRunParamValue("MonitorTimeout", 0)));
    }
}

void DummyAgent::agentSetControl()
{
    if (RunParams::getInstance().isSet("ControlTimeout"))
    {
        boost::this_thread::sleep_for(boost::chrono::seconds(getRunParamValue("ControlTimeout", 0)));
    }
}

void DummyAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
{
}

void DummyAgent::run()
{
    //TestNaming t;

    if (RunParams::getInstance().isSet("StartTimeout"))
    {
        boost::this_thread::sleep_for(boost::chrono::seconds(getRunParamValue("StartTimeout", 0)));
    }

    m_genericAgent->run();
}

void DummyAgent::notifyGroupOffline(const std::string& group)
{
}

void DummyAgent::notifyGroupOnline(const std::string& group)
{
}

void DummyAgent::registerForStateUpdates()
{
}

void DummyAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
{
}
