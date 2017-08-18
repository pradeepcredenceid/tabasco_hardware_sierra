/**
 *
 * @ingroup swiril
 *
 * @file 
 * Provides miscellaneous Sierra specific functions
 *
 * @author
 * Copyright: 2011 Sierra Wireless, Inc.
 *            All rights reserved
 *
 */

#define LOG_TAG "RIL"
#include "swiril_log.h"
#include <stdlib.h>
#include <cutils/properties.h>
#include <telephony/ril.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#include "swiril_misc.h"


/* There are two copies of swiril_main.h, one for the QMI
 * and one for the AT RIL, but both contain a definition 
 * for "ril_iface", passed to our RIL as a startup argument. 
 * This value is required in function waitDHCPnsetDNS(), below.
 */
#include "swiril_main.h"

#define EOS '\0' /* there is another definition in swims/SwiDataTypes.h */

#define PROPERTY_SIERRA_VOICE "ro.sierra.voice"

pthread_mutex_t s_datasession_terminated_mutex = PTHREAD_MUTEX_INITIALIZER;
static bool s_datasession_client_terminated = false;
extern bool IsHL8(void);

/**
 *
 * Determine whether the RIL should be voice enabled
 *
 * @return
 *     true: RIL should be voice enabled
 *     false: RIL should not be voice enabled
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 *
 */
bool isVoiceEnabled(void)
{
    static bool first_time = true;
    static bool sierra_voice_enable = false;

    if (first_time) {
        char propValue[PROPERTY_VALUE_MAX];
        /* Check if the property is defined and value is 1 */
        if ( (property_get(PROPERTY_SIERRA_VOICE, propValue, NULL) > 0) && 
             (0 == strcmp(propValue, "1")) ) {
            LOGI(">>> voice feature enabled %s", propValue);
            sierra_voice_enable = true;
        }
        else {
            LOGI(">>> voice feature disabled");
        }
        first_time = false;
    }
    return sierra_voice_enable;
}

/**
 * check the pointer and print out error message in case of NULL pointer
 *   
 * @param[in] datap
 *     pointer to be checked
 * @param[in] errString
 *     error string to be printed out in case of NULL pointer
 * @return
 *     none
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
void swicheckp(void *datap, char *errString)
{
    if (datap == NULL)
    {
        LOGE("%s", errString);
        exit(1);
    }
}

/**
 * Allocate memory and check the return pointer
 *   
 * @param size
 *     number bytes of memory to be allocated
 * @param[in] errString
 *     error string to be printed out in case of allocation failure
 * @return
 *     pointer to the allocated memory
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
void * swimalloc(int size, char *errString)
{
    void *p = NULL;
    
    p = malloc(size);
    swicheckp(p, errString);
    
    return p;
}

/**
 * Stop the dhcpcd service if already running
 *
 * @return
 *      none
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     None
 */
void checkDHCPnStopService()
{
    char propValue[PROPERTY_VALUE_MAX];
    int ret;

    /* Get the DHCP service status and stop if already running */
    ret = property_get(PROPERTY_SIERRA_DHCP_SRVC, propValue, NULL); 
    LOGI("DHCP: %d, %s", ret, propValue);

#ifndef SWI_RIL_VERSION_6
    if(0 == strcmp(propValue, "running")) {
        if (property_set("ctl.stop", "sierra_dhcpcd")) {
            LOGE("FAILED to set ctl.stop sierra_dhcpcd property!");
        }
    }
#else
    if(0 == strcmp(propValue, "running")) {
        char *property = NULL;
        char dhcp_reason[PROPERTY_VALUE_MAX];
        int i;
        
        /* wait for dhcp client to cleanup routing table after data session terminated.
         * hardcode 6 for now, normally reason changes to "NOCARRIER" less than a second 
         * after data call terminated */
        asprintf(&property, "dhcp.%s.reason", ril_iface);
        /*  */
        for (i = 0; i < 6; i++) {
            sleep(1);
            property_get(property, dhcp_reason, "");
            LOGD("%s %d %s", __func__, i, dhcp_reason);
        
            if (strcmp(dhcp_reason, "NOCARRIER") == 0) {
                break;
            }
            if (strcmp(dhcp_reason, "RELEASE") == 0) {
                break;
            }
        }
        free(property);
        if (property_set("ctl.stop", "sierra_dhcpcd")) {
            LOGE("FAILED to set ctl.stop sierra_dhcpcd property!");
        }
    }
#endif
}


void clearDHCPproperty()
{
    char *property = NULL;
    asprintf(&property, "dhcp.%s.result", ril_iface);
    property_set(property, "");
    LOGD("%s clear %s", __func__, property); 
    free(property);
}
/**
 *
 * Wait until dhcpcd finish and set DNS to system property by 
 * using DNS field of DHCP response
 *
 * @return
 *      0  - successfully wait dhcpcd and set DNS
 *     -1 - otherwise
 *
 * @par Abort:<br> 
 *     none
 *
 * @note
 *     None
 */
