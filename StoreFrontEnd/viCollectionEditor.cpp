#include "viCollectionEditor.h"
#include "vicListSelector.h"
#include "vicCollectionEditorList.h"
#include "StoreFront.h"

wxBEGIN_EVENT_TABLE(viCollectionEditor, wxPanel)
EVT_TEXT(vicListSelector::ComboBox_Text, viCollectionEditor::onComboBoxTextChanged)
EVT_BUTTON(vicListSelector::AcceptButton, viCollectionEditor::onComboBoxAccept)
wxEND_EVENT_TABLE()


viCollectionEditor::viCollectionEditor(wxWindow* aptParent, wxWindowID aiWID, wxString aszColID)
   : wxPanel(aptParent, aiWID), m_szCollectionID(aszColID), m_bHandleTextEvent(true)
{
   wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
   this->SetSizer(boxSizer);

   buildSelectors();
   buildListView();
   buildButtons();
}


viCollectionEditor::~viCollectionEditor()
{
}

void
viCollectionEditor::buildSelectors()
{
   wxPanel* wxTopPanel = new wxPanel(this, wxID_ANY);
   wxBoxSizer* boxSizer = new wxBoxSizer(wxHORIZONTAL);
   wxTopPanel->SetSizer(boxSizer);

   m_vAddSelector = new vicListSelector(wxTopPanel, ADD_BUTTON_TEXT, Selectors::Add);
   wxTopPanel->GetSizer()->Add(m_vAddSelector, wxSizerFlags(1).Expand());

   m_vRemSelector = new vicListSelector(wxTopPanel, REMOVE_BUTTON_TEXT, Selectors::Remove);
   wxTopPanel->GetSizer()->Add(m_vRemSelector, wxSizerFlags(1).Expand());

   this->GetSizer()->Add(wxTopPanel, wxSizerFlags(4).Expand());
}

void 
viCollectionEditor::buildListView()
{
   m_vListView = new vicCollectionEditorList(this);
   this->GetSizer()->Add(m_vListView, wxSizerFlags(9).Expand());
}

void 
viCollectionEditor::buildButtons()
{
   wxPanel* wxTopPanel = new wxPanel(this, wxID_ANY);
   wxBoxSizer* boxSizer = new wxBoxSizer(wxHORIZONTAL);
   wxTopPanel->SetSizer(boxSizer);

   wxButton* acceptButton = new wxButton( wxTopPanel,
                                          Changes_Accept,
                                          "Accept" );
   wxButton* declineButton = new wxButton( wxTopPanel,
                                           Changes_Decline,
                                           "Cancel" );

   wxTopPanel->GetSizer()->Add(acceptButton, wxSizerFlags(1).Shaped());
   wxTopPanel->GetSizer()->Add(declineButton, wxSizerFlags(1).Shaped());

   this->GetSizer()->Add(wxTopPanel, wxSizerFlags(2).Expand());
}

void
viCollectionEditor::onComboBoxTextChanged(wxCommandEvent& awxEvt)
{
   if( !m_bHandleTextEvent ) { return; }

   wxString szEventDets = awxEvt.GetString();
   if( szEventDets.size() < 4 )
   {
      return;
   }

   m_bHandleTextEvent = false;
   if( awxEvt.GetInt() == Selectors::Add )
   {
      StoreFront* ptSF = StoreFrontEnd::Instance();

      auto vecOptions = ptSF->GetAllCardsStartingWith( szEventDets.ToStdString() );
      m_vAddSelector->SetOptions(vecOptions);
      m_vAddSelector->SetText(szEventDets);
      if( vecOptions.size() > 0 )
      {
         m_vAddSelector->SetAutoComplete(vecOptions[0]);
      }
   }
   else if( awxEvt.GetInt() == Selectors::Remove )
   {
      StoreFront* ptSF = StoreFrontEnd::Instance();

      // TODO: Optimize this search.
      // Cache the collection first time, then restrict that list by string.
      Collection::Query query(true);
      query.SearchFor(szEventDets.ToStdString());
      query.Short();

      auto vecOptions = ptSF->GetAllCardsStartingWith( m_szCollectionID.ToStdString(),
                                                       query );
      m_vRemSelector->SetOptions(vecOptions);
      m_vRemSelector->SetText(szEventDets);
      if( vecOptions.size() > 0 )
      {
         m_vRemSelector->SetAutoComplete(vecOptions[0]);
      }
   }
   m_bHandleTextEvent = true;
}

void 
viCollectionEditor::onComboBoxAccept(wxCommandEvent& awxEvt)
{
   if( awxEvt.GetInt() == Selectors::Add )
   {
      // TODO: Construct the label and the command here. or gather it.
      m_vListView->AddItem(m_vAddSelector->GetText());
   }
   else if( awxEvt.GetInt() == Selectors::Remove )
   {

   }
}