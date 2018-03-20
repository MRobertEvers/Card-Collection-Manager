#include "viHistoryViewer.h"
#include "vicBlogItem.h"


viHistoryViewer::viHistoryViewer( wxWindow* aptParent,
                                  wxWindowID aiID,
                                  std::shared_ptr<CollectionInterface> aptInterface)
   : wxFrame( aptParent, aiID, "History" ), m_ptInterface( aptInterface )
{
   wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
   this->SetSizer( sizer );

   auto mapHist = aptInterface->GetHistoryGroups();
   for( auto& histItem : mapHist )
   {
      vicBlogItem* blogItem = new vicBlogItem( this, "Test", histItem.second );
      sizer->Add( blogItem, wxSizerFlags( 1 ).Expand() );
   }
}


viHistoryViewer::~viHistoryViewer()
{
}
