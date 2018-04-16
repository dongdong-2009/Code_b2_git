#include <cmath>

#include "Geometry.h"

namespace Geometry
{

bool sameSigns( const int nSrc, const int nComp )
{
    return (((long) ((unsigned long) nSrc ^ (unsigned long) nComp)) >= 0);
}

bool pointInRectangle( const int nPosX, const int nPosY, const int nLeft, const int nTop, const int nRight, const int nBottom )
{
    return (( nPosX > nLeft ) && ( nPosX < nRight ) && ( nPosY > nTop ) && ( nPosY < nBottom ));
}

bool lineIntersect(const int nStartX1, const int nStartY1, const int nEndX1, const int nEndY1, const int nStartX2, const int nStartY2, const int nEndX2, const int nEndY2, int& nOutX, int& nOutY)
{
    int a1, a2, b1, b2, c1, c2; /* Coefficients of line eqns. */
    int r1, r2, r3, r4;         /* 'Sign' values */
    int denom, offset, num;     /* Intermediate values */

    /* Compute a1, b1, c1, where line joining points 1 and 2
    * is "a1 x  +  b1 y  +  c1  =  0".
    */
    a1 = nEndY1 - nStartY1;
    b1 = nStartX1 - nEndX1;
    c1 = nEndX1 * nStartY1 - nStartX1 * nEndY1;

    /* Compute r3 and r4.
    */
    r3 = a1 * nStartX2 + b1 * nStartY2 + c1;
    r4 = a1 * nEndX2 + b1 * nEndY2 + c1;

    /* Check signs of r3 and r4.  If both point 3 and point 4 lie on
    * same side of line 1, the line segments do not intersect.
    */
    if (r3 != 0 &&
        r4 != 0 &&
        sameSigns( r3, r4 ) )
        return (false);

    /* Compute a2, b2, c2 */
    a2 = nEndY2 - nStartY2;
    b2 = nStartX2 - nEndX2;
    c2 = nEndX2 * nStartY2 - nStartX2 * nEndY2;

    /* Compute r1 and r2 */

    r1 = a2 * nStartX1 + b2 * nStartY1 + c2;
    r2 = a2 * nEndX1 + b2 * nEndY1 + c2;

    /* Check signs of r1 and r2.  If both point 1 and point 2 lie
    * on same side of second line segment, the line segments do
    * not intersect.
    */
    if (r1 != 0 &&
        r2 != 0 &&
        sameSigns( r1, r2 ) )
        return (false);

    /* Line segments intersect: compute intersection point.
    */
    denom = a1 * b2 - a2 * b1;
    if (denom == 0)
        return (false);
    offset = denom < 0 ? -denom / 2 : denom / 2;

    /* The denom/2 is to get rounding instead of truncating.  It
    * is added or subtracted to the numerator, depending upon the
    * sign of the numerator.
    */
    num = b1 * c2 - b2 * c1;
    nOutX = (num < 0 ? num - offset : num + offset) / denom;

    num = a2 * c1 - a1 * c2;
    nOutY = (num < 0 ? num - offset : num + offset) / denom;

    return true;
}

bool lineRectangleIntersect(const int nStartX, const int nStartY, const int nEndX, const int nEndY, const int nLeft, const int nTop, const int nRight, const int nBottom, int& nOutX, int& nOutY)
{
    if (lineIntersect(nStartX, nStartY, nEndX, nEndY, nLeft, nTop, nRight, nTop, nOutX, nOutY))
    {
        return true;
    }
    else if (lineIntersect(nStartX, nStartY, nEndX, nEndY, nLeft, nTop, nLeft, nBottom, nOutX, nOutY))
    {
        return true;
    }
    else if (lineIntersect(nStartX, nStartY, nEndX, nEndY, nRight, nTop, nRight, nBottom, nOutX, nOutY))
    {
        return true;
    }
    else if (lineIntersect(nStartX, nStartY, nEndX, nEndY, nLeft, nBottom, nRight, nBottom, nOutX, nOutY))
    {
        return true;
    }

    return false;
}

bool lineRectangleInsided( const int nStartX, const int nStartY, const int nEndX, const int nEndY, const int nLeft, const int nTop, const int nRight, const int nBottom )
{
    int nOutX = 0;
    int nOutY = 0;

    if ( lineIntersect( nStartX, nStartY, nEndX, nEndY, nLeft, nTop, nRight, nTop, nOutX, nOutY ))
    {
        return true;
    }
    else if ( lineIntersect( nStartX, nStartY, nEndX, nEndY, nLeft, nTop, nLeft, nBottom, nOutX, nOutY ))
    {
        return true;
    }
    else if ( lineIntersect( nStartX, nStartY, nEndX, nEndY, nRight, nTop, nRight, nBottom, nOutX, nOutY ))
    {
        return true;
    }
    else if ( lineIntersect( nStartX, nStartY, nEndX, nEndY, nLeft, nBottom, nRight, nBottom, nOutX, nOutY ))
    {
        return true;
    }
    else if ( pointInRectangle( nStartX, nStartY, nLeft, nTop, nRight, nBottom ) && pointInRectangle( nEndX, nEndY, nLeft, nTop, nRight, nBottom ))
    {
        return true;
    }

    return false;
}

bool pointOnLine( const int nPtX, const int nPtY, const int nLineStartX, const int nLineStartY, const int nLineEndX, const int nLineEndY, const float fTolerant )
{
    float fSlopeR = static_cast<float>(nPtX - nLineStartX) / static_cast<float>(nLineEndX - nLineStartX) -
        static_cast<float>(nPtY - nLineStartY) / static_cast<float>(nLineEndY - nLineStartY);

    if ( abs( fSlopeR ) < fTolerant )
    {
        return true;
    }

    // [ToDo] Add handling when slope is 0 or NA

    return false;
}

}