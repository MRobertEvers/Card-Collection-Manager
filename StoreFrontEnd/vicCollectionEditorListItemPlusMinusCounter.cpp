#include "vicCollectionEditorListItemPlusMinusCounter.h"

wxBEGIN_EVENT_TABLE(vicCollectionEditorListItemPlusMinusCounter, wxPanel)
EVT_BUTTON(UpButton, vicCollectionEditorListItemPlusMinusCounter::onUpButton)
EVT_BUTTON(DownButton, vicCollectionEditorListItemPlusMinusCounter::onDownButton)
wxEND_EVENT_TABLE()

vicCollectionEditorListItemPlusMinusCounter::
vicCollectionEditorListItemPlusMinusCounter( wxWindow* aptParent,
                                             int aiStartVal,
                                             int aiMin, int aiMax )
   : wxPanel(aptParent), m_iMin(aiMin), m_iMax(aiMax), m_iVal(aiStartVal)
{
   wxBoxSizer* boxSizer = new wxBoxSizer(wxHORIZONTAL);
   this->SetSizer(boxSizer);

   buildPMCounter();
}


vicCollectionEditorListItemPlusMinusCounter::
~vicCollectionEditorListItemPlusMinusCounter()
{
}

int 
vicCollectionEditorListItemPlusMinusCounter::GetCount()
{
   return m_iVal;
}

void 
vicCollectionEditorListItemPlusMinusCounter::
buildPMCounter()
{

   wxButton* downButt = new wxButton( this, DownButton, "-",
                                      wxDefaultPosition, wxSize(30, 30) );
   downButt->Enable(m_iMax >= m_iMin);
   this->GetSizer()->Add(downButt, wxSizerFlags(0));

   m_vTextCtrl = new wxTextCtrl( this, wxID_ANY, std::to_string(m_iVal), 
                                 wxDefaultPosition, wxSize(30, 30), wxTE_CENTRE );
   m_vTextCtrl->SetEditable(false);
   this->GetSizer()->Add(m_vTextCtrl, wxSizerFlags(0));

   wxButton* upButt = new wxButton( this, UpButton, "+",
                                    wxDefaultPosition, wxSize(30, 30) );
   upButt->Enable(m_iMax >= m_iMin);
   this->GetSizer()->Add(upButt, wxSizerFlags(0));

   this->Fit();
}

void 
vicCollectionEditorListItemPlusMinusCounter::
onUpButton(wxCommandEvent& awxEvt)
{
   m_iVal++;
   if( m_iVal > m_iMax && m_iMax >= m_iMin )
   {
      m_iVal = m_iMax;
   }
   updateText();
}

void 
vicCollectionEditorListItemPlusMinusCounter::
onDownButton(wxCommandEvent& awxEvt)
{
   m_iVal--;
   if( m_iVal < m_iMin )
   {
      m_iVal = m_iMin;
   }
   updateText();
}

void 
vicCollectionEditorListItemPlusMinusCounter::
updateText()
{
   wxString szText = std::to_string(m_iVal);
   if( m_iMax > m_iMin )
   {
      szText += "/" + std::to_string(m_iMax);
   }
   m_vTextCtrl->SetValue(szText);
}