
#include "app/response_plans/plan_editor_plugin/workflow_drawing/src/IDrawingContext.h"
#include "app/response_plans/plan_editor_plugin/workflow_drawing/src/PresentEntity.h"

#include "Stringification.h"
#include "ElementShortcut.h"
#include "WorkflowActivity.h"

const int SelectionWidth = 1;
const int HoverWidth = 2;
const int StatusOffset = 5;
const int StatusWidth = 3;
const int StatusRound = 12;
const int SelectionOffset = 2;
const int HoverPosOffset = 4;
const unsigned int SelectionColor = 0xFF0000u;
const unsigned int HoverColor = 0x00FF00u;
const unsigned int StatusRunningColor = 0xFF9C00u;
const unsigned int StatusSucceedColor = 0x00FF00u;
const unsigned int StatusFailedColor = 0x0000FFu;
const unsigned int StatusPausedColor = 0x0000FFu;
const unsigned int StatusSkippedColor = 0xB5B5B5u;

PresentEntity* WorkflowActivity::s_pIconRunning = NULL;
PresentEntity* WorkflowActivity::s_pIconSucceed = NULL;
PresentEntity* WorkflowActivity::s_pIconFailed = NULL;
PresentEntity* WorkflowActivity::s_pIconPaused = NULL;
PresentEntity* WorkflowActivity::s_pIconSkipped = NULL;

WorkflowActivity::WorkflowActivity() :
Hovered( false ),
InstanceName( "New Step" ),
m_eStatus( eUnknown )
{
    if ( NULL == s_pIconRunning )
    {
        s_pIconRunning = new PresentEntity( "res\\PlanStepStateRunning.bmp" );
    }
    if ( NULL == s_pIconSucceed )
    {
        s_pIconSucceed = new PresentEntity( "res\\PlanStepStateSucceed.bmp" );
    }
    if ( NULL == s_pIconFailed )
    {
        s_pIconFailed = new PresentEntity( "res\\PlanStepStateFailed.bmp" );
    }
    if ( NULL == s_pIconPaused )
    {
        s_pIconPaused = new PresentEntity( "res\\PlanStepStatePaused.bmp" );
    }
    if ( NULL == s_pIconSkipped )
    {
        s_pIconSkipped = new PresentEntity( "res\\PlanStepStateSkipped.bmp" );
    }
}

WorkflowActivity::~WorkflowActivity()
{
    m_vecShortcuts.clear();
}

void WorkflowActivity::attachShortcut( const std::string& strResource, const std::vector<int>& vecElements )
{
    int nLastBottom = FrameTop;

    if ( !m_vecShortcuts.empty() )
    {
        ElementShortcut* pLastShortcut = m_vecShortcuts.back();

        nLastBottom = pLastShortcut->bottom();
    }

    ElementShortcut* pNewShortcut = new ElementShortcut( strResource, FrameRight + 6, nLastBottom + 3 );
    pNewShortcut->ToCreateElements = vecElements;
    m_vecShortcuts.push_back( pNewShortcut );
}

void WorkflowActivity::move( const bool bMultiMove, const int nPositionX, const int nPositionY )
{
    PositionX = nPositionX;
    PositionY = nPositionY;

    repositionFrame();

    repositionShortcuts();
}

void WorkflowActivity::offset( const int nOffsetX, const int nOffsetY )
{
    PositionX += nOffsetX;
    PositionY += nOffsetY;

    repositionFrame();

    repositionShortcuts();
}

void WorkflowActivity::repositionShortcuts()
{
    int nIndex = 0;
    int nLastBottom = FrameTop;

    for ( std::vector<ElementShortcut*>::iterator itLoop = m_vecShortcuts.begin(); m_vecShortcuts.end() != itLoop; ++itLoop )
    {
        ElementShortcut* pShortcut = *itLoop;

        if ( NULL == pShortcut )
        {
            continue;
        }

        pShortcut->PositionX = FrameRight + 4;
        pShortcut->PositionY = nLastBottom + 2;

        nLastBottom = pShortcut->bottom();

        ++nIndex;
    }
}

void WorkflowActivity::drawShortcuts( IDrawingContext& refDrawing )
{
    if ( !Selected )
    {
        return;
    }

    for ( std::vector<ElementShortcut*>::iterator itLoop = m_vecShortcuts.begin(); m_vecShortcuts.end() != itLoop; ++itLoop  )
    {
        ElementShortcut* pShortcut = *itLoop;

        if ( NULL == pShortcut )
        {
            continue;
        }

        pShortcut->draw( refDrawing );
    }
}

void WorkflowActivity::refresh( IDrawingContext& refDrawing )
{
    draw( refDrawing );

    drawEffect( refDrawing );

    if ( Selected )
    {
        drawShortcuts( refDrawing );
    }
}

bool WorkflowActivity::shortcutTriggered( const int nPositionX, const int nPositionY, std::vector<int>& vecNewElements )
{
    vecNewElements.clear();

    bool bTriggered = false;

    for ( std::vector<ElementShortcut*>::iterator itLoop = m_vecShortcuts.begin(); m_vecShortcuts.end() != itLoop; ++itLoop  )
    {
        ElementShortcut* pShortcut = *itLoop;

        if ( NULL == pShortcut )
        {
            continue;
        }

        if ( pShortcut->isTriggered( nPositionX, nPositionY ))
        {
            bTriggered = true;
            vecNewElements = pShortcut->ToCreateElements;
            break;
        }
    }

    return bTriggered;
}

void WorkflowActivity::init( const int nPositionX, const int nPositionY )
{
    move( false, nPositionX, nPositionY );
}

