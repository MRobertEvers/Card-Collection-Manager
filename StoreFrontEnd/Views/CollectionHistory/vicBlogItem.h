#pragma once
#include <wx/wxprec.h>
#include <wx/grid.h>
#include <vector>
#include <string>

class wxInfiniteGrid;

class myImageGridCellRenderer : public wxGridCellStringRenderer
{
public:
   myImageGridCellRenderer( const wxString& aszImagePath );

   virtual void Draw( wxGrid& grid, wxGridCellAttr& attr, wxDC& dc,
                      const wxRect& rect, int row, int col, bool isSelected );

   virtual wxSize GetBestSize( wxGrid& grid,
                               wxGridCellAttr& attr,
                               wxDC& dc,
                               int row, int col );

private:
   wxString m_szPath;
   wxBitmap m_bitMap;
};

class vicBlogItem : public wxPanel
{
public:
   vicBlogItem( wxWindow* aptParent,
                const wxString& aszTitle,
                const wxString& aszFooter,
                const std::vector<std::string>& avecHistory);
   ~vicBlogItem();

private:
   struct InterfaceLineParser
   {
   public:
      unsigned int Count;
      std::string ParentGroupName;
      std::vector<std::pair<std::string, std::string>> DetailVec;
      std::vector<std::pair<std::string, std::string>> MetaVec;
   };

private:
   DECLARE_EVENT_TABLE()

   wxInfiniteGrid* m_ptGrid;

   void onScroll( wxScrollWinEvent& awxEvt );
   void onResize( wxSizeEvent &ev );
};

