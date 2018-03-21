#include "wxManualText.h"

BEGIN_EVENT_TABLE( wxManualText, wxPanel )
EVT_PAINT( wxManualText::onPaint )
EVT_SIZE( wxManualText::onResize )
END_EVENT_TABLE()

wxManualText::wxManualText( wxWindow* aptParent,
                            const wxString& aszTitle,
                            const wxFont& aFont )
   : wxWindow(aptParent, wxID_ANY), m_szText(aszTitle), m_wxFont(aFont)
{
   wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
   this->SetSizer( sizer );

   this->SetSizeHints( wxSize( 10, 10 ) );
}


wxManualText::~wxManualText()
{
}


void
wxManualText::bufferBitmap( wxSize size )
{
   // make sure we do not attempt to create a bitmap 
   // with invalid size (width and/or height < 1)
   size.IncTo( wxSize( 1, 1 ) );

   m_backBitmap.Create( size.GetWidth(), size.GetHeight() );

   wxMemoryDC mdc;
   mdc.SelectObject( m_backBitmap );
   render( mdc );
   Refresh(true);
}

void
wxManualText::onPaint( wxPaintEvent & evt )
{
   wxPaintDC dc( this );
   dc.DrawBitmap( m_backBitmap, 0, 0, false );
}

void
wxManualText::onResize( wxSizeEvent &ev )
{
   bufferBitmap( ev.GetSize() );
}

void
wxManualText::render( wxDC&  dc )
{
   dc.GradientFillLinear( this->GetSize(), wxColour( 0, 0, 0 ), wxColour( 0, 0, 0 ), wxNORTH );
   dc.SetFont( m_wxFont );
   dc.DrawText( m_szText, 0, 0 );
}

