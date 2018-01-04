#include "viCollectionEditor.h"
#include "vicCollectionEditorList.h"
#include "StoreFront.h"

#include <chrono>

#define TIMER_ID 5
#define DROP_DELAY 370

wxBEGIN_EVENT_TABLE(viCollectionEditor, wxPanel)
EVT_TEXT(vicListSelector::ComboBox, viCollectionEditor::onComboBoxTextChanged)
EVT_BUTTON(vicListSelector::AcceptButton, viCollectionEditor::onComboBoxAccept)
EVT_TIMER(TIMER_ID, viCollectionEditor::onDropDownDelay)
EVT_COMBOBOX(vicListSelector::ComboBox, viCollectionEditor::onComboBoxSelection)
wxEND_EVENT_TABLE()


viCollectionEditor::viCollectionEditor(wxWindow* aptParent, wxWindowID aiWID, wxString aszColID)
   : wxPanel(aptParent, aiWID), m_szCollectionID(aszColID),
     m_bHandleTextEvent(true), m_ulTimeLastKeyStroke(0), m_timer(this, TIMER_ID),
     m_bIsWaitingForDrop(false), m_bIsSelectionFlag(false)
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
   if( !m_bHandleTextEvent || m_bIsSelectionFlag ) 
   {
      m_bIsSelectionFlag = false;
      return;
   }

   m_ulTimeLastKeyStroke = getTime();
   awxEvt.StopPropagation();
   wxString szEventDets = awxEvt.GetString();

   if( szEventDets.size() < 4 )
   {
      return;
   }

   m_bHandleTextEvent = false;
   if( awxEvt.GetInt() == Selectors::Add )
   {
      StoreFront* ptSF = StoreFrontEnd::Instance();

      auto vecszOptions = ptSF->GetAllCardsStartingWith( szEventDets.ToStdString() );

      auto vecOptions = parseCollectionItemsList(vecszOptions);

      m_vAddSelector->ResetOption();
      m_vAddSelector->SetOptions(vecOptions);
      m_vAddSelector->SetText(szEventDets);
      if( !m_bIsWaitingForDrop )
      {
         m_timer.StartOnce(DROP_DELAY);
      }
   }
   else if( awxEvt.GetInt() == Selectors::Remove )
   {
      StoreFront* ptSF = StoreFrontEnd::Instance();

      // TODO: Optimize this search.
      // Cache the collection first time, then restrict that list by string.
      Collection::Query query(true);
      query.SearchFor(szEventDets.ToStdString());
      query.UIDs();

      auto vecszOptions = ptSF->GetAllCardsStartingWith( m_szCollectionID.ToStdString(),
                                                         query );

      auto vecOptions = parseCollectionItemsList(vecszOptions);
      m_vRemSelector->ResetOption();
      m_vRemSelector->SetOptions(vecOptions);
      m_vRemSelector->SetText(szEventDets);  
      if( !m_bIsWaitingForDrop )
      {
         m_timer.StartOnce(DROP_DELAY);
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

void 
viCollectionEditor::onComboBoxSelection(wxCommandEvent& awxEvt)
{
   m_bHandleTextEvent = false;
}

void 
viCollectionEditor::onDropDownDelay(wxTimerEvent& event)
{
   unsigned long ulTime = getTime();
   unsigned long ulTimeSinceLastKeystroke = ulTime - m_ulTimeLastKeyStroke;
   if( ulTimeSinceLastKeystroke > DROP_DELAY )
   {
      m_bHandleTextEvent = false;
      if( m_vAddSelector->IsFocussed() )
      {
         m_vAddSelector->ShowDropdown();
      }
      else if( m_vRemSelector->IsFocussed() )
      {
         m_vRemSelector->ShowDropdown();
      }
      m_bIsWaitingForDrop = false;
      m_bHandleTextEvent = true;
   }
   else
   {
      m_timer.StartOnce(DROP_DELAY - ulTimeSinceLastKeystroke);
   }
}

unsigned long 
viCollectionEditor::getTime()
{
   std::chrono::milliseconds ms =
      std::chrono::duration_cast< std::chrono::milliseconds >(
         std::chrono::system_clock::now().time_since_epoch()
         );
   return ms.count();
}

// TODO: This is ugly.
vector<vicListSelector::Option>
viCollectionEditor::parseCollectionItemsList(const vector<string>& avecItems)
{
   StoreFront* ptSF = StoreFrontEnd::Instance();
   vector<vicListSelector::Option> vecRetVal;
   for( auto& id : avecItems )
   {
      unsigned int Count;
      string Name;
      string DetailString;
      string MetaString;
      vector<pair<string,string>> Identifiers;
      vector<pair<string,string>> MetaTags;

      StringInterface parser;
      parser.ParseCardLine(id, Count, Name, Identifiers, MetaTags);

      vicListSelector::Option option;
      option.Display = ptSF->CollapseCardLine(id, false);

      option.UIDs = MetaTags;
      vecRetVal.push_back(option);
   }

   return vecRetVal;
}