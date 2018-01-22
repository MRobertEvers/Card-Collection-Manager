#include "StoreFrontEnd.h"
#include "wx/wxprec.h"
#include <Windows.h>
#include <wx/url.h>
#include <wx/sstream.h>
#include <wx/imagjpeg.h>
#include <wx/wfstream.h>

StoreFront* StoreFrontEnd::m_ptInstance = nullptr;

StoreFrontEnd::StoreFrontEnd()
{

}

StoreFrontEnd::~StoreFrontEnd()
{

}

StoreFront*
StoreFrontEnd::Instance()
{
   if( m_ptInstance == NULL )
   {
      m_ptInstance = new StoreFront();
   }

   return m_ptInstance;
}

bool 
StoreFrontEnd::DownloadCardImage( const wxString& aszFilePath,
                                  const wxString& aszCardName, 
                                  const wxString& aszSet,
                                  const wxString& aszMUD )
{
   PrepareImageSetFolder(aszSet);

   wxURL url;
   if( aszMUD != "" )
   {
      url = (wxT("http://gatherer.wizards.com/Handlers/Image.ashx?multiverseid=" + aszMUD + "&type=card"));
   }
   else
   {
      url = (wxT("http://gatherer.wizards.com/Handlers/Image.ashx?name=" + aszCardName + "&type=card"));
   }

   if( url.GetError() == wxURL_NOERR )
   {
      wxString htmldata;
      wxInputStream *in = url.GetInputStream();

      if( in && in->IsOk() )
      {
         wxFileOutputStream ofStreamFile(aszFilePath);
         in->Read(ofStreamFile);
         ofStreamFile.Close();
      }
      delete in;
   }

   return true;
}

bool 
StoreFrontEnd::PrepareImagesFolder()
{
   StoreFront* ptSF = StoreFrontEnd::Instance();
   wxString szImagePaths = ptSF->GetImagesPath();

   CreateDirectory( szImagePaths.ToStdWstring().c_str(), NULL );
   return true;
}

bool 
StoreFrontEnd::PrepareImageSetFolder(const wxString& aszSet)
{
   StoreFront* ptSF = StoreFrontEnd::Instance();
   wxString szImagePaths = ptSF->GetImagesPath() ;
   CreateDirectory(szImagePaths.ToStdWstring().c_str(), NULL);
   szImagePaths += "\\_" + aszSet;
   CreateDirectory(szImagePaths.ToStdWstring().c_str(), NULL);
   return true;
}