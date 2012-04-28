#ifndef _UNRAR_DLL_
#define _UNRAR_DLL_

#pragma pack(1)

#define ERAR_END_ARCHIVE        10
#define ERAR_NO_MEMORY          11
#define ERAR_BAD_DATA           12
#define ERAR_BAD_ARCHIVE        13
#define ERAR_UNKNOWN_FORMAT     14
#define ERAR_EOPEN              15
#define ERAR_ECREATE            16
#define ERAR_ECLOSE             17
#define ERAR_EREAD              18
#define ERAR_EWRITE             19
#define ERAR_SMALL_BUF          20
#define ERAR_UNKNOWN            21
#define ERAR_MISSING_PASSWORD   22

#define RAR_OM_LIST              0
#define RAR_OM_EXTRACT           1
#define RAR_OM_LIST_INCSPLIT     2

#define RAR_SKIP              0
#define RAR_TEST              1
#define RAR_EXTRACT           2

#define RAR_VOL_ASK           0
#define RAR_VOL_NOTIFY        1

#define RAR_DLL_VERSION       5

#include <qnamespace.h>

#ifdef __unix__
#define CALLBACK
#define PASCAL
#define LPARAM long
#endif

#ifdef __WIN32__
#include "windef.h"
#else
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif
#endif


struct RARHeaderData
{
  char         ArcName[260];
  char         FileName[260];
  unsigned int Flags;
  unsigned int PackSize;
  unsigned int UnpSize;
  unsigned int HostOS;
  unsigned int FileCRC;
  unsigned int FileTime;
  unsigned int UnpVer;
  unsigned int Method;
  unsigned int FileAttr;
  char         *CmtBuf;
  unsigned int CmtBufSize;
  unsigned int CmtSize;
  unsigned int CmtState;
};


struct RARHeaderDataEx
{
  char         ArcName[1024];
  wchar_t      ArcNameW[1024];
  char         FileName[1024];
  wchar_t      FileNameW[1024];
  unsigned int Flags;
  unsigned int PackSize;
  unsigned int PackSizeHigh;
  unsigned int UnpSize;
  unsigned int UnpSizeHigh;
  unsigned int HostOS;
  unsigned int FileCRC;
  unsigned int FileTime;
  unsigned int UnpVer;
  unsigned int Method;
  unsigned int FileAttr;
  char         *CmtBuf;
  unsigned int CmtBufSize;
  unsigned int CmtSize;
  unsigned int CmtState;
  unsigned int Reserved[1024];
};


struct RAROpenArchiveData
{
  char         *ArcName;
  unsigned int OpenMode;
  unsigned int OpenResult;
  char         *CmtBuf;
  unsigned int CmtBufSize;
  unsigned int CmtSize;
  unsigned int CmtState;
};

typedef int (CALLBACK *UNRARCALLBACK)(unsigned int msg, LPARAM UserData, LPARAM P1, LPARAM P2);

struct RAROpenArchiveDataEx
{
  char         *ArcName;
  wchar_t      *ArcNameW;
  unsigned int  OpenMode;
  unsigned int  OpenResult;
  char         *CmtBuf;
  unsigned int  CmtBufSize;
  unsigned int  CmtSize;
  unsigned int  CmtState;
  unsigned int  Flags;
  UNRARCALLBACK Callback;
  LPARAM        UserData;
  unsigned int  Reserved[28];
};

enum UNRARCALLBACK_MESSAGES {
  UCM_CHANGEVOLUME,UCM_PROCESSDATA,UCM_NEEDPASSWORD
};

typedef int (PASCAL *CHANGEVOLPROC)(char *ArcName,int Mode);
typedef int (PASCAL *PROCESSDATAPROC)(unsigned char *Addr,int Size);

#ifdef __cplusplus
extern "C" {
#endif

typedef Qt::HANDLE (PASCAL *RAROpenArchiveT)(struct RAROpenArchiveData *ArchiveData);
typedef Qt::HANDLE (PASCAL *RAROpenArchiveExT)(struct RAROpenArchiveDataEx *ArchiveData);
typedef int    (PASCAL *RARCloseArchiveT)(Qt::HANDLE hArcData);
typedef int    (PASCAL *RARReadHeaderT)(Qt::HANDLE hArcData,struct RARHeaderData *HeaderData);
typedef int    (PASCAL *RARReadHeaderExT)(Qt::HANDLE hArcData,struct RARHeaderDataEx *HeaderData);
typedef int    (PASCAL *RARProcessFileT)(Qt::HANDLE hArcData,int Operation,char *DestPath,char *DestName);
typedef int    (PASCAL *RARProcessFileWT)(Qt::HANDLE hArcData,int Operation,wchar_t *DestPath,wchar_t *DestName);
typedef void   (PASCAL *RARSetCallbackT)(Qt::HANDLE hArcData,UNRARCALLBACK Callback,long UserData);
typedef void   (PASCAL *RARSetChangeVolProcT)(Qt::HANDLE hArcData,CHANGEVOLPROC ChangeVolProc);
typedef void   (PASCAL *RARSetProcessDataProcT)(Qt::HANDLE hArcData,PROCESSDATAPROC ProcessDataProc);
typedef void   (PASCAL *RARSetPasswordT)(Qt::HANDLE hArcData,char *Password);
typedef int    (PASCAL *RARGetDllVersionT)();

extern RAROpenArchiveT        RAROpenArchive;
extern RAROpenArchiveExT      RAROpenArchiveEx;
extern RARCloseArchiveT       RARCloseArchive;
extern RARReadHeaderT         RARReadHeader;
extern RARReadHeaderExT       RARReadHeaderEx;
extern RARProcessFileT        RARProcessFile;
extern RARProcessFileWT       RARProcessFileW;
extern RARSetCallbackT        RARSetCallback;
extern RARSetChangeVolProcT   RARSetChangeVolProc;
extern RARSetProcessDataProcT RARSetProcessDataProc;
extern RARSetPasswordT        RARSetPassword;
extern RARGetDllVersionT      RARGetDllVersion;



#ifdef __cplusplus
}
#endif

#pragma pack()

#endif
