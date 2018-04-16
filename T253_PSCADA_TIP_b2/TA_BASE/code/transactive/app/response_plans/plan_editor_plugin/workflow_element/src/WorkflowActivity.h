#pragma once

#include "WorkflowElement.h"


enum eActivityStatus
{
    eUnknown = 0,
    eRunning,
    eSucceed,
    eFailed,
    ePaused,
    eSkipped
};

class IDrawingContext;
class PresentEntity;

class WorkflowActivity : public WorkflowElement
{
public:
    WorkflowActivity();
    virtual ~WorkflowActivity();

private:
    void drawShortcuts( IDrawingContext& refDrawing );
    void repositionShortcuts();

protected:
    virtual void drawEffect( IDrawingContext& refDrawing );
    virtual void draw( IDrawingContext& refDrawing ) = 0;
    virtual void serializeBasic( Stringification& ioSerialization );
    virtual void serializeAddons( Stringification& ioSerialization ) = 0;

public:
    //virtual bool isHover( const int nPosX, const int nPosY ) = 0;
    virtual bool isDockable( const int nPosX, const int nPosY ) = 0;
    //virtual bool hitTest( const int nPosX, const int nPosY ) = 0;
    //virtual bool intersect( const int nLeft, const int nTop, const int nRight, const int nBottom ) = 0;
    //virtual void select( const int nPosX, const int nPosY ) = 0;
    //virtual void deselect() = 0;
    //virtual void moveFrame( const int nPosX, const int nPosY ) = 0;
    virtual void getDockPosition(int& nPosX, int& nPosY) = 0;
    //virtual void getMovingPosition( int& nPosX, int& nPosY ) = 0;
    //virtual bool multiSelect( const int nLeft, const int nTop, const int nRight, const int nBottom ) = 0;
    //virtual bool dockedToPosition( const unsigned int unId, const int nPosX, const int nPosY, const int nLeft, const int nTop, const int nRight, const int nBottom ) = 0;
    //virtual bool cancelDocking( const unsigned int unId ) = 0;
    //virtual void deleteDocking( const unsigned int unId ) = 0;
    //virtual void dockingMoved( const unsigned int unId, const int nPosX, const int nPosY, const int nLeft, const int nTop, const int nRight, const int nBottom ) = 0;

public:
    void init( const int nPositionX, const int nPositionY );
    virtual void move( const bool bMultiMove, const int nPositionX, const int nPositionY );
    virtual void offset( const int nOffsetX, const int nOffsetY );
    virtual void refresh( IDrawingContext& refDrawing );
    void attachShortcut( const std::string& strResource, const std::vector<int>& vecElements );
    void getValidRectangle( int& nLeft, int& nTop, int& nRight, int& nBottom );
    bool shortcutTriggered( const int nPositionX, const int nPositionY, std::vector<int>& vecNewElements );
    int status( const int eNewStatus );

public:
    bool Hovered;
    std::string InstanceName;

protected:
    static PresentEntity* s_pIconRunning;
    static PresentEntity* s_pIconSucceed;
    static PresentEntity* s_pIconFailed;
    static PresentEntity* s_pIconPaused;
    static PresentEntity* s_pIconSkipped;
    int m_eStatus;
    std::vector<ElementShortcut*> m_vecShortcuts;
};
