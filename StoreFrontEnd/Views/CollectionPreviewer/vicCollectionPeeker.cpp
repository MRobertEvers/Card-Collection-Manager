#include "vicCollectionPeeker.h"
#include "../ImageViewer/ImageViewer.h"
#include "../StoreFrontEnd/StoreFrontEnd.h"


vicCollectionPeeker::vicCollectionPeeker( wxWindow* aptParent )
   : wxPanel(aptParent, 2)
{
   wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
   this->SetSizer( boxSizer );
   this->SetSizeHints( 223, 500 );
}


vicCollectionPeeker::~vicCollectionPeeker()
{
}

void 
vicCollectionPeeker::SetPeek( const wxString& aszColName,
                              const wxString& aszIconName,
                              const std::vector<std::pair<std::string, std::string>> amapPeekVals )
{

   auto ptSFE = StoreFrontEnd::Client();
   auto ptSF = StoreFrontEnd::Server();

   if( m_Viewer == NULL )
   {
      m_Viewer = new ImageViewer( this, 1 );
      this->GetSizer()->Add( m_Viewer, wxSizerFlags( 1 ).Expand() );
   }
   auto szSet = ptSF->GetDefaultIdentifyingAttributeValue( aszIconName.ToStdString(), "set" );

   auto ptse = StoreFrontEnd::Server();
   auto szPath = ptse->GetImageFilePath( aszIconName.ToStdString(), szSet );
   m_Viewer->DisplayImage( szPath );

   setPeek( aszColName, amapPeekVals );
}

void 
vicCollectionPeeker::SetPeek( const wxString& aszColName,
                              const std::vector<std::pair<std::string, std::string>> amapPeekVals )
{
   auto ptSF = StoreFrontEnd::Server();
   auto szIcon = StringInterface::FindTagInList( amapPeekVals, "Icon" );
   if( szIcon != "" )
   {
      return SetPeek( aszColName, szIcon, amapPeekVals );
   }

   if( m_Viewer != NULL )
   {
      this->GetSizer()->Detach( m_Viewer );
      m_Viewer->Destroy();
      m_Viewer = NULL;
   }

   setPeek( aszColName, amapPeekVals );
}

void 
vicCollectionPeeker::setPeek( const wxString& aszColName,
                              const std::vector<std::pair<std::string, std::string>> amapPeekVals )
{
   if( m_Grid != NULL )
   {
      this->GetSizer()->Detach( m_Grid );
      m_Grid->Destroy();
      m_Grid = NULL;
   }

   m_Grid = new wxGrid( this, 1, wxDefaultPosition, wxDefaultSize, wxBORDER );
   m_Grid->CreateGrid( 0, 1 );
   m_Grid->DisableDragColSize();
   m_Grid->DisableDragRowSize();
   m_Grid->HideRowLabels();
   m_Grid->EnableGridLines( false );
   this->GetSizer()->Add( m_Grid, wxSizerFlags( 1 ).Expand() );

   m_Grid->SetColLabelValue( 0, aszColName );
   for( auto& peek : amapPeekVals )
   {
      addOption( peek.first + ":" + peek.second );
   }

   m_Grid->AutoSizeColumns();
   this->Layout();
   this->GetParent()->Layout();
}

void
vicCollectionPeeker::addOption( const wxString& buf )
{
   int iRow = m_Grid->GetNumberRows();
   m_Grid->InsertRows( m_Grid->GetNumberRows() );
   m_Grid->SetCellValue( iRow, 0, buf );
   m_Grid->SetReadOnly( iRow, 0, true );
   m_Grid->SetCellRenderer( iRow, 0, new wxGridCellAutoWrapStringRenderer() );
}