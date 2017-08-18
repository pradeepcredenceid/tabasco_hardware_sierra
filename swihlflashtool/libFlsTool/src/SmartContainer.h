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
 * Revision Information:
 *  $File name:  /msw_tools/FlashTool/libFlsTool/src/SmartContainer.h $
 *  $CC-Version: .../dhkristx_140314_v0/1 $
 *  $Date:       2014-04-16    12:30:27 UTC $
 */

#ifndef SMARTCONTAINER_H_
#define SMARTCONTAINER_H_

#include <algorithm>
#include <memory>
#include <iostream>
#include <set>
#include <list>
#include <map>

using namespace std;

// Basic Container Iterator
#define foreach(it,list) \
  for ( typeof( list.begin() ) it = list.begin(); it != list.end(); ++it )

#define foreach_ptr(it,list) \
  for ( typeof( list->begin() ) it = list->begin(); it != list->end(); ++it )

#define find_in_vector(list, item) \
  (std::find(list.begin(), list.end(), item) != list.end())


// STL List pointer remover
template <class T>
struct delete_ptr
{
  void operator () (T p)
  {
    delete p;
  }
};


// STL Vector Container of pointers
template <typename T>
class SmartVector : public vector<T>
{
public:
  using vector<T>::size;
  using vector<T>::end;
  using vector<T>::begin;
  using vector<T>::push_back;

  ~SmartVector()
  {
    for (size_t i=0; i < size(); ++i)
      delete ( (*this)[i] );
  }
};

// STL List Container of pointers
template <typename T>
class SmartList : public list<T>
{
public:
  using list<T>::size;
  using list<T>::end;
  //using list<T>::front;
  using list<T>::begin;
  using list<T>::sort;          //< Not in std::vector
  using list<T>::push_back;
  using list<T>::push_front;    //< Not in std::vector

  T operator[](int index)
  {
    typename list<T>::iterator it;
    for (it = begin(); it != end() && index; ++it, --index)
      ;
    return *it;
  }

  ~SmartList()
  {
    std::for_each(begin(), end(), delete_ptr<T>());
    //typename list<T>::iterator it;
    //for (it = begin(); it != end(); ++it)
    //  delete ( *it );
  }
};
/*
template <typename T>
bool operator==(typename vector<T>::iterator& a, typename vector<T>::iterator& b)
  { return a == b; }

template <typename T>
bool operator==(typename list<T>::iterator& a, typename list<T>::iterator& b)
  { return a == b; }
*/
#endif /* SMARTCONTAINER_H_ */
