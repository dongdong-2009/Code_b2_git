All C++ developers,
> 
> I have written a few perl scripts to help me generate C++ 
> source/header stubs. These might be of use for you too.
> 
> They are:
> 	- header_gen.pl (generates an basic header file for a class), 
> 	- interface_gen.pl (generates a header file for a class 
> that inherits from one or more classes)
> 	- source_gen.pl (generates cpp file with appropriate 
> method stubs from a header file etc.)
> 
> 
> header_gen.pl
> --------------------
> Usage:
> 	 perl header_gen.pl <class name> [<namespace1>] [<namespace2>]
> 
> Creates a new file with:
> 	- header guard
> 	- file comments as per coding guidlines
> 	- namespaces (optional)
> 	- default constructor/virtual destructor
> 	- default method comments as per coding guidelines
> 
> 
> interface_gen.pl
> ----------------------
> Usage:
> 	 perl interface_gen.pl <class name> <namespace1> 
> <namespace2> <path1><interface1> [<path2><interface2> .... 
> <pathN><interfaceN>]
> 
> Creates a new file called <class name>.h with:
> 	- All items from header_gen.pl
> 	- #include <interface.h>
> 	- makes <class name> inherit from each interface
> 	- All the pure virtual methods from each interface into the file
> 	- All the method header comments for each pure virtual 
> methods for each interface
> 
> Example:
> 	perl interface_gen.pl MyThread TA_App TA_MyApp 
> ../../../core/threads/src/Thread 
> ../../../core/synchronisation/src/ThreadGuard
> 
> 	generates a file called MyThread.h that:
> 		- #includes <Thread.h> and <ThreadGuard.h>
> 		- inherits from TA_Core::TA_Thread::Thread and 
> TA_Core::TA_Synchronisation::ThreadGuard
> 		- has all the pure virtual thread methods with 
> comments (ie run() and terminate())
> 	
> 
> source_gen.pl
> --------------------
> Usage:
> 	perl source_gen.pl <class name>
> 
> Creates a new file called <class name>.cpp with
> 	- #include <class name.h>
> 	- empty methods stubs for each method in <class name.h>
> 

Daniel Hoey