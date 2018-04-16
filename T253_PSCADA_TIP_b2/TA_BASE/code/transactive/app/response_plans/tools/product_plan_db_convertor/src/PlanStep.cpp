#include "PlanStep.h"

PlanStep::PlanStep( const unsigned long pkey, const unsigned long type, const unsigned long plan, const int position, const int id ) :
pkey( pkey ),
type( type ),
plan( plan ),
position( position ),
id( id )
{
}

PlanStep::~PlanStep()
{
}
