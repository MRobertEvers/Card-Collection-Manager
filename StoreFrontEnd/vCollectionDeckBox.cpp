#include "vCollectionDeckBox.h"
#include "vcCollectionDeckBoxItemList.h"


vCollectionDeckBox::vCollectionDeckBox( wxWindow* aptParent, 
                                        wxWindowID aiWID,
                                        const wxString& aszColID )
   : wxPanel(aptParent, aiWID)
{
   m_wxszColID = aszColID;

   wxBoxSizer* boxSizer = new wxBoxSizer(wxHORIZONTAL);
   this->SetSizer(boxSizer);

   buildItemList();
}


vCollectionDeckBox::~vCollectionDeckBox()
{
}

void 
vCollectionDeckBox::buildItemList()
{
   m_vcItemList = new vcCollectionDeckBoxItemList(this, m_wxszColID);
   this->GetSizer()->Add(m_vcItemList, 1, wxEXPAND);
}