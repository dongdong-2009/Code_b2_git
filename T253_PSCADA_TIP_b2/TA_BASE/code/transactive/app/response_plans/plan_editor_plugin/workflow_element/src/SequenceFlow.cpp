
#include "app/response_plans/plan_editor_plugin/workflow_drawing/src/IDrawingContext.h"
#include "app/response_plans/plan_editor_plugin/workflow_drawing/src/Geometry.h"
#include "Stringification.h"
#include "SequenceFlow.h"

const int DefaultWidth = 70;
const int DefaultHeight = 30;
const int SelectedFrameOffset = 4;
const int ShapeWidth = 3;
const int SelectPointVisibleRadius = 4;
const int SelectPointValidRadius = 6;
const int SelectedFrameWidth = 2;
const int RoundRectX = 10;
const int RoundRectY = 10;
const unsigned int ShapeColor = 0x000000u;
const unsigned int SelectPointColor = 0x00FF00u;
const unsigned int SelectionColor = 0xFF0000u;
const unsigned int SelectedFrameColor = 0xACACACu;

SequenceFlow::SequenceFlow() : WorkflowElement(),
IncomingId( InvalidElementId ),
OutgoingId( InvalidElementId ),
Condition( "" ),
m_nSrcPosX( 0 ),
m_nSrcPosY( 0 ),
m_nArrowPosX( 0 ),
m_nArrowPosY( 0 ),
m_nJointPosX( 0 ),
m_nJointPosY( 0 ),
m_bAutoTunning( false ),
m_bJointValid( false ),
m_bArrowSelected( false ),
m_bSrcSelected( false ),
m_bJointSelected( false )
{
    Type = 1;
    Interactive = true;
}

SequenceFlow::~SequenceFlow()
{
}

void SequenceFlow::draw( IDrawingContext& refDrawing )
{
    refDrawing.selectLineStyle( ShapeColor, ShapeWidth, true );
    refDrawing.selectFillingColor( ShapeColor );

    if ( m_bJointValid )
    {
        refDrawing.drawLine( m_nSrcPosX, m_nSrcPosY, m_nJointPosX, m_nJointPosY );
        refDrawing.drawLine( m_nJointPosX, m_nJointPosY, m_nArrowPosX, m_nArrowPosY );
    }
    else
    {
        refDrawing.drawLine( m_nSrcPosX, m_nSrcPosY, m_nArrowPosX, m_nArrowPosY );
    }

    refDrawing.drawTriangle( m_nArrowPosX, m_nArrowPosY, (m_bJointValid ? m_nJointPosX : m_nSrcPosX), (m_bJointValid ? m_nJointPosY : m_nSrcPosY), 20.0, 3.0 );

    refDrawing.selectTransparentLine();

    //if ( Selected )
    {
        if ( m_bJointValid )
        {
            refDrawing.selectFillingColor( m_bJointSelected ? SelectionColor : SelectPointColor );
            refDrawing.drawEllipse( m_nJointPosX, m_nJointPosY, SelectPointVisibleRadius );
        }

        refDrawing.selectFillingColor( m_bSrcSelected ? SelectionColor : SelectPointColor );
        refDrawing.drawEllipse(m_nSrcPosX, m_nSrcPosY, SelectPointVisibleRadius);

        refDrawing.selectFillingColor( m_bArrowSelected ? SelectionColor : SelectPointColor );
        refDrawing.drawEllipse( m_nArrowPosX, m_nArrowPosY, SelectPointVisibleRadius );
    }

    refDrawing.restoreLineStyle();
    refDrawing.restoreFillingColor();
}

