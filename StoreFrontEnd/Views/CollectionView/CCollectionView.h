#pragma once
#include "..\StoreFrontEnd\CollectionInterface.h"
#include "../Views/ViewTemplates/IMenuEventHandler.h"
#include <memory>

class CardView;
class IMenuEventSource;
class VCollectionView;

class CCollectionView : public IMenuEventHandler
{
public:
   enum Events
   {
      Menu_Save = 0x3,
      Menu_Edit,
      Menu_Stats,
      Menu_History,
      Menu_XMage
   };

   CCollectionView( VCollectionView* aptView, IMenuEventSource* apSource, std::shared_ptr<CollectionInterface> aptModel);
   ~CCollectionView();

   void BindEventHandler();
   void SetCubeRenderer();
   void ViewItem( CardInterface* apItem );
   void OnCollectionEdited();

private:
   
   void handleEvent( unsigned int aiEvent );
   void uiShowCardViewer();

   std::shared_ptr<CardView> m_ptCardViewer;

   VCollectionView* m_ptView;
   std::shared_ptr<CollectionInterface> m_ptModel;
};