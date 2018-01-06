#include "vicCollectionEditorList.h"
#include "vicCollectionEditorListItem.h"

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
   vicCollectionEditorListItem* vicItem = 
      new vicCollectionEditorListItem(this, m_iItemCounts++, aItem, aRemove);
   
   this->GetSizer()->Add(vicItem, wxSizerFlags(0).Expand().Border(wxALL, 0));
   // Causes the children to calculate sizes.
   PostSizeEvent();
}

std::vector<wxString> 
vicCollectionEditorList::GetCommandList()
{
   return std::vector<wxString>();
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
      this->GetSizer()->Detach(remChild);
      remChild->Destroy();
      PostSizeEvent();
   }
}