#pragma once
#include "CURLAPI.h"

#include <wx/wxprec.h>
#include <wx/thread.h>
#include <set>
#include <mutex>

class ImageFetcherCallback
{
public:
   ImageFetcherCallback();
   virtual ~ImageFetcherCallback();

   virtual void CallBack() = 0;

   void SetDoCall( bool abDoCall );
   bool GetDoCall();
private:
   bool m_bDoCall;
};


class FetcherThread : public wxThread
{
public:
   FetcherThread( const wxString& aszFilePath,
                  const wxString& aszCardName,
                  const wxString& aszSet,
                  const wxString& aszMUD,
                  std::shared_ptr<ImageFetcherCallback> aptCallBack = nullptr ) 
      : wxThread(wxTHREAD_DETACHED), m_szFilePath(aszFilePath),
        m_szCardName(aszCardName), m_szSet(aszSet), m_szMUD(aszMUD),
        m_ptCallback(aptCallBack)
   {};
   ~FetcherThread() {};
protected:
   virtual ExitCode Entry();

private:
   wxString m_szFilePath;
   wxString m_szCardName;
   wxString m_szSet;
   wxString m_szMUD;
   std::shared_ptr<ImageFetcherCallback> m_ptCallback;
};

class ImageFetcher
{
public:
   ~ImageFetcher();

   void PDownloadImage( const wxString& aszFilePath,
                        const wxString& aszCardName,
                        const wxString& aszSet,
                        const wxString& aszMUD,
                        std::shared_ptr<ImageFetcherCallback> aptCallback );

   bool DownloadCardImage( const wxString& aszFilePath,
                           const wxString& aszCardName,
                           const wxString& aszSet,
                           const wxString& aszMUD );

   bool DownloadCardImage( const wxString& aszFilePath,
                           const wxString& aszCardName,
                           const wxString& aszSet,
                           const wxString& aszMUD,
                           std::shared_ptr<ImageFetcherCallback> aptCallback );

   bool PrepareImageSetFolder(const wxString& aszSet);

private:
   ImageFetcher();
   bool downloadImage( const wxString& aszFilePath,
                       const wxString& aszCardName,
                       const wxString& aszSet,
                       const wxString& aszMUD );
   bool tryDownload( const wxString& aszFilePath,
                     const wxString& aszCardName,
                     const wxString& aszSet,
                     const wxString& aszMUD );
   std::recursive_mutex m_mutexThreadList;
   std::set<wxString> m_setThreadList;

public:
   static std::recursive_mutex ms_mutexInstance;
   static ImageFetcher* Instance();

private:
   static ImageFetcher* ms_ptInstance;
};

