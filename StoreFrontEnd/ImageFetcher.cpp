#include "ImageFetcher.h"
#include "wx/wxprec.h"
#include <Windows.h>
#include <wx/thread.h>
#include <wx/url.h>
#include <wx/sstream.h>
#include <wx/imagjpeg.h>
#include <wx/wfstream.h>
#include "StoreFrontEnd.h"


class FetcherThread : public wxThread
{
public:
   FetcherThread( const wxString& aszFilePath,
                  const wxString& aszCardName,
                  const wxString& aszSet,
                  const wxString& aszMUD ) 
      : wxThread(wxTHREAD_DETACHED), m_szFilePath(aszFilePath),
        m_szCardName(aszCardName), m_szSet(aszSet), m_szMUD(aszMUD)
   {};
   ~FetcherThread() {};
protected:
   virtual ExitCode Entry();

private:
   wxString m_szFilePath;
   wxString m_szCardName;
   wxString m_szSet;
   wxString m_szMUD;
};


wxThread::ExitCode FetcherThread::Entry()
{
   ImageFetcher::Instance()->DownloadCardImage( m_szFilePath, m_szCardName, 
                                                m_szSet, m_szMUD );
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
                              const wxString& aszMUD )
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
      FetcherThread* pDownloadThread = new FetcherThread(aszFilePath, aszCardName, aszSet, aszMUD);
      pDownloadThread->Run();
   }
}

bool 
ImageFetcher::DownloadCardImage( const wxString& aszFilePath,
                                 const wxString& aszCardName, 
                                 const wxString& aszSet,
                                 const wxString& aszMUD )
{
   PrepareImageSetFolder(aszSet);

   wxURL url;
   if( aszMUD != "" )
   {
      url = (wxT("http://gatherer.wizards.com/Handlers/Image.ashx?multiverseid=" + aszMUD + "&type=card"));
   }
   else
   {
      url = (wxT("http://gatherer.wizards.com/Handlers/Image.ashx?name=" + aszCardName + "&type=card"));
   }

   if( url.GetError() == wxURL_NOERR )
   {
      wxString htmldata;
      wxInputStream *in = url.GetInputStream();

      if( in && in->IsOk() )
      {
         wxFileOutputStream ofStreamFile(aszFilePath);
         in->Read(ofStreamFile);
         ofStreamFile.Close();
      }
      delete in;
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
ImageFetcher::PrepareImagesFolder()
{
   StoreFront* ptSF = StoreFrontEnd::Instance();
   wxString szImagePaths = ptSF->GetImagesPath();

   CreateDirectory(szImagePaths.ToStdWstring().c_str(), NULL);
   return true;
}

bool
ImageFetcher::PrepareImageSetFolder(const wxString& aszSet)
{
   StoreFront* ptSF = StoreFrontEnd::Instance();
   wxString szImagePaths = ptSF->GetImagesPath();
   CreateDirectory(szImagePaths.ToStdWstring().c_str(), NULL);
   szImagePaths += "\\_" + aszSet;
   CreateDirectory(szImagePaths.ToStdWstring().c_str(), NULL);
   return true;
}

ImageFetcher* 
ImageFetcher::Instance()
{
   ms_mutexInstance.lock();
   if( ms_ptInstance == nullptr )
   {
      wxSocketBase::Initialize();
      ms_ptInstance = new ImageFetcher();
   }
   ms_mutexInstance.unlock();

   return ms_ptInstance;
}