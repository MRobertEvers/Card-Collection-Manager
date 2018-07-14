#pragma once
#include "../ViewTemplates/IControlledView.h"
#include "../ViewTemplates/IViewFactory.h"
#include <wx/wxprec.h>

class CardInterface;
class VCardView;
class CCardView;

class CardView : public IViewFactory
{
public:
   CardView();
   ~CardView();

   wxWindow* GetView( wxWindow* apParent );
   CCardView* GetController();

private:
   VCardView* m_pView;
   CCardView* m_pController;
};