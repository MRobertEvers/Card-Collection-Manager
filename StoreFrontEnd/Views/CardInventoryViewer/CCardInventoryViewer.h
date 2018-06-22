#pragma once
class CardInterface;
class VCardInventoryViewer;
class CCardInventoryViewer
{
public:
   CCardInventoryViewer( VCardInventoryViewer* apView );
   ~CCardInventoryViewer();

   void SetModel( CardInterface* apModel );

private:
   VCardInventoryViewer * m_pView;
   CardInterface* m_pModel;
};