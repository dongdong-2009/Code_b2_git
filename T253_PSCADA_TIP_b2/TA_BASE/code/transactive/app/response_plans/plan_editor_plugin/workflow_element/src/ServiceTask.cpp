#include "app/response_plans/plan_editor_plugin/workflow_drawing/src/IDrawingContext.h"

#include "Stringification.h"
#include "ServiceTask.h"

const int IconOffset = 10;
const int NameTextOffestX = 10;
const int NameTextOffestY = 40;
const int NameTextSize = 13;

const std::string ServiceTask::CategroyName = "ServiceTask";
const std::string ResourceFile = "res/PlanServiceTask.bmp";

ServiceTask::ServiceTask() : BitmapActivity( ResourceFile, true )
{
}

ServiceTask::~ServiceTask()
{
}

void ServiceTask::draw( IDrawingContext& refDrawing )
{
    BitmapActivity::draw( refDrawing );

    // Place icon on the entity background
    refDrawing.drawEntity( getIcon(), PositionX + IconOffset, PositionY + IconOffset );

    refDrawing.selectFont( NameTextSize, true );
    refDrawing.selectTransparentFilling();
    refDrawing.drawText( displayName(), PositionX + NameTextOffestX, PositionY + NameTextOffestY );
    refDrawing.restoreFillingColor();
    refDrawing.restoreFont();
}

