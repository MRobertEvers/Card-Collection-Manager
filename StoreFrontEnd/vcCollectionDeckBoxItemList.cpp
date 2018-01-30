#include "vcCollectionDeckBoxItemList.h"
#include "StoreFrontEnd.h"
#include "vcdCDBIListItemData.h"

wxBEGIN_EVENT_TABLE(vcCollectionDeckBoxItemList, wxPanel)
EVT_LIST_ITEM_SELECTED(List, vcCollectionDeckBoxItemList::onItemSelection)
EVT_LIST_ITEM_DESELECTED(List, vcCollectionDeckBoxItemList::onItemDeselection)
wxEND_EVENT_TABLE()


vcCollectionDeckBoxItemList::Group&
vcCollectionDeckBoxItemList::Group::
GroupOn(const wxString& aszKey, bool abIsMetaKey)
{
   Key = aszKey;
   MetaKey = abIsMetaKey;
   return *this;
}

vcCollectionDeckBoxItemList::Group& 
vcCollectionDeckBoxItemList::Group::
BroadenSubGroup(const wxString& aszValue)
{
   BroadenedValues.push_back(aszValue);
   return *this;
}

vcCollectionDeckBoxItemList::Group& 
vcCollectionDeckBoxItemList::Group::
OverrideGrouping(const Group& aGrouping)
{
   Overrides.push_back(aGrouping);
   return *this;
}

wxString
vcCollectionDeckBoxItemList::Group::
GetGroup(const vcdCDBIListItemData& aData)
{
   wxString szGroup;
   for( auto& over : Overrides )
   {
      szGroup = over.GetGroup(aData);
      if( szGroup != "" )
      {
         break;
      }
   }

   if( szGroup == "" )
   {
      if( MetaKey )
      {
         szGroup = aData.GetMetaTag(Key);
      }
      else
      {
         szGroup = aData.GetAttribute(Key);
      }

      for( auto& broadGroup : BroadenedValues )
      {
         if( szGroup.find(broadGroup) != wxString::npos )
         {
            szGroup = broadGroup;
            break;
         }
      }
   }

   return szGroup;
}

vcCollectionDeckBoxItemList::vcCollectionDeckBoxItemList( wxWindow* aptParent,
                                                          const wxString& aszColID )
   : wxPanel(aptParent)
{
   m_wxszColID = aszColID;

   m_iSelection = -1;
   m_wxListControl = new wxListCtrl( this, List,
                                     wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
   m_wxListControl->InsertColumn(0, "#");
   m_wxListControl->InsertColumn(1, "Name");
   m_wxListControl->InsertColumn(2, "Mana Cost");
   m_wxListControl->InsertColumn(3, "Card Type");
   m_wxListControl->InsertColumn(4, "Set");

   wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
   this->SetSizer(boxSizer);

   boxSizer->Add(m_wxListControl, 1, wxEXPAND);

   RefreshList();
}


vcCollectionDeckBoxItemList::~vcCollectionDeckBoxItemList()
{

}

void 
vcCollectionDeckBoxItemList::RefreshList()
{
   auto ptSF = StoreFrontEnd::Instance();
   Query query;
   query.UIDs();
   auto lstCol = ptSF->GetAllCardsStartingWith(m_wxszColID.ToStdString(), query);

   m_vecDataItems.clear();
   for( auto& szItem : lstCol )
   {
      vcdCDBIListItemData data(szItem, vcdCDBIListItemData::LONG_NAME);
      m_vecDataItems.push_back(data);
   }

   m_wxListControl->Freeze();
   m_wxListControl->DeleteAllItems();
   displayGrouping(defaultGrouping(m_vecDataItems));
   m_wxListControl->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
   m_wxListControl->SetColumnWidth(1, wxLIST_AUTOSIZE);
   m_wxListControl->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
   m_wxListControl->SetColumnWidth(3, wxLIST_AUTOSIZE);
   m_wxListControl->SetColumnWidth(4, wxLIST_AUTOSIZE);
   m_wxListControl->Thaw();
}

vcdCDBIListItemData 
vcCollectionDeckBoxItemList::GetItem(int Ind)
{
   if( m_vecDataItems.size() > Ind )
   {
      return m_vecDataItems[Ind];
   }
   else
   {
      return vcdCDBIListItemData();
   }
}

bool 
vcCollectionDeckBoxItemList::IsEmpty()
{
   return m_vecDataItems.size() > 0;
}

void
vcCollectionDeckBoxItemList::onItemSelection(wxListEvent& awxEvt)
{
   m_iSelection = awxEvt.GetIndex();
   if( m_iSelection != -1 )
   {
      awxEvt.Skip();
   }
}

void
vcCollectionDeckBoxItemList::onItemDeselection(wxListEvent& awxEvt)
{
   m_iSelection = -1;
   awxEvt.StopPropagation();
}

// Returns a list of pointers to the input vector. SO DONT DELETE THE VECTOR BEFORE
// USING THE RETURN.
std::map<wxString, std::vector<vcdCDBIListItemData*>>
vcCollectionDeckBoxItemList::defaultGrouping(std::vector<vcdCDBIListItemData>& avecItems)
{
   Group overrideGrp;
   // TODO: Use the stringIface to get the collection.group. write this func.
   overrideGrp.GroupOn("group", true);

   Group defaultGrp;
   defaultGrp.GroupOn("type", false)
             .BroadenSubGroup("Creature")
             .BroadenSubGroup("Instant")
             .BroadenSubGroup("Sorcery")
             .BroadenSubGroup("Enchantment")
             .BroadenSubGroup("Artifact")
             .BroadenSubGroup("Planeswalker")
             .BroadenSubGroup("Land")
             .OverrideGrouping(overrideGrp);


   map<wxString, vector<vcdCDBIListItemData*>> mapGroups;
   for( auto& data : avecItems )
   {
      // TODO: Need way to get meta tags from server so we can sort on them.
      mapGroups[defaultGrp.GetGroup(data)].push_back(&data);
   }

   return mapGroups;
}

void 
vcCollectionDeckBoxItemList::displayGrouping(const map<wxString,
                                             std::vector<vcdCDBIListItemData*>>& amapGrouping)
{
   // TODO: this is inefficient because I am not copying pointers here.
   std::vector<vcdCDBIListItemData> TempData;
   for( auto& group : amapGrouping )
   {
      wxString buf = group.first;
      int i = m_wxListControl->GetItemCount();
      long tmp = m_wxListControl->InsertItem(i, "", 0);
      m_wxListControl->SetItem(tmp, 1, buf);
      wxListItem item;
      item.m_itemId = i;
      item.SetBackgroundColour(*wxLIGHT_GREY);
      m_wxListControl->SetItem(item);
      for( auto& item : group.second )
      {
         addListItem(*item);
         TempData.push_back(*item);
      }
   }

   m_vecDataItems.clear();
   m_vecDataItems = TempData;
}

void 
vcCollectionDeckBoxItemList::addListItem(vcdCDBIListItemData& aData)
{
   wxString buf = std::to_string(aData.GetNumber());
   int i = m_wxListControl->GetItemCount();
   long tmp = m_wxListControl->InsertItem(i, buf, 0);
   m_wxListControl->SetItemData(tmp, i);

   buf = aData.GetName();
   m_wxListControl->SetItem(tmp, 1, buf);

   buf = aData.GetManaCost();
   m_wxListControl->SetItem(tmp, 2, buf);

   buf = aData.GetCardType();
   m_wxListControl->SetItem(tmp, 3, buf);

   buf = aData.GetSet();
   m_wxListControl->SetItem(tmp, 4, buf);
}