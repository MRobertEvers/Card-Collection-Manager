#pragma once
#include "../ViewTemplates/IControlledView.h"
#include <wx/wxprec.h>

class CardInterface;
class VCardView;
class CCardView;

class CardView : public IControlledView
{
public:
   CardView( wxFrame* aptParent, CardInterface* apModel );
   ~CardView();

   wxPanel* GetView();

private:
   VCardView* m_pView;
   CCardView* m_pController;
};