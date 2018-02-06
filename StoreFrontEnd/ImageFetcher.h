#pragma once
#include <set>
#include <mutex>

class wxString;

class ImageFetcher
{
public:
   ~ImageFetcher();

   void PDownloadImage( const wxString& aszFilePath,
                        const wxString& aszCardName,
                        const wxString& aszSet,
                        const wxString& aszMUD );

   bool DownloadCardImage( const wxString& aszFilePath,
                           const wxString& aszCardName,
                           const wxString& aszSet,
                           const wxString& aszMUD );

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

