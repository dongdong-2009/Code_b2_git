/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_gui/src/HelpLauncher.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This singleton contains the methods used to launch TransActive dialogs.
  * These include About boxes, help and context specific help.
  *
  */


#if !defined(HELPLAUNCHER_46EAE0DE_46D9_487b_970C_31C82D0E44C2__INCLUDED_)
#define HELPLAUNCHER_46EAE0DE_46D9_487b_970C_31C82D0E44C2__INCLUDED_

#include <string>

namespace TA_Base_Bus
{
    class HelpLauncher
    {
    public:

        /**
         * Destructor
         */
        virtual ~HelpLauncher() { }


        /**
         * getInstance
         * 
         * This creates an instance of this class on the first call. Following
         * this it will return the created instance on each subsequent call.
         *
         * @return A reference to a HelpLauncher object.
         *
         */
        static HelpLauncher& getInstance();


        /**
          * displayAboutBox
          * 
          * Displays the about box containing all information for this application.
          *
          */
        void displayAboutBox();


        /**
          * displayContextSpecificHelp
          * 
          * Displays the help and opens a relevant section of the help.
          * @param unsigned int itemID - The resource ID of the GUI element for which 
											help must be displayed (e.g. dialog ID)
          *
          */
        void displayContextSpecificHelp(unsigned int itemID);

        /**
          * displayContextSpecificHelp (2)
          * 
          * Displays the specified help file and opens the section relevant to the
          * specified resource.
          * @param unsigned int itemID - The resource ID of the GUI element for which 
											help must be displayed (e.g. dialog ID)
          *
          */
        void displayContextSpecificHelp(unsigned int itemID, const std::string& filePath);
        
        /**
          * displayHelp
          * 
          * Displays the help. This will not change to a particular context
          *
          */
        void displayHelp(const bool topMost = false);

        /**
          * displayHelp
          * 
          * Displays the specified help file
          *
          */
        void displayHelp(const std::string& filePath, const bool topMost = false);


    private:
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        HelpLauncher(){ };
        HelpLauncher(const HelpLauncher& rhs){ };
        HelpLauncher& operator=(const HelpLauncher&){ };

        static HelpLauncher* s_instance;

    };
}

#endif // !defined(HELPLAUNCHER_46EAE0DE_46D9_487b_970C_31C82D0E44C2__INCLUDED_)
