#pragma once
#include "../ViewTemplates/IViewFactory.h"
#include <wx/wxprec.h>

class CCardEditor;
class VCardEditor;
class CardEditor :public IViewFactory
{
public:
   CardEditor( int aiMaxEdit = -1 );
   ~CardEditor();

   wxWindow* GetView( wxWindow* aptParent = nullptr );
   CCardEditor* GetController();

private:
   VCardEditor * m_pView;
   CCardEditor* m_pController;

   int m_iMaxSelectionTemp;
};