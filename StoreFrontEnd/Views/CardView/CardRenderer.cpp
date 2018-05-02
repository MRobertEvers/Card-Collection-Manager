#include "CardRenderer.h"
#include "../StoreFrontEnd/CardInterface.h"
#include "../StoreFrontEnd/StoreFrontEnd.h"
#include "../Views/ImageViewer/ImageViewer.h"
CardRenderer::
ImageDownloadCallback::ImageDownloadCallback( CardRenderer* apHost,
                                              std::shared_ptr<std::mutex> aCallBackMutex )
   : m_uiRenderCountTarget(apHost->m_uiRenderCount)
{

}

CardRenderer::
ImageDownloadCallback::~ImageDownloadCallback()
{

}

void 
CardRenderer::
ImageDownloadCallback::CallBack()
{
   m_mutex->lock();
   if( ( GetDoCall() ) &&
       ( m_pCardEditor->m_uiRenderCount < m_uiRenderCountTarget ) )
   {

      // Perhaps need to perform this on the main thread.
      wxCommandEvent updateEvt( wxEVT_BUTTON );
      updateEvt.SetId( Image_Changed );
      updateEvt.SetString( m_ptCardEditor->m_szDisplayingHash );
      ::wxPostEvent( m_ptCardEditor->GetParent(), updateEvt );

   }
   m_mutex->unlock();
}

unsigned int 
CardRenderer::
ImageDownloadCallback::GetRenderCountTarget()
{
   return m_uiRenderCountTarget;
}

CardRenderer::CardRenderer( wxPanel* apParent )
   : wxPanel(apParent), m_pImagePanel(nullptr), m_uiRenderCount(0)
{

}

CardRenderer::~CardRenderer()
{

}

void 
CardRenderer::DisplayImage( CardInterface* apCard )
{
   // Display the image.
   m_pCardInterface = apCard;

   uiDisplayCard();
}

std::string 
CardRenderer::getCardImageFile()
{
   auto ptse = StoreFrontEnd::Server();
   return ptse->GetImageFilePath( m_pCardInterface->GetName(), m_pCardInterface->GetSet() );
}

void 
CardRenderer::uiDisplayCard()
{
   if( m_pImagePanel == nullptr )
   {
      m_pImagePanel = new ImageViewer( this, wxID_ANY, false );
   }

   // Check if the image file exists.
   auto ptse = StoreFrontEnd::Client();
   auto szFilePath = getCardImageFile();

   auto bFileExists = ptse->IsFileExist( szFilePath );
   if( ( !bFileExists ) ||
       ( !m_pImagePanel->DisplayImage(szFilePath) ) )  // If the file exists, try displaying
   {
      // If we couldn't display for some reason
      // Send out callback to download
   }

   ;
}