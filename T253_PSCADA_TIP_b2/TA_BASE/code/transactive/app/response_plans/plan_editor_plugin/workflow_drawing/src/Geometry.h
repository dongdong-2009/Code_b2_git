#pragma once

namespace Geometry
{
    bool lineIntersect(const int nStartX1, const int nStartY1, const int nEndX1, const int nEndY1, const int nStartX2, const int nStartY2, const int nEndX2, const int nEndY2, int& nOutX, int& nOutY);
    bool pointInRectangle( const int nPosX, const int nPosY, const int nLeft, const int nTop, const int nRight, const int nBottom );
    bool pointOnLine( const int nPtX, const int nPtY, const int nLineStartX, const int nLineStartY, const int nLineEndX, const int nLineEndY, const float fTolerant );
    bool lineRectangleIntersect( const int nStartX, const int nStartY, const int nEndX, const int nEndY, const int nLeft, const int nTop, const int nRight, const int nBottom, int& nOutX, int& nOutY );
    bool lineRectangleInsided( const int nStartX, const int nStartY, const int nEndX, const int nEndY, const int nLeft, const int nTop, const int nRight, const int nBottom );
};

