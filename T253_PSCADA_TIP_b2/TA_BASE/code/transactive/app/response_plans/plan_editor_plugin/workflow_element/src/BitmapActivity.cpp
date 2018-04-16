#include "app/response_plans/plan_editor_plugin/workflow_drawing/src/IDrawingContext.h"

#include "Stringification.h"
#include "BitmapActivity.h"

const int InstanceTextOffset = 5;
const int InstanceTextSize = 10;

const std::string ShortcutResFile = "res/PlanShortcutArrow.bmp";

BitmapActivity::BitmapActivity( const std::string& strRes, const bool bHasInstanceName, const bool bHasFlowShortcut ) : WorkflowActivity(),
m_oEntity( strRes ),
m_bHasInstanceName( bHasInstanceName ),
m_nWidth(m_oEntity.width()),
m_nHeight(m_oEntity.height())
{
    if ( bHasFlowShortcut )
    {
        std::vector<int> vecElements;
        attachShortcut(ShortcutResFile, vecElements);
    }
}

BitmapActivity::~BitmapActivity()
{
}

void BitmapActivity::draw( IDrawingContext& refDrawing )
{
    refDrawing.drawEntity( m_oEntity, PositionX, PositionY );

    if ( m_bHasInstanceName )
    {
        refDrawing.selectFont( InstanceTextSize, false );
        refDrawing.selectTransparentFilling();
        refDrawing.drawText( InstanceName, FrameLeft, FrameBottom + InstanceTextOffset );
        refDrawing.restoreFillingColor();
        refDrawing.restoreFont();
    }
}

void BitmapActivity::selectPosition( const int nPosX, const int nPosY )
{
    Selected = true;
}

void BitmapActivity::deselect()
{
    Selected = false;
}

bool BitmapActivity::multiSelect( const int nLeft, const int nTop, const int nRight, const int nBottom )
{
    bool bOldSelcted = Selected;

    return ( Selected != bOldSelcted );
}

void BitmapActivity::repositionFrame()
{
    FrameLeft = PositionX;
    FrameTop = PositionY;
    FrameRight = PositionX + m_nWidth;
    FrameBottom = PositionY + m_nHeight;
}

void BitmapActivity::getDockPosition( int& nPosX, int& nPosY )
{
    nPosX = PositionX + m_nWidth / 2;
    nPosY = PositionY + m_nHeight / 2;
}

bool BitmapActivity::dockedToPosition( const unsigned int unId, const int nPosX, const int nPosY, const int nLeft, const int nTop, const int nRight, const int nBottom )
{
    return false;
}

void BitmapActivity::dockingMoved( const unsigned int unId, const int nPosX, const int nPosY, const int nLeft, const int nTop, const int nRight, const int nBottom )
{

}

void BitmapActivity::getMovingPosition( int& nPosX, int& nPosY )
{

}

bool BitmapActivity::isDockable(const int nPosX, const int nPosY)
{
    return hitTest( nPosX, nPosY );
}

bool BitmapActivity::cancelDocking(const unsigned int unId)
{
    return false;
}

bool BitmapActivity::isHover(const int nPosX, const int nPosY)
{
    return hitTest( nPosX, nPosY );
}

void BitmapActivity::deleteDocking( const unsigned int unId )
{

}

bool BitmapActivity::hitTest( const int nPosX, const int nPosY )
{
    bool bHit = (( nPosX >= PositionX ) && ( nPosX <= ( PositionX + m_nWidth )) && ( nPosY >= PositionY ) && ( nPosY <= ( PositionY + m_nHeight )));

    return bHit;
}

bool BitmapActivity::intersect( const int nLeft, const int nTop, const int nRight, const int nBottom )
{
    bool bInterected = false;

    if (( PositionX <= nRight ) && (( PositionX + m_nWidth ) >= nLeft ) &&( PositionY <= nBottom ) && (( PositionY + m_nHeight ) >= nTop ))
    {
        bInterected = true;
    }

    return bInterected;
}
