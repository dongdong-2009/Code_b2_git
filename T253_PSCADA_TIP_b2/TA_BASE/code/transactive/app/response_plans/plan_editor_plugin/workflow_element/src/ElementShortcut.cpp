#include "app/response_plans/plan_editor_plugin/workflow_drawing/src/IDrawingContext.h"
#include "ElementShortcut.h"

ElementShortcut::ElementShortcut( const std::string& strResource, const int nPosX, const int nPosY ) :
PositionX( nPosX ),
PositionY( nPosY ),
m_oEntity( strResource )
{
}

ElementShortcut::~ElementShortcut()
{
}

void ElementShortcut::draw( IDrawingContext& refDrawing )
{
    refDrawing.drawEntity( m_oEntity, PositionX, PositionY );
}

bool ElementShortcut::isTriggered( const int nPosX, const int nPosY )
{
    bool bSelcted = (( right() >= nPosX ) &&( PositionX <= nPosX ) && ( bottom() >= nPosY ) && ( PositionY <= nPosY ));

    return bSelcted;
}

int ElementShortcut::right()
{
    return ( PositionX + m_oEntity.width() );
}

int ElementShortcut::bottom()
{
    return ( PositionY + m_oEntity.height() );
}
