/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mfc_extensions/src/decorator_button/BitmapDecorator.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 * 
 * Display an Icon/Bitmap at a specified position on the button
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "bus/mfc_extensions/src/decorator_button/BitmapDecorator.h"

namespace TA_Base_Bus
{
    BitmapDecorator::BitmapDecorator(AbstractDecoratorButton* pComponent) : 
                m_bmpImage(NULL),m_xPos(-1),m_yPos(-1), ButtonDecorator(pComponent)
    {

    }


    BitmapDecorator::~BitmapDecorator()
    {
        delete m_bmpImage;
        m_bmpImage = NULL;
    }

    void BitmapDecorator::Draw(LPDRAWITEMSTRUCT lpDrawItemStruct)
    {
	    if ( m_pcomponent != NULL )
	    {
		    m_pcomponent->Draw(lpDrawItemStruct);
	    }

	    if ( m_bmpImage )
	    {
            CDC* pDC   = CDC::FromHandle(lpDrawItemStruct->hDC);
	        CRect rect = lpDrawItemStruct->rcItem;
	        UINT state = lpDrawItemStruct->itemState;

		    CDC tempDC;
		    tempDC.CreateCompatibleDC( pDC );
		    CBitmap* pbmpOld = tempDC.SelectObject( m_bmpImage );

		    if ((state & ODS_SELECTED) || (state & ODS_CHECKED))
		    {
			    pDC->BitBlt( m_xPos+1,m_yPos+1, m_bmpInfo.bmWidth, m_bmpInfo.bmHeight,
		                 &tempDC, 0,0, SRCCOPY );
		    }
		    else
		    {
			    pDC->BitBlt( m_xPos,m_yPos, m_bmpInfo.bmWidth, m_bmpInfo.bmHeight,
		                 &tempDC, 0,0, SRCCOPY );
		    }

		    tempDC.SelectObject( pbmpOld );
	    }
	    

    }


    void BitmapDecorator::setBitmap(CBitmap* bitmap, int x, int y)
    {
        if (m_bmpImage)
        {
            delete m_bmpImage;
        }
        m_bmpImage = bitmap;

	    // image position
	    m_xPos = x;
	    m_yPos = y;

	    if (m_bmpImage)
	    {
		    m_bmpImage->GetObject(sizeof(m_bmpInfo), &m_bmpInfo);
	    }
  
    }
}

