#include "ImageViewer.h"
#include "../StoreFrontEnd/StoreFrontEnd.h"

wxBEGIN_EVENT_TABLE( ImageViewer, wxPanel )
wxEND_EVENT_TABLE()

ImageViewer::ImageViewer( wxWindow* aptParent, wxWindowID aiWID, bool abDoScale )
   : wxPanel( aptParent, aiWID ), m_ptImageWrapper( new ImageWrapper( this, 3, abDoScale ) ), m_bDoScale(abDoScale)
{
   wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
   this->SetSizer( boxSizer );
   boxSizer->Add( m_ptImageWrapper, wxSizerFlags( 1 ).Expand() );
}


ImageViewer::~ImageViewer()
{
}


bool
ImageViewer::DisplayImage( const wxString& aszFilePath )
{
   wxLog::EnableLogging( false );
   bool bRetVal = m_ptImageWrapper->SetImage( aszFilePath );
   if( bRetVal )
   {
      this->SetSize( m_ptImageWrapper->GetSize() );
      this->SetSizeHints( m_ptImageWrapper->GetSize() );
   }
   wxLog::EnableLogging( true );
   return bRetVal;
}
