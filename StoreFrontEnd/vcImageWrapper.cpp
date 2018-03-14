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
   this->SetBackgroundStyle( wxBG_STYLE_TRANSPARENT );
}

vcImageWrapper::~vcImageWrapper()
{

}

bool 
vcImageWrapper::SetImage(const wxString& aszImagePath)
{
   freeImage();
   m_jpgPanel = new wxImagePanel(this, aszImagePath, wxBitmapType::wxBITMAP_TYPE_ANY);
   if( m_jpgPanel->IsOk )
   {
      this->GetSizer()->Add(m_jpgPanel, wxSizerFlags(1).Expand());
      auto imageSize = m_jpgPanel->GetBestSize();
      this->SetSizeHints( imageSize );
      return true;
   }
   else
   {
      m_jpgPanel->Destroy();
      return false;
   }
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