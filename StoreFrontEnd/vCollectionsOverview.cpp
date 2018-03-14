#include "vCollectionsOverview.h"
#include <wx/listctrl.h>
#include <wx/filedlg.h>
#include <wx/wfstream.h>
#include "StoreFrontEnd.h"
#include "vcCollectionsMenuList.h"

//#include "viCollectionSelector.h"


wxBEGIN_EVENT_TABLE(vCollectionsOverview, wxPanel)
EVT_BUTTON(vCollectionsOverview::Load_Collection, vCollectionsOverview::OnLoadCollection)
wxEND_EVENT_TABLE()

vCollectionsOverview::vCollectionsOverview( wxWindow* aptParent,
                                            wxWindowID aiWID = wxID_ANY ) 
   : wxPanel(aptParent, aiWID)
{
   wxBoxSizer* boxSizer = new wxBoxSizer(wxHORIZONTAL);
   this->SetSizer(boxSizer);
   this->GetParent()->SetSize(350, 500);
   buildCollectionSelector();
   getLoadedCollections();
}


vCollectionsOverview::~vCollectionsOverview()
{
   delete m_vcCollectionsPanel;
}

void 
vCollectionsOverview::buildCollectionsPreview() 
{

}

void 
vCollectionsOverview::buildCollectionSelector() 
{
   m_vcCollectionsPanel = new vcCollectionsMenuList(this);
   this->GetSizer()->Add(m_vcCollectionsPanel, 1, wxEXPAND);
}

void 
vCollectionsOverview::getLoadedCollections()
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
vCollectionsOverview::OnLoadCollection(wxCommandEvent& awxEvt)
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
   //auto sel = new viCollectionSelector( this, 4 );
   //sel->Show();
}