void SequenceFlow::selectPosition( const int nPosX, const int nPosY )
{
    Selected = true;

    int unCompareSize = SelectPointValidRadius * SelectPointValidRadius;

    // Check arrow first
    if ((( m_nArrowPosX - nPosX ) * ( m_nArrowPosX - nPosX ) + ( m_nArrowPosY - nPosY ) * ( m_nArrowPosY - nPosY )) <= unCompareSize )
    {
        m_bJointSelected = false;
        m_bArrowSelected = true;
        m_bSrcSelected = false;
        Selected = m_bArrowSelected;
    }
    // Then check source point
    else if ((( m_nSrcPosX - nPosX ) * ( m_nSrcPosX - nPosX ) + ( m_nSrcPosY - nPosY ) * ( m_nSrcPosY - nPosY )) <= unCompareSize )
    {
        m_bJointSelected = false;
        m_bArrowSelected = false;
        m_bSrcSelected = true;
        Selected = m_bArrowSelected;
    }
    // Finally check tunning point
    else if ( m_bJointValid && ((( m_nJointPosX - nPosX ) * ( m_nJointPosX - nPosX ) + ( m_nJointPosY - nPosY ) * ( m_nJointPosY - nPosY )) <= unCompareSize ))
    {
        m_bJointSelected = true;
        m_bArrowSelected = false;
        m_bSrcSelected = false;
    }
    else
    {
        m_bJointSelected = false;
        m_bArrowSelected = false;
        m_bSrcSelected = false;
    }
}

void SequenceFlow::deselect()
{
    Selected = false;
    m_bJointSelected = false;
    m_bArrowSelected = false;
    m_bSrcSelected = false;
}

bool SequenceFlow::multiSelect( const int unLeft, const int unTop, const int unRight, const int unBottom )
{
    bool bOldSelcted = Selected;

    return ( Selected != bOldSelcted );
}

void SequenceFlow::repositionFrame()
{
    FrameLeft = m_bJointValid ? __min( __min( m_nSrcPosX, m_nArrowPosX ), m_nJointPosX ) : __min( m_nSrcPosX, m_nArrowPosX );
    FrameTop = __min( __min( m_nSrcPosY, m_nArrowPosY ), m_nJointPosY );
    FrameRight = m_bJointValid ? __max( __max( m_nSrcPosX, m_nArrowPosX ), m_nJointPosX ) : __max( m_nSrcPosX, m_nArrowPosX );
    FrameBottom = __max( __max( m_nSrcPosY, m_nArrowPosY ), m_nJointPosY );
}

void SequenceFlow::init( const int nPositionX, const int nPositionY )
{
    m_nSrcPosX = nPositionX;
    m_nSrcPosY = nPositionY;
    m_nJointPosX = nPositionX;
    m_nJointPosY = nPositionY;
    m_nArrowPosX = nPositionX;
    m_nArrowPosY = nPositionY;

    PositionX = nPositionX;
    PositionY = nPositionY;
}

void SequenceFlow::getDockPosition( int& nPosX, int& nPosY )
{
    // Always dock to arrow
    nPosX = m_nArrowPosX;
    nPosY = m_nArrowPosY;
}

void SequenceFlow::updateIncoming( const unsigned int unId, const int nPosX, const int nPosY, const int nLeft, const int nTop, const int nRight, const int nBottom )
{
    IncomingId = unId;

    m_nIncomingX = nPosX;
    m_nIncomingY = nPosY;
    m_nIncomingLeft = nLeft;
    m_nIncomingTop = nTop;
    m_nIncomingRight = nRight;
    m_nIncomingBottom = nBottom;
}

void SequenceFlow::updateOutgoing( const unsigned int unId, const int nPosX, const int nPosY, const int nLeft, const int nTop, const int nRight, const int nBottom )
{
    OutgoingId = unId;

    m_nOutgoingX = nPosX;
    m_nOutgoingY = nPosY;
    m_nOutgoingLeft = nLeft;
    m_nOutgoingTop = nTop;
    m_nOutgoingRight = nRight;
    m_nOutgoingBottom = nBottom;
}

