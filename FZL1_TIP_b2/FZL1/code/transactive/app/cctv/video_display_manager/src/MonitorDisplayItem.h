#pragma once

#include "afxwin.h"
#include "Monitor.h"
#include "Camera.h"
#include "Sequence.h"
#include "Label.h"


namespace TA_IRS_App
{

struct MonitorWnd
{
    CStatic* m_wnd;
    HBITMAP m_standardBitmap;
    HBITMAP m_highlightBitmap;
    MonitorWnd( CStatic* monitorWnd,
                HBITMAP standardBitmap,
                HBITMAP highlightBitmap )
        : m_wnd( monitorWnd ),
          m_standardBitmap( standardBitmap ),
          m_highlightBitmap( highlightBitmap )
    {

    }
};

class MonitorDisplayItem
{

public:
    MonitorDisplayItem( void )
        : m_monitorWnd( NULL ),
          m_textLabel( NULL ),
          m_monitor( NULL ),
          m_index( -1 ),
          m_parentWnd( NULL ),
          m_localMonitor( false ),
          m_input( NULL ),
          m_indexLabel( NULL )
    {}

    ~MonitorDisplayItem( void ) {}

    void setMonitorwnd( MonitorWnd* monitorWnd )
    {
        m_monitorWnd = monitorWnd;
    }

    void setTextLabel( CLabel* lable )
    {
        m_textLabel = lable;
    }

    void setIndexLabel( CLabel* indexLabel )
    {
        m_indexLabel = indexLabel;
    }

    void SetMonitor( Monitor* monitor )
    {
        m_monitor = monitor;
    }

    Monitor* getMonitor()
    {
        return m_monitor;
    }

    MonitorWnd* getMonitorWnd()
    {
        return m_monitorWnd;
    }

    CLabel* getTextLabel()
    {
        return  m_textLabel;
    }

    CLabel* getIndexLable()
    {
        return  m_indexLabel;
    }

    BOOL isInRect( CPoint& pt )
    {
        CRect wndRect;;
        m_monitorWnd->m_wnd->GetWindowRect( &wndRect );
        m_parentWnd->ScreenToClient( &wndRect );
        return  wndRect.PtInRect( pt );
    }

    int getIndex()
    {
        return m_index;
    }

    void setIndex( int index )
    {
        m_index = index;
    }

    void setLocalMonitor( bool isLocal )
    {
        m_localMonitor = isLocal;
    }

    bool isLocalMonitor()
    {
        return m_localMonitor;
    }

    bool isQuad()
    {
        return m_monitor->isQuad();
    }

    void setParentWnd( CWnd* parentWnd )
    {
        m_parentWnd = parentWnd;
    }

    void setVideoInput( VideoInput* input )
    {
        m_input = input;
    }

    VideoInput* getVideoInput()
    {
        return m_input;
    }

    bool inputIsCamera()
    {
        if ( m_input == NULL )
        {
            return false;
        }
        Camera* camera = dynamic_cast<Camera*>( m_input );
        return ( camera != NULL );
    }

    bool inputIsSequence()
    {
        if ( m_input == NULL )
        {
            return false;
        }
        Sequence* seq = dynamic_cast<Sequence*>( m_input );
        return ( seq != NULL );
    }

    Camera* getCamera()
    {
        Camera* camera = dynamic_cast<Camera*>( m_input );
        return camera;
    }

    Sequence* getSequence()
    {
        Sequence* seq = dynamic_cast<Sequence*>( m_input );
        return seq;
    }

    std::string getInputName()
    {
        std::string inputName = "";
        if ( m_input != NULL )
        {
            inputName = m_input->getName();
        }
        return inputName;
    }
private:
    MonitorWnd* m_monitorWnd;
    CLabel* m_textLabel;
    Monitor* m_monitor;
    int m_index;
    bool m_localMonitor;
    CWnd* m_parentWnd;
    VideoInput* m_input;
    CLabel* m_indexLabel;
};
}
