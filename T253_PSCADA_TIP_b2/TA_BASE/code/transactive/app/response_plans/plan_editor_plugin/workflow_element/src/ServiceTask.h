#pragma once

#include <string>

#include "app/response_plans/plan_editor_plugin/workflow_drawing/src/PresentEntity.h"
#include "BitmapActivity.h"

class ServiceTask : public BitmapActivity
{
public:
    ServiceTask();
    virtual ~ServiceTask();

public:
    virtual std::string displayName() = 0;
    virtual std::string name() = 0;
    virtual std::string resource() = 0;

protected:
    virtual void draw( IDrawingContext& refDrawing );
    virtual PresentEntity& getIcon() = 0;

public:
    static const std::string CategroyName;

    std::string InstanceName;
};
