#include "CardRenderer.h"
#include "../StoreFrontEnd/CardInterface.h"
#include "../StoreFrontEnd/StoreFrontEnd.h"
#include "../Views/ImageViewer/ImageViewer.h"
CardRenderer::
ImageDownloadCallback::ImageDownloadCallback( CardRenderer * apHost, 
                                              const std::string & aszCardName, const std::string & aszSet,
                                              std::shared_ptr<std::recursive_mutex> aCallBackMutex )
   : m_pCardEditor( apHost ), m_uiRenderCountTarget( apHost->m_uiRenderCount ), m_szCardName(aszCardName), m_szSet(aszSet)
{
   m_mutex = std::shared_ptr<std::recursive_mutex>( aCallBackMutex );
}
CardRenderer::
ImageDownloadCallback::ImageDownloadCallback( CardRenderer* apHost,
                                              CardInterface* apCallbackCard,
                                              std::shared_ptr<std::recursive_mutex> aCallBackMutex )
   : ImageDownloadCallback(apHost, apCallbackCard->GetName(), apCallbackCard->GetSet(), aCallBackMutex)
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
       ( m_pCardEditor->m_uiRenderCount == m_uiRenderCountTarget ) )
   {
      // Perhaps need to perform this on the main thread.

      // Data to isolate the correct callback function
      wxCommandEvent updateEvt( wxEVT_BUTTON );
      updateEvt.SetId( CardRenderer::Image_Downloaded );

      // Data used in the callback.
      updateEvt.SetInt( m_pCardEditor->m_uiRenderCount );

      // The pair MUST BE DELETED!
      updateEvt.SetClientData( new std::pair<std::string, std::string>(m_szCardName, m_szSet) );
      
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
   m_mutex = std::shared_ptr<std::recursive_mutex>( new std::recursive_mutex() );
   m_pImagePanel = new ImageViewer( this, wxID_ANY, false );
}

CardRenderer::~CardRenderer()
{
   m_mutex->lock();
   if( m_pCurrentCallback != nullptr )
   {
      m_pCurrentCallback->SetDoCall( false );
   }
   m_mutex->unlock();
}

void 
CardRenderer::DisplayImage( CardInterface* apCard )
{
   // Display the image.
   m_pCardInterface = std::shared_ptr<CardInterface>( new CardInterface(*apCard) );

   uiDisplayCard( m_pCardInterface->GetName(), m_pCardInterface->GetSet() );
}

void
CardRenderer::DisplayImage( const wxString & aszCardname, const wxString & aszSet )
{
   if( !aszSet.IsEmpty() )
   {
      auto ptSF = StoreFrontEnd::Server();
      Query cardSearch;
      cardSearch.SearchFor( aszCardname.ToStdString() );
      auto vecFound = ptSF->GetAllCardsStartingWith( cardSearch );
      if( vecFound.size() > 0 )
      {
         uiDisplayCard( aszCardname, aszSet );
      }
   }
}

std::string 
CardRenderer::getCardImageFile( const wxString& aszCardname, const wxString& aszSet )
{
   auto ptse = StoreFrontEnd::Server();
   return ptse->GetImageFilePath( aszCardname.ToStdString(), aszSet.ToStdString() );
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
      auto pair = (std::pair<std::string, std::string>*)awxEvt.GetClientData();
      DisplayImage( pair->first, pair->second );
      delete pair;
   }

   m_pCurrentCallback = nullptr;
}

void 
CardRenderer::uiDisplayCard( const wxString & aszCardname, const wxString & aszSet )
{
   auto szFilePath = getCardImageFile( aszCardname, aszSet );

   auto ptse = StoreFrontEnd::Client();
   auto bFileExists = ptse->IsFileExist( szFilePath );
   if( ( !bFileExists) ||
       ( !m_pImagePanel->DisplayImage( szFilePath )) )  // If the file exists, try displaying
   {
      // If we couldn't display for some reason
      // Send out callback to download

      // Disable the last callback
      m_mutex->lock();
      if( m_pCurrentCallback != nullptr )
      {
         m_pCurrentCallback->SetDoCall( false );
      }
      m_mutex->unlock();

      auto callBack = std::shared_ptr<ImageDownloadCallback>( new ImageDownloadCallback( this, aszCardname.ToStdString(), aszSet.ToStdString(), m_mutex ) );
      m_pCurrentCallback = callBack;

      // The thread keeps a shared pointer to the callback so we don't have to worry about randomly deleting
      // callbacks. 
      ptse->DownloadCardImage( aszCardname, aszSet, std::shared_ptr<ImageFetcherCallback>( callBack ) );
   }
}
