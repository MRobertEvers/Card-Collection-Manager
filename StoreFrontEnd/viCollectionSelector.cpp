#include "viCollectionSelector.h"
#include "vicCollectionSelectorSelection.h"
#include "StoreFrontEnd.h"


viCollectionSelector::viCollectionSelector( wxPanel* aptParent, wxWindowID aiWID )
   : wxFrame(aptParent, aiWID, "Collection Selector")
{
   wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
   this->SetSizer( boxSizer );

   wxScrolledWindow* wxScroll = new wxScrolledWindow( this, aiWID, wxDefaultPosition, wxDefaultSize, wxLB_ALWAYS_SB );
   wxBoxSizer* scollboxSizer = new wxBoxSizer( wxVERTICAL );
   wxScroll->SetSizer( scollboxSizer );
   wxScroll->SetScrollRate( 10, 20 );
   boxSizer->Add( wxScroll, wxSizerFlags( 1 ).Expand() );

   auto ptSFE = StoreFrontEnd::Client();
   auto ptSF = StoreFrontEnd::Server();
   auto szCollectionsPath = ptSF->GetCollectionsDirectory();
   auto vecFiles = ptSFE->GetTextFilesInDirectory( szCollectionsPath );
   for( auto& szFile : vecFiles )
   {
      auto vecPeek = ptSF->GetPeekValues( szFile.ToStdString() );
      auto szImagePeek = StringInterface::FindTagInList( vecPeek, "Icon" );
      auto szNamePeek = StringInterface::FindTagInList( vecPeek, "Name" );

      if( szNamePeek != "" )
      {
         auto szSetImage = ptSF->GetDefaultIdentifyingAttributeValue( szImagePeek, "set" );
         auto szMUDImage = ptSF->GetDefaultIdentifyingAttributeValue( szImagePeek, "multiverseid" );
         auto ptNewSelection = new vicCollectionSelectorSelection( wxScroll, szNamePeek, szImagePeek, szSetImage, szMUDImage );
         wxScroll->GetSizer()->Add( ptNewSelection, wxSizerFlags( 1 ).Expand() );
      }
   }
}


viCollectionSelector::~viCollectionSelector()
{
}
