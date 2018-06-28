#pragma once
#include "..\StoreFrontEnd\CollectionInterface.h"
#include "../Views/ViewTemplates/IMenuEventHandler.h"
#include <memory>

class CardView;
class CardInventoryViewer;
class IMenuEventSource;
class VCollectionView;
class CollectionDelta;

class CCollectionView
{
public:
   CCollectionView( VCollectionView* aptView, std::shared_ptr<CollectionInterface> aptModel);
   ~CCollectionView();

   void SetCubeRenderer();
   void ViewItem( CardInterface* apItem );

   void OnDoEdit();
   void OnViewStats();
   void OnViewHistory();
   void OnExportXMage();
   void OnSave();

   void OnCollectionEdited();
   void OnCollectionEdited( std::shared_ptr<CollectionDelta> apDelta );

private:
   
   void uiShowCardViewer();
   void uiShowNewestCard( CardInterface* apItem );

   std::shared_ptr<CardView> m_ptCardViewer;
   std::shared_ptr<CardInventoryViewer> m_ptInventoryEditor;

   VCollectionView* m_ptView;
   std::shared_ptr<CollectionInterface> m_ptModel;
};