int SequenceFlow::dockedToPosition( const unsigned int unId, const int nPosX, const int nPosY, const int nLeft, const int nTop, const int nRight, const int nBottom )
{
    int nSuccess = eNoChange;

    if ( m_bSrcSelected && ( unId != IncomingId ) && ( unId != OutgoingId ))
    {
        IncomingId = unId;

        m_nIncomingX = nPosX;
        m_nIncomingY = nPosY;
        m_nIncomingLeft = nLeft;
        m_nIncomingTop = nTop;
        m_nIncomingRight = nRight;
        m_nIncomingBottom = nBottom;

        m_bAutoTunning = (InvalidElementId != OutgoingId);

        repositionAutoJoint();

        m_nSrcPosX = nPosX;
        m_nSrcPosY = nPosY;
        repositionDockedSrc();

        repositionDockedArrow();

        nSuccess = eIncomingUpdated;
    }
    else if ( m_bArrowSelected && ( unId != IncomingId ) && ( unId != OutgoingId ))
    {
        OutgoingId = unId;

        m_nOutgoingX = nPosX;
        m_nOutgoingY = nPosY;
        m_nOutgoingLeft = nLeft;
        m_nOutgoingTop = nTop;
        m_nOutgoingRight = nRight;
        m_nOutgoingBottom = nBottom;

        m_bAutoTunning = (InvalidElementId != IncomingId);

        repositionAutoJoint();

        m_nArrowPosX = nPosX;
        m_nArrowPosY = nPosY;
        repositionDockedArrow();

        repositionDockedSrc();

        nSuccess = eOutgoingUpdated;
    }
    else if ( !m_bArrowSelected && !m_bSrcSelected )
    {
        // Must be created by shortcut, auto set to arrow selected and source position to dock position
        if ( InvalidElementId == IncomingId )
        {
            m_nJointPosX = nPosX;
            m_nJointPosY = nPosY;

            IncomingId = unId;

            m_nIncomingX = nPosX;
            m_nIncomingY = nPosY;
            m_nIncomingLeft = nLeft;
            m_nIncomingTop = nTop;
            m_nIncomingRight = nRight;
            m_nIncomingBottom = nBottom;

            m_nSrcPosX = nPosX;
            m_nSrcPosY = nPosY;

            nSuccess = eIncomingUpdated;
        }
        else if ( InvalidElementId == OutgoingId )
        {
            m_nJointPosX = nPosX;
            m_nJointPosY = nPosY;

            OutgoingId = unId;

            m_nOutgoingX = nPosX;
            m_nOutgoingY = nPosY;
            m_nOutgoingLeft = nLeft;
            m_nOutgoingTop = nTop;
            m_nOutgoingRight = nRight;
            m_nOutgoingBottom = nBottom;

            m_bAutoTunning = true;

            repositionAutoJoint();

            m_nArrowPosX = nPosX;
            m_nArrowPosY = nPosY;
            repositionDockedSrc();
            repositionDockedArrow();

            nSuccess = eOutgoingUpdated;
        }
    }
    else
    {
        nSuccess = eNoChange;
    }

    if ( eNoChange != nSuccess )
    {
        repositionFrame();
    }

    return nSuccess;
}

void SequenceFlow::dockingMoved( const unsigned int unId, const int nPosX, const int nPosY, const int nLeft, const int nTop, const int nRight, const int nBottom )
{
    if ( unId == OutgoingId )
    {
        repositionAutoJoint();

        m_nOutgoingX = nPosX;
        m_nOutgoingY = nPosY;
        m_nOutgoingLeft = nLeft;
        m_nOutgoingRight = nRight;
        m_nOutgoingTop = nTop;
        m_nOutgoingBottom = nBottom;

        m_nArrowPosX = nPosX;
        m_nArrowPosY = nPosY;
        repositionDockedArrow();

        repositionDockedSrc();
    }
    else if ( unId == IncomingId )
    {
        repositionAutoJoint();

        m_nIncomingX = nPosX;
        m_nIncomingY = nPosY;
        m_nIncomingLeft = nLeft;
        m_nIncomingRight = nRight;
        m_nIncomingTop = nTop;
        m_nIncomingBottom = nBottom;

        m_nSrcPosX = nPosX;
        m_nSrcPosY = nPosY;
        repositionDockedSrc();

        repositionDockedArrow();
    }

    repositionFrame();
}

