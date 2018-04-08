#include "vicListSelector.h"
#include "../StoreFrontEnd/StoreFrontEnd.h"
#include <wx/arrstr.h>
#include <algorithm>

using namespace std;

wxBEGIN_EVENT_TABLE(vicListSelector, wxPanel)
EVT_TEXT(ComboBox, vicListSelector::onComboBoxTextChanged)
EVT_BUTTON(AcceptButton, vicListSelector::onAcceptButton)
EVT_COMBOBOX(ComboBox, vicListSelector::onSelectionMade)
wxEND_EVENT_TABLE()

vicListSelector::vicListSelector(wxWindow* aptParent, wxString aszButtonText, wxWindowID aiID)
   : wxPanel(aptParent, aiID)
{
   m_wxComboBox = new wxComboBox( this, ComboBox, wxEmptyString, wxDefaultPosition,
                                  wxDefaultSize, 0, NULL, wxCB_DROPDOWN);

   wxButton* addButt = new wxButton( this, vicListSelector::AcceptButton,
                                     aszButtonText );

   wxFlexGridSizer* boxSizer = new wxFlexGridSizer(2, 1, 0, 0);
   boxSizer->AddGrowableCol(0);
   this->SetSizer(boxSizer);

   boxSizer->Add(m_wxComboBox, 1, wxEXPAND);
   boxSizer->Add(addButt, 1, wxEXPAND);
}


vicListSelector::~vicListSelector()
{

}

void 
vicListSelector::ResetOption()
{
   m_wxComboBox->Clear();
   m_oSelection = CELIOption();
}

void
vicListSelector::SetOptions(const vector<CELIOption>& avecOptions)
{
   wxArrayString wxArStr;
   m_vecOptions = avecOptions;
   for( auto option : avecOptions )
   {
      wxArStr.Add(option.Display);
   }
   m_wxComboBox->Append(wxArStr);
}

void 
vicListSelector::SetText(wxString aszText)
{
   m_wxComboBox->SetValue(aszText);
   m_wxComboBox->SetInsertionPointEnd();
}

// TODO: This Does not work.
void 
vicListSelector::SetAutoComplete(const std::vector<CELIOption>& avecOptions)
{
   wxArrayString wxArStr;
   m_vecOptions = avecOptions;
   for( auto option : avecOptions )
   {
      wxArStr.Add(option.Display);
   }
   m_wxComboBox->AutoComplete(wxArStr);
}

void 
vicListSelector::ShowDropdown()
{
   auto szText = m_wxComboBox->GetValue();
   m_wxComboBox->Freeze();
   m_wxComboBox->Popup();
   m_wxComboBox->SetValue(szText);
   m_wxComboBox->SetInsertionPointEnd();
   m_wxComboBox->Thaw();
}

void 
vicListSelector::DismissDropDown()
{
   m_wxComboBox->Dismiss();
}

bool 
vicListSelector::IsFocussed()
{
   return m_wxComboBox->HasFocus();
}

CELIOption 
vicListSelector::GetBestSelection()
{
   if( m_oSelection.Display == "" && m_wxComboBox->GetValue().size() >= 3 )
   {
      auto ptSF = StoreFrontEnd::Server();
      Query query;
      query.SearchFor( m_wxComboBox->GetValue().ToStdString( ) );
      query.UIDs();

      auto vecPossible = ptSF->GetAllCardsStartingWith( query );
      auto vecOptions = CELIOption::ParseCollectionItemsList( vecPossible );
      if( vecOptions.size() > 0 )
      {
         return vecOptions[0];
      }
      else
      {
         return m_oSelection;
      }
   }
   else
   {
      return m_oSelection;
   }
}

CELIOption
vicListSelector::GetSelection()
{
   return m_oSelection;
}

wxString
vicListSelector::GetText()
{
   return m_wxComboBox->GetValue();
}

void
vicListSelector::onComboBoxTextChanged(wxCommandEvent& awxEvt)
{
   // For some reason, when an option in the combobox is selected via mouse,
   // the event fires twice. Once with the option, and then once with blank text...
   auto szSend = m_wxComboBox->GetValue().ToStdString();
   if( szSend.size() == 0 )
   {
      awxEvt.StopPropagation();
      return;
   }

   awxEvt.SetString(szSend);

   transform( szSend.begin(), szSend.end(),
              szSend.begin(), ::tolower );
   for( auto& option : m_vecOptions )
   {
      string szText = option.Display;
      transform(szText.begin(), szText.end(), szText.begin(), ::tolower);
      if( szText == szSend )
      {
         // Perfect match. Send the event up.
         m_oSelection = option;
      }
   }

   awxEvt.SetInt(GetId());
   awxEvt.Skip();
}

void 
vicListSelector::onAcceptButton(wxCommandEvent& awxEvt)
{
   // Looks at the text that is currently in the box and gets
   // the option if it matches one.
   auto szCurrentText = GetText().ToStdString();
   transform( szCurrentText.begin(), szCurrentText.end(),
              szCurrentText.begin(), ::tolower );
   for( auto& option : m_vecOptions )
   {
      string szText = option.Display;
      transform(szText.begin(), szText.end(), szText.begin(), ::tolower);
      if( szText == szCurrentText )
      {
         // Perfect match. Send the event up.
         m_oSelection = option;
         awxEvt.SetInt(GetId());
         awxEvt.Skip();
         SetText("");
         m_wxComboBox->SetFocus();
      }
   }
}

void 
vicListSelector::onSelectionMade(wxCommandEvent& awxEvt)
{
   awxEvt.Skip();
}