#include "stdafx.h"

#include <math.h>

#include "PresentEntity.h"
#include "DrawingContext.h"


DrawingContext::DrawingContext( CDC& refDC ) : IDrawingContext(),
m_refDC( refDC ),
m_hOrigPen( NULL ),
m_hOrigBrush( NULL ),
m_hOrigFont( NULL )
{
}

DrawingContext::~DrawingContext()
{
}

void DrawingContext::selectLineStyle( const unsigned int nColor, const int nWidth, const bool bSolid )
{
    // Release previous pen if pen handle is not released
    releaseCurLineStyle();

    m_gdiCurPen.CreatePen( bSolid ? PS_SOLID : PS_DOT, nWidth, nColor );
    HGDIOBJ hPrevPen = m_refDC.SelectObject( m_gdiCurPen.GetSafeHandle() );

    if ( NULL == m_hOrigPen )
    {
        m_hOrigPen = hPrevPen;
    }
}

void DrawingContext::selectTransparentLine()
{
    // Release previous pen if pen handle is not released
    releaseCurLineStyle();

    HGDIOBJ hPrevPen = m_refDC.SelectStockObject( NULL_PEN )->GetSafeHandle();

    if ( NULL == m_hOrigPen )
    {
        m_hOrigPen = hPrevPen;
    }
}

void DrawingContext::selectFillingColor( const unsigned int nColor )
{
    // Release previous brush if brush handle is not released
    releaseCurFillingColor();

    m_gdiCurBrush.CreateSolidBrush( nColor );

    HGDIOBJ hPrevBrush = m_refDC.SelectObject( m_gdiCurBrush.GetSafeHandle() );

    if ( NULL == m_hOrigBrush )
    {
        m_hOrigBrush = hPrevBrush;
    }
}

void DrawingContext::selectTransparentFilling()
{
    // Release previous brush if brush handle is not released
    releaseCurFillingColor();

    HGDIOBJ hPrevBrush = m_refDC.SelectStockObject( NULL_BRUSH )->GetSafeHandle();

    if ( NULL == m_hOrigBrush )
    {
        m_hOrigBrush = hPrevBrush;
    }
}

void DrawingContext::selectFont( const int nFontSize, const bool bBold, const std::string& strFontName )
{
    // Release previous font if font handle is not released
    releaseCurFont();

    std::string strRealFont = strFontName.empty() ? "Arial" : strFontName;

    CWindowDC dcWin( NULL );
    int nHeight = -MulDiv( nFontSize, m_refDC.GetDeviceCaps( LOGPIXELSY ), 72 );
    m_gdiCurFont.CreateFont( nHeight, 0, 0, 0, (bBold ? FW_BOLD : FW_NORMAL), false, 0, 0, ANSI_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_DONTCARE | FF_DONTCARE, strRealFont.c_str() );

    HGDIOBJ hPrevFont = m_refDC.SelectObject( m_gdiCurFont.GetSafeHandle() );

    if ( NULL == m_hOrigFont )
    {
        m_hOrigFont = hPrevFont;
    }
}

void DrawingContext::drawEllipse( const int nSrcX, const int nSrcY, const int nRadius )
{
    m_refDC.Ellipse( nSrcX - nRadius, nSrcY - nRadius, nSrcX + nRadius, nSrcY + nRadius );
}

void DrawingContext::drawLine( const int nBeginX, const int nBeginY, const int nEndX, const int nEndY )
{
    m_refDC.MoveTo( nBeginX, nBeginY );
    m_refDC.LineTo( nEndX, nEndY );
}

void DrawingContext::drawTriangle( const int nBeginX, const int nBeginY, const int nJointX, const int nJointY, const int nEndX, const int nEndY )
{
    m_refDC.MoveTo( nBeginX, nBeginY );

    m_refDC.BeginPath();
    m_refDC.LineTo( nJointX, nJointY );
    m_refDC.LineTo( nEndX, nEndY );
    m_refDC.LineTo( nBeginX, nBeginY );
    m_refDC.EndPath();

    m_refDC.FillPath();
}

void DrawingContext::drawTriangle( const int nBeginX, const int nBeginY, const int nDirX, const int nDirY, const double dPar, const double dWid )
{
    double slopy = atan2( static_cast<double>(nBeginY - nDirY), static_cast<double>(nBeginX - nDirX) );
    double cosy = cos( slopy );
    double siny = sin( slopy );

    m_refDC.MoveTo( nBeginX, nBeginY );

    m_refDC.BeginPath();

    m_refDC.LineTo( nBeginX + static_cast<int>(-dPar * cosy - (dPar / dWid * siny)), nBeginY + static_cast<int>(-dPar * siny + (dPar / dWid * cosy)) );
    m_refDC.LineTo( nBeginX + static_cast<int>(-dPar * cosy + (dPar / dWid * siny)), nBeginY - static_cast<int>(dPar / dWid * cosy + dPar * siny) );
    m_refDC.LineTo( nBeginX, nBeginY );
    m_refDC.EndPath();
    m_refDC.FillPath();
}

void DrawingContext::drawRectangle( const int nLeft, const int nTop, const int nRight, const int nBottom, const int nRound )
{
    m_refDC.RoundRect( nLeft, nTop, nRight, nBottom, nRound, nRound );
    //m_refDC.Rectangle( nLeft, nTop, nRight, nBottom );
}

void DrawingContext::drawText( const std::string& strText, const int nSrcX, const int nSrcY )
{
    m_refDC.TextOut( nSrcX, nSrcY, strText.c_str() );
}

void DrawingContext::restoreLineStyle()
{
    if ( NULL != m_hOrigPen )
    {
        m_refDC.SelectObject( m_hOrigPen );
        m_hOrigPen = NULL;

        releaseCurLineStyle();
    }
}

void DrawingContext::restoreFillingColor()
{
    if ( NULL != m_hOrigBrush )
    {
        m_refDC.SelectObject( m_hOrigBrush );
        m_hOrigBrush = NULL;

        releaseCurFillingColor();
    }
}

void DrawingContext::restoreFont()
{
    if ( NULL != m_hOrigFont )
    {
        m_refDC.SelectObject( m_hOrigFont );
        m_hOrigFont = NULL;

        releaseCurFont();
    }
}

void DrawingContext::drawEntity( const PresentEntity& refEntity, const int nPosX, const int nPosY )
{
    CDC dcMem;

    if ( !dcMem.CreateCompatibleDC( &m_refDC ) )
    {
        return;
    }

    if ( NULL == refEntity.getResource().GetSafeHandle() )
    {
        return;
    }

    HGDIOBJ hOldBitmap = dcMem.SelectObject( refEntity.getResource().GetSafeHandle() );
    m_refDC.BitBlt( nPosX, nPosY, refEntity.width(), refEntity.height(), &dcMem, 0, 0, SRCCOPY );

    dcMem.SelectObject( hOldBitmap );

    dcMem.DeleteDC();
}

void DrawingContext::releaseCurLineStyle()
{
    if ( NULL != m_gdiCurPen.GetSafeHandle() )
    {
        m_gdiCurPen.DeleteObject();
    }
}

void DrawingContext::releaseCurFillingColor()
{
    if ( NULL != m_gdiCurBrush.GetSafeHandle() )
    {
        m_gdiCurBrush.DeleteObject();
    }
}

void DrawingContext::releaseCurFont()
{
    if ( NULL != m_gdiCurFont.GetSafeHandle() )
    {
        m_gdiCurFont.DeleteObject();
    }
}

