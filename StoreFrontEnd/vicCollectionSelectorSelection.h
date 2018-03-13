#pragma once
#include "wx/wxprec.h"
#include "ImageViewer.h"

#include <vector>

class ImageViewer;

class vicCollectionSelectorSelection : public wxPanel
{
public:
   vicCollectionSelectorSelection( wxWindow* aptParent,
                                   const wxString& aszName,
                                   const wxString& aszIconName,
                                   const wxString& aszSet,
                                   const wxString& aszMud );
   ~vicCollectionSelectorSelection();

private:
   ImageViewer * m_ptImage;
   std::shared_ptr<std::mutex> m_mutex;
};