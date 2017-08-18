/*
 * $Id: qludefs.h,v 1.3 2008/05/13 04:59:16 rfiler Exp $
 *
 * Filename: qluser.h
 *
 * Purpose:  User definitions for queue management
 *
 * Copyright: C 2008 Sierra Wireless Inc., All rights reserved
 *
 */

#ifndef qluser_h
#define qluser_h

/*
 * Name: qllink
 *
 * Purpose: Defines a queue link. Required by all users of this package
 *          when they define a queue link.
 *
 * Members: qlnextp - pointer to next queue link
 *          qlprevp - pointer to previous queue link
 *          qluserp - pointer to user data being linked
 *
 * Notes: none
 *
 */

struct qllink
{
    struct qllink *qlnextp;
    struct qllink *qlprevp;
    void *qluserp;
};

#include "qluproto.h"

#endif

/* 
 * $Log: qludefs.h,v $
 */
