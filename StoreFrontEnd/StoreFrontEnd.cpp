#include "StoreFrontEnd.h"
#include "ImageFetcher.h"

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
   ImageFetcher::Instance()->PDownloadImage(aszFilePath, aszCardName, aszSet, aszMUD);

   return true;
}