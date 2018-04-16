/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  oliverk
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#if !defined(ITrainStaticGroupComboEventListener_H)
#define ITrainStaticGroupComboEventListener_H


namespace TA_IRS_Bus
{
    /**
     * This is implemented by the TrainSelectorGUI to be notified of when the selection or text in the static group combo changes.
     * @author oliverk
     * @version 1.0
     * @created 18-Mar-2008 5:46:20 PM
     */
    class ITrainStaticGroupComboEventListener
    {

    public:

        ITrainStaticGroupComboEventListener() {

        }

        virtual ~ITrainStaticGroupComboEventListener() {

        }


        /**
         * Called by the static group combo when its selection is changed.
         */
        virtual void selectionChangedStaticGroupCombo() =0;


        /**
         * Called by the static group combo when its edit box is changed.
         */
        virtual void editChangedStaticGroupCombo() =0;

    };

}
#endif // !defined(ITrainStaticGroupComboEventListener_H)
