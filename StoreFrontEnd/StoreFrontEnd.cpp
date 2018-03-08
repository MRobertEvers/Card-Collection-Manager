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
                                  std::shared_ptr<ImageFetcherCallback> aptCallback )
{
   ImageFetcher::Instance()->PDownloadImage(aszFilePath, aszCardName, aszSet, aszMUD, aptCallback);

   return true;
}

bool 
StoreFrontEnd::DownloadImportSourceFile()
{
   SourceDownloader SD;
   SD.FetchMTGJson();
   SD.UnzipMTGJson();
   return true;
}

void 
StoreFrontEnd::EstablishFolderStructure()
{
   // /Config/Source folder
   auto szSourceFolder = Server()->GetSourceFilePath();
   EstablishFolder( szSourceFolder );

   // /Config/Images folder
   auto szImagesFolder = Server()->GetImagesDirectory();
   EstablishFolder( szImagesFolder );

   // Collections/<Data>
   auto szDataFolder = Server()->GetCollectionsHistoryDirectory();
   EstablishFolder( szDataFolder );

   szDataFolder = Server()->GetCollectionsMetaDirectory();
   EstablishFolder( szDataFolder );

   szDataFolder = Server()->GetCollectionsOverheadDirectory();
   EstablishFolder( szDataFolder );
}

void 
StoreFrontEnd::EstablishFolder( const wxString& aszRelOrAbsPath )
{
   // Make it readable by windows api.
   auto szPath = aszRelOrAbsPath;
   szPath.Replace( "\\", "\\\\" );

   // Get the full filename
   wchar_t fullFilename[MAX_PATH];
   GetFullPathName( szPath.c_str(), MAX_PATH, fullFilename, nullptr );
   szPath = wxString( fullFilename );

   SHCreateDirectoryEx( NULL, szPath.ToStdWstring().c_str(), NULL );
}