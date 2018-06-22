#pragma once
#include "wx/wxprec.h"
class CardInterface;
class wxRichTextCtrl;
class CCardInventoryViewer;

class VCardInventoryViewer : public wxPanel
{
public:
   VCardInventoryViewer( wxWindow* aptParent, wxWindowID aiWID );
   ~VCardInventoryViewer();

   void SetController( CCardInventoryViewer* apController );

   void ViewCard( CardInterface* apInterface );

private:
   wxRichTextCtrl * m_pTitle;
   CCardInventoryViewer* m_pController;
};