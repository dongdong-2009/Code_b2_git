#include "app/response_plans/plan_step_plugin/common/src/BasicParameter.h"

namespace TA_Base_App
{

BasicParameter::BasicParameter()
{
}

BasicParameter::BasicParameter( const int type, const unsigned int pkey, const unsigned int id, const unsigned int position,
    const unsigned int delay, const time_t timeout, const std::string name, const std::string description,
    const std::string graph, const bool skip, const bool skippable, const bool ignoreFailure ) :
type( type ),
pkey( pkey ),
id( id ),
position( position ),
delay( delay ),
timeout( timeout ),
name( name.c_str() ),
description( description.c_str() ),
graph( graph.c_str() ),
skip( skip ),
skippable( skippable ),
ignoreFailure( ignoreFailure )
{
}

BasicParameter::~BasicParameter()
{
}

}