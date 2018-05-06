#pragma once
#include "..\StoreFrontEnd\CollectionInterface.h"
#include "../Views/ViewTemplates/IMenuEventHandler.h"
#include <memory>

class CardView;
class IMenuEventSource;
class VCollectionView;

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

private:
   
   void uiShowCardViewer();

   std::shared_ptr<CardView> m_ptCardViewer;

   VCollectionView* m_ptView;
   std::shared_ptr<CollectionInterface> m_ptModel;
};