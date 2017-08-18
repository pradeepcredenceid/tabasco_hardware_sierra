/* -------------------------------------------------------------------------
 Copyright (C) 2012-2014 Intel Mobile Communications GmbH

      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/MemberCallback.h $
    $CC-Version: .../dhkristx_140314_v0/1 $
    $Date:       2014-04-22    9:14:08 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12h)
 ----------------------------------------------------------------------
    Description:

       Works now with 0-2 arguments. If more is needed just add a:
        - class CallbackX
        - class MemberCallbackFactoryX
        - template<> MemberCallbackFactoryX GetCallbackFactory() function

    Note: Up until now we only need callbacks with two arguments...
          If more is needed just implement them!

 ----------------------------------------------------------------------
    Example Code:

    class Foo
    {
    public:
      float Average(int n1, int n2)
      {
        return (n1 + n2) / 2.0f;
      }
    };

    void* cb_normal(void* arg)
    {
      cout << static_cast<const char*>(arg) << endl;
    }

    void main()
    {
      Foo f;
      Callback0<void*> normal( &cb_normal, (void*)"HelloWorld" );

      Callback2<float, int, int> average = MemFun(&Foo::Average, &f);
    }
    ------------------------------------------------------------------------- */
#ifdef __cplusplus
#ifndef MEMBERCALLBACK_H_
#define MEMBERCALLBACK_H_

 #include <iostream>
 #include <typeinfo>  //for 'typeid' to work

#include <vector>

using namespace std;

#define foreach(it,list) \
  for ( typeof( list.begin() ) it = list.begin(); it != list.end(); ++it )

/**
 * Callback Template
 */
template<typename R>
class Callback0
{
public:
  typedef R (*FuncType)(void*);
  Callback0() : func(0), obj(0) {}
  Callback0(FuncType f, void* o) : func(f), obj(o) {}
  void clear() { func = 0; }
  operator bool() { return (obj); }
  R operator()()  { return (*func)(obj); }
  R operator()() const { return (*func)(obj); }
private:
  FuncType func;
  void* obj;
};

template<typename R>
class FunPtr0
{
public:
  typedef R (*FuncType)();
  FunPtr0(FuncType f) : func(f) {}
  void clear() { func = 0; }
  R operator()()  { return (*func)(); }
  R operator()() const { return (*func)(); }
private:
  FuncType func;
};

template<typename R>
FunPtr0<R> FunPtr(R (*Func)())
{
  return FunPtr0<R>(Func);
}

template<typename R>
class Signal0
{
public:
  void connect(FunPtr0<R> f)    {    fun_list.push_back( f );  }
  void connect(Callback0<R> f)  {    mem_list.push_back( f );  }
  void operator()()             {    emit();  }
  void emit()
  {
    foreach (it, fun_list)      (*it)();
    foreach (it, mem_list)      (*it)();
  }
private:
  vector< FunPtr0<R> > fun_list;
  vector< Callback0<R> > mem_list;
};

/* Wrapper of the Signal1 class
template<typename R>
class Signal <R>
  : public Signal0<R>
{
};
*/
template<typename R, typename P1>
class Callback1
{
public:
  typedef R (*FuncType)(void*, P1);
  Callback1() : func(0), obj(0) {}
  Callback1(FuncType f, void* o) : func(f), obj(o) {}
  void clear() { func = 0; }
  operator bool() { return (obj); }
  R operator()(P1 a1)  { return (*func)(obj, a1); }
  R operator()(P1 a1) const { return (*func)(obj, a1); }
private:
  FuncType func;
  void* obj;
};

template<typename R, typename P1>
class FunPtr1
{
public:
  typedef R (*FuncType)(P1);
  FunPtr1(FuncType f) : func(f) {}
  void clear() { func = 0; }
  R operator()(P1 a1) { return (*func)(a1); }
  R operator()(P1 a1) const { return (*func)(a1); }
private:
  FuncType func;
};

template<typename R, typename P1>
FunPtr1<R, P1> FunPtr(R (*Func)(P1))
{
  return FunPtr1<R, P1>(Func);
}

template<typename R, typename P1>
class Signal1
{
public:
  void connect(FunPtr1<R,P1> f)    {    fun_list.push_back( f );  }
  void connect(Callback1<R,P1> f)  {    mem_list.push_back( f );  }
  void operator()(P1 a1)           {    emit(a1);  }
  void emit(P1 a1)
  {
    foreach(it, fun_list)     (*it)(a1);
    foreach(it, mem_list)     (*it)(a1);
  }
private:
  vector< FunPtr1<R,P1> > fun_list;
  vector< Callback1<R,P1> > mem_list;
};

/* Wrapper of the Signal1 class
template<typename R, typename P1>
class Signal <R, P1>
  : public Signal1<R, P1>
{
};
*/

template<typename R, typename P1, typename P2>
class Callback2
{
public:
  typedef R (*FuncType)(void*, P1, P2);
  Callback2() : func(0), obj(0) {}
  Callback2(FuncType f, void* o) : func(f), obj(o) {}
  void clear() { func = 0; }
  operator bool() { return (obj); }
  R operator()(P1 a1, P2 a2) { return (*func)(obj, a1, a2); }
  R operator()(P1 a1, P2 a2) const { return (*func)(obj, a1, a2); }
private:
  FuncType func;
  void* obj;
};

