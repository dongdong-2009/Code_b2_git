/**
  * The source code in this file is the property of
  * MI Services Group (Australia) and is not for
  * redistribution in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/history/traffic_historian/src/HistorianMain.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
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