#pragma once
#include <wx/wxprec.h>

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

   void paintEvent( wxPaintEvent & evt );
   void paintNow();

   void render( wxDC& dc );
};