template<typename R, typename P1, typename P2>
class FunPtr2
{
public:
  typedef R (*FuncType)(P1, P2);
  FunPtr2(FuncType f) : func(f) {}
  void clear() { func = 0; }
  R operator()(P1 a1, P2 a2) { return (*func)(a1, a2); }
  R operator()(P1 a1, P2 a2) const { return (*func)(a1, a2); }
private:
  FuncType func;
};

template<typename R, typename P1, typename P2>
FunPtr2<R, P1, P2> FunPtr(R (*Func)(P1, P2))
{
  return FunPtr2<R, P1, P2>(Func);
}

template<typename R, typename P1, typename P2>
class Signal2
{
public:
  void connect(FunPtr2<R,P1,P2> f)    {    fun_list.push_back( f );  }
  void connect(Callback2<R,P1,P2> f)  {    mem_list.push_back( f );  }
  void operator()(P1 a1, P2 a2)       {    emit(a1,a2);  }
  void emit(P1 a1, P2 a2)
  {
    foreach(it, fun_list)      (*it)(a1, a2);
    foreach(it, mem_list)      (*it)(a1, a2);
  }
private:
  vector< Callback2<R,P1,P2> > fun_list;
  vector< Callback2<R,P1,P2> > mem_list;
};

/* Wrapper of the Signal2 class */
//template<typename R, typename P1, typename P2>
//class Signal<R, P1, P2> : public Signal2<R, P1, P2> { };


template<typename R, typename P1, typename P2, typename P3>
class Callback3
{
public:
  typedef R (*FuncType)(void*, P1, P2, P3);
  //Callback3() : func(0), obj(0) {}
  Callback3(FuncType f, void* o) : func(f), obj(o) {}
  void clear() { func = 0; }
  operator bool() { return (obj); }
  R operator()(P1 a1, P2 a2, P3 a3) { return (*func)(obj, a1, a2, a3); }
  R operator()(P1 a1, P2 a2, P3 a3) const { return (*func)(obj, a1, a2, a3); }
private:
  FuncType func;
  void* obj;
};

template<typename R, typename P1, typename P2, typename P3>
class FunPtr3
{
public:
  typedef R (*FuncType)(P1, P2, P3);
  FunPtr3(FuncType f) : func(f) {}
  void clear() { func = 0; }
  R operator()(P1 a1, P2 a2, P3 a3) { return (*func)(a1, a2, a3); }
  R operator()(P1 a1, P2 a2, P3 a3) const { return (*func)(a1, a2, a3); }
private:
  FuncType func;
};

template<typename R, typename P1, typename P2, typename P3>
FunPtr3<R, P1, P2, P3> FunPtr(R (*Func)(P1, P2, P3))
{
  return FunPtr3<R, P1, P2, P3>(Func);
}

template<typename R, typename P1, typename P2, typename P3>
class Signal3
{
public:
  void connect(FunPtr3<R, P1, P2, P3> f)  {    fun_list.push_back( f );  }
  void connect(Callback3<R,P1,P2,P3> f)   {    mem_list.push_back( f );  }
  void operator()(P1 a1, P2 a2, P3 a3)    {    emit(a1,a2,a3);  }
  void emit(P1 a1, P2 a2, P3 a3)
  {
    foreach(it, fun_list)  (*it)(a1, a2, a3);
    foreach(it, mem_list)  (*it)(a1, a2, a3);
  }
private:
  vector< FunPtr3<R, P1, P2, P3> > fun_list;
  vector< Callback3<R,P1,P2,P3> > mem_list;
};

/**
 *  Wrapper of the Signal3 class
 *
 *  TODO: To make more General templates take a look at sigc++/signal.h:3508
 *
 **/
template<typename R, typename P1, typename P2, typename P3>
class Signal : public Signal3<R, P1, P2, P3> { };

//template<class R, class P1 = 0, class P2 = 0, class P3 = 0>
//class Signal : public Signal3<R, P1, P2, P3> { };

//template<class R, class P1, class P2>
//class Signal <R, P1, P2> : public Signal2<R, P1, P2> { };


/**
 * MemberCallbackFactory Template
 */
template<typename R, class T>
class MemberCallbackFactory0
{
private:
  template<R (T::*Func)()>
  static R Wrapper(void* o)
  {
    return (static_cast<T*>(o)->*Func)();
  }
  template<R (T::*Func)() const>
  static R Wrapper(void* o)
  {
    return (static_cast<T*>(o)->*Func)();
  }
public:
  template<R (T::*Func)()>
  static Callback0<R> Bind(T* o)
  {
    return Callback0<R>(&Wrapper<Func>, o);
  }

  template<R (T::*Func)() const>
  static Callback0<R> Bind(T* o)
  {
    return Callback0<R>(&Wrapper<Func>, o);
  }

};