void SequenceFlow::getMovingPosition( int& nPosX, int& nPosY )
{
    if (m_bArrowSelected)
    {
        nPosX = m_nArrowPosX;
        nPosY = m_nArrowPosY;
    }
    else if (m_bSrcSelected)
    {
        nPosX = m_nSrcPosX;
        nPosY = m_nSrcPosY;
    }
}

bool SequenceFlow::isDockable(const int nPosX, const int nPosY)
{
    return false;
}

int SequenceFlow::cancelDocking(const unsigned int unId)
{
    int nCanceled = eNoChange;

    if ( m_bArrowSelected && ( OutgoingId == unId ))
    {
        OutgoingId = InvalidElementId;
        nCanceled = eOutgoingUpdated;
    }
    else if ( m_bSrcSelected && ( IncomingId == unId ))
    {
        IncomingId = InvalidElementId;
        nCanceled = eIncomingUpdated;
    }
    else
    {
        nCanceled = eNoChange;
    }

    return nCanceled;
}

int SequenceFlow::dockedTest( const unsigned int unId )
{
    int nDocked = eNoChange;

    if ( m_bArrowSelected && ( OutgoingId == unId ))
    {
        nDocked = eOutgoingUpdated;
    }
    else if ( m_bSrcSelected && ( IncomingId == unId ))
    {
        nDocked = eIncomingUpdated;
    }
    else
    {
        nDocked = eNoChange;
    }

    return nDocked;
}

bool SequenceFlow::isHover( const int nPosX, const int nPosY )
{
    return false;
}

int SequenceFlow::deleteDocking( const unsigned int unId )
{
    int nCanceled = eNoChange;

    if ( OutgoingId == unId )
    {
        OutgoingId = InvalidElementId;
        nCanceled = eOutgoingUpdated;
    }
    else if ( IncomingId == unId )
    {
        IncomingId = InvalidElementId;
        nCanceled = eIncomingUpdated;
    }

    return nCanceled;
}

bool SequenceFlow::hitTest( const int nPosX, const int nPosY )
{
    bool bHit = false;

    int unCompareSize = SelectPointValidRadius * SelectPointValidRadius;

    // Check arrow first
    if ((( m_nArrowPosX - nPosX ) * ( m_nArrowPosX - nPosX ) + ( m_nArrowPosY - nPosY ) * ( m_nArrowPosY - nPosY )) <= unCompareSize )
    {
        bHit = true;
    }
    // Then check source point
    else if ((( m_nSrcPosX - nPosX ) * ( m_nSrcPosX - nPosX ) + ( m_nSrcPosY - nPosY ) * ( m_nSrcPosY - nPosY )) <= unCompareSize )
    {
        bHit = true;
    }
    else if ( m_bJointValid )
    {
        // And check tunning point
        if ((( m_nJointPosX - nPosX ) * ( m_nJointPosX - nPosX ) + ( m_nJointPosY - nPosY ) * ( m_nJointPosY - nPosY )) <= unCompareSize )
        {
            bHit = true;
        }
        // Check line
        else if ((( nPosX <= m_nSrcPosX && nPosX >= m_nJointPosX ) || ( nPosX >= m_nSrcPosX && nPosX <= m_nJointPosX )) &&
            (( nPosY <= m_nSrcPosY && nPosY >= m_nJointPosY ) || ( nPosY >= m_nSrcPosY && nPosY <= m_nJointPosY )))
        {
            //bHit = Geometry::pointOnLine( nPosX, nPosY, m_nJointPosX, m_nJointPosY, m_nSrcPosX, m_nSrcPosY, 0.1f );
        }
        // Check line
        else if ((( nPosX <= m_nArrowPosX && nPosX >= m_nJointPosX ) || ( nPosX >= m_nArrowPosX && nPosX <= m_nJointPosX )) &&
            (( nPosY <= m_nArrowPosY && nPosY >= m_nJointPosY ) || ( nPosY >= m_nArrowPosY && nPosY <= m_nJointPosY )))
        {
            //bHit = Geometry::pointOnLine( nPosX, nPosY, m_nJointPosX, m_nJointPosY, m_nArrowPosX, m_nArrowPosY, 0.1f );
        }
    }
    else
    {
        // Check line
        if ((( nPosX <= m_nSrcPosX && nPosX >= m_nArrowPosX ) || ( nPosX >= m_nSrcPosX && nPosX <= m_nArrowPosX )) &&
            (( nPosY <= m_nSrcPosY && nPosY >= m_nArrowPosY ) || ( nPosY >= m_nSrcPosY && nPosY <= m_nArrowPosY )))
        {
            //bHit = Geometry::pointOnLine( nPosX, nPosY, m_nArrowPosX, m_nArrowPosY, m_nArrowPosX, m_nArrowPosY, 0.1f );
        }
    }

    return bHit;
}

