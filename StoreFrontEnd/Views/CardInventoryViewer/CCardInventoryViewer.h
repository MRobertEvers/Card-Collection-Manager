#pragma once
class CardInterface;
class VCardInventoryViewer;
class CCardInventoryViewer
{
public:
   CCardInventoryViewer( VCardInventoryViewer* apView );
   ~CCardInventoryViewer();

   void SetModel( CardInterface* apModel );

   void OnSave( CardInterface* apModel, const wxString& aszNewVal );
   void OnReset( CardInterface* apModel );
   void OnOpenEditor( CardInterface* apModel ); //

private:
   VCardInventoryViewer * m_pView;
   CardInterface* m_pModel;
};