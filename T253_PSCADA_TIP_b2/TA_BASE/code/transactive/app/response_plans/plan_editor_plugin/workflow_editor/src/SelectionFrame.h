#pragma once

class IDrawingContext;

class SelectionFrame
{
public:
    SelectionFrame();
    virtual ~SelectionFrame();

public:
    void draw( IDrawingContext& refDrawing );

public:
    bool Hidden;
	int Left;
	int Top;
    int Right;
    int Bottom;
};
