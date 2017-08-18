 /*
*       Copyright (C) 2011-2014 Intel Mobile Communications GmbH
*       
*            Sec Class: Intel Confidential (IC)
*       
*
* This document contains proprietary information belonging to IMC.
* Design A/S. Passing on and copying of this document, use and communication
* of its contents is not permitted without prior written authorisation.
*
* Description: ANSI C String List Class
*
*
* Revision Information:
*  $File name:  /msw_tools/FlashTool/libDownloadTool/src/Portable_code/single_file_handler/SList.c $
*  $CC-Version: .../oint_tools_flashtoole2_01/7 $
*  $Date:       2014-03-13    15:40:31 UTC $
*   Resonsible:
*   Comment:
*
*/

/*---------------------------------------------------------------------------*/
/* ANSI C String List "Class"                                                */
/*---------------------------------------------------------------------------*/

#include "SList.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*---------------------------------------------------------------------------*/
#include <IFWD_mem.h>

#define CLIST_CHUNK_SIZE 20

/*---------------------------------------------------------------------------*/
SList *SList_new(void)
{
  SList *p_this = (SList *)MEM_malloc(sizeof(SList));
  if(p_this)
  {
    p_this->Count = 0;
    p_this->NofAllocated = 0;
    p_this->IsSorted = 0;
    p_this->OnDelete = NULL;
  }
  return p_this;
}
/*---------------------------------------------------------------------------*/
void SList_delete(SList *p_this)
{
  if(p_this)
  {
    SList_Clear(p_this);
    MEM_free((uint8_t*)p_this);
  }
}
/*---------------------------------------------------------------------------*/

#if defined(__BORLANDC__) && !defined(LINT)
static int _USERENTRY SListItem_compare( const void *a, const void *b)
#else
static int SListItem_compare( const void *a, const void *b)
#endif
{
  char *p1 = ((SListItem*)a)->text, *p2 = ((SListItem*)b)->text;
  if(p1 && p2)
    return strcmp(p1,p2);
  if(p1 == p2)
    return 0;
  /* one of the pointers are now NULL, but only one of them */
  if(p1)
    return 1; // p1 - p2: p1 greatest => 1
  return -1; /* it was p2 that was NULL. p1 - p2: p2 greatest => -1 */
}

/*---------------------------------------------------------------------------*/

int SList_AddObject(char *text, void *object, SList *p_this)
{
  char *p = NULL;
  if (!p_this)
	  return 0;

  if(p_this->Count + 1 >= p_this->NofAllocated) /* not enough room one more? */
  {
    int nof = p_this->Count + CLIST_CHUNK_SIZE;
    SListItem *tmp_Items;

    tmp_Items = (SListItem *)MEM_malloc(nof * sizeof(SListItem));
    if(!tmp_Items)
      return -1;
    p_this->NofAllocated = nof;
    if(p_this->Items && p_this->Count)
    {
      memcpy(tmp_Items, p_this->Items, p_this->Count * sizeof( SListItem)); /* copy old items over to new */
      MEM_free((uint8_t*)p_this->Items);
    }
    p_this->Items = tmp_Items;
  }
  /* add new: */
  p_this->Items[p_this->Count].object = object;
  if(text && *text)
  {
    int n = strlen(text);
    p = (char *)MEM_malloc(n + 1);
    if(p)
      strncpy(p,text, n);
  }
  p_this->Items[p_this->Count].text = p;
  p_this->Count++;
  p_this->IsSorted = 0; /* remember that we can't do binary search now (until it is sorted) */
  return p_this->Count - 1;
}
/*---------------------------------------------------------------------------*/
int SList_Add(char *text, SList *p_this)
{
  return SList_AddObject(text, NULL, p_this);
}
/*---------------------------------------------------------------------------*/
int SList_IndexOf(char *text, SList *p_this)
{
  if(p_this && text)
  {
    SListItem *p = p_this->Items;
    if(p)
    {
      if(p_this->IsSorted == 0) /* Not sorted? Then do linear search: */
      {
        int n;
        for(n = 0 ; n < p_this->Count ; n++)
        {
          if(p->text)
          {
            if(strcmp(p->text, text) == 0) /* found? */
              return n;
          }
          p++;
        }
      }
      else /* Do a binary search: */
      {
        SListItem key, *found;
        key.text = text;
        key.object = NULL;
        found = bsearch(&key, p, p_this->Count, sizeof(SListItem), SListItem_compare);
        if(found)
          return ((found-p) / (int)sizeof(SListItem)); /* calculate index from pointer to found item */
      }
    }
  }
  return -1; /* indicate not found */
}
/*---------------------------------------------------------------------------*/
void SList_Clear(SList *p_this)
{
  if(p_this->NofAllocated)
  {
    SListItem *p = p_this->Items;
    if(p)
    {
      int n = p_this->Count;
      while(n-- > 0)
      {
        if(p_this->OnDelete && p->object != NULL)
          p_this->OnDelete(p->object);
        if(p->text)
        {
          MEM_free((uint8_t*)p->text);
          p->text = NULL;
        }
        p++;
      }
      MEM_free((uint8_t*)p_this->Items);
      p_this->Items = NULL;
    }
    p_this->NofAllocated = 0;
    p_this->Count = 0;
    p_this->IsSorted = 0;
  }
}

/*---------------------------------------------------------------------------*/
void SList_Delete(int i, SList *p_this)
{
  if(p_this)
  {
    if(i >= 0 && i < p_this->Count)
    {
      SListItem *p = &p_this->Items[i];
      if(p_this->OnDelete && p->object != NULL)
      {
        p_this->OnDelete(p->object);
        p->object = NULL;
      }
      if(p->text)
      {
        MEM_free((uint8_t*)p->text);
        p->text = NULL;
      }
      if(i < p_this->Count - 1) /* not deleting the last? ...then move list */
        memmove(p,p+1,sizeof(SListItem)*(p_this->Count - i - 1));
      p_this->Count--; /* we have one less valid item */
    }
  }
}
/*---------------------------------------------------------------------------*/

char *SList_String(int i, SList *p_this)
{
  char *result = NULL;
  if(p_this)
  {
    if(i >= 0 && i < p_this->Count)
      result = p_this->Items[i].text;
  }
  return result;
}
/*---------------------------------------------------------------------------*/

void *SList_Object(int i, SList *p_this)
{
  void *result = NULL;
  if(p_this)
  {
    if(i >= 0 && i < p_this->Count)
      result = p_this->Items[i].object;
  }
  return result;
}

/*---------------------------------------------------------------------------*/

void SList_Sort(SList *p_this)
{
  if(p_this)
  {
    qsort((void *)p_this->Items, p_this->Count, sizeof(SListItem), SListItem_compare);
    p_this->IsSorted = 1;
  }
}
/*---------------------------------------------------------------------------*/

