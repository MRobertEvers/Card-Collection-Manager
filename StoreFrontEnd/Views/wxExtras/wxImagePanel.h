#pragma once

#include <wx/wx.h>
#include <wx/sizer.h>

class wxImagePanel : public wxPanel
{
   wxImage image;
   wxBitmap resized;
   int w, h;

public:
   wxImagePanel(wxWindow* parent, wxString file, wxBitmapType format, bool abDoScale);

   void paintEvent(wxPaintEvent & evt);
   void paintNow();
   void OnSize(wxSizeEvent& event);
   void render(wxDC& dc);

   bool IsOk;
   bool DoScale;

   DECLARE_EVENT_TABLE()
};

