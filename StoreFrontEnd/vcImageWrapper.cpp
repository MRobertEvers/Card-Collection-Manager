#include "vcImageWrapper.h"
#include "wxImagePanel.h"

vcImageWrapper::vcImageWrapper(wxWindow* aptParent,
                               wxWindowID aiID,
                               bool abDoScale)
   : wxPanel(aptParent, aiID), m_bDoScale(abDoScale)
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

bool 
vcImageWrapper::SetImage(const wxString& aszImagePath)
{
   freeImage();
   m_jpgPanel = new wxImagePanel(this, aszImagePath, wxBitmapType::wxBITMAP_TYPE_ANY, m_bDoScale);
   if( m_jpgPanel->IsOk )
   {
      this->GetSizer()->Add(m_jpgPanel, wxSizerFlags(1).Expand());
      auto imageSize = m_jpgPanel->GetBestSize();
      this->SetSizeHints( imageSize );
      this->SetSize( imageSize );
      return true;
   }
   else
   {
      freeImage();
      return false;
   }
}

void
vcImageWrapper::freeImage()
{
   if( m_jpgPanel != nullptr )
   {
      this->GetSizer()->Detach(m_jpgPanel);
      m_jpgPanel->Destroy();
      m_jpgPanel = nullptr;
   }
}