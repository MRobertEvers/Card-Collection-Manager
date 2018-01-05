#pragma once
#include <wx\wxprec.h>

class vicCollectionEditorListItemPlusMinusCounter : public wxPanel
{
public:
   enum
   {
      UpButton = 0x0,
      DownButton = 0x1
   };

   vicCollectionEditorListItemPlusMinusCounter( wxWindow* aptParent,
                                                int aiStartVal,
                                                int aiMin, int aiMax );
   ~vicCollectionEditorListItemPlusMinusCounter();

private:
   wxDECLARE_EVENT_TABLE();
   void buildPMCounter();
};

