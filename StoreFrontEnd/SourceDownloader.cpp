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
   m_ptOfStream->open( ptSF->GetImportSourceFilePath() + ".zip", ofstream::binary );
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
   m_ptOfStream->open( ptSF->GetImportSourceFilePath(), ofstream::binary );
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
   auto szRelSourceFilePath = ptSF->GetImportSourceFilePath();

   // Get the zip file full path.
   wxString szRelZipFilePath = string( szRelSourceFilePath + ".zip" );
   wchar_t fullFilename[MAX_PATH];
   GetFullPathName( szRelZipFilePath.c_str(), MAX_PATH, fullFilename, nullptr );
   auto szFullZipFilePath = wxString( fullFilename ).ToStdString();
 
   zlib_filefunc64_def ffunc;
   fill_win32_filefunc64A( &ffunc );
   auto zipFile = unzOpen2_64( szFullZipFilePath.c_str(), &ffunc );

   unz_global_info64 global_info;
   char read_buffer[READ_SIZE];
   int iError = unzGetGlobalInfo64( zipFile, &global_info );
   for( int i = 0; i<global_info.number_entry; i++ )
   {
      char szFilenameInZip[256];
      unz_file_info64 file_info;

      iError = unzGetCurrentFileInfo64( zipFile, &file_info, szFilenameInZip, 
                                       sizeof( szFilenameInZip ), NULL, 0, NULL, 0 );
      if( iError != UNZ_OK )
      {
         // TODO: Error.
      }
      
      const size_t filename_length = strlen( szFilenameInZip );
      if( szFilenameInZip[filename_length - 1] != dir_delimter )
      {
         // Entry is a file, so extract it. If this case isn't true,
         // then it would be a folder.

         iError = unzOpenCurrentFile( zipFile );
         if( iError != UNZ_OK )
         {
            // TODO: Error.
            unzClose( zipFile );
            return -1;
         }

         // Open a file to write out the data.
         auto fileWriter = unique_ptr<SourceDecompressorFunctor>( new SourceDecompressorFunctor() );

         // >0 means readsize, <0 error, =0 done.
         int iReadSizeOrErr = 0;
         do
         {
            iReadSizeOrErr = unzReadCurrentFile( zipFile, read_buffer, READ_SIZE );
            if( iReadSizeOrErr < 0 )
            {
               unzCloseCurrentFile( zipFile );
               unzClose( zipFile );
               return -1;
            }

            // Write data to file.
            if( iReadSizeOrErr > 0 )
            {
               fileWriter->Append( read_buffer, iReadSizeOrErr );
            }
         } while( iReadSizeOrErr > 0 );

      }

      unzCloseCurrentFile( zipFile );

      // Go the the next entry listed in the zip file.
      if( (i + 1) < global_info.number_entry )
      {
         if( unzGoToNextFile( zipFile ) != UNZ_OK )
         {
            unzClose( zipFile );
            return -1;
         }
      }
   }
   unzClose( zipFile );
   return 0;
}
