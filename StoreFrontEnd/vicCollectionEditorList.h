#pragma once
#include<wx/wxprec.h>
#include<vector>

class vicCollectionEditorList : public wxPanel
{
public:
   vicCollectionEditorList(wxWindow* aptParent);
   ~vicCollectionEditorList();

   void AddItem(const wxString& aszItem);
   std::vector<wxString> GetCommandList();
   
};