bool SequenceFlow::intersect( const int nLeft, const int nTop, const int nRight, const int nBottom )
{
    bool bIntersected = false;

    if ( m_bJointValid )
    {
        bIntersected |= Geometry::lineRectangleInsided( m_nSrcPosX, m_nSrcPosY, m_nJointPosX, m_nJointPosY, nLeft, nTop, nRight, nBottom );
        bIntersected |= Geometry::lineRectangleInsided( m_nArrowPosX, m_nArrowPosY, m_nJointPosX, m_nJointPosY, nLeft, nTop, nRight, nBottom );
    }
    else
    {
        bIntersected |= Geometry::lineRectangleInsided( m_nSrcPosX, m_nSrcPosY, m_nArrowPosX, m_nArrowPosY, nLeft, nTop, nRight, nBottom );
    }

    return bIntersected;
}

void SequenceFlow::drawEffect( IDrawingContext& refDrawing )
{
}

void SequenceFlow::refresh( IDrawingContext& refDrawing )
{
    draw( refDrawing );

    drawEffect( refDrawing );
}

void SequenceFlow::serializeBasic( Stringification& ioSerialization )
{
    ioSerialization.serialize( IncomingId );
    ioSerialization.serialize( OutgoingId );
    ioSerialization.serialize( m_nSrcPosX );
    ioSerialization.serialize( m_nSrcPosY );
    ioSerialization.serialize( m_nArrowPosX );
    ioSerialization.serialize( m_nArrowPosY );
    ioSerialization.serialize( m_nJointPosX );
    ioSerialization.serialize( m_nJointPosY );
    ioSerialization.serialize( m_bAutoTunning );
    ioSerialization.serialize( m_bJointValid );
    ioSerialization.serialize( m_nIncomingX );
    ioSerialization.serialize( m_nIncomingY );
    ioSerialization.serialize( m_nIncomingLeft );
    ioSerialization.serialize( m_nIncomingTop );
    ioSerialization.serialize( m_nIncomingRight );
    ioSerialization.serialize( m_nIncomingBottom );
    ioSerialization.serialize( m_nOutgoingX );
    ioSerialization.serialize( m_nOutgoingY );
    ioSerialization.serialize( m_nOutgoingLeft );
    ioSerialization.serialize( m_nOutgoingTop );
    ioSerialization.serialize( m_nOutgoingRight );
    ioSerialization.serialize( m_nOutgoingBottom );
}

void SequenceFlow::move( const bool bMultiMove, const int nPosX, const int nPosY )
{
    if ( bMultiMove || ( Selected && !m_bSrcSelected && !m_bArrowSelected && !m_bJointSelected ))
    {
        m_nSrcPosX += (nPosX - PositionX);
        m_nSrcPosY += (nPosY - PositionY);
        m_nJointPosX += (nPosX - PositionX);
        m_nJointPosY += (nPosY - PositionY);
        m_nArrowPosX += (nPosX - PositionX);
        m_nArrowPosY += (nPosY - PositionY);
    }
    else
    {
        if ( m_bSrcSelected )
        {
            m_nSrcPosX += (nPosX - PositionX);
            m_nSrcPosY += (nPosY - PositionY);

            if ( !m_bJointValid )
            {
                repositionDockedArrow();
            }
        }

        if ( m_bJointSelected )
        {
            m_nJointPosX += (nPosX - PositionX);
            m_nJointPosY += (nPosY - PositionY);

            m_bAutoTunning = false;

            repositionDockedSrc();
            repositionDockedArrow();
        }

        if ( m_bArrowSelected )
        {
            m_nArrowPosX += (nPosX - PositionX);
            m_nArrowPosY += (nPosY - PositionY);

            if ( !m_bJointValid )
            {
                repositionDockedSrc();
            }
        }
    }

    PositionX = nPosX;
    PositionY = nPosY;

    repositionFrame();
}

