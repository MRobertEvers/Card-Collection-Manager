#include "SourceDownloader.h"
#include "wx/wxprec.h"
#include "curl/curl.h"
#include "StoreFrontEnd.h"

#include "Zip\unzip.h"
#include "iowin32.h"

SourceDownloader::
SourceDownloaderFunctor::SourceDownloaderFunctor()
   : m_ptOfStream(new std::ofstream )
{
   StoreFront* ptSF = StoreFrontEnd::Server();
   m_ptOfStream->open( ptSF->GetImportSourceFilePath() + ".zip" );
}

SourceDownloader::
SourceDownloaderFunctor::~SourceDownloaderFunctor()
{
   m_ptOfStream->close();
}

size_t 
SourceDownloader::
SourceDownloaderFunctor::Append( void *contents, size_t size, size_t nmemb )
{
   m_ptOfStream->write( (char*)contents, size * nmemb );
   return size * nmemb;
}


SourceDownloader::
SourceDecompressorFunctor::SourceDecompressorFunctor()
   : m_ptOfStream( new std::ofstream )
{
   StoreFront* ptSF = StoreFrontEnd::Server();
   m_ptOfStream->open( ptSF->GetImportSourceFilePath() );
}

SourceDownloader::
SourceDecompressorFunctor::~SourceDecompressorFunctor()
{
   m_ptOfStream->close();
}

size_t
SourceDownloader::
SourceDecompressorFunctor::Append( const void* pBuf, int len )
{
   m_ptOfStream->write( (char*)pBuf, len );
   return len;
}

const char* SourceDownloader::API_URL_BASE = "http://mtgjson.com/json/AllSets.json.zip";

SourceDownloader::SourceDownloader()
{

}


SourceDownloader::~SourceDownloader()
{

}

void 
SourceDownloader::FetchMTGJson()
{
   StoreFront* ptSF = StoreFrontEnd::Server();
   auto imageFile = std::ifstream( ptSF->GetImportSourceFilePath() + ".zip" );
   if( imageFile.good() )
   {
      imageFile.close();
      std::remove(ptSF->GetImportSourceFilePath().c_str());
   }
   else
   {
      imageFile.close();
   }

   auto ptFunctor = unique_ptr<SourceDownloaderFunctor>( new SourceDownloaderFunctor() );
   CURLAPI::Easy_HTTP( API_URL_BASE, ptFunctor.get() );
}

#define dir_delimter '/'
#define MAX_FILENAME 1024
#define READ_SIZE 8192

int 
SourceDownloader::UnzipMTGJson()
{
   StoreFront* ptSF = StoreFrontEnd::Server();
   auto relSourcePath = ptSF->GetImportSourceFilePath();

   // Get the zip file full path.
   wxString sourcePath = string( relSourcePath + ".zip" );
   wchar_t fullFilename[MAX_PATH];
   GetFullPathName( sourcePath.c_str(), MAX_PATH, fullFilename, nullptr );
   auto cStr = wxString( fullFilename ).ToStdString();
 
   zlib_filefunc64_def ffunc;
   fill_win32_filefunc64A( &ffunc );
   auto uf = unzOpen2_64( cStr.c_str(), &ffunc );

   unz_global_info64 global_info;
   int err;
   char read_buffer[READ_SIZE];
   err = unzGetGlobalInfo64( uf, &global_info );
   for( int i = 0; i<global_info.number_entry; i++ )
   {
      char filename_inzip[256];
      unz_file_info64 file_info;
      uLong ratio = 0;
      const char *string_method;
      char charCrypt = ' ';


      err = unzGetCurrentFileInfo64( uf, &file_info, filename_inzip, sizeof( filename_inzip ), NULL, 0, NULL, 0 );
      if( err != UNZ_OK )
      {
         printf( "error %d with zipfile in unzGetCurrentFileInfo\n", err );
         break;
      }
      
      const size_t filename_length = strlen( filename_inzip );
      if( filename_inzip[filename_length - 1] == dir_delimter )
      {
         // Removed.
      }
      else
      {
         // Entry is a file, so extract it.
         err = unzOpenCurrentFile( uf );
         if( err != UNZ_OK )
         {
            printf( "could not open file\n" );
            unzClose( uf );
            return -1;
         }

         // Open a file to write out the data.
         auto dler = unique_ptr<SourceDecompressorFunctor>( new SourceDecompressorFunctor() );
         do
         {
            // There is a bug in unzReadCurrentFile where
            // we cant read more than ~255(or around there) bytes at a time
            // not sure why.
            err = unzReadCurrentFile( uf, read_buffer, 255 );
            if( err < 0 )
            {
               printf( "error %d\n", err );
               unzCloseCurrentFile( uf );
               unzClose( uf );
               return -1;
            }

            // Write data to file.
            if( err > 0 )
            {
               dler->Append( read_buffer, err );
            }
         } while( err > 0 );

      }

      unzCloseCurrentFile( uf );

      // Go the the next entry listed in the zip file.
      if( (i + 1) < global_info.number_entry )
      {
         if( unzGoToNextFile( uf ) != UNZ_OK )
         {
            printf( "cound not read next file\n" );
            unzClose( uf );
            return -1;
         }
      }
   }
   unzClose( uf );
   return 0;
}

int 
SourceDownloader::DecompressionCallback( const void* pBuf, int len, void *pUser )
{
   return ((SourceDecompressorFunctor*)pUser)->Append( pBuf, len );
}