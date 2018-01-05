#pragma once
#include "wx/wxprec.h"
#include "vicCollectionEditorListItemPlusMinusCounter.h"

class vicCollectionEditorListItem : public wxPanel
{
public:
   enum
   {
      DeleteButton = 0x0
   };

   vicCollectionEditorListItem( wxWindow* aptParent,
                                wxWindowID aiID,
                                const wxString& aszLabel, 
                                const wxString& aszCmd );
   ~vicCollectionEditorListItem();

   wxString GetLabel();
   wxString GetCmd();

private:
   wxDECLARE_EVENT_TABLE();
   vicCollectionEditorListItemPlusMinusCounter* m_vPMCounter;

   wxString m_szLabel;
   wxString m_szCmd;
   
   void buildItem();

   void onCancelItem(wxCommandEvent& awxEvt);
};

