#pragma once
#include "wx/wxprec.h"
#include "vicCollectionEditorListItemPlusMinusCounter.h"
#include "vicCollectionEditListItemOption.h"
#include "CELIOption.h"

class vicCollectionEditorListItem : public wxPanel
{
public:
   enum
   {
      DeleteButton = 0x0
   };

   vicCollectionEditorListItem( wxWindow* aptParent,
                                wxWindowID aiID,
                                CELIOption aOption,
                                CELIOption aOptionTwo );
   ~vicCollectionEditorListItem();

   wxString GetLabel();
   wxString GetCmd();

private:
   wxDECLARE_EVENT_TABLE();
   vicCollectionEditorListItemOption* m_vOption;
   vicCollectionEditorListItemOption* m_vOptionTwo;
   vicCollectionEditorListItemPlusMinusCounter* m_vPMCounter;

   CELIOption m_oOption;
   CELIOption m_oOptionTwo;

   // Used to remember if the first option was empty while the second wasnt.
   bool m_bOptionSwitched;

   void buildItem();

   void onCancelItem(wxCommandEvent& awxEvt);
};

