#include "app\radio\radio_manager_gui\src\RadioStatusPage\ResourceButton\SameWidthBorderDecorator.h"



SameWidthBorderDecorator::SameWidthBorderDecorator(AbstractDecoratorButton* pComponent) : m_cWidth(0), 
m_cBorderColor(::GetSysColor(COLOR_BTNFACE)), 
ButtonDecorator(pComponent)
{

}

SameWidthBorderDecorator::~SameWidthBorderDecorator()
{

}

void SameWidthBorderDecorator::Draw(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if ( m_pcomponent != NULL )
	{
		m_pcomponent->Draw(lpDrawItemStruct);
	}

	CDC* pDC   = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rect = lpDrawItemStruct->rcItem;
	UINT state = lpDrawItemStruct->itemState;

	int width = m_cWidth;
	rect.DeflateRect( CSize(GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE)));

	CPen pen(PS_SOLID, 1, m_cBorderColor);
	CPen *oldPen = pDC->SelectObject(&pen);

	// Make the end points to draw the lines. We use 1 pixel wide pen
	// to make square border.
	std::vector<POINT> points;
	for (int i = 0; i < width; ++i)
	{
		POINT topLeft, topRight, bottomRight, bottomLeft;
		topLeft.x = rect.left + i;
		topLeft.y = rect.top + i;
		topRight.x = rect.right - i - 1;
		topRight.y = topLeft.y;
		bottomRight.x = topRight.x;
		bottomRight.y = rect.bottom - i - 1;
		bottomLeft.x = topLeft.x;
		bottomLeft.y = bottomRight.y;

		points.push_back(topLeft);
		points.push_back(topRight);
		points.push_back(bottomRight);
		points.push_back(bottomLeft);
		points.push_back(topLeft);
	}

	pDC->Polyline(&points[0], points.size());

	pDC->SelectObject(oldPen);

	if (state & ODS_FOCUS)
	{
		const int edge = 6;
		rect.top += edge;
		rect.left += edge;
		rect.bottom -= edge;
		rect.right -=edge;

		DrawFocusRect(pDC->m_hDC, &rect);
	}
}

void SameWidthBorderDecorator::setBorderProperties(COLORREF borderColor, int width)
{
	m_cBorderColor = borderColor;
	m_cWidth = width;
}