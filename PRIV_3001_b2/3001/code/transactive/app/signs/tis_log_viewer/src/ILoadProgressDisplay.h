/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_log_viewer/src/ILoadProgressDisplay.h $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Implemented by the GUI that will display loading progress.
  */


#if !defined(ILoadProgressDisplay_H)
#define ILoadProgressDisplay_H


namespace TA_IRS_App
{
    class ILoadProgressDisplay
    {

    public:

        /**
          * showProgressBar
          *
          * Called to show the progress bar.
          * If the bar is being shown, initialise it with some values.
          *
          * @param show true if the bar should be shown
          * @param min  the minimum value (0 usually does fine)
          * @param max  the maximum value
          * @param step the amount to increment
          * @param pos  the initial position of the bar (between min and max).
          */
        virtual void showProgressBar( bool show,
                                      int min = 0,
                                      int max = 100,
                                      int step = 1,
                                      int pos = 0) = 0;


        /**
          * stepProgressBar
          *
          * Increment the progress bar position
          */
        virtual void stepProgressBar() = 0;

    };

} // TA_App

#endif // !defined(ILoadProgressDisplay_H)
