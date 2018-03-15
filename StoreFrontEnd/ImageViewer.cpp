#include "ImageViewer.h"
#include "StoreFrontEnd.h"

ImageViewer::
ImageViewerCallback::
ImageViewerCallback( ImageViewer* aptCE, std::shared_ptr<std::recursive_mutex> amutex, const wxString& aszFilePath )
   : ImageFetcherCallback(), m_ptViewer( aptCE ), m_mutex( amutex ), m_szFilePath(aszFilePath)
{

}

void
ImageViewer::
ImageViewerCallback::CallBack()
{
   m_mutex->lock();
   if( GetDoCall() )
   {
      // Perhaps need to perform this on the main thread.
      wxCommandEvent updateEvt( wxEVT_BUTTON );
      updateEvt.SetId( Image_Ready );
      updateEvt.SetString( m_szFilePath );
      ::wxPostEvent( m_ptViewer, updateEvt );
   }
   m_mutex->unlock();
}

wxBEGIN_EVENT_TABLE( ImageViewer, wxPanel )
EVT_BUTTON( Image_Ready, ImageViewer::onImageReady )
wxEND_EVENT_TABLE()

ImageViewer::ImageViewer( wxWindow* aptParent, wxWindowID aiWID )
   : wxPanel( aptParent, aiWID ), m_ptImageWrapper( new vcImageWrapper( this, 3 ) )
{
   m_mutex = std::shared_ptr<std::recursive_mutex>( new std::recursive_mutex() );

   wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
   this->SetSizer( boxSizer );
   boxSizer->Add( m_ptImageWrapper, wxSizerFlags( 1 ).Shaped() );
}


ImageViewer::~ImageViewer()
{
   stopCallbacks();
   m_vecImageCallbacks.clear();
}

bool
ImageViewer::DisplayImage( const wxString& aszCardName,
                           const wxString& aszMultiVerseID,
                           const wxString& aszSet,
                           bool abFetchIfNeeded )
{
   auto ptSF = StoreFrontEnd::Server();
   auto szImagePath = ptSF->GetImageFilePath( aszCardName.ToStdString(), aszSet.ToStdString() );
   bool bNeedFetch = !DisplayImage( wxString(szImagePath) );

   if( bNeedFetch )
   {
      fetchImage( aszCardName, aszMultiVerseID, aszSet );
      return false;
   }
   else
   {
      SetVirtualSize( m_ptImageWrapper->GetSize() );
   }

   return true;
}

bool
ImageViewer::DisplayImage( const wxString& aszFilePath )
{
   m_mutex->lock();
   wxLog::EnableLogging( false );
   bool bRetVal = m_ptImageWrapper->SetImage( aszFilePath );
   wxLog::EnableLogging( true );
   m_mutex->unlock();
   return bRetVal;
}

void 
ImageViewer::fetchImage( const wxString& aszCardName,
                         const wxString& aszMultiVerseID,
                         const wxString& aszSet )
{
   StoreFront* ptSF = StoreFrontEnd::Server();
   StoreFrontEnd* ptSFE = StoreFrontEnd::Client();

   wxString szFullPath = ptSF->GetImageFilePath( aszCardName.ToStdString(), aszSet.ToStdString() );

   ifstream imageFile( szFullPath.ToStdString().c_str() );
   if( !imageFile.good() )
   {
      imageFile.close();

      // Delete the other callbacks.
      stopCallbacks();
      m_vecImageCallbacks.clear();

      // Prepare a callback if the image isnt downloaded.
      auto callBack = std::shared_ptr<ImageFetcherCallback>( new ImageViewerCallback( this, m_mutex, szFullPath ) );
      m_vecImageCallbacks.push_back( callBack );
      ptSFE->DownloadCardImage( szFullPath, aszCardName, aszSet, aszMultiVerseID,
                                std::shared_ptr<ImageFetcherCallback>( callBack ) );
   }
   else
   {
      // TODO: There is a file already there?
      imageFile.close();
   }
   
}

void
ImageViewer::stopCallbacks( bool abBlock )
{
   if( abBlock ) { m_mutex->lock(); }

   for( auto& ptCallback : m_vecImageCallbacks )
   {
      if( !ptCallback ) { continue; }
      ptCallback->SetDoCall( false );
   }
   if( abBlock ) { m_mutex->unlock(); }
}


void 
ImageViewer::onImageReady( wxCommandEvent& awxEvt )
{
   DisplayImage( awxEvt.GetString() );
}