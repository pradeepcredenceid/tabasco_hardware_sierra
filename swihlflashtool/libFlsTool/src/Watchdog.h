/* -------------------------------------------------------------------------
 Copyright (C) 2013-2014 Intel Mobile Communications GmbH
 
      Sec Class: Intel Confidential (IC)
 ----------------------------------------------------------------------
 Revision Information:
    $File name:  /msw_tools/FlashTool/libFlsTool/src/Watchdog.h $
    $CC-Version: .../dhkristx_140314_v0/2 $
    $Date:       2014-04-16    12:30:46 UTC $
 ----------------------------------------------------------------------
 by CCCT (0.12l)
 ---------------------------------------------------------------------- */

#ifndef WATCHDOG_H_
#define WATCHDOG_H_

#include "Thread.h"
#include "Condition.h"

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct
  {
    int wait_ms;
    CondType cond;
    ThreadType thread;
    void (*signal_fun)(int);
  } WatchdogType;


  void Watchdog_init(WatchdogType* wd, int timeout_ms);

  void Watchdog_destroy(WatchdogType* wd);

#ifdef __cplusplus
}
#endif

/**
 * The C++ implementation of the Watchdog class
 */
#ifdef __cplusplus

  class Watchdog
  {
  public:
    explicit Watchdog(int timeout_ms);
    explicit Watchdog(int timeout_ms, void (*signal_fun)(int));

    ~Watchdog();

    void setSignalFunction(void (*signal_fun)(int));

  private:
    WatchdogType wd;
  };

#endif // __cplusplus

#endif /* WATCHDOG_H_ */
