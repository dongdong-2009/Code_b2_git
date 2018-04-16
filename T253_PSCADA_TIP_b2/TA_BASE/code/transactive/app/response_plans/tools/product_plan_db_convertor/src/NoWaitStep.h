#pragma once

class NoWaitStep
{
public:
    NoWaitStep( const unsigned long pkey, const unsigned long type, const unsigned long plan, const int position, const bool nowait );
    ~NoWaitStep( void );

public:
    unsigned long pkey;
    unsigned long type;
    unsigned long plan;
    int position;
    bool nowait;
};
