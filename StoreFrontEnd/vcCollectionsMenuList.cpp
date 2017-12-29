#include "vcCollectionsMenuList.h"
#include <wx/button.h>

vcCollectionsMenuList::vcCollectionsMenuList(wxWindow* aptParent)
   : wxPanel(aptParent)
{
   m_wxListControl = new wxListCtrl(this, wxID_ANY);
   m_wxListControl->InsertItem(0, "Test1");

   wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
   this->SetSizer(boxSizer);

   boxSizer->Add(m_wxListControl, 1, wxEXPAND);
}


vcCollectionsMenuList::~vcCollectionsMenuList() 
{
   delete m_wxListControl;
}
