/**
  * The source code in this file is the property of
  * MI Services Group (Australia) and is not for
  * redistribution in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/history/traffic_historian/src/HistorianMain.cpp $
  * @author Katherine Thomson
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:
  *
  * The HistorianMain object starts Historian running.
  *
  */ 


// Ignore STL warnings on Windows.

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include "Historian.h"

int main (int argc, char *argv[])
{
	Historian historian;
   	return historian.Run(argc, argv);
}