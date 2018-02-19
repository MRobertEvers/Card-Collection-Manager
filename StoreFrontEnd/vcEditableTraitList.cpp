#include "vcEditableTraitList.h"
#include <map>
#include <vector>
#include "StoreFrontEnd.h"
#include "vcEditableTraitListItem.h"

wxBEGIN_EVENT_TABLE(vcEditableTraitList, wxScrolledWindow)
EVT_CHOICE(vcEditableTraitList::Selection_Changed, vcEditableTraitList::onItemSelectionChanged)
wxEND_EVENT_TABLE()

vcEditableTraitList::vcEditableTraitList( wxWindow* aptParent,
                                          wxWindowID aiID,
                                          const wxString& aszName,
                                          const wxString& aszUID )
   : wxScrolledWindow(aptParent, aiID)
{
   wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
   this->SetSizer(boxSizer);

   RefreshNew(aszName, aszUID);
}

vcEditableTraitList::~vcEditableTraitList()
{

}


void 
vcEditableTraitList::RefreshNew( const wxString& aszName,
                                 const wxString& aszUID )
{
   clearList();
   auto ptSF = StoreFrontEnd::Instance();

   // Lift the rest a function.
   auto vecPairAttrs = ptSF->GetPairedAttributes();
   for( auto& szAttr : vecPairAttrs )
   {
      auto iter_hasAttr = m_mapPairedAttrs.find(szAttr.first);
      if( iter_hasAttr != m_mapPairedAttrs.end() )
      {
         iter_hasAttr->second.push_back(szAttr.second);
      }
      else
      {
         m_mapPairedAttrs.insert(std::make_pair(szAttr.first, std::vector<wxString>({ szAttr.second })));
      }

      iter_hasAttr = m_mapPairedAttrs.find(szAttr.second);
      if( iter_hasAttr != m_mapPairedAttrs.end() )
      {
         iter_hasAttr->second.push_back(szAttr.first);
      }
      else
      {
         m_mapPairedAttrs.insert(std::make_pair(szAttr.second, std::vector<wxString>({ szAttr.first })));
      }
   }

   auto mapOptions = ptSF->GetIdentifyingAttributeOptions(aszName.ToStdString());
   auto vecSelections = ptSF->GetIdentifyingAttributes(aszName.ToStdString(),
      aszUID.ToStdString());
   std::map<std::string, std::string> mapSelections;
   for( auto& selection : vecSelections )
   {
      mapSelections.insert(std::make_pair(selection.first, selection.second));
   }

   int i = 0; // Used to assign a number to the trait name
   for( auto& opt : mapOptions )
   {
      m_mapAttrToID.insert(std::make_pair(i, wxString(opt.first)));
      auto item = new vcEditableTraitListItem(this, i, opt.first, opt.second);
      m_mapTraitItems.insert(std::make_pair(opt.first, item));

      this->GetSizer()->Add(item, wxSizerFlags(1).Expand());

      auto iter_selection = mapSelections.find(opt.first);
      if( iter_selection != mapSelections.end() )
      {
         item->TrySetSelection(iter_selection->second);
         // Error if this fails.
      }
      else
      {
         // ERROR
      }

      i++;
   }
}

void 
vcEditableTraitList::onItemSelectionChanged(wxCommandEvent& awxEvt)
{
   int iTrait = awxEvt.GetInt();
   auto szTrait = m_mapAttrToID[iTrait];

   wxString szSelection;
   int iSelection;
   auto iter_item = m_mapTraitItems.find(szTrait);
   if( iter_item != m_mapTraitItems.end() )
   {
      szSelection = iter_item->second->GetSelection();
      iSelection = iter_item->second->GetSelectionInt();
   }

   if( iSelection >= 0 )
   {
      auto iter_pairs = m_mapPairedAttrs.find(szTrait);
      if( iter_pairs != m_mapPairedAttrs.end() )
      {
         auto vecPairs = iter_pairs->second;
         for( auto& pairItem : vecPairs )
         {
            auto iter_pairedItem = m_mapTraitItems.find(pairItem);
            if( iter_pairedItem != m_mapTraitItems.end() )
            {
               iter_pairedItem->second->TrySetSelection(iSelection);
            }
         }
      }
   }
}

void 
vcEditableTraitList::clearList()
{
   m_mapAttrToID.clear();
   m_mapPairedAttrs.clear();

   for( auto& items : m_mapTraitItems )
   {
      items.second->Destroy();
   }

   m_mapTraitItems.clear();
}