#include "vicListSelector.h"
#include <wx/arrstr.h>

using namespace std;

wxBEGIN_EVENT_TABLE(vicListSelector, wxPanel)
EVT_TEXT(ComboBox_Text, vicListSelector::onComboBoxTextChanged)
EVT_BUTTON(AcceptButton, vicListSelector::onAcceptButton)
wxEND_EVENT_TABLE()

vicListSelector::vicListSelector(wxWindow* aptParent, wxString aszButtonText, wxWindowID aiID)
   : wxPanel(aptParent, aiID), m_szButtonText(aszButtonText)
{
   m_wxComboBox = new wxComboBox( this, ComboBox_Text, wxEmptyString, wxDefaultPosition,
                                  wxDefaultSize, 0, NULL, wxCB_DROPDOWN );

   wxButton* addButt = new wxButton( this, vicListSelector::AcceptButton,
                                     aszButtonText );

   wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
   this->SetSizer(boxSizer);

   boxSizer->Add(m_wxComboBox, 1, wxEXPAND);
   boxSizer->Add(addButt, 1, wxEXPAND);
}


vicListSelector::~vicListSelector()
{

}

void
vicListSelector::SetOptions(const std::vector<std::string>& avecOptions)
{
   m_wxComboBox->Clear();
   for( auto szOption : avecOptions )
   {
      m_wxComboBox->Append(szOption);
   }
}

void 
vicListSelector::SetText(wxString aszText)
{
   m_wxComboBox->SetValue(aszText);
   m_wxComboBox->SetInsertionPointEnd();
}

void 
vicListSelector::SetAutoComplete(wxString aszText)
{
   wxArrayString wxArStr;
   wxArStr.Add(aszText);
   m_wxComboBox->AutoComplete(wxArStr);
}

wxString
vicListSelector::GetText()
{
   return m_wxComboBox->GetValue();
}

void
vicListSelector::onComboBoxTextChanged(wxCommandEvent& awxEvt)
{
   awxEvt.SetString(m_wxComboBox->GetValue());
   awxEvt.SetInt(GetId());
   awxEvt.Skip();
}

void 
vicListSelector::onAcceptButton(wxCommandEvent& awxEvt)
{
   awxEvt.SetInt(GetId());
   awxEvt.Skip();
}