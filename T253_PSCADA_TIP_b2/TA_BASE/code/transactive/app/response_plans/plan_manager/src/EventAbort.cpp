#include "app/response_plans/plan_manager/src/EventAbort.h"

namespace TA_Base_App
{

const std::string EventAbort::Name = "Abort";
const std::string EventAbort::DisplayName = "Abort";
const std::string EventAbort::Resource = "res/PlanActivityAbort.bmp";
const std::string ActivityIconResource = "res/PlanActivityIconAbort.bmp";

EventAbort::EventAbort() : BitmapActivity( ActivityIconResource, false )
{
}

EventAbort::~EventAbort()
{
}

void EventAbort::serializeAddons( Stringification& ioSerilization )
{
}

}
