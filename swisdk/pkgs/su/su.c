/*************
 *
 * Filename:  su.c
 *
 * Purpose:   SDK Startup package. SDK package initializations and task
 *            creation synchronization.
 *
 * Copyright: © 2011 Sierra Wireless Inc., all rights reserved
 *
 **************/

/*-------------
  include files
 --------------*/

#include "aa/aaglobal.h"
#include "am/amudefs.h"
#include "ci/ciudefs.h"
#include "cn/cnudefs.h"
#include "dl/dludefs.h"
#include "dr/drudefs.h"
#include "ds/dsudefs.h"
#include "er/erudefs.h"
#include "hd/hdudefs.h"
#include "hp/hpudefs.h"
#include "ic/icudefs.h"
#include "mm/mmudefs.h"
#include "pi/piudefs.h"
#include "ql/qludefs.h"
#include "sdk/sdkudefs.h"
#include "sl/sludefs.h"
#include "ui/uiudefs.h"
#include "us/usudefs.h"

/*-------
  Defines
 --------*/

/*-------------
  Local storage
 --------------*/
local struct dlusercb sudlcb;

/*---------
  Functions
 ----------*/

/*************
 *
 * Name:    sutaskinitcb
 *
 * Purpose: Task creation callback function used to enforce a deterministic
 *          task startup sequence
 *
 * Parms:   sdkpkg - the package for which the required task(s) has been created
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:   every package for which a task is created must call this function
 *          to report that its task(s) is(are) running
 *
 *          Each time this function is called, it is called from within the 
 *          context of the last task that was started.
 *
 **************/
global void sutaskinitcb(enum sdkpkg_e sdkpkg)
{
    dlLog(  &sudlcb,
            DLCLASSALL,
            "%d -> task started",
            (swi_uint32)sdkpkg,
            (swi_uint32)NULL,
            (swi_uint32)NULL,
            (swi_uint32)NULL );

    /* Tasks are started serially */
    switch(sdkpkg)
    {
        case SDK_ICPKG:
            usstaskinit();
            break;
        case SDK_USSPKG:
            usdtaskinit();
            break;
        case SDK_USDPKG:
            hptxtaskinit();
            break;
        case SDK_HPTXPKG:
            hpphtaskinit();
            break;
        case SDK_HPPHPKG:
            hprxtaskinit();
            break;
        case SDK_HPRXPKG:
            amipctaskinit();
            break;
        case SDK_AMPKG:
            cnrrtaskinit();
            break;
        case SDK_CNPKG:
            drtaskinit();
            break;
        case SDK_DRPKG:
/* RILSTART */
/* Removing this case because the RIL does not require the UI task */
#if 0
            uitaskinit();
            break;
        case SDK_UIPKG:
#endif
/* RILSTOP */
            dlLog(  &sudlcb,
                    DLCLASSALL,
                    "Task initialization complete",
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL,
                    (swi_uint32)NULL );
            break;
        default:
            break;
    }
}

/*************
 *
 * Name:    suinitlocal
 *
 * Purpose: SU package internal initialization
 *
 * Parms:   none
 *
 * Return:  none
 *
 * Abort:   none
 *
 * Notes:
 *
 **************/
local void suinitlocal(void)
{
    dlregister("SU", &sudlcb, TRUE);
}

/*************
 *
 * Name:    suinit
 *
 * Purpose: Call all package initialization routines for unit
 *          testing on the desktop
 *
 * Parms:   none
 *
 * Return:  no value
 *
 * Abort:   none
 *
 * Notes:   To be called during SDK startup from the "main"
 *          entry point.
 *
 *          Initialization performed within each "xxinit()" call is
 *          limited to setting up internal structures within the
 *          package itself. It is not permitted that one package
 *          call into another to use its services during this phase
 *          of system initialization. However, it is permissible to
 *          make calls into the OS package for memory allocation,
 *          logging, etc. It is assumed that the OS Wrapper is fully
 *          initialized and ready to offer its services to outside
 *          callers.
 *
 *          This function is split into two parts, individual package
 *          initialization is first, followed by creation of tasks.
 *          Task creation should only be started after all package
 *          initialization routines have been called.
 *
 *          Task creation is serialized too, meaning that only one 
 *          task initialization function appears in part 2, below.
 *          When it is done starting, it calls sutaskinitcb(), above,
 *          which launches the next task in the sequence. This way,
 *          tasks are started in a controlled order
 *
 **************/
global void suinit( void )
{

    /************ Part 1 - Package Initialization ***************/
    /* First call the package initialization functions */
    dlinit();
    mminit();          /* Must be first */
    icinitipc();
    ciinit();
    hdinit(HPRESERVE);
    hpinit();
    cninit();
    usinit();
    aminit();
    dsinit();
    drinit();
    suinitlocal();

    /***************** Part 2 - Task Creation *****************/

    /* next, kick off package task creation */
    ictttaskinit();
}