int waitDHCPnsetDNS(void)
{
    char dhcp_result[PROPERTY_VALUE_MAX];
    int dhcp_ok = 0;
    int i,retry;
    char *property = NULL;
    int iret = -1;
    #define MAX_RETRY 60

    /* wait for dhcp client to finish successfully */
    asprintf(&property, "dhcp.%s.result", ril_iface);
    for (retry = 0; retry < MAX_RETRY ; retry++) {
        sleep(1);

        if (!isDataSessionActive()) {
            LOGE("%s:: Data Session is not active. \n", __func__);
            break;
        }
        
        property_get(property, dhcp_result, "");
    
        if (strcmp(dhcp_result, "ok") == 0) {
            dhcp_ok = 1;
            break;
        }
        if (strcmp(dhcp_result, "failed") == 0) {
            LOGE("%s: %s", property, dhcp_result);
            break;
        }
    }
    free(property);

    /* if init was successful, grab the DNS and gateway addresses */
    if (dhcp_ok == 1) {
        for (i = 1; i <= SWI_MAX_DNS; i++) {
            asprintf(&property, "dhcp.%s.dns%d", ril_iface, i);
            property_get(property, dhcp_result, "");
            free(property);
            
            /* if we got something other than an empty string */      
            if (dhcp_result[0] != '\0') {
                asprintf(&property, "net.%s.dns%d", ril_iface, i);
                LOGI("DNS Server %d: %s %s\n", i, property, dhcp_result);
                if (property_set(property, dhcp_result)) {
                    LOGE("FAILED to set dns%d property!", i);
                }
                free(property);
            }
        }
        /* get gateway */
        asprintf(&property, "dhcp.%s.gateway", ril_iface);
        property_get(property, dhcp_result, "");
        free(property);
        
        /* if we got something other than an empty string */      
        if (dhcp_result[0] != '\0') {
            asprintf(&property, "net.%s.gw", ril_iface);
            LOGI("Gateway: %s %s\n", property, dhcp_result);
            if (property_set(property, dhcp_result)) {
                LOGE("FAILED to set gateway property!");
            }
            free(property);
        }
        iret = 0;
    }
    else
    {
        #if 0
        if(retry==MAX_RETRY)
        {
            /* restart wwan0 */
            LOGD("Restart wwan ... \n");
            setPowerOff();
        }
        #endif
    }
    return iret;
}

/**
 *
 * Poll dhcpcd finish and set DNS to system property by
 * using DNS field of DHCP response
 *
 * @return
 *      0  - successfully wait dhcpcd and set DNS
 *     -1 - otherwise
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     None
 */
int PollDHCPnsetDNS(void)
{
    char dhcp_result[PROPERTY_VALUE_MAX];
    int dhcp_ok = 0;
    int i;
    char *property = NULL;
    int iret = -1;

    /* wait for dhcp client to finish successfully */
    asprintf(&property, "dhcp.%s.result", ril_iface);
    if (!isDataSessionActive()) {
        LOGE("%s:: Data Session is not active. \n", __func__);
    }
    else
    {
        property_get(property, dhcp_result, "");
        if (strcmp(dhcp_result, "ok") == 0)
            dhcp_ok = 1;
        if (strcmp(dhcp_result, "failed") == 0)
            LOGE("%s: %s", property, dhcp_result);
    }

    LOGI("%s: %s", property, dhcp_result);


    free(property);

    /* if init was successful, grab the DNS and gateway addresses */
	#ifndef SWI_HL_RIL
    if (dhcp_ok == 1)
	#else
    if ((dhcp_ok == 1) || (!IsHL8()))
	#endif
    {
        for (i = 1; i <= SWI_MAX_DNS; i++)
        {
            asprintf(&property, "dhcp.%s.dns%d", ril_iface, i);
            property_get(property, dhcp_result, "");
            free(property);
            /* if we got something other than an empty string */
            if (dhcp_result[0] != '\0')
            {
                asprintf(&property, "net.%s.dns%d", ril_iface, i);
                LOGI("DNS Server %d: %s %s\n", i, property, dhcp_result);
                if (property_set(property, dhcp_result))
                    LOGE("FAILED to set dns%d property!", i);
                free(property);
            }
        }
        /* get gateway */
        asprintf(&property, "dhcp.%s.gateway", ril_iface);
        property_get(property, dhcp_result, "");
        free(property);
        /* if we got something other than an empty string */
        if (dhcp_result[0] != '\0')
        {
            asprintf(&property, "net.%s.gw", ril_iface);
            LOGI("Gateway: %s %s\n", property, dhcp_result);
            if (property_set(property, dhcp_result))
                LOGE("FAILED to set gateway property!");
            free(property);
        }

        LOGI("Gateway: %s %s\n", property, dhcp_result);

        iret = 0;
    }
    return iret;
}

/**
 *
 * activateNetworkInterface
 * Returns nothing.
 *
 * Used to put ril_iface UP or DOWN
 */