void SequenceFlow::offset( const int nOffsetX, const int nOffsetY )
{
    m_nSrcPosX += nOffsetX;
    m_nSrcPosY += nOffsetY;

    m_nJointPosX += nOffsetX;
    m_nJointPosY += nOffsetY;

    m_nArrowPosX += nOffsetX;
    m_nArrowPosY += nOffsetY;

    PositionX += nOffsetX;
    PositionY += nOffsetY;

    repositionFrame();
}

void SequenceFlow::selectSrc()
{
    Selected = true;
    m_bArrowSelected = false;
    m_bSrcSelected = true;
    m_bJointSelected = false;
}

void SequenceFlow::selectArrow()
{
    Selected = true;
    m_bArrowSelected = true;
    m_bSrcSelected = false;
    m_bJointSelected = false;
}

void SequenceFlow::select()
{
    Selected = true;
    m_bArrowSelected = false;
    m_bSrcSelected = false;
    m_bJointSelected = false;
}

void SequenceFlow::repositionDockedArrow()
{
    int nJointX = m_bJointValid ? m_nJointPosX : ((InvalidElementId != IncomingId) ? m_nIncomingX : m_nSrcPosX);
    int nJointY = m_bJointValid ? m_nJointPosY : ((InvalidElementId != IncomingId) ? m_nIncomingY : m_nSrcPosY);

    int nOutX = 0;
    int nOutY = 0;
    if (( InvalidElementId != OutgoingId ) && Geometry::lineRectangleIntersect( nJointX, nJointY, m_nOutgoingX, m_nOutgoingY, m_nOutgoingLeft, m_nOutgoingTop, m_nOutgoingRight, m_nOutgoingBottom, nOutX, nOutY ))
    {
        m_nArrowPosX = nOutX;
        m_nArrowPosY = nOutY;
    }
}

void SequenceFlow::repositionDockedSrc()
{
    int nJointX = m_bJointValid ? m_nJointPosX : ((InvalidElementId != OutgoingId) ? m_nOutgoingX : m_nArrowPosX);
    int nJointY = m_bJointValid ? m_nJointPosY : ((InvalidElementId != OutgoingId) ? m_nOutgoingY : m_nArrowPosY);

    int nOutX = 0;
    int nOutY = 0;
    if (( InvalidElementId != IncomingId ) && Geometry::lineRectangleIntersect( nJointX, nJointY, m_nIncomingX, m_nIncomingY, m_nIncomingLeft, m_nIncomingTop, m_nIncomingRight, m_nIncomingBottom, nOutX, nOutY ))
    {
        m_nSrcPosX = nOutX;
        m_nSrcPosY = nOutY;
    }
}

void SequenceFlow::repositionAutoJoint()
{
    if ( !m_bAutoTunning )
    {
        return;
    }

    m_nJointPosX = m_nOutgoingX;
    m_nJointPosY = m_nIncomingY;

    if ( Geometry::pointInRectangle( m_nJointPosX, m_nJointPosY, m_nIncomingLeft, m_nIncomingTop, m_nIncomingRight, m_nIncomingBottom ) ||
        Geometry::pointInRectangle( m_nJointPosX, m_nJointPosY, m_nOutgoingLeft, m_nOutgoingTop, m_nOutgoingRight, m_nOutgoingBottom ))
    {
        m_bJointValid = false;
    }
    else
    {
        m_bJointValid = true;
    }
}
