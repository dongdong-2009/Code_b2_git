
#include "app/response_plans/plan_editor_plugin/workflow_drawing/src/IDrawingContext.h"
#include "SelectionFrame.h"

const int SelectedFrameOffset = 4;
const int SelectedFrameWidth = 1;
const unsigned int SelectedFrameColor = 0xACACAC;

SelectionFrame::SelectionFrame() :
Hidden( true ),
Left(0),
Top(0),
Right(0),
Bottom(0)
{
}

SelectionFrame::~SelectionFrame()
{
}

void SelectionFrame::draw( IDrawingContext& refDrawing )
{
    if ( Hidden )
    {
        return;
    }

    refDrawing.selectLineStyle( SelectedFrameColor, SelectedFrameWidth, false );
    refDrawing.selectTransparentFilling();

    refDrawing.drawRectangle( Left - SelectedFrameOffset, Top - SelectedFrameOffset, Right + SelectedFrameOffset + SelectedFrameWidth, Bottom + SelectedFrameOffset + SelectedFrameWidth );

    refDrawing.restoreLineStyle();
    refDrawing.restoreFillingColor();
}