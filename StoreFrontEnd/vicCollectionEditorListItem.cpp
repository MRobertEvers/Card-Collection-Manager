#include "vicCollectionEditorListItem.h"
#include <wx/stattext.h>
#include <wx/colour.h>
#include "vicCollectionEditorListItemPlusMinusCounter.h"

vicCollectionEditorListItem::vicCollectionEditorListItem( wxWindow* aptParent,
                                                          const wxString& aszLabel, 
                                                          const wxString& aszCmd )
   : wxPanel(aptParent), m_szLabel(aszLabel), m_szCmd(aszCmd)
{
   wxFlexGridSizer* fgridSizer = new wxFlexGridSizer(0, 2, 0, 0);
   fgridSizer->AddGrowableCol(0);
   fgridSizer->SetFlexibleDirection(wxHORIZONTAL);
   fgridSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_NONE);
   this->SetSizer(fgridSizer);

   SetBackgroundColour(*wxGREEN);
   buildItem();
}


vicCollectionEditorListItem::~vicCollectionEditorListItem()
{
}

void 
vicCollectionEditorListItem::buildItem()
{
   wxStaticText* wxText = new wxStaticText(this, wxID_ANY, m_szLabel);
   this->GetSizer()->Add(wxText, wxSizerFlags(0).Expand().Border(wxALL, 2));

   vicCollectionEditorListItemPlusMinusCounter* vicPM = new
      vicCollectionEditorListItemPlusMinusCounter(this, 1, 0, 0);
   this->GetSizer()->Add(vicPM, wxSizerFlags(0));
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