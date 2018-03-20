#include "vicBlogItem.h"
#include "StoreFrontEnd.h"
#include "ImageViewer.h"

vicBlogItem::vicBlogItem( wxWindow* aptParent,
                          const wxString& aszTitle, 
                          const std::vector<std::string>& avecHistory )
 : wxPanel( aptParent, 2 )
{
   auto ptSF = StoreFrontEnd::Server();

   wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
   this->SetSizer( sizer );


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
         wxPanel* actionPanel = new wxPanel( this );
         wxFlexGridSizer* subsizer = new wxFlexGridSizer( 2 );
         subsizer->AddGrowableCol( 1 );
         subsizer->SetFlexibleDirection( wxHORIZONTAL );
         subsizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_NONE );
         actionPanel->SetSizer( subsizer );

         if( Type == StringInterface::AddLine )
         {
            wxString szAddFileImg = ptSF->GetConfigDirectory() + "View\\AddSymb.jpg";
            ImageViewer* imgTest = new ImageViewer( actionPanel, 1, false );
            imgTest->DisplayImage( szAddFileImg );
            auto test = imgTest->GetSize();
            subsizer->Add( imgTest, wxSizerFlags( 1 ).CenterVertical() );

            wxStaticText* txt = new wxStaticText( actionPanel, 1, FirstItem.Name );
            subsizer->Add( txt, wxSizerFlags( 1 ).Expand() );
         }
         else if( Type == StringInterface::RemoveLine )
         {

         }
         else if( ( Type == StringInterface::ChangeLine ) &&
                  ( SecondItem.Name != "" ) )
         {

         }

         sizer->Add( actionPanel, wxSizerFlags( 1 ).Expand() );
      }
   }
}


vicBlogItem::~vicBlogItem()
{
}
