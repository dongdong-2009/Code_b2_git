#pragma once

#include "app/response_plans/plan_editor_plugin/workflow_element/src/ServiceTask.h"

namespace TA_Base_App
{

class TaskLaunchSchematic : public ServiceTask
{
public:
    TaskLaunchSchematic();
    virtual ~TaskLaunchSchematic();

protected:
	virtual void serializeAddons( Stringification& ioSerilization );
    virtual PresentEntity& getIcon();

public:
    static const std::string DisplayName;
    static const std::string Name;
    static const std::string Resource;

public:
    virtual std::string displayName();
    virtual std::string name();
    virtual std::string resource();

private:
    PresentEntity m_oIcon;
};

}

