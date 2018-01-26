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
   }



   fetchImage();
}

viCardEditor::~viCardEditor()
{

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
      if( m_jpgPanel )
      {
         this->GetSizer()->Detach(m_jpgPanel);
         m_jpgPanel->Destroy();
      }
      m_jpgPanel = new wxImagePanel(this, szFullPath, wxBitmapType::wxBITMAP_TYPE_ANY);
      this->GetSizer()->Add(m_jpgPanel, wxSizerFlags(1).Expand());
   }
   else
   {
      imageFile.close();
   }
}

void 
viCardEditor::downloadImage(const wxString& aszMUD, const wxString& aszFileName)
{
   wxURL url;
   if( aszMUD != "" )
   {
      url = (wxT("http://gatherer.wizards.com/Handlers/Image.ashx?multiverseid=" + aszMUD + "&type=card"));
   }
   else
   {
      url = (wxT("http://gatherer.wizards.com/Handlers/Image.ashx?name=" + m_szCardName + "&type=card"));
   }

   if( url.GetError() == wxURL_NOERR )
   {
      wxString htmldata;
      wxInputStream *in = url.GetInputStream();

      if( in && in->IsOk() )
      {
         wxFileOutputStream ofStreamFile("Test.jpg");
         in->Read(ofStreamFile);
         ofStreamFile.Close();
      }
      delete in;
   }
}