#include "vicCollectionEditorListItem.h"
#include <wx/stattext.h>


vicCollectionEditorListItem::vicCollectionEditorListItem( wxWindow* aptParent,
                                                          const wxString& aszLabel, 
                                                          const wxString& aszCmd )
   : wxPanel(aptParent), m_szLabel(aszLabel), m_szCmd(aszCmd)
{
   wxBoxSizer* boxSizer = new wxBoxSizer(wxHORIZONTAL);
   this->SetSizer(boxSizer);
   buildItem();
}


vicCollectionEditorListItem::~vicCollectionEditorListItem()
{
}

void 
vicCollectionEditorListItem::buildItem()
{
   wxStaticText* wxText = new wxStaticText(this, wxID_ANY, m_szLabel);
   this->GetSizer()->Add(wxText, wxSizerFlags(1).Border(wxALL, 3));
}


wxString 
vicCollectionEditorListItem::GetLabel()
{
   return m_szLabel;
}

wxString 
vicCollectionEditorListItem::GetCmd()
{
   return m_szCmd;
}