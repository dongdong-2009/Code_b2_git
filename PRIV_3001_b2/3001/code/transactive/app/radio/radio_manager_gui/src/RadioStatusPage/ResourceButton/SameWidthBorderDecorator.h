#pragma once

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "bus/mfc_extensions/src/decorator_button/ButtonDecorator.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"



class SameWidthBorderDecorator : public TA_Base_Bus::ButtonDecorator
{
	public:	    
		SameWidthBorderDecorator(AbstractDecoratorButton* pComponent);
	    virtual ~SameWidthBorderDecorator();
		
	    virtual void Draw(LPDRAWITEMSTRUCT lpDrawItemStruct);
	    void setBorderProperties(COLORREF borderColor, int width);
		
    protected:

	    COLORREF m_cBorderColor;

	    int m_cWidth;
};