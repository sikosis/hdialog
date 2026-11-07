#include "IconView.h"

#include <Bitmap.h>

IconView::IconView(BBitmap* bitmap)
	:
	BView("icon", B_WILL_DRAW),
	fBitmap(bitmap)
{
	SetViewUIColor(B_PANEL_BACKGROUND_COLOR);
	SetExplicitPreferredSize(BSize(96, 96));
	SetExplicitMaxSize(BSize(96, 96));
}
//---------------------------------------------------------------------------------------------------------------------------------//


IconView::~IconView()
{
	delete fBitmap;
}
//---------------------------------------------------------------------------------------------------------------------------------//


void
IconView::Draw(BRect updateRect)
{
	BView::Draw(updateRect);
	if (fBitmap != nullptr)
		DrawBitmap(fBitmap, fBitmap->Bounds(), Bounds(), B_FILTER_BITMAP_BILINEAR);
}
//---------------------------------------------------------------------------------------------------------------------------------//
