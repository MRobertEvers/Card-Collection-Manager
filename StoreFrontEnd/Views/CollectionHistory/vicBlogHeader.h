#pragma once
#include <wx/wxprec.h>
#include <wx/bitmap.h>

class vicBlogHeader : public wxPanel
{
public:
   vicBlogHeader( wxWindow* aptParent,
                  const wxString& aszTitle,
                  const wxString& aszSubTitle );
   ~vicBlogHeader();

private:
   DECLARE_EVENT_TABLE()

   wxString m_szTitle;
   wxString m_szSubTitle;
   wxBitmap m_backBitmap;

   void bufferBitmap( wxSize size );

   void onPaint( wxPaintEvent & evt );
   void onResize( wxSizeEvent &ev );

   void render( wxDC& dc );
};

