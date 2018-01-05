#include "vicCollectionEditorListItemPlusMinusCounter.h"

wxBEGIN_EVENT_TABLE(vicCollectionEditorListItemPlusMinusCounter, wxPanel)
wxEND_EVENT_TABLE()

vicCollectionEditorListItemPlusMinusCounter::
vicCollectionEditorListItemPlusMinusCounter( wxWindow* aptParent,
                                             int aiStartVal,
                                             int aiMin, int aiMax )
   : wxPanel(aptParent)
{


   buildPMCounter();

}


vicCollectionEditorListItemPlusMinusCounter::
~vicCollectionEditorListItemPlusMinusCounter()
{
}

void 
vicCollectionEditorListItemPlusMinusCounter::
buildPMCounter()
{
   wxBoxSizer* boxSizer = new wxBoxSizer(wxHORIZONTAL);

   wxButton* upButt = new wxButton( this, UpButton, "+", wxDefaultPosition, wxSize(30, 30));
   boxSizer->Add(upButt, wxSizerFlags(0));

   wxTextCtrl* textBox = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(30, 30));
   boxSizer->Add(textBox, wxSizerFlags(0));

   wxButton* downButt = new wxButton( this, DownButton, "-", wxDefaultPosition, wxSize(30, 30));
   boxSizer->Add(downButt, wxSizerFlags(0));

   this->SetSizer(boxSizer);
   this->Layout();
   boxSizer->Fit(this);
   
}
