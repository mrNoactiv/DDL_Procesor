#include "cDirUtil.h"

namespace common {
	namespace utils {

void cDirUtil::Append(char* dir, const char* file, char *output)
{
	strcpy(output, dir);
	strcpy(output + strlen(dir), file);
}


bool cDirUtil::DeleteDirectory(char* sPath) 
{
   HANDLE hFind;    // file handle
   WIN32_FIND_DATA FindFileData;

   char DirPath[MAX_PATH];
   char FileName[MAX_PATH];

   strcpy(DirPath,sPath);
   strcat(DirPath,"\\*");    // searching all files
   strcpy(FileName,sPath);
   strcat(FileName,"\\");

   // find the first file
   hFind = FindFirstFile(DirPath,&FindFileData);
   if(hFind == INVALID_HANDLE_VALUE) return FALSE;
   strcpy(DirPath,FileName);

   bool bSearch = true;
   while(bSearch) {    // until we find an entry
      if(FindNextFile(hFind,&FindFileData)) {
         if(IsDots(FindFileData.cFileName)) continue;
         strcat(FileName,FindFileData.cFileName);
         if((FindFileData.dwFileAttributes &
            FILE_ATTRIBUTE_DIRECTORY)) {

            // we have found a directory, recurse
            if(!DeleteDirectory(FileName)) {
                FindClose(hFind);
                return FALSE;    // directory couldn't be deleted
            }
            // remove the empty directory
            RemoveDirectory(FileName);
             strcpy(FileName,DirPath);
         }
         else {
            //if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
               // change read-only file mode
               //   _chmod(FileName, _S_IWRITE);
                  if(!DeleteFile(FileName)) {    // delete the file
                    FindClose(hFind);
                    return FALSE;
               }
               strcpy(FileName,DirPath);
         }
      }
      else {
         // no more files there
         if(GetLastError() == ERROR_NO_MORE_FILES)
         bSearch = false;
         else {
            // some error occurred; close the handle and return FALSE
               FindClose(hFind);
               return FALSE;
         }

      }

   }
   FindClose(hFind);                  // close the file handle

   return RemoveDirectory(sPath);     // remove the empty directory

}

bool cDirUtil::IsDots(char* str) 
{
   if(strcmp(str,".") && strcmp(str,"..")) 
   {
	   return false;
   }

   return true;
}


bool cDirUtil::Exists(char* filename)
{
return GetFileAttributes(filename) != 0xFFFFFFFF;
}

}}