#include "viCardEditor.h"
#include "StoreFrontEnd.h"
#include "vcEditableTraitList.h"
#include "vcEditableItemList.h"
#include "vcImageWrapper.h"
#include <string>
#include <wx/url.h>
#include <wx/sstream.h>
#include <wx/imagjpeg.h>
#include <wx/wfstream.h>

wxBEGIN_EVENT_TABLE(viCardEditor, wxPanel)
EVT_BUTTON(Changes_Submit, viCardEditor::onChangesAccept)
EVT_BUTTON(Changes_Reset, viCardEditor::onChangesReset)
wxEND_EVENT_TABLE()

viCardEditor::viCardEditor( wxWindow* aptParent, wxWindowID aiWID,
                            wxString aszColID, wxString aszCardHash )
   : wxPanel(aptParent, aiWID),  m_szColID(aszColID)
{
   wxFlexGridSizer* boxSizer = new wxFlexGridSizer(4, 1, 0, 0);//(wxVERTICAL);//(2,1,0,0);
   boxSizer->SetFlexibleDirection(wxVERTICAL);
   boxSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_NONE);
   // boxSizer->AddGrowableRow(1);
   boxSizer->AddGrowableRow(2);
  
   this->SetSizer(boxSizer);
   this->SetSize(250, 500);

   DisplayNew(aszColID, aszCardHash);
   buildSubmitResetButtons();
}

viCardEditor::~viCardEditor()
{

}

void 
viCardEditor::DisplayNew(wxString aszColID, wxString aszCardHash)
{
   if( parseNew(aszColID, aszCardHash) )
   {
      this->Freeze();
      refreshDisplay();
      refreshEditor();
      refreshSelector();
      this->Thaw();
   }
}

void 
viCardEditor::fetchImage()
{
   StoreFront* ptSF = StoreFrontEnd::Instance();
   StoreFrontEnd SFE;

   wxString szMUD = "";
   wxString szSet = "";
   for( auto& trait : m_vecAttrs )
   {
      if( trait.Key == "multiverseid" )
      {
         szMUD = trait.Selection;
      }
      else if( trait.Key == "set" )
      {
         szSet = trait.Selection;
      }
   }

   wxString szImagePaths = ptSF->GetImagesPath();
   szImagePaths += "\\_";
   szImagePaths += szSet;

   wxString szFullPath = szImagePaths + "\\" + m_szCardName + ".jpg";

   ifstream imageFile(szFullPath.ToStdString().c_str());
   if( !imageFile.good() )
   {
      imageFile.close();
      SFE.DownloadCardImage(szFullPath, m_szCardName, szSet, szMUD);
   }
   else
   {
      imageFile.close();
      setImage(szFullPath);
   }
}

void 
viCardEditor::setImage(const wxString& aszImagePath)
{
   this->Freeze();
   m_jpgPanel->SetImage(aszImagePath);
   this->Thaw();
}


bool 
viCardEditor::parseNew(wxString aszColID, wxString aszCardHash)
{
   if( aszCardHash.IsEmpty() )
   {
      return false;
   }
   m_szDisplayingHash = aszCardHash;

   m_vecAttrs.clear();
   m_vecUIDs.clear();
   m_szColID = aszColID;

   StoreFront* ptSF = StoreFrontEnd::Instance();
   StringInterface parser;
   Query query;
   query.FindHash(aszCardHash.ToStdString());
   query.UIDs();

   auto vecItems = ptSF->GetAllCardsStartingWith(m_szColID.ToStdString(), query);

   // There should only be ONE item in the list.
   for( auto& item : vecItems )
   {
      unsigned int Count;
      string Name;
      vector<pair<string, string>> Identifiers;
      vector<pair<string, string>> MetaTags;

      parser.ParseCardLine(item, Count, Name, Identifiers, MetaTags);
      m_szCardName = Name;

      auto mapOptions = ptSF->GetIdentifyingAttributeOptions(Name);
      for( auto& pairAttr : Identifiers )
      {
         CETraitOption option;
         option.Selection = pairAttr.second;

         option.Key = pairAttr.first;

         auto iter_traits = mapOptions.find(pairAttr.first);
         option.Options = iter_traits->second;

         m_vecAttrs.push_back(option);
      }

      for( auto& pairUID : MetaTags )
      {
         m_vecUIDs.push_back(pairUID.second);
      }

      // Break if there is more than one for some reason.
      break;
   }

   return true;
}

void 
viCardEditor::refreshDisplay()
{
   if( m_jpgPanel == NULL )
   {
      m_jpgPanel = new vcImageWrapper(this, 1);
      this->GetSizer()->Add(m_jpgPanel, wxSizerFlags(1).Expand());
      //this->Layout();
   }

   fetchImage();
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

   wxButton* acceptButton = new wxButton(wxTopPanel,
      Changes_Submit,
      "Submit");
   wxButton* declineButton = new wxButton(wxTopPanel,
      Changes_Reset,
      "Cancel");

   wxTopPanel->GetSizer()->Add(acceptButton, wxSizerFlags(1).Center().Shaped());
   wxTopPanel->GetSizer()->Add(declineButton, wxSizerFlags(1).Center().Shaped());

   this->GetSizer()->Add(wxTopPanel, wxSizerFlags(1).Expand());
}


void 
viCardEditor::onChangesAccept(wxCommandEvent& awxEvt)
{
   StringInterface stringEditor;
   auto ptSF = StoreFrontEnd::Instance();

   // Send to server
   auto mapEditedTraits = m_wxTraitList->GetCurrentSelections();
   auto mapEditedItems = m_wxEditableItemList->GetUIDModifiedMap();
   std::vector<std::pair<std::string, std::string>> vecStringCmds;
   for( auto& item : mapEditedTraits )
   {
      vecStringCmds.push_back( std::make_pair(item.first.ToStdString(), 
                                              item.second.ToStdString()) );
   }

   std::vector<std::string> vecBulkChanges;
   for( auto& item : mapEditedItems )
   {
      if( item.second )
      {
         std::string szChangeCmd = stringEditor.CmdCreateModify( m_szCardName.ToStdString(), 
                                                                 item.first.ToStdString(),
                                                                 vecStringCmds );
         vecBulkChanges.push_back( szChangeCmd );
      }
   }

   ptSF->SubmitBulkChanges( m_szColID.ToStdString(), vecBulkChanges );
   // Send refresh event up to collection viewer.
}

void 
viCardEditor::onChangesReset(wxCommandEvent& awxEvt)
{
   DisplayNew(m_szColID, m_szDisplayingHash);
}