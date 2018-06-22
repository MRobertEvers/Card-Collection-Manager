#pragma once
#include "wx/wxprec.h"
class CardInterface;
class CCardInventoryViewer;

class VCardInventoryViewer : public wxPanel
{
public:
   VCardInventoryViewer( wxWindow* aptParent, wxWindowID aiWID );
   ~VCardInventoryViewer();

   void SetController( CCardInventoryViewer* apController );

   void ViewCard( CardInterface* apInterface );
};