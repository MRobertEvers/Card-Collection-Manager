#include "vcEditableTraitListItem.h"

wxBEGIN_EVENT_TABLE(vcEditableTraitListItem, wxPanel)
EVT_CHOICE(vcEditableTraitList::Selection_Changed, vcEditableTraitListItem::onSelectionChanged)
wxEND_EVENT_TABLE()

vcEditableTraitListItem::vcEditableTraitListItem( wxWindow* aptParent,
                                                  wxWindowID aiID,
                                                  const wxString& aszTraitName,
                                                  std::vector<std::string> avecOptions )
   : wxPanel(aptParent, aiID), m_vecOptions(avecOptions), m_szName(aszTraitName)
{
   wxFlexGridSizer* fgridSizer = new wxFlexGridSizer(1, 2, 0, 0);
   fgridSizer->AddGrowableCol(0);
   fgridSizer->SetFlexibleDirection(wxHORIZONTAL);
   fgridSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_NONE);
   this->SetSizer(fgridSizer);

   wxStaticText* wxText = new wxStaticText( this, wxID_ANY, aszTraitName,
                                            wxDefaultPosition, wxDefaultSize );
   fgridSizer->Add(wxText, wxSizerFlags(1).Left());

   m_wxDropBox = new wxChoice(this, vcEditableTraitList::Selection_Changed);
   for( auto& option : m_vecOptions )
   {
      m_wxDropBox->Append(option);
   }
   if( m_vecOptions.size() > 0 )
   {
      m_wxDropBox->SetSelection(0);
   }

   fgridSizer->Add(m_wxDropBox, wxSizerFlags(1).Right());

}

vcEditableTraitListItem::~vcEditableTraitListItem()
{

}

wxString 
vcEditableTraitListItem::GetTraitName()
{
   return m_szName;
}

wxString 
vcEditableTraitListItem::GetSelection()
{
   return m_wxDropBox->GetStringSelection();
}

int 
vcEditableTraitListItem::GetSelectionInt()
{
   return m_wxDropBox->GetSelection();
}

bool 
vcEditableTraitListItem::TrySetSelection(const wxString& aszSelect)
{
   for( int i = 0; i < m_vecOptions.size(); i++ )
   {
      auto szOpt = m_vecOptions[i];
      if( szOpt == aszSelect.ToStdString() )
      {
         m_wxDropBox->SetSelection(i);
         return true;
      }
   }

   return false;
}

bool 
vcEditableTraitListItem::TrySetSelection(const int aiSelect)
{
   if( aiSelect < m_vecOptions.size() )
   {
      m_wxDropBox->SetSelection(aiSelect);
      return true;
   }

   return false;
}

void 
vcEditableTraitListItem::onSelectionChanged(wxCommandEvent& awxEvt)
{
   awxEvt.SetInt(GetId());
   awxEvt.Skip();
}