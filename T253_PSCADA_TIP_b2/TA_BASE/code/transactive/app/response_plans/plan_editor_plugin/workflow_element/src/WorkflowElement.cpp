
#include "Stringification.h"
#include "WorkflowElement.h"

WorkflowElement::WorkflowElement() :
Editable( true ),
Interactive( false ),
Collision( false ),
Pointed( false ),
Selected( false ),
ID( InvalidElementId ),
Type( 0 ),
OrderZ( 0 ),
PositionX( 0 ),
PositionY( 0 ),
FrameTop( 0 ),
FrameLeft( 0 ),
FrameRight( 0 ),
FrameBottom( 0 )
{
}

WorkflowElement::~WorkflowElement()
{
}

void WorkflowElement::serialize( Stringification& ioSerialization )
{
    ioSerialization.serialize( Collision );
    ioSerialization.serialize( OrderZ );
    ioSerialization.serialize( PositionX );
    ioSerialization.serialize( PositionY );
    ioSerialization.serialize( FrameTop );
    ioSerialization.serialize( FrameLeft );
    ioSerialization.serialize( FrameRight );
    ioSerialization.serialize( FrameBottom );
}

std::string WorkflowElement::serialize()
{
    Stringification ioSerialization;

    serialize( ioSerialization );

    serializeBasic( ioSerialization );

    return ioSerialization.str();
}

void WorkflowElement::deserialize(const std::string& strSource)
{
    if ( strSource.empty() )
    {
        return;
    }

    Stringification ioSerialization( strSource );

    serialize( ioSerialization );

    serializeBasic( ioSerialization );
}

bool WorkflowElement::trySelect( const bool bEnableMultiple, const int nPosX, const int nPosY )
{
    bool bOldSelected = Selected;

    if ( bEnableMultiple && Selected )
    {
        deselect();
    }
    else /*if ( !Selected )*/
    {
        selectPosition( nPosX, nPosY );
    }

    return ( bOldSelected != Selected );
}

void WorkflowElement::select()
{
    Selected = true;
}
