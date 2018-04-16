#include "app/response_plans/plan_editor_plugin/workflow_drawing/src/IDrawingContext.h"
#include "app/response_plans/plan_manager/src/EventStart.h"

namespace TA_Base_App
{

const std::string EventStart::Name = "Start";
const std::string EventStart::DisplayName = "Start";
const std::string EventStart::Resource = "res/PlanActivityStart.bmp";
const std::string ActivityIconResource = "res/PlanActivityIconStart.bmp";

EventStart::EventStart() : BitmapActivity( ActivityIconResource, false, true ),
m_nRadius( m_oEntity.height() / 2 )
{
    Editable = false;
}

EventStart::~EventStart()
{
}

bool EventStart::hitTest(const int nPosX, const int nPosY)
{
    bool bHit = false;

    int nDistance = ( nPosX - ( PositionX + m_nRadius )) * ( nPosX - ( PositionX + m_nRadius )) + ( nPosY - ( PositionY + m_nRadius )) * ( nPosY - ( PositionY + m_nRadius ));

    bHit = nDistance < ( m_nRadius * m_nRadius );

    return bHit;
}

bool EventStart::intersect(const int nLeft, const int nTop, const int nRight, const int nBottom)
{
    bool bInterected = false;

    if (( PositionX <= nRight ) && (( PositionX + 2 * m_nRadius ) >= nLeft ) && ( PositionY <= nBottom ) && (( PositionY + 2 * m_nRadius ) >= nTop ))
    {
        bInterected = true;
    }

    return bInterected;
}

void EventStart::serializeAddons( Stringification& ioSerilization )
{
}

}

