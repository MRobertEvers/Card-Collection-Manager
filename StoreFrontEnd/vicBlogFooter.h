#pragma once
#include <wx/wxprec.h>

class vicBlogFooter : public wxPanel
{
public:
   vicBlogFooter( wxWindow* aptParent, const wxString& aszText );
   ~vicBlogFooter();

private:
   DECLARE_EVENT_TABLE()

   wxString m_szTitle;
   wxBitmap m_backBitmap;

   void bufferBitmap( wxSize size );

   void onPaint( wxPaintEvent & evt );
   void onResize( wxSizeEvent &ev );

   void render( wxDC& dc );
};

