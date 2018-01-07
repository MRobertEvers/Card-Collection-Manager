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
EVT_BUTTON(Changes_Accept, viCollectionEditor::onAccept)
EVT_BUTTON(Changes_Decline, viCollectionEditor::onDecline)
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
      Query query;
      query.SearchFor(szEventDets.ToStdString());
      query.UIDs();

      auto vecszOptions = ptSF->GetAllCardsStartingWith(query);

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
      Query query(true);
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
      auto oSelectionAdd = m_vAddSelector->GetSelection();
      if( oSelectionAdd.Display != "" )
      {
         m_vListView->AddItem(oSelectionAdd);
      }
   }
   else if( awxEvt.GetInt() == Selectors::Remove )
   {
      auto oSelectionRemove = m_vRemSelector->GetSelection();
      auto oSelectionAdd = m_vAddSelector->GetSelection();
      if( oSelectionRemove.Display != "" )
      {
         if( oSelectionAdd.Display != "" )
         {
            m_vListView->AddItem(oSelectionAdd, oSelectionRemove);
            m_vAddSelector->SetText("");
         }
         else
         {
            m_vListView->AddItem(CELIOption(), oSelectionRemove);
         }
      }
   }
}

void 
viCollectionEditor::onComboBoxSelection(wxCommandEvent& awxEvt)
{
   m_bIsSelectionFlag = true;
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

void 
viCollectionEditor::onAccept(wxCommandEvent& awxEvt)
{
   StringInterface parser;
   std::vector<std::string> vecCmds;
   // Go through each of the list items. Construct the command, then
   // Send it to the server.
   for( auto& deltaItem : m_vListView->GetCommandList() )
   {
      string szCmd;
      if( !deltaItem.IsSwitched )
      {
         // Its an addition.
         szCmd =
            parser.CmdCreateAddition( deltaItem.DisplayOne.ToStdString(),
                                      deltaItem.SelectionOne.ToStdString());
      }
      else if(deltaItem.DisplayTwo == "")
      {
         // Its a remove
         szCmd =
            parser.CmdCreateRemove( deltaItem.DisplayOne.ToStdString(),
                                    deltaItem.SelectionOne.ToStdString());
      }
      else
      {
         // Its a replace.
         szCmd =
            parser.CmdCreateReplace(deltaItem.DisplayOne.ToStdString(),
                                    deltaItem.SelectionOne.ToStdString(),
                                    deltaItem.DisplayTwo.ToStdString(),
                                    deltaItem.SelectionTwo.ToStdString());
      }
      szCmd = parser.CmdAppendCount(szCmd, deltaItem.Count);
      vecCmds.push_back(szCmd);
   }

   StoreFrontEnd::Instance()->
      SubmitBulkChanges(m_szCollectionID.ToStdString(), vecCmds);
   m_vListView->ClearList();
   m_vAddSelector->ResetOption();
   m_vRemSelector->ResetOption();
   awxEvt.Skip();
}

void 
viCollectionEditor::onDecline(wxCommandEvent&)
{
   // TODO: CLOSE THIS WINDOW>
   return;
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
// Expects LongName { options }
vector<CELIOption>
viCollectionEditor::parseCollectionItemsList(const vector<string>& avecItems)
{
   StoreFront* ptSF = StoreFrontEnd::Instance();
   StringInterface parser;
   
   vector<CELIOption> vecRetVal;
   for( auto& id : avecItems )
   {
      unsigned int Count;
      string Name;
      string DetailString;
      string MetaString;
      vector<pair<string,string>> Identifiers;
      vector<pair<string,string>> MetaTags;

      parser.ParseCardLine(id, Count, Name, Identifiers, MetaTags);

      CELIOption option;
      option.Display = ptSF->CollapseCardLine(id, false);

      option.IDs = MetaTags;
      vecRetVal.push_back(option);
   }

   return vecRetVal;
}