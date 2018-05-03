#include "CardRenderer.h"
#include "../StoreFrontEnd/CardInterface.h"
#include "../StoreFrontEnd/StoreFrontEnd.h"
#include "../Views/ImageViewer/ImageViewer.h"
CardRenderer::
ImageDownloadCallback::ImageDownloadCallback( CardRenderer* apHost,
                                              CardInterface* apCallbackCard,
                                              std::shared_ptr<std::mutex> aCallBackMutex )
   : m_uiRenderCountTarget(apHost->m_uiRenderCount), m_pCardInterface(apCallbackCard)
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

      // Data to isolate the correct callback function
      wxCommandEvent updateEvt( wxEVT_BUTTON );
      updateEvt.SetId( CardRenderer::Image_Downloaded );

      // Data used in the callback.
      updateEvt.SetInt( m_pCardEditor->m_uiRenderCount );
      updateEvt.SetClientData( m_pCardInterface );

      ::wxPostEvent( m_pCardEditor , updateEvt );
   }
   m_mutex->unlock();
}

unsigned int 
CardRenderer::
ImageDownloadCallback::GetRenderCountTarget()
{
   return m_uiRenderCountTarget;
}

wxBEGIN_EVENT_TABLE( CardRenderer, wxPanel )
EVT_BUTTON( CardRenderer::Image_Downloaded, CardRenderer::onImageCallback )
wxEND_EVENT_TABLE()


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
CardRenderer::onImageCallback( wxCommandEvent& awxEvt )
{
   // We don't have to worry about the CardInterface being deleted
   // at this point because interface's are only deleted when there are no
   // displaying collections. If there are no displaying connections, that
   // means that all callbacks have been invalidated. (i.e. they won't call)

   // We are also on the main thread so we don't have to worry about syncing
   // with other display image.
   auto iCallbackRenderCount = (unsigned int)awxEvt.GetInt();
   if( iCallbackRenderCount >= m_uiRenderCount )
   {
      auto pCard = (CardInterface*)awxEvt.GetClientData();
      DisplayImage( pCard );
   }
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
}