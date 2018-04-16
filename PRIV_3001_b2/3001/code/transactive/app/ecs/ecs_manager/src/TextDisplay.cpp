/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ecs_manager/src/TextDisplay.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * <description>
  *
  */

#include "app/ecs/ecs_manager/src/stdafx.h"
#include "app/ecs/ecs_manager/src/TextDisplay.h"

#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_IRS_App
{

    //
    // Constructor
    //
    TextDisplay::TextDisplay() 
        : m_textColour( ::GetSysColor( COLOR_BTNTEXT ) ), m_backgroundColour( ::GetSysColor( COLOR_BTNFACE ) ), 
        m_isBackgroundTransparent( false )
    {
        FUNCTION_ENTRY( "Constructor" );

        // Set default background brush
        m_backgroundBrush.CreateSolidBrush(m_backgroundColour);

        FUNCTION_EXIT;
    }


    //
    // Destructor
    //
    TextDisplay::~TextDisplay()
    {
        FUNCTION_ENTRY( "Destructor" );

        //
        // Nothing to clean up.
        //

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(TextDisplay, CStatic)
        //{{AFX_MSG_MAP(TextDisplay)
        ON_WM_CTLCOLOR_REFLECT()
        ON_WM_DESTROY()
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // TextDisplay message handlers

    //
    // CtlColor
    //
    HBRUSH TextDisplay::CtlColor(CDC* pDC, UINT nCtlColor) 
    {
        pDC->SetTextColor(m_textColour);

        if ( m_isBackgroundTransparent )
        {
            m_backgroundBrush.DeleteObject();
            m_backgroundBrush.CreateStockObject(HOLLOW_BRUSH);
            pDC->SetBkMode(TRANSPARENT);
        }
        else
        {
            pDC->SetBkColor(m_backgroundColour);
        }

        // Return a non-NULL brush if the parent's handler should not be called
        return (HBRUSH)m_backgroundBrush;
    }

    
    //
    // OnDestroy
    //
    void TextDisplay::OnDestroy() 
    {
        CStatic::OnDestroy();
        m_backgroundBrush.DeleteObject();    
    }


    //
    // setTextColour
    //
    void TextDisplay::setTextColour( COLORREF textColour )
    {
        FUNCTION_ENTRY( "setTextColour" );

        // If the colour is null, paint the default button text colour
        if (textColour != 0xffffffff)
        {
            m_textColour = textColour;
        }
        else 
        {
            m_textColour = ::GetSysColor(COLOR_BTNTEXT);
        }

        // Force the static to repaint
        Invalidate();

        FUNCTION_EXIT;
    }


    //
    // setBackgroundColour
    //
    void TextDisplay::setBackgroundColour(COLORREF backgroundColour)
    {
        FUNCTION_ENTRY( "setBackgroundColour" );

        // If the passed in colour is null, set the colour to the default button face colour
        if (backgroundColour != 0xffffffff)
        {
            m_backgroundColour = backgroundColour;
        }
        else
        {
            m_backgroundColour = ::GetSysColor(COLOR_BTNFACE);
        }

        m_backgroundBrush.DeleteObject();
        m_backgroundBrush.CreateSolidBrush(m_backgroundColour);

        // Repaint control
        RedrawWindow();

        FUNCTION_EXIT;
    }


    //
    // setBackgroundTransparency
    //
    void TextDisplay::setBackgroundTransparency( bool doSetAsTransparent /*=true*/ )
    {
        FUNCTION_ENTRY( "setBackgroundTransparency" );

        if ( m_isBackgroundTransparent && ( false == doSetAsTransparent ) )
        {
            m_backgroundBrush.DeleteObject();
            m_backgroundBrush.CreateSolidBrush(m_backgroundColour);
            m_isBackgroundTransparent = doSetAsTransparent;
            RedrawWindow();
            FUNCTION_EXIT;
            return;
        }

        m_isBackgroundTransparent = doSetAsTransparent;
        FUNCTION_EXIT;
    }

} // namspace TA_IRS_App
