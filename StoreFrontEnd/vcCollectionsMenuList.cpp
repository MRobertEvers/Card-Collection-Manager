#include "vcCollectionsMenuList.h"
#include "vCollectionsOverview.h"
#include <wx/button.h>

vcCollectionsMenuList::vcCollectionsMenuList(wxWindow* aptParent)
   : wxPanel(aptParent)
{
   m_wxListControl = new wxListCtrl( this, wxID_ANY,
                                     wxDefaultPosition, wxDefaultSize, wxLC_LIST );
   m_wxListControl->InsertItem(0, "Test1");

   wxButton* testButt = new wxButton(this, vCollectionsOverview::Load_Collection, "TestButt");

   wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
   this->SetSizer(boxSizer);

   boxSizer->Add(m_wxListControl, 1, wxEXPAND);
   boxSizer->Add(testButt, wxSizerFlags().Expand());

}


vcCollectionsMenuList::~vcCollectionsMenuList() 
{
   delete m_wxListControl;
}

void 
vcCollectionsMenuList::AddCollectionOption(std::string aszCollectionName)
{
   m_wxListControl->InsertItem( 0, aszCollectionName );
}