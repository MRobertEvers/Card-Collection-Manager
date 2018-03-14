#include "ImageFetcher.h"
#include "wx/wxprec.h"
#include "StoreFrontEnd.h"
#include "CURLAPI.h"

#include <wx/sstream.h>
#include <wx/imagjpeg.h>


ImageFetcherCallback::ImageFetcherCallback()
   : m_bDoCall(true)
{

}

ImageFetcherCallback::~ImageFetcherCallback()
{

}

void 
ImageFetcherCallback::SetDoCall( bool abDoCall )
{
   m_bDoCall = abDoCall;
}

bool 
ImageFetcherCallback::GetDoCall()
{
   return m_bDoCall;
}


wxThread::ExitCode 
FetcherThread::Entry()
{
   ImageFetcher::Instance()->DownloadCardImage( m_szFilePath, m_szCardName, 
                                                m_szSet, m_szMUD, m_ptCallback );
   return (wxThread::ExitCode)0;
}


ImageFetcher* ImageFetcher::ms_ptInstance = nullptr;
std::mutex ImageFetcher::ms_mutexInstance;

ImageFetcher::ImageFetcher()
{
}


ImageFetcher::~ImageFetcher()
{
}

void 
ImageFetcher::PDownloadImage( const wxString& aszFilePath,
                              const wxString& aszCardName,
                              const wxString& aszSet,
                              const wxString& aszMUD,
                              std::shared_ptr<ImageFetcherCallback> aptCallback )
{
   bool bStartThread = false;
   m_mutexThreadList.lock();
   {
      bStartThread = m_setThreadList.find(aszFilePath) == m_setThreadList.end();
      m_setThreadList.insert(aszFilePath);
   }
   m_mutexThreadList.unlock();

   if( bStartThread )
   {
      // The FetcherThread will delete itself upon completion. See wxThread docs.
      FetcherThread* pDownloadThread = new FetcherThread( aszFilePath, aszCardName, aszSet, aszMUD,
                                                          aptCallback );
      pDownloadThread->Run();
   }
}

bool 
ImageFetcher::DownloadCardImage( const wxString& aszFilePath,
                                 const wxString& aszCardName, 
                                 const wxString& aszSet,
                                 const wxString& aszMUD )
{
   return DownloadCardImage( aszFilePath, aszCardName, aszSet, aszMUD, nullptr );
}

bool 
ImageFetcher::DownloadCardImage( const wxString& aszFilePath,
                                 const wxString& aszCardName,
                                 const wxString& aszSet,
                                 const wxString& aszMUD,
                                 std::shared_ptr<ImageFetcherCallback> aptCallback )
{
   PrepareImageSetFolder(aszSet);

   bool bSuccess = downloadImage(aszFilePath, aszCardName, aszSet, aszMUD);

   if( aptCallback != nullptr && bSuccess )
   {
      aptCallback->CallBack();
   }

   m_mutexThreadList.lock();
   auto szThread = m_setThreadList.find(aszFilePath);
   if( szThread != m_setThreadList.end() )
   {
      m_setThreadList.erase(szThread);
   }
   m_mutexThreadList.unlock();

   return true;
}


bool 
ImageFetcher::downloadImage( const wxString& aszFilePath,
                             const wxString& aszCardName,
                             const wxString& aszSet,
                             const wxString& aszMUD )
{
   for( int i = 0; i < 10; i++ )
   {
      if( tryDownload( aszFilePath, aszCardName, aszSet, aszMUD ) )
      {
         return true;
      }
      else
      {
         // Delete the corrupt file.
         auto imageFile = std::ifstream( aszFilePath.ToStdString().c_str() );
         if( imageFile.good() )
         {
            imageFile.close();
            std::remove( aszFilePath.ToStdString().c_str() );
         }
         else
         {
            imageFile.close();
         }
      }
   }

   return false;
}

bool 
ImageFetcher::tryDownload( const wxString& aszFilePath,
                           const wxString& aszCardName,
                           const wxString& aszSet,
                           const wxString& aszMUD )
{
   // I don't know why the downloads fail sometimes.
   // But they are. So just try a couple times.
   wxString szUrl;
   if( !wxString( aszMUD ).Trim().IsEmpty() )
   {
      szUrl = "http://gatherer.wizards.com/Handlers/Image.ashx?multiverseid=" + aszMUD + "&type=card";
   }
   else
   {
      szUrl = "http://gatherer.wizards.com/Handlers/Image.ashx?name=" + aszCardName + "&type=card";
   }

   {
      FileWriterFunctor dler( aszFilePath.ToStdString() );
      CURLAPI::Easy_HTTP( szUrl.ToStdString(), &dler );
   }

   // Check if the download works
   bool bGood = false;

   wxLog::EnableLogging( false );
   wxLogNull logNo;
   wxImage* img = new wxImage();
   bGood = img->LoadFile( aszFilePath );
   img->Destroy();
   wxLog::EnableLogging( true );

   return bGood;
}

bool
ImageFetcher::PrepareImageSetFolder(const wxString& aszSet)
{
   StoreFront* ptSF = StoreFrontEnd::Server();

   wxString szImagePaths = ptSF->GetImagesDirectory();
   szImagePaths += "\\_" + aszSet;

   StoreFrontEnd::Client()->EstablishFolder( szImagePaths );
   return true;
}

ImageFetcher* 
ImageFetcher::Instance()
{
   ms_mutexInstance.lock();
   if( ms_ptInstance == nullptr )
   {
      ms_ptInstance = new ImageFetcher();
   }
   ms_mutexInstance.unlock();

   return ms_ptInstance;
}