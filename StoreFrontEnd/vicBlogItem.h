#pragma once
#include <wx/wxprec.h>
#include <vector>
#include <string>

class vicBlogItem : public wxPanel
{
public:
   vicBlogItem( wxWindow* aptParent,
                const wxString& aszTitle,
                const std::vector<std::string>& avecHistory);
   ~vicBlogItem();

private:
   struct InterfaceLineParser
   {
   public:
      unsigned int Count;
      std::string Name;
      std::vector<std::pair<std::string,std::string>> DetailVec;
      std::vector<std::pair<std::string, std::string>> MetaVec;
   };
};

