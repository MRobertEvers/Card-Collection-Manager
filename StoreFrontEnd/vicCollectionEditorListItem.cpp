#include "vicCollectionEditorListItem.h"
#include <wx/stattext.h>
#include <wx/colour.h>

wxBEGIN_EVENT_TABLE(vicCollectionEditorListItem, wxPanel)
EVT_BUTTON(DeleteButton, vicCollectionEditorListItem::onCancelItem)
wxEND_EVENT_TABLE()

vicCollectionEditorListItem::vicCollectionEditorListItem( wxWindow* aptParent,
                                                          wxWindowID aiID,
                                                          CELIOption aOption,
                                                          CELIOption aOptionTwo )
   : wxPanel(aptParent, aiID, wxDefaultPosition, wxDefaultSize, wxBORDER | wxTAB_TRAVERSAL),
     m_oOption(aOption), m_oOptionTwo(aOptionTwo)
{
   wxFlexGridSizer* fgridSizer = new wxFlexGridSizer(0, 3, 0, 0);
   fgridSizer->AddGrowableCol(0);
   fgridSizer->SetFlexibleDirection(wxHORIZONTAL);
   fgridSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_NONE);
   this->SetSizer(fgridSizer);

   // Option two is always displayed first.
   // If there is no option two, then option one is displayed.
   // If there is an option two, it is always a removal indicator.
   m_bOptionSwitched = m_oOptionTwo.Display != "";
   if( m_bOptionSwitched )
   {
      auto tempOption = m_oOption;
      m_oOption = m_oOptionTwo;
      m_oOptionTwo = tempOption;
   }

   buildItem();
}


vicCollectionEditorListItem::~vicCollectionEditorListItem()
{
}

void 
vicCollectionEditorListItem::buildItem()
{
   m_vOption = new
      vicCollectionEditorListItemOption( this, m_oOption );
   this->GetSizer()->Add(m_vOption, wxSizerFlags(0).Expand());

   m_vPMCounter = new
      vicCollectionEditorListItemPlusMinusCounter(this, 1, 0, 14);
   this->GetSizer()->Add(m_vPMCounter, wxSizerFlags(0));

   wxButton* deleteButt = new wxButton( this, DeleteButton, "X",
                                        wxDefaultPosition, wxSize(30, 30));
   this->GetSizer()->Add(deleteButt, wxSizerFlags(0));

   // This is added last so it will go onto the second row.
   if( m_oOptionTwo.Display != "" )
   {
      m_vOptionTwo = new
         vicCollectionEditorListItemOption(this, m_oOptionTwo);
      this->GetSizer()->Add(m_vOptionTwo, wxSizerFlags(0).Expand());
   }
}


wxString 
vicCollectionEditorListItem::GetLabel()
{
   return m_oOption.Display;
}

wxString 
vicCollectionEditorListItem::GetCmd()
{
   return "";
}

void 
vicCollectionEditorListItem::onCancelItem(wxCommandEvent& awxEvt)
{
   awxEvt.SetInt(GetId());
   awxEvt.Skip();
}