#include "vcEditableItemList.h"
#include "vcItemNameCheckBox.h"

vcEditableItemList::vcEditableItemList(wxWindow* aptParent,
                                       wxWindowID aiID,
                                       std::vector<wxString> avecOptions)
   : wxScrolledWindow(aptParent, aiID, wxDefaultPosition, wxDefaultSize, wxVSCROLL)
{
   wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
   this->SetSizer(boxSizer);
   this->FitInside();
   this->SetScrollRate(0, 5);
   // SetBackgroundColour(*wxRED);
   RefreshNew(avecOptions);
}

vcEditableItemList::~vcEditableItemList()
{

}

void 
vcEditableItemList::RefreshNew(std::vector<wxString> avecOptions)
{
   for( auto& item : m_mapItemToVC )
   {
      item.second->Destroy();
   }
   m_mapItemToVC.clear();

   bool doCheck = true;
   for( auto& opt : avecOptions )
   {
      auto wxItem = new vcItemNameCheckBox(this, wxID_ANY, opt);
      m_mapItemToVC.insert(std::make_pair(opt, wxItem));
      this->GetSizer()->Add(wxItem, wxSizerFlags(0).Expand().Border(wxALL, 0));

      wxItem->SetChecked(doCheck);
      doCheck = false;
   }
}

std::map<wxString, bool> 
vcEditableItemList::GetUIDModifiedMap()
{
   std::map<wxString, bool> mapRetval;
   for( auto& item : m_mapItemToVC )
   {
      mapRetval.insert(std::make_pair(item.first, item.second->IsChecked()));
   }
   return mapRetval;
}
