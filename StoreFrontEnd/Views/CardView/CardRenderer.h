#pragma once
#include "../StoreFrontEnd/ImageFetcher.h"
#include <string>
#include <wx/wxprec.h>

class ImageViewer;
class CardInterface;

// Provides a fixed min size image panel.
class CardRenderer : public wxPanel
{
public:
   class ImageDownloadCallback : public ImageFetcherCallback
   {
   public:
      ImageDownloadCallback( CardRenderer* apHost,
                             std::shared_ptr<std::mutex> aCallBackMutex );
      ~ImageDownloadCallback();

      virtual void CallBack() override;

      unsigned int GetRenderCountTarget();

   private:
      unsigned int m_uiRenderCountTarget;
      CardRenderer* m_pCardEditor;
      std::shared_ptr<std::mutex> m_mutex;
   };

public:
   CardRenderer(wxPanel* apParent);
   ~CardRenderer();

   void DisplayImage( CardInterface* apCard );

private:
   // If a callback comesback with an older render count, ignore it.
   unsigned int m_uiRenderCount;
   ImageViewer* m_pImagePanel;
   CardInterface* m_pCardInterface;

   std::string getCardImageFile();

   void uiDisplayCard();
};