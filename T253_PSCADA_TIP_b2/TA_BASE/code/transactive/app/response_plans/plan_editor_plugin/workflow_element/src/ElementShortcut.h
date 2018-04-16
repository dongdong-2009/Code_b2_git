#pragma once

#include <vector>

#include "app/response_plans/plan_editor_plugin/workflow_drawing/src/PresentEntity.h"

class IDrawingContext;

class ElementShortcut
{
public:
    ElementShortcut( const std::string& strResource, const int nPosX, const int nPosY );
    virtual ~ElementShortcut();

public:
    void draw( IDrawingContext& refDrawing );
    bool isTriggered( const int nPosX, const int nPosY );
    int right();
    int bottom();

public:
    std::vector<int> ToCreateElements;
    int PositionX;
    int PositionY;

protected:
    PresentEntity m_oEntity;
};
