#pragma once
#include <wx/wxprec.h>

class CardRenderer;
class CCardView;
class CardInterface;

class VCardView : public wxPanel
{
public:
   VCardView( wxWindow* aptParent, wxWindowID aiWID );
   ~VCardView();

   void SetController( CCardView* apController );

   void ViewCard( CardInterface* apInterface );

private:
   CCardView* m_pController;
   CardRenderer* m_pRenderer;
};