#include "vicBlogHeader.h"

BEGIN_EVENT_TABLE( vicBlogHeader, wxPanel )
EVT_PAINT( vicBlogHeader::paintEvent )
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
}


vicBlogHeader::~vicBlogHeader()
{

}

void 
vicBlogHeader::paintEvent( wxPaintEvent & evt )
{
   // depending on your system you may need to look at double-buffered dcs
   wxPaintDC dc( this );
   render( dc );
}

void
vicBlogHeader::paintNow()
{
   // depending on your system you may need to look at double-buffered dcs
   wxClientDC dc( this );
   render( dc );
}

void 
vicBlogHeader::render( wxDC&  dc )
{
   dc.GradientFillLinear(this->GetSize(), wxColour( 255, 255, 255 ), wxColour( 155, 155, 155 ), wxNORTH );

   dc.SetFont( wxFont( wxFontInfo( 9 ).FaceName( "Trebuchet MS" ) ) );
   dc.DrawText( m_szTitle, 0, 2 );

   dc.SetFont( wxFont( wxFontInfo( 9 ).FaceName( "Trebuchet MS" ) ) );
   dc.DrawText( m_szSubTitle, 0, 14 );
}

