/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/configuration/configuration_editor/src/TitleView.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This class paints the bar holding the description of what is currently displaying in the
  * Configuration Editor.
  */

#if !defined(AFX_TITLEVIEW_H__339E8EC1_D699_4BE7_84CA_BA512E148A15__INCLUDED_)
#define AFX_TITLEVIEW_H__339E8EC1_D699_4BE7_84CA_BA512E148A15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_App
{
    class TitleView : public CView
    {
    protected:
        /**
         * Constructor
         *
         * Protected constructor used by dynamic creation
         */
	    TitleView();
	    DECLARE_DYNCREATE(TitleView)

    public:

        /**
         * Destructor
         */
	    virtual ~TitleView();


        /**
         * setTitleToDisplay
         *
         * @param CString& - The title to display in this view
         */
        void setTitleToDisplay(const CString& title);


	    //{{AFX_VIRTUAL(TitleView)
	    protected:
	    virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	    //}}AFX_VIRTUAL

    // Implementation
    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(TitleView)
	    afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        TitleView( const TitleView&){ };


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        TitleView& operator=(const TitleView&);


        // See cpp file for comments about these constants
        static const int TITLE_FONT_SIZE;
        static const CString TITLE_FONT_TYPEFACE;
        static const CString TITLE_PADDING;

        CString m_titleToDisplay;     // This is the title to paint and display. It is updated using the
                                      // setTitleToDisplay() method.
    };
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TITLEVIEW_H__339E8EC1_D699_4BE7_84CA_BA512E148A15__INCLUDED_)
