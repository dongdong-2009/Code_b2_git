#if !defined(AFX_TEXTDISPLAY_H_1343048E4952__INCLUDED_)
#define AFX_TEXTDISPLAY_H_1343048E4952__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/TextDisplay.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * CStatic derivation that allows background colour and text colour to be adjusted. Background can
  * optionally be set to transparent (i.e. Text only will be drawn ). Note the transparency feature
  * will override any backgound colour setting when active (i.e. when isBackgroundTransparent() == true )
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_IRS_App
{
    class TextDisplay : public CStatic
    {
    // Construction
    public:

        /** 
          * Constructor
          */
        TextDisplay();


        /** 
          * Destructor
          */
        virtual ~TextDisplay();


        /** 
          * setBackgroundTransparency
          *
          * Set background transparency of text display.
          *
          * @param doSetAsTransparent - true if should set as transparent, false otherwise.
          */
        void setBackgroundTransparency( bool doSetAsTransparent = true );


        /** 
          * isBackgroundTransparent
          *
          * Indicate whether background of text display is currently set as transparent.
          *
          * @return true if is currently set as transparent, false otherwise.
          */
        bool isBackgroundTransparent() { return m_isBackgroundTransparent; }
        

        /** 
          * setTextColour
          *
          * Set text colour of display.
          *
          * @param textColour - COLOREF to set text colour to (defaults to system default if no value specified).
          */
        void setTextColour( COLORREF textColour = 0xffffffff );


        /** 
          * getTextColour
          *
          * Get current text colour display is set to.
          *
          * @return COLOREF of current text colour (0xffffffff indicates system default).
          */
        COLORREF getTextColour() { return m_textColour; }

        
        /** 
          * setBackgroundColour
          *
          * Set background colour of display.
          *
          * @param textColour - COLOREF to set background colour to (defaults to system default if no value specified).
          */
        void setBackgroundColour( COLORREF backgroundColour = 0xffffffff );

        
        /** 
          * getBackgroundColour
          *
          * Get current background colour display is set to.
          *
          * @return COLOREF of current background colour (0xffffffff indicates system default).
          */
        COLORREF getBackgroundColour() { return m_backgroundColour; }

    // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(TextDisplay)
        //}}AFX_VIRTUAL

        // Generated message map functions
    protected:
        //{{AFX_MSG(TextDisplay)
        afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
        afx_msg void OnDestroy();
        //}}AFX_MSG

        DECLARE_MESSAGE_MAP()

    private:
        bool m_isBackgroundTransparent;

        COLORREF m_textColour;

        COLORREF m_backgroundColour;

        CBrush m_backgroundBrush;
    };


} // namespace TA_IRS_App
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTDISPLAY_H_1343048E4952__INCLUDED_)
