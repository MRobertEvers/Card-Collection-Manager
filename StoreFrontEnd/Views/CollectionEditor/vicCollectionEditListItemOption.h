#pragma once
#include "CELIOption.h"
#include <wx/wxprec.h>  
#include <wx/choice.h>

class vicCollectionEditorListItemOption : public wxPanel
{
public:
   vicCollectionEditorListItemOption(wxWindow* aptParent,
                                     CELIOption aOption);
   ~vicCollectionEditorListItemOption();

   wxString GetSelection();

private:
   CELIOption m_oOption;
   wxChoice* m_wxComboBox;

   void buildOptionSelector();
   void buildOption(wxChoice** aChoice, const CELIOption& aOption);
};

