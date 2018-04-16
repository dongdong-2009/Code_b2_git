/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/mfc_extensions/src/decorator_button/BackgroundDecorator.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 * 
 * Manipulate the background colour, i.e. the background colour can be specified.
 * It is also possible to display the "default" background colour. This can be
 * used to simulate "flashing" behaviour by alternating between the default and
 * specified background colour on a timed interval.
 */

#if !defined(BackgroundDecorator_A8988A2F_ACD0_4bd9_9586_2EDF3CCC39DE__INCLUDED_)
#define BackgroundDecorator_A8988A2F_ACD0_4bd9_9586_2EDF3CCC39DE__INCLUDED_

#include "bus/mfc_extensions/src/decorator_button/ButtonDecorator.h"

class AbstractDecoratorButton;

namespace TA_Base_Bus
{
    class AFX_EXT_CLASS BackgroundDecorator : public ButtonDecorator 
    {

    public:
	    BackgroundDecorator(AbstractDecoratorButton* pComponent);
	    virtual ~BackgroundDecorator();

    public:
	    virtual void Draw(LPDRAWITEMSTRUCT lpDrawItemStruct);

	    void setBackgroundColor(COLORREF backgroundColor);


    private:
	    COLORREF m_backgroundColor;
	    COLORREF m_defaultColor;

    };
};
#endif // !defined(BackgroundDecorator_A8988A2F_ACD0_4bd9_9586_2EDF3CCC39DE__INCLUDED_)
