#pragma once

#include <wingdi.h>
#include "IDrawingContext.h"

class DrawingContext : public IDrawingContext
{
public:
    DrawingContext( CDC& refDC );
    ~DrawingContext();

public:
    virtual void selectLineStyle( const unsigned int nColor, const int nWidth, const bool bSolid );
    virtual void selectTransparentLine();
    virtual void selectFillingColor( const unsigned int nColor );
    virtual void selectTransparentFilling();
    virtual void selectFont( const int nFontSize, const bool bBold, const std::string& strFontName = "" );

    virtual void restoreLineStyle();
    virtual void restoreFillingColor();
    virtual void restoreFont();

    virtual void drawText( const std::string& strText, const int nSrcX, const int nSrcY );
    virtual void drawEllipse( const int nSrcX, const int nSrcY, const int nRadius );
    virtual void drawLine( const int nBeginX, const int nBeginY, const int nEndX, const int nEndY );
    virtual void drawTriangle( const int nBeginX, const int nBeginY, const int nJointX, const int nJointY, const int nEndX, const int nEndY );
    virtual void drawTriangle( const int nBeginX, const int nBeginY, const int nDirX, const int nDirY, const double dPar, const double dWid );
    virtual void drawRectangle( const int nLeft, const int nTop, const int nRight, const int nBottom, const int nRound = 0 );

    virtual void drawEntity( const PresentEntity& refEntity, const int nPosX, const int nPosY );

private:
    void releaseCurLineStyle();
    void releaseCurFillingColor();
    void releaseCurFont();

private:
    DrawingContext();

private:
    CDC& m_refDC;
    HGDIOBJ m_hOrigPen;
    HGDIOBJ m_hOrigBrush;
    HGDIOBJ m_hOrigFont;
    CFont m_gdiCurFont;
    CPen m_gdiCurPen;
    CBrush m_gdiCurBrush;
};

