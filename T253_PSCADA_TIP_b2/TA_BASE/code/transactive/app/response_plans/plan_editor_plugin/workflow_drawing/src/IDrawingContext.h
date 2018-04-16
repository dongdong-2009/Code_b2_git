#pragma once

#include <string>

class PresentEntity;

class IDrawingContext
{
public:
    virtual void selectLineStyle( const unsigned int nColor, const int nWidth, const bool bSolid ) = 0;
    virtual void selectTransparentLine() = 0;
    virtual void selectFillingColor( const unsigned int nColor ) = 0;
    virtual void selectTransparentFilling() = 0;
    virtual void selectFont( const int nFontSize, const bool bBold, const std::string& strFontName = "" ) = 0;

    virtual void restoreLineStyle() = 0;
    virtual void restoreFillingColor() = 0;
    virtual void restoreFont() = 0;

    virtual void drawText( const std::string& strText, const int nSrcX, const int nSrcY ) = 0;
    virtual void drawEllipse( const int nSrcX, const int nSrcY, const int nRadius ) = 0;
    virtual void drawLine( const int nBeginX, const int nBeginY, const int nEndX, const int nEndY ) = 0;
    virtual void drawTriangle( const int nBeginX, const int nBeginY, const int nJointX, const int nJointY, const int nEndX, const int nEndY ) = 0;
    virtual void drawTriangle( const int nBeginX, const int nBeginY, const int nDirX, const int nDirY, const double dPar, const double dWid ) = 0;
    virtual void drawRectangle( const int nLeft, const int nTop, const int nRight, const int nBottom, const int nRound = 0 ) = 0;

    virtual void drawEntity( const PresentEntity& refEntity, const int nPosX, const int nPosY ) = 0;
};

