#pragma once
#include "../StoreFrontEnd/ImageFetcher.h"
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <wx/wxprec.h>

class ImageViewer;
class CardInterface;

// Provides a fixed min size image panel.
class CardRenderer : public wxPanel
{
public:
   enum Events
   {
      Image_Downloaded = 0x5
   };

   class ImageDownloadCallback : public ImageFetcherCallback
   {
   public:
      ImageDownloadCallback( CardRenderer* apHost,
                             const std::string& aszCardName, const std::string& aszSet,
                             std::shared_ptr<std::recursive_mutex> aCallBackMutex );
      ImageDownloadCallback( CardRenderer* apHost,
                             CardInterface* apCallbackCard,
                             std::shared_ptr<std::recursive_mutex> aCallBackMutex );
      ~ImageDownloadCallback();

      virtual void CallBack() override;

      unsigned int GetRenderCountTarget();

   private:
      unsigned int m_uiRenderCountTarget;
      CardRenderer* m_pCardEditor;

      std::string m_szCardName;
      std::string m_szSet;

      std::shared_ptr<std::recursive_mutex> m_mutex;
   };

public:
   CardRenderer(wxPanel* apParent);
   ~CardRenderer();

   void DisplayImage( CardInterface* apCard );
   void DisplayImage( const wxString& aszCardname, const wxString& aszSet );

private:
   wxDECLARE_EVENT_TABLE();

   // If a callback comesback with an older render count, ignore it.
   unsigned int m_uiRenderCount;
   ImageViewer* m_pImagePanel;
   std::shared_ptr<CardInterface> m_pCardInterface;
   std::shared_ptr<std::recursive_mutex> m_mutex;
   std::shared_ptr<ImageDownloadCallback> m_pCurrentCallback;

   std::string getCardImageFile( const wxString& aszCardname, const wxString& aszSet );

   void onImageCallback( wxCommandEvent& awxEvt );
   void uiDisplayCard( const wxString& aszCardname, const wxString& aszSet, const wxString& aszMud );
};