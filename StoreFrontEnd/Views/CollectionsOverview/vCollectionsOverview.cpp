#include "vCollectionsOverview.h"
#include "vcCollectionsMenuList.h"
#include "../StoreFrontEnd/StoreFrontEnd.h"
#include "../CollectionPreviewer/viCollectionSelector.h"
#include <wx/listctrl.h>
#include <wx/filedlg.h>
#include <wx/wfstream.h>


wxBEGIN_EVENT_TABLE(VCollectionsOverview, wxPanel)
EVT_BUTTON(VCollectionsOverview::Load_Collection, VCollectionsOverview::OnLoadCollection)
EVT_BUTTON( viCollectionSelector::Load_Button, VCollectionsOverview::onLoadCollection )
wxEND_EVENT_TABLE()

VCollectionsOverview::VCollectionsOverview( wxWindow* aptParent,
                                            wxWindowID aiWID = wxID_ANY ) 
   : wxPanel(aptParent, aiWID)
{
   wxBoxSizer* boxSizer = new wxBoxSizer(wxHORIZONTAL);
   this->SetSizer(boxSizer);

   auto pTargetFrame = this->GetParent();
   pTargetFrame->SetMinSize( wxSize( 350, 500 ) );
   if( pTargetFrame->GetSize().GetWidth() < pTargetFrame->GetMinSize().GetWidth() ||
      pTargetFrame->GetSize().GetHeight() < pTargetFrame->GetMinSize().GetHeight() )
   {
      pTargetFrame->SetSize( pTargetFrame->GetMinSize() );
   }

   buildCollectionSelector();
   getLoadedCollections();
}


VCollectionsOverview::~VCollectionsOverview()
{
   delete m_vcCollectionsPanel;
}

void 
VCollectionsOverview::onLoadCollection( wxCommandEvent& awxEvt )
{
   StoreFront* ptSF = StoreFrontEnd::Server();
   auto szPath = awxEvt.GetString();

   // Load collection returns the Col ID.
   auto szColID = ptSF->LoadCollection( szPath.ToStdString() );
   auto szColName = ptSF->GetCollectionName( szColID );
   m_vcCollectionsPanel->AddCollectionOption( szColName );

   // This event is handled.
   awxEvt.StopPropagation();
}

void 
VCollectionsOverview::buildCollectionsPreview() 
{

}

void 
VCollectionsOverview::buildCollectionSelector() 
{
   m_vcCollectionsPanel = new vcCollectionsMenuList(this);
   this->GetSizer()->Add(m_vcCollectionsPanel, 1, wxEXPAND);
}

void 
VCollectionsOverview::getLoadedCollections()
{
   auto ptSF = StoreFrontEnd::Server();
   auto vecCols = ptSF->GetLoadedCollections();
   for( auto& szColID : vecCols )
   {
      auto szColName = ptSF->GetCollectionName(szColID);
      m_vcCollectionsPanel->AddCollectionOption(szColName);
   }
}


void
VCollectionsOverview::OnLoadCollection(wxCommandEvent& awxEvt)
{
/*
   wxFileDialog openFileDialog( this, _("Open Collection file"), "", "",
                                "text files (*.txt)|*.txt",
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST );

   if( openFileDialog.ShowModal() == wxID_CANCEL )
      return; 

   // proceed loading the file chosen by the user;
   // this can be done with e.g. wxWidgets input streams:
   wxFileInputStream input_stream(openFileDialog.GetPath());
   if( !input_stream.IsOk() )
   {
      wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
      return;
   }

   StoreFront* ptSF = StoreFrontEnd::Server();
   auto szPath = openFileDialog.GetPath().ToStdString();

   // Load collection returns the Col ID.
   auto szColID = ptSF->LoadCollection(szPath);
   auto szColName = ptSF->GetCollectionName(szColID);
   m_vcCollectionsPanel->AddCollectionOption(szColName);

   // This event is handled.
   awxEvt.StopPropagation();
*/
   auto sel = new viCollectionSelector( this, 4 );
   sel->Show();
   wxFrame* pTargetFrame = sel;
   pTargetFrame->SetMinSize( wxSize( 450, 600 ) );
   if( pTargetFrame->GetSize().GetWidth() < pTargetFrame->GetMinSize().GetWidth() ||
      pTargetFrame->GetSize().GetHeight() < pTargetFrame->GetMinSize().GetHeight() )
   {
      pTargetFrame->SetSize( pTargetFrame->GetMinSize() );
   }
}
