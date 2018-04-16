#include "app/response_plans/plan_editor_plugin/workflow_drawing/src/IDrawingContext.h"
#include "app/response_plans/plan_manager/src/EventEnd.h"

namespace TA_Base_App
{
    
const std::string EventEnd::Name = "End";
const std::string EventEnd::DisplayName = "End";
const std::string EventEnd::Resource = "res/PlanActivityEnd.bmp";
const std::string ActivityIconResource = "res/PlanActivityIconEnd.bmp";

EventEnd::EventEnd() : BitmapActivity( ActivityIconResource, false, false ),
m_nRadius( m_oEntity.height() / 2 )
{
    Editable = false;
}

EventEnd::~EventEnd()
{
}

bool EventEnd::hitTest(const int nPosX, const int nPosY)
{
    bool bHit = false;

    int nDistance = ( nPosX - ( PositionX + m_nRadius )) * ( nPosX - ( PositionX + m_nRadius )) + ( nPosY - ( PositionY + m_nRadius )) * ( nPosY - ( PositionY + m_nRadius ));

    bHit = nDistance < ( m_nRadius * m_nRadius );

    return bHit;
}

bool EventEnd::intersect(const int nLeft, const int nTop, const int nRight, const int nBottom)
{
    bool bInterected = false;

    if (( PositionX <= nRight ) && (( PositionX + 2 * m_nRadius ) >= nLeft ) && ( PositionY <= nBottom ) && (( PositionY + 2 * m_nRadius ) >= nTop ))
    {
        bInterected = true;
    }

    return bInterected;
}

void EventEnd::serializeAddons( Stringification& ioSerilization )
{
}

}