#include "viCollectionSelector.h"
#include "StoreFrontEnd.h"
#include "vicCollectionPeeker.h"

wxBEGIN_EVENT_TABLE( viCollectionSelector, wxFrame )
EVT_SIZE( viCollectionSelector::onResize )
EVT_BUTTON( viCollectionSelector::Load_Button, viCollectionSelector::onLoadButton )
EVT_GRID_RANGE_SELECT( viCollectionSelector::onSelectRange )
EVT_GRID_LABEL_LEFT_CLICK( viCollectionSelector::onSelectColumn )
EVT_GRID_CELL_LEFT_CLICK( viCollectionSelector::onItemSelection )
wxEND_EVENT_TABLE()

viCollectionSelector::viCollectionSelector( wxPanel* aptParent, wxWindowID aiWID )
   : wxFrame(aptParent, aiWID, "Collection Selector")
{
   wxBoxSizer* boxSizer = new wxBoxSizer( wxHORIZONTAL );
   this->SetSizer( boxSizer );

   wxPanel* leftSide = new wxPanel( this, 1 );
   wxBoxSizer* leftSize = new wxBoxSizer( wxVERTICAL );
   leftSide->SetSizer( leftSize );

   m_Peeker = new vicCollectionPeeker( leftSide );
   leftSize->Add( m_Peeker, wxSizerFlags( 1 ).Expand() );

   wxButton* loadButt = new wxButton( leftSide, Load_Button, "Load" );
   leftSize->Add( loadButt, wxSizerFlags( 1 ).Expand() );

   boxSizer->Add( leftSide, wxSizerFlags( 1 ).Expand() );

   m_Grid = new wxGrid( this, aiWID, wxDefaultPosition, wxDefaultSize, wxBORDER );
   m_Grid->CreateGrid( 0, 1 );
   m_Grid->DisableDragColSize();
   m_Grid->DisableDragRowSize();
   m_Grid->HideColLabels();
   m_Grid->HideRowLabels();
   m_Grid->EnableGridLines( false );
   boxSizer->Add( m_Grid, wxSizerFlags( 1 ).Expand() );

   peekCollections();

   if( m_mapPeekedCollections.size() > 0 )
   {
      auto iter_first = m_mapPeekedCollections.begin();
      setPeeking( iter_first->first );
   }
}


viCollectionSelector::~viCollectionSelector()
{
}

void 
viCollectionSelector::setPeeking( const wxString& aszPeek )
{
   auto iter_find = m_mapPeekedCollections.find( aszPeek );
   if( m_mapPeekedCollections.end() != iter_find )
   {
      m_Peeker->SetPeek( aszPeek, iter_find->second );
      m_szPeeking = aszPeek;
   }
}

void
viCollectionSelector::onItemSelection( wxGridEvent& awxEvt )
{
   auto szItem = m_Grid->GetCellValue( awxEvt.GetRow(), awxEvt.GetCol() );
   setPeeking( szItem );
}

void
viCollectionSelector::onResize( wxSizeEvent& awxEvt )
{
   awxEvt.Skip();
}

void
viCollectionSelector::onSelectRange( wxGridRangeSelectEvent& awxEvt )
{
   awxEvt.StopPropagation();
}

void
viCollectionSelector::onSelectColumn( wxGridEvent& awxEvt )
{
   awxEvt.StopPropagation();
}

void 
viCollectionSelector::onLoadButton( wxCommandEvent& awxEvt )
{
   auto szFile = m_mapFiles[m_szPeeking];
   awxEvt.SetString( szFile );
   awxEvt.Skip();
}

void 
viCollectionSelector::peekCollections()
{
   auto ptSFE = StoreFrontEnd::Client();
   auto ptSF = StoreFrontEnd::Server();
   auto szCollectionsPath = ptSF->GetCollectionsDirectory();
   auto vecFiles = ptSFE->GetTextFilesInDirectory( szCollectionsPath );
   for( auto& szFile : vecFiles )
   {
      auto vecPeek = ptSF->GetPeekValues( szFile.ToStdString() );
      auto szNamePeek = StringInterface::FindTagInList( vecPeek, "Name" );
      if( szNamePeek != "" )
      {
         m_mapFiles.insert( std::make_pair( szNamePeek, wxString( ptSF->GetCollectionsDirectory() + "\\" + szFile ) ) );
         m_mapPeekedCollections.insert( std::make_pair( wxString(szNamePeek), vecPeek ) );
      }
   }

   for( auto& col : m_mapPeekedCollections )
   {
      addOption( col.first );
   }
}

void 
viCollectionSelector::addOption( const wxString& buf )
{
   int iRow = m_Grid->GetNumberRows();
   m_Grid->InsertRows( m_Grid->GetNumberRows() );
   m_Grid->SetCellValue( iRow, 0, buf );
   m_Grid->SetReadOnly( iRow, 0, true );
   m_Grid->SetCellRenderer( iRow, 0, new wxGridCellAutoWrapStringRenderer() );
}