#include "viCardEditor.h"
#include "vcEditableTraitList.h"
#include "vcEditableItemList.h"
#include "../StoreFrontEnd/StoreFrontEnd.h"
#include "../ImageViewer/ImageViewer.h"
#include <string>
#include <wx/url.h>
#include <wx/sstream.h>
#include <wx/imagjpeg.h>
#include <wx/wfstream.h>
#include <wx/listctrl.h>
#include <algorithm>


viCardEditor::
viCardEditorImageCallBack::viCardEditorImageCallBack( viCardEditor* aptCE,
                                                      std::shared_ptr<std::mutex> amutex )
   : ImageFetcherCallback(), m_ptCardEditor(aptCE), m_mutex(amutex)
{

}

void
viCardEditor::
viCardEditorImageCallBack::CallBack()
{
   m_mutex->lock();
   if( GetDoCall() )
   {
      for( auto& ptCallback : m_ptCardEditor->m_vecImageCallbacks )
      {
         ptCallback->SetDoCall(false);
      }
      // Perhaps need to perform this on the main thread.
      wxCommandEvent updateEvt( wxEVT_BUTTON );
      updateEvt.SetId( Image_Changed );
      updateEvt.SetString( m_ptCardEditor->m_szDisplayingHash );
      ::wxPostEvent(m_ptCardEditor->GetParent(), updateEvt );

   }
   m_mutex->unlock();
}

wxBEGIN_EVENT_TABLE(viCardEditor, wxPanel)
EVT_BUTTON(Changes_Submit, viCardEditor::onChangesAccept)
EVT_BUTTON(Changes_Reset, viCardEditor::onChangesReset)
wxEND_EVENT_TABLE()

viCardEditor::viCardEditor( wxWindow* aptParent, wxWindowID aiWID,
                            wxString aszColID, wxString aszCardHash )
   : wxPanel(aptParent, aiWID),  m_szColID(aszColID)
{
   m_mutex = std::shared_ptr<std::mutex>( new std::mutex() );

   wxFlexGridSizer* boxSizer = new wxFlexGridSizer(4, 1, 0, 0);//(wxVERTICAL);//(2,1,0,0);
   boxSizer->SetFlexibleDirection(wxVERTICAL);
   boxSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_NONE);
   // boxSizer->AddGrowableRow(1);
   boxSizer->AddGrowableRow(2);
  
   this->SetSizer(boxSizer);
   this->SetSize(250, 500);

   DisplayNew(aszColID, aszCardHash );
   buildSubmitResetButtons();
}

viCardEditor::~viCardEditor()
{

}

void 
viCardEditor::DisplayNew(wxString aszColID, wxString aszCardHash )
{
   if( parseNew(aszColID, aszCardHash) )
   {
      this->Freeze();
      refreshDisplay();
      refreshEditor();
      refreshSelector();
      this->Layout();
      this->Thaw();
   }
}

wxString 
viCardEditor::GetDisplayingCard()
{
   return m_szCardName;
}

bool 
viCardEditor::parseNew(wxString aszColID, wxString aszCardHash )
{
   if( aszCardHash.IsEmpty() )
   {
      return false;
   }

   m_szDisplayingHash = aszCardHash;

   m_vecAttrs.clear();
   m_vecUIDs.clear();
   m_szColID = aszColID;

   StoreFront* ptSF = StoreFrontEnd::Server();
   std::vector<std::string> vecItems;
   Query query;
   query.FindHash( aszCardHash.ToStdString() );
   query.UIDs();

   vecItems = ptSF->GetAllCardsStartingWith( m_szColID.ToStdString(), query );

   // There should only be ONE item in the list.
   if( vecItems.size() > 0 )
   {
      auto item = vecItems.at( 0 );
      unsigned int Count;
      string ParentGroupName;
      vector<pair<string, string>> Identifiers;
      vector<pair<string, string>> MetaTags;

      StringInterface::ParseCardLine( item, Count, ParentGroupName, Identifiers, MetaTags );
      m_szCardName = ParentGroupName;

      auto mapOptions = ptSF->GetIdentifyingAttributeOptions( ParentGroupName );
      for( auto& pairAttr : Identifiers )
      {
         CETraitOption option;
         option.Selection = pairAttr.second;

         option.Key = pairAttr.first;

         auto iter_traits = mapOptions.find( pairAttr.first );
         option.Options = iter_traits->second;

         m_vecAttrs.push_back( option );
      }

      for( auto& pairUID : MetaTags )
      {
         m_vecUIDs.push_back( pairUID.second );
      }

      for( auto& trait : m_vecAttrs )
      {
         if( trait.Key == "multiverseid" )
         {
            m_szDisplayMUD = trait.Selection;
         }
         else if( trait.Key == "set" )
         {
            m_szDisplaySet = trait.Selection;
         }
      }
   }

   return true;
}

