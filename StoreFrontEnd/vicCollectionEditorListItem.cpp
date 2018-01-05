#include "vicCollectionEditorListItem.h"
#include <wx/stattext.h>
#include <wx/colour.h>

wxBEGIN_EVENT_TABLE(vicCollectionEditorListItem, wxPanel)
EVT_BUTTON(DeleteButton, vicCollectionEditorListItem::onCancelItem)
wxEND_EVENT_TABLE()

vicCollectionEditorListItem::vicCollectionEditorListItem( wxWindow* aptParent,
                                                          wxWindowID aiID,
                                                          const wxString& aszLabel, 
                                                          const wxString& aszCmd )
   : wxPanel(aptParent, aiID, wxDefaultPosition, wxDefaultSize, wxBORDER | wxTAB_TRAVERSAL),
     m_szLabel(aszLabel), m_szCmd(aszCmd)
{
   wxFlexGridSizer* fgridSizer = new wxFlexGridSizer(0, 3, 0, 0);
   fgridSizer->AddGrowableCol(0);
   fgridSizer->SetFlexibleDirection(wxHORIZONTAL);
   fgridSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_NONE);
   this->SetSizer(fgridSizer);

   buildItem();
}


vicCollectionEditorListItem::~vicCollectionEditorListItem()
{
}

void 
vicCollectionEditorListItem::buildItem()
{
   wxStaticText* wxText = new wxStaticText( this, wxID_ANY, m_szLabel,
                                            wxDefaultPosition, wxDefaultSize );
   this->GetSizer()->Add(wxText, wxSizerFlags(0).CenterVertical().Border(wxALL, 2));

   m_vPMCounter = new
      vicCollectionEditorListItemPlusMinusCounter(this, 1, 0, 14);
   this->GetSizer()->Add(m_vPMCounter, wxSizerFlags(0));

   wxButton* deleteButt = new wxButton( this, DeleteButton, "X",
                                        wxDefaultPosition, wxSize(30, 30));
   this->GetSizer()->Add(deleteButt, wxSizerFlags(0));
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

void 
vicCollectionEditorListItem::onCancelItem(wxCommandEvent& awxEvt)
{
   awxEvt.SetInt(GetId());
   awxEvt.Skip();
}