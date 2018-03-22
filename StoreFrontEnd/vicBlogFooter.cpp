#include "vicBlogFooter.h"


BEGIN_EVENT_TABLE( vicBlogFooter, wxPanel )
EVT_PAINT( vicBlogFooter::onPaint )
EVT_SIZE( vicBlogFooter::onResize )
END_EVENT_TABLE()

vicBlogFooter::vicBlogFooter( wxWindow* aptParent, const wxString& aszText )
   : wxPanel(aptParent), m_szTitle(aszText)
{
   wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
   this->SetSizer( sizer );

   wxSize size( 200, 20 );

   this->SetSize( size );
   this->SetSizeHints( size );

   bufferBitmap( this->GetSize() );
}


vicBlogFooter::~vicBlogFooter()
{
}


void
vicBlogFooter::bufferBitmap( wxSize size )
{
   // make sure we do not attempt to create a bitmap 
   // with invalid size (width and/or height < 1)
   size.IncTo( wxSize( 1, 1 ) );

   m_backBitmap.Create( size.GetWidth(), size.GetHeight() );

   wxMemoryDC mdc;
   mdc.SelectObject( m_backBitmap );
   render( mdc );
   Refresh();
}

void
vicBlogFooter::onPaint( wxPaintEvent & evt )
{
   wxPaintDC dc( this );
   dc.DrawBitmap( m_backBitmap, 0, 0, false );
}

void
vicBlogFooter::onResize( wxSizeEvent &ev )
{
   bufferBitmap( ev.GetSize() );
}

void
vicBlogFooter::render( wxDC&  dc )
{
   dc.SetBrush( wxBrush(wxColour( 222, 222, 222 )) );
   dc.SetPen( wxPen( wxColour( 155, 155, 155 ), 2 ) );
   dc.DrawRectangle( wxPoint(0,0), this->GetSize() );

   dc.SetFont( wxFont( wxFontInfo( 9 ).FaceName( "Trebuchet MS" ) ) );
   auto extent = dc.GetTextExtent( m_szTitle );
   dc.DrawText( m_szTitle, 
      this->GetSize().GetWidth() - extent.GetWidth()-4,
      this->GetSize().GetHeight()/2 - extent.GetHeight()/2 );
}

