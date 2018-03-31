#pragma once
#include <wx/wxprec.h>

class wxManualText : public wxWindow
{
public:
   wxManualText( wxWindow* aptParent,
                 const wxString& aszTitle,
                 const wxFont& aFont = wxFont());
   ~wxManualText();

private:
   DECLARE_EVENT_TABLE()

   wxString m_szText;
   wxFont m_wxFont;
   wxBitmap m_backBitmap;

   void bufferBitmap( wxSize size );

   void onPaint( wxPaintEvent & evt );
   void onResize( wxSizeEvent &ev );

   void render( wxDC& dc );
};