void 
viCardEditor::refreshDisplay()
{
   if( m_jpgPanel == NULL )
   {
      m_jpgPanel = new ImageViewer(this, 1);
      this->GetSizer()->Add(m_jpgPanel, wxSizerFlags(1).Expand());
   }

   m_jpgPanel->DisplayImage( m_szCardName, m_szDisplayMUD, m_szDisplaySet );
}

void 
viCardEditor::refreshEditor()
{
   if( m_wxTraitList == NULL )
   {
      buildTraitListEditor();
   }

   if( m_vecUIDs.size() > 0 )
   {
      m_wxTraitList->RefreshNew(m_szCardName, m_vecUIDs[0]);
   }
   // TODO: ELSE FAIL or CLEAR OPTIONS
}

void 
viCardEditor::refreshSelector()
{
   if( m_wxEditableItemList == NULL )
   {
      m_wxEditableItemList = new vcEditableItemList(this, 0, m_vecUIDs);
      this->GetSizer()->Add(m_wxEditableItemList, wxSizerFlags(1).Expand());
   }
   else
   {
      m_wxEditableItemList->RefreshNew(m_vecUIDs);
   }
   
}

void 
viCardEditor::buildTraitListEditor()
{
   if( m_vecUIDs.size() == 0 )
   {
      return;
   }
   // TODO: ELSE DO SOMETHING

   m_wxTraitList = new vcEditableTraitList(this, 5, m_szCardName, m_vecUIDs[0]);
   this->GetSizer()->Add(m_wxTraitList, wxSizerFlags(1).Expand());
}

void 
viCardEditor::buildSubmitResetButtons()
{
   wxPanel* wxTopPanel = new wxPanel(this, wxID_ANY);
   wxBoxSizer* boxSizer = new wxBoxSizer(wxHORIZONTAL);
   wxTopPanel->SetSizer(boxSizer);

   wxButton* acceptButton = new wxButton( wxTopPanel,
                                          Changes_Submit,
                                          "Submit" );
   wxButton* declineButton = new wxButton( wxTopPanel,
                                           Changes_Reset,
                                           "Cancel" );

   wxTopPanel->GetSizer()->Add(acceptButton, wxSizerFlags(1).Center().Shaped());
   wxTopPanel->GetSizer()->Add(declineButton, wxSizerFlags(1).Center().Shaped());

   this->GetSizer()->Add(wxTopPanel, wxSizerFlags(1).Expand());
}


void 
viCardEditor::onChangesAccept(wxCommandEvent& awxEvt)
{
   // Send to server.
   auto vecEditedItems = submitChangesToServer();

   // Save one of the editted UIDs so that the parent can find the new
   // card.
   if( vecEditedItems.size() > 0 )
   {
      awxEvt.SetString( vecEditedItems.front() );

      // Send refresh event up to collection viewer.
      awxEvt.Skip();
   }
}

std::vector<std::string>
viCardEditor::submitChangesToServer()
{
   // Returns a list of modified UIDs.
   std::vector<std::string> vecRetVal;

   auto ptSF = StoreFrontEnd::Server();

   auto mapEditedTraits = m_wxTraitList->GetCurrentSelections();
   std::vector<std::pair<std::string, std::string>> vecStringCmds;
   for( auto& item : mapEditedTraits )
   {
      vecStringCmds.push_back( std::make_pair( item.first.ToStdString(),
                                               item.second.ToStdString() ) );
   }

   std::vector<std::string> vecBulkChanges;
   int iChanged = 0;
   auto mapEditedItems = m_wxEditableItemList->GetUIDModifiedMap();
   for( auto& item : mapEditedItems )
   {
      // item.second is a bool indicating if the item is checked.
      if( item.second )
      {
         iChanged++;
         std::string szChangeCmd = StringInterface::CmdCreateModify( m_szCardName.ToStdString(),
                                                                     item.first.ToStdString(),
                                                                     vecStringCmds );
         vecBulkChanges.push_back( szChangeCmd );
         vecRetVal.push_back( item.first.ToStdString() );
      }
   }

   ptSF->SubmitBulkChanges( m_szColID.ToStdString(), vecBulkChanges );
   return vecRetVal;
}

void 
viCardEditor::onChangesReset(wxCommandEvent& awxEvt)
{
   DisplayNew(m_szColID, m_szDisplayingHash);
   this->Layout();
}