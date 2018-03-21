#include "vicBlogHeader.h"

BEGIN_EVENT_TABLE( vicBlogHeader, wxPanel )
EVT_PAINT( vicBlogHeader::onPaint )
EVT_SIZE( vicBlogHeader::onResize )
END_EVENT_TABLE()

vicBlogHeader::vicBlogHeader( wxWindow* aptParent,
                              const wxString& aszTitle,
                              const wxString& aszSubTitle )
   : wxPanel(aptParent), m_szTitle(aszTitle), m_szSubTitle(aszSubTitle)
{
   wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
   this->SetSizer( sizer );

   wxSize size;
   if( aszSubTitle == "" )
   {
      size = wxSize( 200, 20 );

   }
   else
   {
      size = wxSize( 200, 32 );
   }

   this->SetSize( size );
   this->SetSizeHints( size );

   bufferBitmap( this->GetSize() );
}


vicBlogHeader::~vicBlogHeader()
{

}

void 
vicBlogHeader::bufferBitmap( wxSize size )
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
vicBlogHeader::onPaint( wxPaintEvent & evt )
{
   wxPaintDC dc( this );
   dc.DrawBitmap( m_backBitmap, 0, 0, false );
}

void 
vicBlogHeader::onResize( wxSizeEvent &ev )
{
   bufferBitmap(ev.GetSize());
}

void 
vicBlogHeader::render( wxDC&  dc )
{
   dc.GradientFillLinear( this->GetSize(), wxColour( 255, 255, 255 ), wxColour( 155, 155, 155 ), wxNORTH );

   dc.SetFont( wxFont( wxFontInfo( 9 ).FaceName( "Trebuchet MS" ) ) );
   dc.DrawText( m_szTitle, 0, 2 );

   if( m_szSubTitle != "" )
   {
      dc.DrawText( m_szSubTitle, 0, 14 );
   }
}

