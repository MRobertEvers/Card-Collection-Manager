#include "vicBlogItem.h"
#include "StoreFrontEnd.h"
#include "wxImagePanel.h"
#include "vicBlogHeader.h"
#include "wxManualText.h"

vicBlogItem::vicBlogItem( wxWindow* aptParent,
                          const wxString& aszTitle, 
                          const std::vector<std::string>& avecHistory )
 : wxPanel( aptParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER )
{
   auto ptSF = StoreFrontEnd::Server();
   auto ptSFE = StoreFrontEnd::Client();

   wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
   this->SetSizer( sizer );

   // Add the header
   vicBlogHeader* header = new vicBlogHeader( this, aszTitle, "" );
   sizer->Add( header, wxSizerFlags( 0 ).Expand() );

   for( auto& szHistLine : avecHistory )
   {
      InterfaceLineParser FirstItem;
      InterfaceLineParser SecondItem;
      StringInterface::InterfaceLineType Type;

      bool ParseResult = StringInterface::ParseInterfaceLine( szHistLine,
         FirstItem.Count, FirstItem.Name, FirstItem.DetailVec, FirstItem.MetaVec,
         SecondItem.Count, SecondItem.Name, SecondItem.DetailVec, SecondItem.MetaVec,
         Type );

      if( !ParseResult )
      {
         continue;
      }

      if( FirstItem.Name != "" )
      {
         wxString szImgFilePath;
         wxString szLineText;
         if( Type == StringInterface::AddLine )
         {
            szImgFilePath = ptSFE->GetAddSymbolFilePath();
            szLineText = FirstItem.Name;
         }
         else if( Type == StringInterface::RemoveLine )
         {
            szImgFilePath = ptSFE->GetRemSymbolFilePath();
            szLineText = FirstItem.Name;
         }
         else if( ( Type == StringInterface::ChangeLine ) &&
                  ( SecondItem.Name != "" ) )
         {
            szImgFilePath = ptSFE->GetSwapSymbolFilePath();
            szLineText = FirstItem.Name + " > " + SecondItem.Name;
         }

         if( szImgFilePath != "" && szLineText != "" )
         {
            wxPanel* actionPanel = new wxPanel( this );
            wxFlexGridSizer* subsizer = new wxFlexGridSizer( 2 );
            subsizer->AddGrowableCol( 1 );
            subsizer->SetFlexibleDirection( wxHORIZONTAL );
            subsizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );
            actionPanel->SetSizer( subsizer );

            wxImagePanel* imgTest = new wxImagePanel( actionPanel, szImgFilePath, wxBitmapType::wxBITMAP_TYPE_ANY, false );
            subsizer->Add( imgTest, wxSizerFlags( 1 ).CenterVertical() );

            //wxManualText* txt = new wxManualText( actionPanel, szLineText );
            wxStaticText* txt = new wxStaticText( actionPanel, wxID_ANY, szLineText );
            subsizer->Add( txt, wxSizerFlags( 1 ).Expand() );
            subsizer->Layout();

            sizer->Add( actionPanel, wxSizerFlags( 1 ).Expand() );
         }
      }
   }
}


vicBlogItem::~vicBlogItem()
{
}
