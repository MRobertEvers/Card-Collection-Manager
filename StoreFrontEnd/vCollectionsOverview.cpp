#include "vCollectionsOverview.h"
#include <wx/listctrl.h>

wxBEGIN_EVENT_TABLE(vCollectionsOverview, wxPanel)
EVT_BUTTON(vCollectionsOverview::Load_Collection, vCollectionsOverview::OnLoadCollection)
wxEND_EVENT_TABLE()

vCollectionsOverview::vCollectionsOverview( wxWindow* aptParent,
                                          wxWindowID aiWID = wxID_ANY ) 
   : wxPanel(aptParent, aiWID)
{
   wxBoxSizer* boxSizer = new wxBoxSizer(wxHORIZONTAL);
   this->SetSizer(boxSizer);

   buildCollectionSelector();
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
vCollectionsOverview::OnLoadCollection(wxCommandEvent &)
{
   m_vcCollectionsPanel->AddCollectionOption("EventCol");

}