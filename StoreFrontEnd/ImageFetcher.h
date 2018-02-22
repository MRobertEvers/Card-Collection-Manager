#pragma once
#include <set>
#include <mutex>

class wxString;

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

   bool PrepareImagesFolder();
   bool PrepareImageSetFolder(const wxString& aszSet);

private:
   ImageFetcher();
   std::mutex m_mutexThreadList;
   std::set<wxString> m_setThreadList;

public:
   static std::mutex ms_mutexInstance;
   static ImageFetcher* Instance();

private:
   static ImageFetcher* ms_ptInstance;
};

