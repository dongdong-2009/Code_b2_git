#pragma once

#include "app/response_plans/plan_editor_plugin/workflow_element/src/BitmapActivity.h"

namespace TA_Base_App
{

class GatewayInclusive : public BitmapActivity
{
public:
    GatewayInclusive();
    virtual ~GatewayInclusive();

public:
    virtual bool hitTest(const int nPosX, const int nPosY);
    virtual bool intersect(const int nLeft, const int nTop, const int nRight, const int nBottom);

protected:
	virtual void serializeAddons( Stringification& ioSerilization );

public:
    static const std::string DisplayName;
    static const std::string Name;
    static const std::string Resource;
};

}

