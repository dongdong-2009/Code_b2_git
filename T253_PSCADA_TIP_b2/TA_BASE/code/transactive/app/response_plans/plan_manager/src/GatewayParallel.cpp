#include "GatewayParallel.h"

namespace TA_Base_App
{

const std::string GatewayParallel::Name = "Parallel Gateway";
const std::string GatewayParallel::DisplayName = "Parallel";
const std::string GatewayParallel::Resource = "res/PlanActivityParallelGateway.bmp";
const std::string ActivityIconResource = "res/PlanActivityIconParallelGateway.bmp";

GatewayParallel::GatewayParallel() : BitmapActivity( ActivityIconResource, false )
{
}

GatewayParallel::~GatewayParallel()
{
}

bool GatewayParallel::hitTest(const int nPosX, const int nPosY)
{
    // Overwrite this function when necessary
    return BitmapActivity::hitTest( nPosX, nPosY );
}

bool GatewayParallel::intersect(const int nLeft, const int nTop, const int nRight, const int nBottom)
{
    // Overwrite this function when necessary
    return BitmapActivity::intersect( nLeft, nTop, nRight, nBottom );
}

void GatewayParallel::serializeAddons( Stringification& ioSerilization )
{
}

}

