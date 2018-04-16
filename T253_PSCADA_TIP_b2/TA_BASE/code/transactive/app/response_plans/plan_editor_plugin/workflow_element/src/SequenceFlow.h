#pragma once

#include "WorkflowElement.h"

enum DockMode
{
    eNoChange = 0,
    eIncomingUpdated,
    eOutgoingUpdated
};

class SequenceFlow : public WorkflowElement
{
public:
    SequenceFlow();
    virtual ~SequenceFlow();

public:
    virtual void init( const int nPositionX, const int nPositionY );
    virtual void move( const bool bMultiMove, const int nPositionX, const int nPositionY );
    virtual void offset( const int nOffsetX, const int nOffsetY );
    virtual void refresh( IDrawingContext& refDrawing );
    virtual void serializeBasic( Stringification& ioSerialization );
    virtual void select();

public:
    virtual bool isHover( const int nPosX, const int nPosY );
    virtual bool isDockable( const int nPosX, const int nPosY );
    virtual bool hitTest( const int nPosX, const int nPosY );
    virtual bool intersect( const int nLeft, const int nTop, const int nRight, const int nBottom );
    virtual void selectPosition( const int nPosX, const int nPosY );
    virtual void deselect();
    virtual void getDockPosition( int& nPosX, int& nPosY );
    virtual void getMovingPosition( int& nPosX, int& nPosY );
    virtual int dockedToPosition( const unsigned int unId, const int nPosX, const int nPosY, const int nLeft, const int nTop, const int nRight, const int nBottom );
    virtual int cancelDocking( const unsigned int unId );
    virtual int deleteDocking( const unsigned int unId );
    virtual int dockedTest( const unsigned int unId );
    virtual void dockingMoved( const unsigned int unId, const int nPosX, const int nPosY, const int nLeft, const int nTop, const int nRight, const int nBottom );
    virtual bool multiSelect( const int unLeft, const int unTop, const int unRight, const int unBottom );

public:
    void updateIncoming( const unsigned int unId, const int nPosX, const int nPosY, const int nLeft, const int nTop, const int nRight, const int nBottom );
    void updateOutgoing( const unsigned int unId, const int nPosX, const int nPosY, const int nLeft, const int nTop, const int nRight, const int nBottom );
    void selectArrow();
    void selectSrc();

protected:
    virtual void draw( IDrawingContext& refDrawing );
    virtual void drawEffect( IDrawingContext& refDrawing );
    virtual void repositionFrame();
    void repositionDockedArrow();
    void repositionDockedSrc();
    void repositionAutoJoint();

public:
    unsigned int IncomingId;
    unsigned int OutgoingId;
    std::string Condition;

private:
    int m_nSrcPosX;
    int m_nSrcPosY;
    int m_nArrowPosX;
    int m_nArrowPosY;
    int m_nJointPosX;
    int m_nJointPosY;

    bool m_bAutoTunning;
    bool m_bJointValid;
    bool m_bArrowSelected;
    bool m_bSrcSelected;
    bool m_bJointSelected;

    int m_nIncomingX;
    int m_nIncomingY;
    int m_nIncomingLeft;
    int m_nIncomingTop;
    int m_nIncomingRight;
    int m_nIncomingBottom;

    int m_nOutgoingX;
    int m_nOutgoingY;
    int m_nOutgoingLeft;
    int m_nOutgoingTop;
    int m_nOutgoingRight;
    int m_nOutgoingBottom;
};
