#ifndef STATION_TIS_STEP_PARAMETER_INCLUDE_H
#define STATION_TIS_STEP_PARAMETER_INCLUDE_H

#include <string>
#include <vector>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
#include "app/response_plans/plan_step_plugin/tis/common/TisCommonDef.h"
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"


namespace TA_Base_App
{
const char* const ParamLibVersion = "Library Version";
const char* const ParamLibSection = "Library Section";
const char* const ParamMessageId = "Message Id";

const char* const ParamLocationKey = "Location Key";
const char* const ParamStation = "Station";
const char* const ParamAllStations = "All Stations";
const char* const ParamStationLevel = "Station Level";
const char* const ParamStationPids = "PID List";
const char* const ParamMessage = "Message";
const char* const ParamPriority = "Priority";
const char* const ParamDuration = "Duration";

class  StationTisStepParameter: public BaseStepParameter
{
public:
    StationTisStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail),
          locationKey(0),
		  station(""),
          allStations(false),
          priority(0),
          duration(60)
    {
    }

    StationTisStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType),
          locationKey(0),
		  station(""),
		  allStations(false),
          priority(0),
          duration(60)
    {
    }

    virtual ~StationTisStepParameter()
    {

    }

    unsigned long locationKey;
    std::string station;
    bool allStations;

    std::vector<std::string> stationPids;
    TISMessage message;
    unsigned short priority;
    unsigned long duration;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        //ioSerialization.serialize(locationKey, ParamLocationKey);
        ioSerialization.serialize(station, ParamStation);
        ioSerialization.serialize(allStations, ParamAllStations);
        ioSerialization.structSerialize(message, ParamMessage);
        ioSerialization.serialize(priority, ParamPriority);
        ioSerialization.serialize(duration, ParamDuration);
        ioSerialization.serialize(stationPids, ParamStationPids);

    }

private:
    //StationTisStepParameter(const StationTisStepParameter&);
    StationTisStepParameter& operator=(const StationTisStepParameter&);
};
}

namespace TA_Base_Bus
{
	TA_SERIALIZATION(TA_Base_App::TISMessage, (libraryVersion)(librarySection)(messageId))
}

#endif //STATION_TIS_STEP_PARAMETER_INCLUDE_H
