#include "NoWaitStep.h"

NoWaitStep::NoWaitStep( const unsigned long pkey, const unsigned long type, const unsigned long plan, const int position, const bool nowait ) :
pkey( pkey ),
type( type ),
plan( plan ),
position( position ),
nowait( nowait )
{
}

NoWaitStep::~NoWaitStep()
{
}
