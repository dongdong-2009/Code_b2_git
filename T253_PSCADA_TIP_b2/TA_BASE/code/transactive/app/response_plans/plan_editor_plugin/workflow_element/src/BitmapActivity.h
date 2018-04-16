#pragma once

#include <string>

#include "app/response_plans/plan_editor_plugin/workflow_drawing/src/PresentEntity.h"
#include "WorkflowActivity.h"

class BitmapActivity : public WorkflowActivity
{
public:
    BitmapActivity( const std::string& strRes, const bool bHasInstanceName, const bool bHasFlowShortcut = true );
    virtual ~BitmapActivity();
    
public:
    virtual bool isHover( const int nPosX, const int nPosY );
    virtual bool isDockable( const int nPosX, const int nPosY );
    virtual bool hitTest( const int nPosX, const int nPosY );
    virtual bool intersect( const int nLeft, const int nTop, const int nRight, const int nBottom );
    virtual void selectPosition( const int nPosX, const int nPosY );
    virtual void deselect();
    virtual void getDockPosition( int& nPosX, int& nPosY );
    virtual void getMovingPosition( int& nPosX, int& nPosY );
    virtual bool dockedToPosition( const unsigned int unId, const int nPosX, const int nPosY, const int nLeft, const int nTop, const int nRight, const int nBottom );
    virtual bool cancelDocking( const unsigned int unId );
    virtual void deleteDocking( const unsigned int unId );
    virtual void dockingMoved( const unsigned int unId, const int nPosX, const int nPosY, const int nLeft, const int nTop, const int nRight, const int nBottom );
    virtual bool multiSelect( const int nLeft, const int nTop, const int nRight, const int nBottom );

protected:
    virtual void draw( IDrawingContext& refDrawing );
    virtual void repositionFrame();

protected:
    PresentEntity m_oEntity;

private:
    bool m_bHasInstanceName;
	int m_nWidth;
	int m_nHeight;
};