void WorkflowActivity::drawEffect( IDrawingContext& refDrawing )
{
    if ( Hovered )
    {
        refDrawing.selectLineStyle( HoverColor, HoverWidth, true );

        const int nHeightMargin = ( FrameBottom - FrameTop ) / 6;
        const int nWidthMargin = ( FrameRight - FrameLeft ) / 6;
        const int nHoverLeft = FrameLeft - HoverPosOffset;
        const int nHoverTop = FrameTop - HoverPosOffset;
        const int nHoverRight = FrameRight + HoverPosOffset;
        const int nHoverBottom = FrameBottom + HoverPosOffset;

        refDrawing.drawLine( nHoverLeft, nHoverTop + nWidthMargin, nHoverLeft, nHoverTop );
        refDrawing.drawLine( nHoverLeft, nHoverTop, nHoverLeft + nWidthMargin, nHoverTop );
        refDrawing.drawLine( nHoverRight - nWidthMargin, nHoverTop, nHoverRight, nHoverTop );
        refDrawing.drawLine( nHoverRight, nHoverTop, nHoverRight, nHoverTop + nHeightMargin );
        refDrawing.drawLine( nHoverRight, nHoverBottom - nHeightMargin, nHoverRight, nHoverBottom );
        refDrawing.drawLine( nHoverRight, nHoverBottom, nHoverRight - nWidthMargin, nHoverBottom );
        refDrawing.drawLine( nHoverLeft + nWidthMargin, nHoverBottom, nHoverLeft, nHoverBottom );
        refDrawing.drawLine( nHoverLeft, nHoverBottom, nHoverLeft, nHoverBottom - nHeightMargin );

        refDrawing.restoreLineStyle();
    }

    if ( Selected )
    {
        refDrawing.selectLineStyle( SelectionColor, SelectionWidth, true );

        const int nHeightMargin = (FrameBottom - FrameTop) / 8;
        const int nWidthMargin = (FrameRight - FrameLeft) / 8;
        const int nSelectionLeft = FrameLeft - SelectionOffset;
        const int nSelectionTop = FrameTop - SelectionOffset;
        const int nSelectionRight = FrameRight + SelectionOffset;
        const int nSelectionBottom = FrameBottom + SelectionOffset;

        refDrawing.drawLine( nSelectionLeft, nSelectionTop + nWidthMargin, nSelectionLeft, nSelectionTop );
        refDrawing.drawLine( nSelectionLeft, nSelectionTop, nSelectionLeft + nWidthMargin, nSelectionTop );
        refDrawing.drawLine( nSelectionRight - nWidthMargin, nSelectionTop, nSelectionRight, nSelectionTop );
        refDrawing.drawLine( nSelectionRight, nSelectionTop, nSelectionRight, nSelectionTop + nHeightMargin );
        refDrawing.drawLine( nSelectionRight, nSelectionBottom - nHeightMargin, nSelectionRight, nSelectionBottom );
        refDrawing.drawLine( nSelectionRight, nSelectionBottom, nSelectionRight - nWidthMargin, nSelectionBottom );
        refDrawing.drawLine( nSelectionLeft + nWidthMargin, nSelectionBottom, nSelectionLeft, nSelectionBottom );
        refDrawing.drawLine( nSelectionLeft, nSelectionBottom, nSelectionLeft, nSelectionBottom - nHeightMargin );

        refDrawing.restoreLineStyle();
    }

    if ( eUnknown != m_eStatus )
    {
        PresentEntity* pIcon = NULL;
        unsigned int clrStatusColor = 0u;
        switch ( m_eStatus )
        {
        case eRunning:
            clrStatusColor = StatusRunningColor;
            pIcon = s_pIconRunning;
            break;
        case eSucceed:
            clrStatusColor = StatusSucceedColor;
            pIcon = s_pIconSucceed;
            break;
        case eFailed:
            clrStatusColor = StatusFailedColor;
            pIcon = s_pIconFailed;
            break;
        case ePaused:
            clrStatusColor = StatusPausedColor;
            pIcon = s_pIconPaused;
            break;
        case eSkipped:
            clrStatusColor = StatusSkippedColor;
            pIcon = s_pIconSkipped;
            break;
        default:
            break;
        }

        refDrawing.selectLineStyle( clrStatusColor, StatusWidth, true );
        refDrawing.selectTransparentFilling();

        refDrawing.drawRectangle( FrameLeft - SelectionOffset - StatusWidth, FrameTop - SelectionOffset - StatusWidth, FrameRight + SelectionOffset + StatusWidth, FrameBottom + SelectionOffset + StatusWidth, StatusRound );

        refDrawing.restoreLineStyle();
        refDrawing.restoreFillingColor();

        if ( NULL != pIcon )
        {
            refDrawing.drawEntity( *pIcon, FrameRight - pIcon->width() - StatusOffset, FrameTop + StatusOffset );
        }
    }
}

void WorkflowActivity::serializeBasic( Stringification& ioSerialization )
{
    serializeAddons( ioSerialization );
}

void WorkflowActivity::getValidRectangle( int& nLeft, int& nTop, int& nRight, int& nBottom )
{
    nLeft = FrameLeft;
    nTop = FrameTop;

    nRight = 0;
    for ( std::vector<ElementShortcut*>::iterator itLoop = m_vecShortcuts.begin(); m_vecShortcuts.end() != itLoop; ++itLoop )
    {
        ElementShortcut* pShortcut = *itLoop;

        if ( NULL == pShortcut )
        {
            continue;
        }

        if ( pShortcut->right() > nRight )
        {
            nRight = pShortcut->right();
        }
    }

    nBottom = FrameBottom;
}

int WorkflowActivity::status( const int eNewStatus )
{
    int eOldStatus = m_eStatus;

    m_eStatus = eNewStatus;

    return eOldStatus;
}