void activateNetworkInterface(bool bEnable)
{
    int fd;
    struct ifreq ifr;
    unsigned short flags;
   
    memset(&ifr, 0 , sizeof(struct ifreq));
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        LOGE("%s: socket creation returned error %d", __func__, errno);
        return;
    }

    (void) strncpy(ifr.ifr_name, ril_iface, sizeof(ifr.ifr_name));

    if (ioctl(fd, SIOCGIFFLAGS, &ifr) == -1) {
        LOGE("%s: could not get network interface flags, errno is %d", __func__, errno);
        close(fd);
        return;
    }

    flags = ifr.ifr_flags;
  
    if (bEnable) {
        if (!(flags & IFF_UP)) {
            flags |= IFF_UP;
            ifr.ifr_flags = flags;
            if (ioctl(fd, SIOCSIFFLAGS, &ifr) == -1) {
                LOGE("%s: could not enable IFF_UP flag, errno is %d", __func__, errno);
                close(fd);
                return;
            }
        }
    }
    else {
        if (flags & IFF_UP) {
            flags &= ~IFF_UP;
            ifr.ifr_flags = flags;
            if (ioctl(fd, SIOCSIFFLAGS, &ifr) == -1) {
                LOGE("%s: could not disable IFF_UP flag, errno is %d", __func__, errno);
                close(fd);
                return;
            }
        }
    }
    close(fd);
}

/**
 * String Copy (with max.)
 * Copies not more than "length" characters
 * (characters that follow a null character
 * are not copied) from the array pointed to
 * by srcp into the array pointed to by destp.
 * If copying takes place between objects
 * that overlap, the behaviour is undefined.
 *   
 * @param[out] *destp
 *     pointer to location of memory
 *     to where string will be copied
 * @param[in] *srcp
 *     pointer to location of memory
 *     from where string will be copied
 * @param length
 *     maximum characters to copy
 * @return
 *     The value of destp
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     No pointer verification
 *     If the string pointed to by srcp is longer than length characters,
 *     the resultant array pointed to by destp will not be null terminated.
 *     If the string pointed to by srcp is shorter than length characters,
 *     null characters are continually copied to the string pointed by
 *     destp until "length" characters have been copied.
 */
char *slstrncpy(char *destp, const char *srcp, unsigned long length)
{
  char *destcpyp;     /* copy of destp */
  char *srccpyp;      /* copy of srcp */

  destcpyp = destp;
  srccpyp = (char *)srcp;

  /* copy characters in the array pointed by srcp into the array pointed to by
   * destp
   */
  while(length && (*srccpyp != EOS))
  {
    *destcpyp++ = *srccpyp++;
    length--;
  }

  /* null characters are continually copied to the string pointed by destp
   * until "length" characters have been copied.
   */
  while(length--)
    *destcpyp++ = EOS;

  return destp;
}

/**
 * Convert to Upper Case
 * If the passed-in character is a lower-
 * case character then return the
 * corresponding upper-case letter.
 * Otherwise, return the passed-in
 * character unchanged.
 *   
 * @param c
 *     lower-case character to convert to upper-case
 * @return
 *     Upper-case converted character
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     none
 */
char sltoupper(char c)
{
  char retval;    /* return value */

  /* initialize return value */
  retval = c;

  /* If we have a lower case character */
  if (c >= 'a' && c <= 'z')
  {
    /* Convert lower case character to upper case */
    retval = c - 'a' + 'A';
  }

  return (retval);
}


/**
 * converts string to upper-case.   
 *
 * @param[out] *destp
 *     pointer to location of memory
 *     to where string will be copied
 * @param[in] *srcp
 *     pointer to location of memory
 *     from where string will be copied
 * @param length
 *     maximum characters to copy
 * @return
 *     The value of destp
 *
 * @par Abort:<br>
 *     none
 *
 * @note
 *     If the string pointed to by srcp is longer than length characters,
 *     the resultant array pointed to by destp will not be null terminated.
 *     If the string pointed to by srcp is shorter than length characters,
 *     null characters are continually copied to the string pointed by
 *     destp until "length" characters have been copied.
 */
char *sltoupperstrn(char *destp, const char *srcp, unsigned long length)
{
  char *workp;


  slstrncpy( destp, srcp, length);

  /* convert to upper-case */
  workp = destp;
  while( (length > 0) && (*workp != EOS))
  {
    *workp = sltoupper(*workp);
    length--;
    workp++;
  }

  return destp;
}

/**
 *
 * Check and set the data session deactivated state by client/framework
 *
 * @param new client state
 *      data session deactivated by client state (1 or 0)
 * @return old client state
 *      1 - deactivated by client
 *      0 - default, not deactivated by client
 *
 * @note
 *     None
 */

bool isDataSessionCloseByClient(bool state)
{
    bool ret = false;

    /* Protect the data session terminated state */
    pthread_mutex_lock(&s_datasession_terminated_mutex);
    ret = s_datasession_client_terminated;
    s_datasession_client_terminated = state;
    LOGD("isDataSessionCloseByClient : return state=%d , new state = %d \n", ret, s_datasession_client_terminated );
    pthread_mutex_unlock(&s_datasession_terminated_mutex);

    return ret;
}
