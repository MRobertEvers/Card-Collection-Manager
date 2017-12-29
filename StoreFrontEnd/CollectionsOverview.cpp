#include "CollectionsOverview.h"
#include <wx/listctrl.h>

CollectionsOverview::CollectionsOverview( wxWindow* aptParent,
                                          wxWindowID aiWID = wxID_ANY ) 
   : wxPanel(aptParent, aiWID)
{
   wxWindowID iLBoxID = 9;

   wxListCtrl* vlbCollections = new wxListCtrl(this, iLBoxID);
   vlbCollections->InsertItem(0, "Dog");
   wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
   boxSizer->Add(vlbCollections, 1, wxEXPAND);
   this->SetSizer(boxSizer);
}


CollectionsOverview::~CollectionsOverview() {
}
