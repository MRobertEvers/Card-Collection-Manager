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
   
   int GetCount();
private:
   wxDECLARE_EVENT_TABLE();
   void buildPMCounter();

   wxTextCtrl* m_vTextCtrl;
   int m_iMin;
   int m_iMax;
   int m_iVal;

   void onUpButton(wxCommandEvent& awxEvt);
   void onDownButton(wxCommandEvent& awxEvt);
   void updateText();
};