template<typename R, class T, typename P1>
class MemberCallbackFactory1
{
private:
  template<R (T::*Func)(P1)>
  static R Wrapper(void* o, P1 a1)
  {
    return (static_cast<T*>(o)->*Func)(a1);
  }
  template<R (T::*Func)(P1) const>
  static R Wrapper(void* o, P1 a1)
  {
    return (static_cast<T*>(o)->*Func)(a1);
  }
public:
  template<R (T::*Func)(P1)>
  static Callback1<R, P1> Bind(T* o)
  {
    return Callback1<R, P1>(&Wrapper<Func>, o);
  }
  template<R (T::*Func)(P1) const>
  static Callback1<R, P1> Bind(T* o)
  {
    return Callback1<R, P1>(&Wrapper<Func>, o);
  }
};

template<typename R, class T, typename P1, typename P2>
class MemberCallbackFactory2
{
private:
  template<R (T::*Func)(P1, P2)>
  static R Wrapper(void* o, P1 a1, P2 a2)
  {
    return (static_cast<T*>(o)->*Func)(a1, a2);
  }
  template<R (T::*Func)(P1, P2) const>
  static R Wrapper(void* o, P1 a1, P2 a2)
  {
    return (static_cast<T*>(o)->*Func)(a1, a2);
  }
public:
  template<R (T::*Func)(P1, P2)>
  static Callback2<R, P1, P2> Bind(T* o)
  {
    return Callback2<R, P1, P2>(&Wrapper<Func>, o);
  }
  template<R (T::*Func)(P1, P2) const>
  static Callback2<R, P1, P2> Bind(T* o)
  {
    return Callback2<R, P1, P2>(&Wrapper<Func>, o);
  }
};

template<typename R, class T, typename P1, typename P2, typename P3>
class MemberCallbackFactory3
{
private:
  template<R (T::*Func)(P1, P2, P3)>
  static R Wrapper(void* o, P1 a1, P2 a2, P3 a3)
  {
    return (static_cast<T*>(o)->*Func)(a1, a2, a3);
  }
  template<R (T::*Func)(P1, P2, P3) const>
  static R Wrapper(void* o, P1 a1, P2 a2, P3 a3)
  {
    return (static_cast<T*>(o)->*Func)(a1, a2, a3);
  }
public:
  template<R (T::*Func)(P1, P2, P3)>
  static Callback3<R, P1, P2, P3> Bind(T* o)
  {
    return Callback3<R, P1, P2, P3>(&Wrapper<Func>, o);
  }
  template<R (T::*Func)(P1, P2, P3) const>
  static Callback3<R, P1, P2, P3> Bind(T* o)
  {
    return Callback3<R, P1, P2, P3>(&Wrapper<Func>, o);
  }
};

/**
 * GetCallbackFactory()
 */
template<typename R, class T>
MemberCallbackFactory0<R, T> GetCallbackFactory(R (T::*Func)())
{
  (void)(Func);
  return MemberCallbackFactory0<R, T>();
}

template<typename R, class T>
MemberCallbackFactory0<R, T> GetCallbackFactory(R (T::*Func)() const)
{
  (void)(Func);
  return MemberCallbackFactory0<R, T>();
}

template<typename R, class T, typename P1>
MemberCallbackFactory1<R, T, P1> GetCallbackFactory(R (T::*Func)(P1))
{
  (void)(Func);
  return MemberCallbackFactory1<R, T, P1>();
}

template<typename R, class T, typename P1>
MemberCallbackFactory1<R, T, P1> GetCallbackFactory(R (T::*Func)(P1) const)
{
  (void)(Func);
  return MemberCallbackFactory1<R, T, P1>();
}

template<typename R, class T, typename P1, typename P2>
MemberCallbackFactory2<R, T, P1, P2> GetCallbackFactory(R (T::*Func)(P1, P2))
{
  (void)(Func);
  return MemberCallbackFactory2<R, T, P1, P2>();
}

template<typename R, class T, typename P1, typename P2>
MemberCallbackFactory2<R, T, P1, P2> GetCallbackFactory(R (T::*Func)(P1, P2) const)
{
  (void)(Func);
  return MemberCallbackFactory2<R, T, P1, P2>();
}

//
// TODO: AUTO_PTR
//
template<typename R, class T, typename P1, typename P2, typename P3>
MemberCallbackFactory3<R, T, P1, P2, P3> GetCallbackFactory(R (T::*Func)(P1, P2, P3))
{
  (void)(Func);
  return MemberCallbackFactory3<R, T, P1, P2, P3>();
}

template<typename R, class T, typename P1, typename P2, typename P3>
MemberCallbackFactory3<R, T, P1, P2, P3> GetCallbackFactory(R (T::*Func)(P1, P2, P3) const)
{
  (void)(Func);
  return MemberCallbackFactory3<R, T, P1, P2, P3>();
}

/**
 * MACRO for MemFun()
 */
#define MemFun(memFuncPtr, instancePtr) \
  (GetCallbackFactory(memFuncPtr).Bind<memFuncPtr>(instancePtr))

#endif /* MEMBERCALLBACK_H_ */
#endif /* __cplusplus */
