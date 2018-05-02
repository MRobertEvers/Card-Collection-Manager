#pragma once
#include <wx/wxprec.h>

class CCardView;

class VCardView : public wxPanel
{
public:
   VCardView( wxWindow* aptParent, wxWindowID aiWID );
   ~VCardView();

   void SetController( CCardView* apController );

private:
   CCardView* m_pController;
};