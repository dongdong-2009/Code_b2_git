#pragma once

#include <vector>
#include <string>

const unsigned int InvalidElementId = 0u;

class ElementShortcut;
class Stringification;
class IDrawingContext;

class WorkflowElement
{
public:
    WorkflowElement();
    virtual ~WorkflowElement();

protected:
    virtual void drawEffect( IDrawingContext& refDrawing ) = 0;
    virtual void draw( IDrawingContext& refDrawing ) = 0;
    virtual void repositionFrame() = 0;
    virtual void serializeBasic( Stringification& ioSerialization ) = 0;

public:
    virtual bool isHover( const int nPosX, const int nPosY ) = 0;
    //virtual bool isDockable( const int nPosX, const int nPosY ) = 0;
    virtual bool hitTest( const int nPosX, const int nPosY ) = 0;
    virtual bool intersect( const int nLeft, const int nTop, const int nRight, const int nBottom ) = 0;
    virtual void selectPosition( const int nPosX, const int nPosY ) = 0;
    virtual void deselect() = 0;
    //virtual void getDockPosition(int& nPosX, int& nPosY) = 0;
    virtual void getMovingPosition( int& nPosX, int& nPosY ) = 0;
    virtual bool multiSelect( const int nLeft, const int nTop, const int nRight, const int nBottom ) = 0;
    //virtual bool dockedToPosition( const unsigned int unId, const int nPosX, const int nPosY, const int nLeft, const int nTop, const int nRight, const int nBottom ) = 0;
    //virtual bool cancelDocking( const unsigned int unId ) = 0;
    //virtual void deleteDocking( const unsigned int unId ) = 0;
    //virtual void dockingMoved( const unsigned int unId, const int nPosX, const int nPosY, const int nLeft, const int nTop, const int nRight, const int nBottom ) = 0;

public:
    virtual void refresh( IDrawingContext& refDrawing ) = 0;
    virtual void move( const bool bMultiMove, const int nPositionX, const int nPositionY ) = 0;
    virtual void offset( const int nOffsetX, const int nOffsetY ) = 0;
    virtual void serialize( Stringification& ioSerialization );
    std::string serialize();
    void deserialize( const std::string& strSource );
    bool trySelect( const bool bEnableMultiple, const int nPosX, const int nPosY );
    virtual void select();

public:
	bool Editable;
    bool Interactive;
    bool Collision;
    bool Pointed;
    bool Selected;
    unsigned int ID;
    int Type;
    int OrderZ;
    int PositionX;
    int PositionY;
    int FrameTop;
    int FrameLeft;
    int FrameRight;
    int FrameBottom;
};
