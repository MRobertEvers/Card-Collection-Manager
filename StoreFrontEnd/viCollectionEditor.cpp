#include "viCollectionEditor.h"
#include "vicCollectionEditorList.h"
#include "StoreFrontEnd.h"
#include <chrono>

#define TIMER_ID 5
#define DROP_DELAY 210

wxBEGIN_EVENT_TABLE(viCollectionEditor, wxFrame )
EVT_TEXT(vicListSelector::ComboBox, viCollectionEditor::onComboBoxTextChanged)
EVT_BUTTON(vicListSelector::AcceptButton, viCollectionEditor::onComboBoxAccept)
EVT_TIMER(TIMER_ID, viCollectionEditor::onDropDownDelay)
EVT_COMBOBOX(vicListSelector::ComboBox, viCollectionEditor::onComboBoxSelection)
EVT_BUTTON(Changes_Accept, viCollectionEditor::onAccept)
EVT_BUTTON(Changes_Decline, viCollectionEditor::onDecline)
EVT_CLOSE( viCollectionEditor::onClose )
wxEND_EVENT_TABLE()


viCollectionEditor::viCollectionEditor(wxWindow* aptParent, wxWindowID aiWID, wxString aszColID)
   : wxFrame(aptParent, aiWID, "Collection Editor", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP ),
     m_szCollectionID(aszColID),
     m_bHandleTextEvent(true), m_ulTimeLastKeyStroke(0), m_timer(this, TIMER_ID),
     m_bIsWaitingForDrop(false), m_bIsSelectionFlag(false)
{
   wxFlexGridSizer* boxSizer = new wxFlexGridSizer(3,1,0,0);
   boxSizer->AddGrowableCol(0);
   boxSizer->AddGrowableRow(1);
   this->SetSizer(boxSizer);

   buildSelectors();
   buildListView();
   buildButtons();

   this->SetSize(wxSize(350, 400));
   this->SetSizeHints(wxSize(350, 400));
   this->Layout();
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

   wxTopPanel->GetSizer()->Add(acceptButton, wxSizerFlags(1).Center().Shaped());
   wxTopPanel->GetSizer()->Add(declineButton, wxSizerFlags(1).Center().Shaped());

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
      StoreFront* ptSF = StoreFrontEnd::Server();
      Query query;
      query.SearchFor(szEventDets.ToStdString());
      query.UIDs();

      auto vecszOptions = ptSF->GetAllCardsStartingWith(query);

      auto vecOptions = CELIOption::ParseCollectionItemsList(vecszOptions);

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
      StoreFront* ptSF = StoreFrontEnd::Server();

      // TODO: Optimize this search.
      // Cache the collection first time, then restrict that list by string.
      Query query(true);
      query.SearchFor(szEventDets.ToStdString());
      query.UIDs();

      auto vecszOptions = ptSF->GetAllCardsStartingWith( m_szCollectionID.ToStdString(),
                                                         query );

      auto vecOptions = CELIOption::ParseCollectionItemsList(vecszOptions);
      m_vRemSelector->ResetOption();
      m_vRemSelector->SetOptions(vecOptions);
      m_vRemSelector->SetText(szEventDets);  
      if( !m_bIsWaitingForDrop )
      {
         m_timer.StartOnce(DROP_DELAY);
      }
   }
   m_bHandleTextEvent = true;
   // This fixes the issue where sometimes the dropbox doesn't close right.
   PostSizeEvent();
}

void 
viCollectionEditor::onComboBoxAccept(wxCommandEvent& awxEvt)
{
   if( awxEvt.GetInt() == Selectors::Add )
   {
      auto oSelectionAdd = m_vAddSelector->GetBestSelection();
      if( oSelectionAdd.Display != "" )
      {
         m_vListView->AddItem(oSelectionAdd);
      }
   }
   else if( awxEvt.GetInt() == Selectors::Remove )
   {
      auto oSelectionRemove = m_vRemSelector->GetSelection();
      auto oSelectionAdd = m_vAddSelector->GetBestSelection();
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
            StringInterface::CmdCreateAddition( deltaItem.DisplayOne.ToStdString(),
                                                deltaItem.SelectionOne.ToStdString());
      }
      else if(deltaItem.DisplayTwo == "")
      {
         // Its a remove
         szCmd =
            StringInterface::CmdCreateRemove( deltaItem.DisplayOne.ToStdString(),
                                              deltaItem.SelectionOne.ToStdString());
      }
      else
      {
         // Its a replace.
         szCmd =
            StringInterface::CmdCreateReplace( deltaItem.DisplayOne.ToStdString(),
                                               deltaItem.SelectionOne.ToStdString(),
                                               deltaItem.DisplayTwo.ToStdString(),
                                               deltaItem.SelectionTwo.ToStdString() );
      }
      szCmd = StringInterface::CmdAppendCount(szCmd, deltaItem.Count);
      vecCmds.push_back(szCmd);
   }

   StoreFrontEnd::Server()->
      SubmitBulkChanges(m_szCollectionID.ToStdString(), vecCmds);
   m_vListView->ClearList();
   m_vAddSelector->ResetOption();
   m_vRemSelector->ResetOption();
   awxEvt.Skip();
}

void 
viCollectionEditor::onDecline(wxCommandEvent& awxEvt)
{
   // TODO: CLOSE THIS WINDOW>
   awxEvt.Skip();
}

void 
viCollectionEditor::onClose( wxCloseEvent& awxEvt )
{
   wxCommandEvent wxCloseEvent( wxEVT_BUTTON );
   wxCloseEvent.SetId( Changes_Decline );
   ::wxPostEvent( this->GetParent(), wxCloseEvent );

   // Need to veto because the parent will destroy this.
   awxEvt.Veto();
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
