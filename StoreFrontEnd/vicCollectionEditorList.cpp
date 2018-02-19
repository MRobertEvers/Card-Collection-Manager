#include "vicCollectionEditorList.h"
#include <algorithm>

wxBEGIN_EVENT_TABLE(vicCollectionEditorList, wxPanel)
EVT_BUTTON(vicCollectionEditorListItem::DeleteButton, vicCollectionEditorList::onCancelItem)
wxEND_EVENT_TABLE()

vicCollectionEditorList::vicCollectionEditorList(wxWindow* aptParent)
   : wxScrolledWindow(aptParent), m_iItemCounts(0)
{
   wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
   this->SetSizer(boxSizer);
   this->FitInside(); // ask the sizer about the needed size

   // this->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_DEFAULT);
   this->SetScrollRate(5, 5);
}


vicCollectionEditorList::~vicCollectionEditorList()
{
}

void 
vicCollectionEditorList::AddItem(CELIOption aItem, CELIOption aRemove)
{
   int iMax;
   if( aRemove.Display == "" )
   {
      iMax = vicCollectionEditorListItemPlusMinusCounter::NO_MAX;
   }
   else
   {
      iMax = 1;
   }

   vicCollectionEditorListItem* vicItem = 
      new vicCollectionEditorListItem(this, m_iItemCounts++, aItem, aRemove, iMax);
   
   m_vecItems.push_back(vicItem);
   this->GetSizer()->Add(vicItem, wxSizerFlags(0).Expand().Border(wxALL, 0));
   // Causes the children to calculate sizes.
   PostSizeEvent();
}

// Returns the list of all items where the selection is the
// the only item in the IDs vector of the option.
std::vector<vicCollectionEditorListItem::ItemSelectionData>
vicCollectionEditorList::GetCommandList()
{
   std::vector<vicCollectionEditorListItem::ItemSelectionData> vecRetVal;
   for( auto& item : m_vecItems )
   {
      vecRetVal.push_back(item->GetSelection());
   }
   return vecRetVal;
}

void 
vicCollectionEditorList::ClearList()
{
   for( auto& child : m_vecItems )
   {
      removeChild(child);
   }
   m_vecItems.clear();
}

void
vicCollectionEditorList::onCancelItem(wxCommandEvent& awxEvt)
{
   wxWindow* remChild = NULL;
   for( auto child : GetChildren() )
   {
      if( child->GetId() == awxEvt.GetInt() )
      {
         remChild = child;
         break;
      }
   }

   if( remChild != NULL )
   {
      removeChild((vicCollectionEditorListItem*)remChild);
   }
}

void 
vicCollectionEditorList::removeChild(vicCollectionEditorListItem* aptChild)
{
   auto iter_item = find(m_vecItems.begin(), m_vecItems.end(), aptChild);
   m_vecItems.erase(iter_item);

   this->GetSizer()->Detach(aptChild);
   aptChild->Destroy();
   PostSizeEvent();
}