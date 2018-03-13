#pragma once
#include <wx/wxprec.h>

class CEOption
{
public:
   CEOption(const wxString& aszFilePath);
   ~CEOption();

   wxString m_szIcon;
   wxString m_szName;
};

