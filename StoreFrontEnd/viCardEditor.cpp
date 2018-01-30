#include "viCardEditor.h"
#include "StoreFrontEnd.h"
#include <wx/url.h>
#include <wx/sstream.h>
#include <wx/imagjpeg.h>
#include <wx/wfstream.h>

viCardEditor::viCardEditor( wxWindow* aptParent, wxWindowID aiWID,
                            wxString aszColID, wxString aszCardHash )
   : wxPanel(aptParent, aiWID),  m_szColID(aszColID)
{
   wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
   this->SetSizer(boxSizer);
   this->SetSize(250, 350);

   DisplayNew(aszColID, aszCardHash);
}

viCardEditor::~viCardEditor()
{

}

void 
viCardEditor::DisplayNew(wxString aszColID, wxString aszCardHash)
{
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

   fetchImage();
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
      SFE.DownloadCardImage(szFullPath, m_szCardName, szSet, szMUD);
   }
   imageFile.close();

   imageFile = ifstream(szFullPath.ToStdString().c_str());
   if( imageFile.good() )
   {
      imageFile.close();
      freeImage();
      setImage(szFullPath);
   }
   else
   {
      imageFile.close();
   }
}

void 
viCardEditor::freeImage()
{
   if( m_jpgPanel )
   {
      this->GetSizer()->Detach(m_jpgPanel);
      m_jpgPanel->Destroy();
   }
}

void 
viCardEditor::setImage(const wxString& aszImagePath)
{
   m_jpgPanel = new wxImagePanel(this, aszImagePath, wxBitmapType::wxBITMAP_TYPE_ANY);
   this->GetSizer()->Add(m_jpgPanel, wxSizerFlags(1).Expand());
}
