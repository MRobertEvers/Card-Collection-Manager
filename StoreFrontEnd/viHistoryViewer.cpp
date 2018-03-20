#include "viHistoryViewer.h"
#include "vicBlogItem.h"

viHistoryViewer::viHistoryViewer( wxWindow* aptParent,
                                  wxWindowID aiID,
                                  std::shared_ptr<CollectionInterface> aptInterface)
   : wxFrame( aptParent, aiID, "History" ), m_ptInterface( aptInterface )
{
   wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
   this->SetSizer( sizer );

   wxScrolledWindow* scrollWindow = new wxScrolledWindow( this );
   scrollWindow->SetScrollRate( 10, 20 );
   wxBoxSizer* scrollsizer = new wxBoxSizer( wxVERTICAL );
   scrollWindow->SetSizer( scrollsizer );

   auto mapHist = aptInterface->GetHistoryGroups();

   // Want to start with most recent.
   auto riter_hist = mapHist.rbegin();
   for( ; riter_hist != mapHist.rend(); ++riter_hist )
   {
      vicBlogItem* blogItem = new vicBlogItem( scrollWindow, "Test", riter_hist->second );
      scrollsizer->Add( blogItem, wxSizerFlags( 0 ).Expand() );
   }

   sizer->Add( scrollWindow, wxSizerFlags( 1 ).Expand() );

}


viHistoryViewer::~viHistoryViewer()
{
}
