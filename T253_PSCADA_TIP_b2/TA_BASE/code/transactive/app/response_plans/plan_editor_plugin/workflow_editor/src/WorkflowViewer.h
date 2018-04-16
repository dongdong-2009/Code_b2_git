#pragma once

#include "IDiagramViewer.h"

// WorkflowView view
const unsigned int WM_ACTIVITY_CLICKED = WM_USER + 0x1002;

class WorkflowDiagram;

class WorkflowViewer : public CScrollView, IDiagramViewer
{
public:
	WorkflowViewer( WorkflowDiagram& refDiagram );           // protected constructor used by dynamic creation
	virtual ~WorkflowViewer();
    void zoomIn();
    void zoomOut();
    void actualZoom();
    void fitToWin();
    void fitToSize( SIZE& szTotal );
    void screenToDiagram( CPoint& ptPage );

public:
    virtual void diagramUpdated();
    virtual void elementAdded( const unsigned int unId );
    virtual void elementDeleted( const unsigned int unId );
    virtual void elementUpdated( const unsigned int unId );
    virtual void elementSelected( const unsigned int unId, const std::vector<unsigned int>& vecDeselectedIds );
    virtual void elementsSelected( const std::vector<unsigned int>& vecSelectedIds, const std::vector<unsigned int>& vecDeselectedIds );

protected:
	afx_msg virtual void OnDraw(CDC* pDC);      // overridden to draw this view
    afx_msg virtual void OnInitialUpdate();     // first time after construct
    afx_msg virtual void OnSize(UINT nType, int cx, int cy);
    afx_msg virtual void OnLButtonDown( UINT nFlags, CPoint ptCursor );
    afx_msg virtual void OnLButtonUp( UINT nFlags, CPoint ptCursor );
    afx_msg virtual void OnMouseMove( UINT nFlag, CPoint ptCursor );
    afx_msg virtual void OnMouseLeave();
    afx_msg virtual BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg virtual int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);

	DECLARE_MESSAGE_MAP()

protected:
    void drawSelection();
    void resyncScrollSizes();
    void pagePtToTotalPt(CPoint& ptPage);

protected:
    WorkflowDiagram& m_refDiagram;
    CPoint m_ptMouseDown;
    CRect m_rtSelection;
    CSize m_szOrgTotal; // Original total size in device units
    CSize m_szOrgPage; // Original per page scroll size in device units
    CSize m_szOrgLine; // Original per line scroll size in device units
    float m_fZoomScale;
    bool m_bDrawSelection;
};


