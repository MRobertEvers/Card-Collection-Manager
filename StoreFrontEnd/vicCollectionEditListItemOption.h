#pragma once
#include <wx/wxprec.h>  
#include <wx/choice.h>
#include "CELIOption.h"

class vicCollectionEditorListItemOption : public wxPanel
{
public:
   vicCollectionEditorListItemOption(wxWindow* aptParent,
                                     CELIOption aOption);
   ~vicCollectionEditorListItemOption();

private:
   CELIOption m_oOption;
   wxChoice* m_wxComboBox;

   void buildOptionSelector();
   void buildOption(wxChoice* aChoice, const CELIOption& aOption);
};

