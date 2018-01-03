#include "vCollectionDeckBox.h"
#include "vcCollectionDeckBoxItemList.h"
#include "viCollectionEditor.h"

vCollectionDeckBox::vCollectionDeckBox( wxWindow* aptParent, 
                                        wxWindowID aiWID,
                                        const wxString& aszColID )
   : wxPanel(aptParent, aiWID)
{
   m_wxszColID = aszColID;

   wxBoxSizer* boxSizer = new wxBoxSizer(wxHORIZONTAL);
   this->SetSizer(boxSizer);

   buildItemList();

   ShowCollectionEditor();
}


vCollectionDeckBox::~vCollectionDeckBox()
{
}

void 
vCollectionDeckBox::ShowCollectionEditor()
{
   auto viColEd = new viCollectionEditor(this, 4, m_wxszColID);
   this->GetSizer()->Add(viColEd, wxSizerFlags(1).Center().Shaped());

}

void 
vCollectionDeckBox::buildItemList()
{
   m_vcItemList = new vcCollectionDeckBoxItemList(this, m_wxszColID);
   this->GetSizer()->Add(m_vcItemList, 1, wxEXPAND);
}