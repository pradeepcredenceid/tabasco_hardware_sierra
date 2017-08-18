/*
*       Copyright (C) 2011-2013 Intel Mobile Communications GmbH
*
*            Sec Class: Intel Confidential (IC)
*
*
* This document contains proprietary information belonging to IMC.
* Design A/S. Passing on and copying of this document, use and communication
* of its contents is not permitted without prior written authorisation.
*
* Description: Interface for: ANSI C String List Class
*
*
* Revision Information:
*  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/single_file_handler/SList.h $
*  $CC-Version: .../oint_tools_flashtoole2_01/5 $
*  $Date:       2013-10-02    15:01:06 UTC $
*   Resonsible:
*   Comment:
*
*/

/*---------------------------------------------------------------------------*/
#ifndef SListH
#define SListH
/*---------------------------------------------------------------------------*/
typedef struct
{
  char *text;
  void *object;
} SListItem;

typedef struct SListTag
{
/* public: */
  int Count;
  void (*OnDelete)(void *object);  /* External callback to handle object deletion (in case memory needs deallocation) */

/* private: */
  int NofAllocated;
  SListItem *Items;
  char IsSorted;
} SList;

/*---------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

extern SList *SList_new(void);
extern  void  SList_delete(SList *p_this);

extern  int   SList_Add(char *text, SList *p_this);
extern  int   SList_AddObject(char *text, void *object, SList *p_this);
extern  void  SList_Delete(int index, SList *p_this);
extern  int   SList_IndexOf(char *text, SList *p_this);

extern  char *SList_String(int index, SList *p_this);
extern  void *SList_Object(int index, SList *p_this);

extern  void  SList_Clear(SList *p_this);
extern  void  SList_Sort(SList *p_this);

#ifdef __cplusplus
}
#endif

/*---------------------------------------------------------------------------*/

#endif


