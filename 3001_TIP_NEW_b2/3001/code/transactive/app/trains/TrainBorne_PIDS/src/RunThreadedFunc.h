#ifndef _RUNTHREADEDFUNC_H
#define _RUNTHREADEDFUNC_H

#include "boost/function.hpp"

typedef boost::function<void ()> ThreadFuncType;
void runThreadedFunc(ThreadFuncType func);

#endif