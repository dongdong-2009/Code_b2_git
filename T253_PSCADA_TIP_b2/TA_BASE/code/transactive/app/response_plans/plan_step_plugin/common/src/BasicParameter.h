#ifndef BASICPARAMETER_INCLUDE
#define BASICPARAMETER_INCLUDE

#include <string>

namespace TA_Base_App
{

const char* const ParamStep = "Step";
const char* const ParamName = "Name";
const char* const ParamDesc = "Description";
const char* const ParamSkip = "Skip";
const char* const ParamIgnoreFailure = "IgnoreFailure";
const char* const ParamDelay = "Delay(second)";
const char* const ParamTimeout = "Timeout(second)";

class BasicParameter
{
public:
    BasicParameter();
    BasicParameter( const int type, const unsigned int pkey, const unsigned int id, const unsigned int position,
        const unsigned int delay, const time_t timeout, const std::string name, const std::string description,
        const std::string graph, const bool skip, const bool skippable, const bool ignoreFailure );
    virtual ~BasicParameter();

public:
    int type;
    unsigned int pkey;
    unsigned int id;
    unsigned int position;
    time_t delay;
    time_t timeout;

    std::string name;
    std::string description;
    std::string graph;

    bool skip;
    bool skippable;
    bool ignoreFailure;
};

}

#endif