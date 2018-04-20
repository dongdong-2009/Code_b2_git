This directory will house the application used to monitor (evesdrop) radio calls.  It will be called from the comms menu.

NOTE: The file included directly at:
bus\radio\RadioSearchPage\src\SearchPage.cpp
Is included because taking it out will result in the following linker errors:

TA_Bus.lib(SearchPage.obj) : error LNK2001: unresolved external symbol "protected: static struct AFX_MSGMAP const  CDialog::messageMap" (?messageMap@CDialog@@1UAFX_MSGMAP@@B)
TA_Bus.lib(SearchPage.obj) : error LNK2001: unresolved external symbol "public: static struct CRuntimeClass const  CDialog::classCDialog" (?classCDialog@CDialog@@2UCRuntimeClass@@B)

There is probably a better solution to overcome this (not sure what though)..

The bus\radio\RadioSearchPage\src\RadioSearchPage.rc file is directly included
from within our own .rc file, to ensure resources can be loaded at runtime