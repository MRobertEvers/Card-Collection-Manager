#include "vicCollectionEditorList.h"
#include "vicCollectionEditorListItem.h"


vicCollectionEditorList::vicCollectionEditorList(wxWindow* aptParent)
   : wxPanel(aptParent)
{
   wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
   this->SetSizer(boxSizer);
}


vicCollectionEditorList::~vicCollectionEditorList()
{
}

void 
vicCollectionEditorList::AddItem(const wxString& aszItem)
{
   vicCollectionEditorListItem* vicItem = 
      new vicCollectionEditorListItem(this, aszItem, "");
   this->GetSizer()->Add(vicItem, wxSizerFlags(1).Expand());
   // Causes the children to calculate sizes.
   PostSizeEvent();
}

std::vector<wxString> 
vicCollectionEditorList::GetCommandList()
{
   return std::vector<wxString>();
}