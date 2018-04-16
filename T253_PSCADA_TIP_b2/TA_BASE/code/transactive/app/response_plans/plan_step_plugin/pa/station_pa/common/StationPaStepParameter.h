#ifndef STATION_PA_STEP_PARAMETER_INCLUDE_H
#define STATION_PA_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
#include "app/response_plans/plan_step_plugin/pa/common/PaCommonDef.h"
#include "bus/response_plans/PlanDataAccess/src/Serialization.h"

namespace TA_Base_Core
{
struct StepDetail;
}

namespace TA_Base_App
{

const char* const ParamLibVersion = "Library Version";
const char* const ParamLibSection = "Library Section";
const char* const ParamMessageId = "Message Id";

const char* const ParamLocationKey = "Location Key";
const char* const ParamStation = "PA Station";
const char* const ParamAllStations = "All Stations";
const char* const ParamZones = "Zones";
const char* const ParamMessage = "Message";
const char* const ParamChime = "Chime";
const char* const ParamCyclic = "Cyclic";
const char* const ParamTISSynch = "TIS Synchronised";
const char* const ParamDuration = "Duration";
const char* const ParamInterval = "Interval (sec)";

class StationPaStepParameter: public BaseStepParameter
{
public:
    StationPaStepParameter(const BasicParameter& detail) : BaseStepParameter(detail),
        locationKey(0),
        station(""),
        allStations(false),
        chime(false),
        cyclic(false),
        TISSynch(false),
        duration(60),
        interval(5u)
    {

    }

    StationPaStepParameter(const unsigned long stepId, const int stepType) : BaseStepParameter(stepId, stepType),
        locationKey(0),
        station(""),
        allStations(false),
        chime(false),
        cyclic(false),
        TISSynch(false),
        duration(60),
        interval(5u)
    {
    }

    ~StationPaStepParameter()
    {
    }

    unsigned long locationKey;
    std::string station;
    bool allStations;

    std::vector<unsigned long> zones;
    PAMessage message;
    bool chime;
    bool cyclic;
    bool TISSynch;
    unsigned long duration;
    unsigned long interval;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(station, ParamStation);
        ioSerialization.serialize(allStations, ParamAllStations);
        ioSerialization.structSerialize(message, ParamMessage);
        ioSerialization.serialize(chime, ParamChime);
        ioSerialization.serialize(cyclic, ParamCyclic);
        ioSerialization.serialize(TISSynch, ParamTISSynch);
        ioSerialization.serialize(duration, ParamDuration);
        ioSerialization.serialize(interval, ParamInterval);
        ioSerialization.serialize(zones, ParamZones);
    }

private:
    //StationPaStepParameter(const StationPaStepParameter&);
    StationPaStepParameter& operator=(const StationPaStepParameter&);
};
}

namespace TA_Base_Bus
{
	TA_SERIALIZATION(TA_Base_App::PAMessage, (libVersion)(libSection)(messageId))
}

#endif //STATION_PA_STEP_PARAMETER_INCLUDE_H
