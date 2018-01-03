#pragma once
#include "wx/wxprec.h"

class vicCollectionEditorListItem : public wxPanel
{
public:
   vicCollectionEditorListItem( wxWindow* aptParent,
                                const wxString& aszLabel, 
                                const wxString& aszCmd );
   ~vicCollectionEditorListItem();

   wxString GetLabel();
   wxString GetCmd();

private:
   void buildItem();

   wxString m_szLabel;
   wxString m_szCmd;
};

