/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mfc_extensions/src/decorator_button/TextDecorator.h $
 * @author:  
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 * 
 * Manipulate the text position.
 */

#if !defined(TextDecorator_3EB524E2_D450_4786_8AE4_73FB349B4CF2__INCLUDED_)
#define TextDecorator_3EB524E2_D450_4786_8AE4_73FB349B4CF2__INCLUDED_

#include "bus/mfc_extensions/src/decorator_button/ButtonDecorator.h"

namespace TA_Base_Bus
{
    class AbstractDecoratorButton;

    class AFX_EXT_CLASS TextDecorator : public ButtonDecorator 
    {

    public:
	    /**
	      * Constructor
	      * 
	      */
	    TextDecorator(AbstractDecoratorButton* pcomponent);

	    /**
	      * Destructor
	      * 
	      */
	    virtual ~TextDecorator();

	    /**
	      * Draw
	      * 
	      * Perform drawing, this function will be called by the concrete decorator class
	      */
	    virtual void Draw(LPDRAWITEMSTRUCT lpDrawItemStruct);


	    /**
	      * setTextPosition
	      * 
	      * Change text position
	      */
	    void setTextPosition(int x, int y);


	    /**
	      * setTextColor
	      * 
	      * Change text color
	      */
	    void setTextColor(COLORREF textColor);

    private:
        TextDecorator();
        TextDecorator(const TextDecorator&);
        void operator=(const TextDecorator&);
    
    private:
	    int m_xPos;
	    int m_yPos;
        COLORREF m_textColor;
    };
};

#endif // !defined(TextDecorator_3EB524E2_D450_4786_8AE4_73FB349B4CF2__INCLUDED_)
