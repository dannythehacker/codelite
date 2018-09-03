#include "clToolBarSpacer.h"

clToolBarSpacer::clToolBarSpacer(clToolBar* parent)
    : clToolBarButtonBase(parent, wxID_SEPARATOR, wxNullBitmap, "", kDisabled | kSpacer)
{
}

clToolBarSpacer::~clToolBarSpacer() {}

wxSize clToolBarSpacer::CalculateSize(wxDC& dc) const
{
    return wxSize(CL_TOOL_BAR_X_MARGIN * 10 + 1, -1);
}

void clToolBarSpacer::Render(wxDC& dc, const wxRect& rect)
{    
    wxCoord xx = rect.GetX();
    xx += CL_TOOL_BAR_X_MARGIN * 9;
    
    wxColour c = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
    dc.SetPen(c.ChangeLightness(150));
    dc.DrawLine(xx, rect.GetY() + 2, xx, rect.GetY() + rect.GetHeight() - 2);
}
