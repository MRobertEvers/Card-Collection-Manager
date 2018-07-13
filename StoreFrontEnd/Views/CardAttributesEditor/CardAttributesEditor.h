#pragma once
#include "../ViewTemplates/IViewFactory.h"
#include <wx/wxprec.h>

class CCardAttributesEditor;
class VCardAttributesEditor;
class CardAttributesEditor :public IViewFactory
{
public:
   CardAttributesEditor(int aiMaxSelection);
   ~CardAttributesEditor();

   wxWindow* GetView( wxWindow* aptParent );
   CCardAttributesEditor* GetController();

private:
   int m_iMaxSelection;
   VCardAttributesEditor * m_pView;
   CCardAttributesEditor* m_pController;
};