#include "app/response_plans/plan_manager/src/GatewayInclusive.h"

namespace TA_Base_App
{

const std::string GatewayInclusive::Name = "Inclusive Gateway";
const std::string GatewayInclusive::DisplayName = "Inclusive";
const std::string GatewayInclusive::Resource = "res/PlanActivityInclusiveGateway.bmp";
const std::string ActivityIconResource = "res/PlanActivityIconInclusiveGateway.bmp";

GatewayInclusive::GatewayInclusive() : BitmapActivity( ActivityIconResource, false )
{
}

GatewayInclusive::~GatewayInclusive()
{
}

bool GatewayInclusive::hitTest(const int nPosX, const int nPosY)
{
    // Overwrite this function when necessary
    return BitmapActivity::hitTest( nPosX, nPosY );
}

bool GatewayInclusive::intersect(const int nLeft, const int nTop, const int nRight, const int nBottom)
{
    // Overwrite this function when necessary
    return BitmapActivity::intersect( nLeft, nTop, nRight, nBottom );
}

void GatewayInclusive::serializeAddons( Stringification& ioSerilization )
{
}

}
