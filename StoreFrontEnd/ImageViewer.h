#pragma once
#include "ImageFetcher.h"
#include "vcImageWrapper.h"

#include <wx/wxprec.h>
#include <vector>
#include <memory>
#include <string>
#include <mutex>

class ImageViewer : public wxPanel
{
public:
   class ImageViewerCallback : public ImageFetcherCallback
   {
   public:
      ImageViewerCallback( ImageViewer* aptCE, std::shared_ptr<std::mutex> amutex, const wxString& aszFilePath );

      virtual void CallBack() override;

   private:
      ImageViewer* m_ptViewer;
      std::shared_ptr<std::mutex> m_mutex;
      wxString m_szFilePath;
   };

   enum : int
   {
      Image_Ready = 0x91
   };
public:
   ImageViewer( wxWindow* aptParent, wxWindowID aiWID );
   ~ImageViewer();

   bool DisplayImage( const wxString& aszCardName, 
                      const wxString& aszMultiVerseID,
                      const wxString& aszSet,
                      bool abFetchIfNeeded = true );

   bool DisplayImage( const wxString& aszFilePath );

private:
   wxDECLARE_EVENT_TABLE();

   std::vector<std::shared_ptr<ImageFetcherCallback>> m_vecImageCallbacks;
   std::shared_ptr<std::mutex> m_mutex;
   vcImageWrapper* m_ptImageWrapper;

   void fetchImage( const wxString& aszCardName,
                    const wxString& aszMultiVerseID = "",
                    const wxString& aszSet = "" );
   void stopCallbacks( bool abBlock = true );
   void onImageReady( wxCommandEvent& awxEvt );
};

