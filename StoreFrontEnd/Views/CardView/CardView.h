#pragma once
#include "../ViewTemplates/IControlledView.h"
#include <wx/wxprec.h>

class CardInterface;
class VCardView;
class CCardView;

class CardView : public IControlledView
{
public:
   CardView( wxPanel* aptParent );
   ~CardView();

   wxPanel* GetView();
   CCardView* GetController();

private:
   VCardView* m_pView;
   CCardView* m_pController;
};