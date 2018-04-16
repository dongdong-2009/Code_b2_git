#pragma once

class PlanStep
{
public:
    PlanStep( const unsigned long pkey, const unsigned long type, const unsigned long plan, const int position, const int id = 0 );
    ~PlanStep( void );

public:
    unsigned long pkey;
    unsigned long type;
    unsigned long plan;
    int position;
    int id;
};
