#pragma once

#include <string>

struct ActivityData
{
    unsigned int Type;
    std::string Graph;
    std::string InstanceName;
};

struct SequenceFlowData
{
    unsigned int Incoming;
    unsigned int Outgoing;
    std::string Condition;
    std::string Graph;
};

