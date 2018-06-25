#pragma once
#include <wx/wxprec.h>

class CardInterface;
class VCardInventoryViewer;
class CCardInventoryViewer
{
public:
   CCardInventoryViewer( VCardInventoryViewer* apView );
   ~CCardInventoryViewer();

   void SetModel( CardInterface* apModel );

   void OnSave( const CardInterface& aszColID, const wxString& aszNewVal );
   void OnReset( CardInterface* apModel );
   void OnOpenEditor( CardInterface* apModel ); //

private:
   VCardInventoryViewer * m_pView;
   CardInterface* m_pModel;
};