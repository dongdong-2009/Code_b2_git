/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/TransparentImage.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/11/18 18:38:22 $
  * Last modified by:  $Author: huang.wenbo $
  *
  */
#ifndef __TRANSPARENTIMAGE_H_TRANSPARENTIMAGE_42A6E395_97E4_11D3_B6F0_005004024A9E
#define __TRANSPARENTIMAGE_H_TRANSPARENTIMAGE_42A6E395_97E4_11D3_B6F0_005004024A9E

#if _MSC_VER >= 1000
#pragma once
#endif

class CTransparentImage
    : public CStatic
{
    public:

    CTransparentImage();

    virtual ~CTransparentImage();
    // 
    ////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////
    // ClassWizard generated virtual function overrides
    // 
    // {{AFX_VIRTUAL( CTransparentImage )
    // }}AFX_VIRTUAL
    // 
    ////////////////////////////////////////////////////////////////////////////
    protected:
    // 
    // {{AFX_MSG( CTransparentImage )
    afx_msg void OnPaint();
    // }}AFX_MSG
    // 
    DECLARE_MESSAGE_MAP()
    // 
    ////////////////////////////////////////////////////////////////////////////
};

//{{AFX_INSERT_LOCATION}}

#endif
