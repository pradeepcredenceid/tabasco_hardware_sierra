/*
 * Filename: $Id: dlreadme.txt,v 1.2 2009/11/03 19:18:00 rfiler Exp $
 * Name:  DL Package User Information
 *
 * Purpose DL package provides debug logging to the user. It allows all other
 * packages to log arbitrary messages to /var/log/user.log (in Debian-style
 * Linux systems) during runtime from within the SDK Daemon. Up to 4 
 * additional format arguments can be passed in the global entry point using 
 * standard C-Language-style formatting commands (e.g. %d for decimal, %x for
 * hexadecimal, etc)
 *
 * When new packages are created and require logging, the owner of the package
 * must register with DL by using the global entry point dlregister(). The
 * function also accepts a boolean which is the master enable switch for the
 * package, FALSE disables logging and TRUE enables it. 
 * 
 * C files are:
 * dlpackage.c - User functions of the package
 * Header files are:
 * dluproto.h - Global function prototypes
 * dludefs.h - Global definitions
 * dliproto.h - Internal function prototypes
 * dlidefs.h - Internal definitions
 * 
 * Make files:
 * - dl.mak
 * 
 * Usage: 
 *    All packages must provide their own control block for registration 
 *    dlregister (myPID, &usercb, masterenable);  
 *
 *     /* Sample call to log to /var/log/user.log */
 *   dlLog (myPID, DLCLASSALL, "Message to be logged with up to %d format args",
 *          4,
 *          arg2,
 *          arg3, 
 *          arg4 );
 *
 *   Whether they are required in the logging message or not, callers must
 *   always provide the 4 arguments in swi_uint32 format. If they are not
 *   required NULL should be passed instead.
 *
 * Testing:
 *          To test this package, compile and link the images including
 *          dltest.c from the command line. Execute the target and review
 *          the output. The test will print out some messages on the console.
 *          The actual logs are appended to /var/log/user.log
 *          
 * $Log: dlreadme.txt,v $
 * Revision 1.2  2009/11/03 19:18:00  rfiler
 * Bug 19444, Bringing this file up-to-date with changes made for
 * bug 16145 as part of 19444 activities
 *
 *
 */
