#include "vcImageWrapper.h"
#include "wxImagePanel.h"

vcImageWrapper::vcImageWrapper(wxWindow* aptParent,
                               wxWindowID aiID)
   : wxPanel(aptParent, aiID)
{
   wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
   this->SetSizer(boxSizer);
   this->SetSize(wxSize(223, 311));
   this->SetSizeHints(this->GetSize());
   this->SetMinSize(this->GetSize());
}

vcImageWrapper::~vcImageWrapper()
{

}

void 
vcImageWrapper::SetImage(const wxString& aszImagePath)
{
   freeImage();
   m_jpgPanel = new wxImagePanel(this, aszImagePath, wxBitmapType::wxBITMAP_TYPE_ANY);
   this->GetSizer()->Add(m_jpgPanel, wxSizerFlags(1).Expand());
}

void
vcImageWrapper::freeImage()
{
   if( m_jpgPanel )
   {
      this->GetSizer()->Detach(m_jpgPanel);
      m_jpgPanel->Destroy();
   }
}