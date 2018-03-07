#include "StoreFrontEnd.h"
#include "ImageFetcher.h"
#include "wx\wxprec.h"
#include "SourceDownloader.h"

#include <Windows.h>
#include <Shlobj.h>

StoreFront* StoreFrontEnd::m_ptInstance = nullptr;
StoreFrontEnd* StoreFrontEnd::m_ptClient = nullptr;

StoreFrontEnd::StoreFrontEnd()
{
   EstablishFolderStructure();
}

StoreFrontEnd::~StoreFrontEnd()
{

}

StoreFront*
StoreFrontEnd::Server()
{
   if( m_ptInstance == NULL )
   {
      m_ptInstance = new StoreFront();
      Client();
   }

   return m_ptInstance;
}

StoreFrontEnd*
StoreFrontEnd::Client()
{
   if( m_ptClient == NULL )
   {
      m_ptClient = new StoreFrontEnd();
      Server();
   }

   return m_ptClient;
}

bool
StoreFrontEnd::DownloadCardImage( const wxString& aszFilePath,
                                  const wxString& aszCardName,
                                  const wxString& aszSet,
                                  const wxString& aszMUD,
                                  std::shared_ptr<ImageFetcherCallback> aptCallback)
{
   ImageFetcher::Instance()->PDownloadImage(aszFilePath, aszCardName, aszSet, aszMUD, aptCallback);

   return true;
}

bool 
StoreFrontEnd::DownloadImportSourceFile()
{
   SourceDownloader SD;
   SD.FetchMTGJson();
}

void 
StoreFrontEnd::EstablishFolderStructure()
{
   // /Config/Source folder
   auto szSourceFolder = Server()->GetSourceFilePath();
   establishFolder( szSourceFolder );

   // /Config/Images folder
   auto szImagesFolder = Server()->GetImagesDirectory();
   establishFolder( szImagesFolder );

   // Collections/<Data>
   auto szDataFolder = Server()->GetCollectionsHistoryDirectory();
   establishFolder( szDataFolder );

   szDataFolder = Server()->GetCollectionsMetaDirectory();
   establishFolder( szDataFolder );

   szDataFolder = Server()->GetCollectionsOverheadDirectory();
   establishFolder( szDataFolder );
}

void 
StoreFrontEnd::establishFolder( const wxString& aszPath )
{
   SHCreateDirectoryEx( NULL, aszPath.ToStdWstring().c_str(), NULL );
}