#pragma once
#include "..\StoreFrontEnd\CollectionInterface.h"
#include <memory>

class CardView;
class VCollectionView;

class CCollectionView
{
public:
   CCollectionView( VCollectionView* aptView, std::shared_ptr<CollectionInterface> aptModel);
   ~CCollectionView();

   void SetCubeRenderer();

private:
   void uiShowCardViewer();

   std::shared_ptr<CardView> m_ptCardViewer;

   VCollectionView* m_ptView;
   std::shared_ptr<CollectionInterface> m_ptModel;
};