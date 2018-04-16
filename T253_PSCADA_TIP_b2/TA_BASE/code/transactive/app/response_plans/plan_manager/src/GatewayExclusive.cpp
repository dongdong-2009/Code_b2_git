#include "app/response_plans/plan_manager/src/GatewayExclusive.h"

namespace TA_Base_App
{

const std::string GatewayExclusive::Name = "Exclusive Gateway";
const std::string GatewayExclusive::DisplayName = "Exclusive";
const std::string GatewayExclusive::Resource = "res/PlanActivityExclusiveGateway.bmp";
const std::string ActivityIconResource = "res/PlanActivityIconExclusiveGateway.bmp";

GatewayExclusive::GatewayExclusive() : BitmapActivity( ActivityIconResource, false )
{
}

GatewayExclusive::~GatewayExclusive()
{
}

void GatewayExclusive::serializeAddons( Stringification& ioSerilization )
{
}

bool GatewayExclusive::hitTest(const int nPosX, const int nPosY)
{
    // Overwrite this function when necessary
    return BitmapActivity::hitTest( nPosX, nPosY );
}

bool GatewayExclusive::intersect(const int nLeft, const int nTop, const int nRight, const int nBottom)
{
    // Overwrite this function when necessary
    return BitmapActivity::intersect( nLeft, nTop, nRight, nBottom );
}

}
