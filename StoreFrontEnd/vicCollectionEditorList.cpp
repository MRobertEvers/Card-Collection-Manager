#include "vicCollectionEditorList.h"
#include "vicCollectionEditorListItem.h"


vicCollectionEditorList::vicCollectionEditorList(wxWindow* aptParent)
   : wxScrolledWindow(aptParent)
{
   wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
   this->SetSizer(boxSizer);
   this->FitInside(); // ask the sizer about the needed size
   this->SetScrollRate(5, 5);
}


vicCollectionEditorList::~vicCollectionEditorList()
{
}

void 
vicCollectionEditorList::AddItem(const wxString& aszItem)
{
   vicCollectionEditorListItem* vicItem = 
      new vicCollectionEditorListItem(this, aszItem, "");
   this->GetSizer()->Add(vicItem, wxSizerFlags(0).Border(wxALL, 0));
   // Causes the children to calculate sizes.
   PostSizeEvent();
}

std::vector<wxString> 
vicCollectionEditorList::GetCommandList()
{
   return std::vector<wxString>();
}