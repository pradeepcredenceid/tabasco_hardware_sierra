/**
 *
 *    $Id: SwiIntDecodeNotify.h,v 1.27 2011/02/23 01:00:52 ckaria Exp $
 *
 *    Name       : SwiIntDecodeNotify.h
 *    
 *    Purpose    : This file contains definitions, enumerations, structures and
 *                 forward declarations for SwiIntDecodeNotify.h
 *
 *    Copyright (c) 2011 Sierra Wireless, Inc.  All rights reserved
 *
 */

/* include files */
#include "SwiDataTypes.h"
#include "SwiRcodes.h"

/* Defines */
#define AP_RSSI_GSM_LENGTH  4

/* CnS object range constants */
#define SWIDUALRANGEMIN 0x1000
#define SWIDUALRANGEMAX 0x1FFF
#define SWICWERANGEMIN  0x0000
#define SWIDMRANGEMIN   0x0E00
#define SWILBSRANGEMIN  0x0F00
#define SWICDMARANGEMIN 0x1000
#define SWIGSMRANGEMIN  0x1000
#define SWISMSRANGEMIN  CNS_SMS_OBJECT_START
#define SWIIOTARANGEMIN CNS_IOTA_STATUS
#define SWIIOTARANGEMIN_LEGACY CNS_IOTA_STATUS_LEGACY
#define SWICALLRANGEMIN CNS_CALL_OBJECT_START
#define SWITECHRANGEMIN CNS_TECH_OBJECT_START
#define SWITEMPRANGEMIN CNS_TEMP_OBJECT_START
#define SWIUMTSRANGEMIN 0x7000

/**
 *
 * Name:    swidualrangeuser - technology user of 0x1000 ranged objectids
 *
 * Purpose: This enumeration is used to indicate how the SDK should 
 *          interpret incoming objects in the 0x1000 to 0x1FFF range.
 *          The interpretation depends upon the technology of the 
 *          attached modem which is determined from information contained
 *          within the Air Server Change notification 
 *             
 * Members: SWIGSMRANGE   The range is mapped to GSM object definitions.
 *          SWICDMARANGE  The range is mapped to CDMA object definitions.
 *
 * Notes:   none
 *
 */
enum swidualrangeuser
{
    SWIGSMRANGE = 0,
    SWICDMARANGE
};

/* definitions */
/**
 *
 * This structure contains a pointer to a function to 
 * decode a notification for a particular CnS object.
 *             
 * The callback function takes two parameters:
 *
 * paramfldp - A pointer to the parameter field 
 *             of the notification to be parsed. The 
 *             contents of this memory field depend
 *             upon the CnS object being reported and
 *             varies from one object to another
 *
 * notifyvariantp - memory location to return parsed 
 *                  values in. The CnS notification
 *                  parameter field is unpacked into
 *                  this structure and returned to
 *                  the caller
 */
struct swinotifyhandler
{
 /*! type of notification */
 enum SWI_TYPE_Notify       swinotifytype;

 /*! pointer to a notification decode function */
 SWI_RCODE (*swihandlerfp)( swi_uint8 *paramfldp, 
                            struct SwiNotifyVariant *notifyvariantp);
};

/* internal prototypes */
extern SWI_RCODE swiradiodecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swirssidecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swichannelnumberdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swichannelstatedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiserviceindicationdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiroamingstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiactivationstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swieciodecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiprevdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiv42bisenableddecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiauthenticationstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiactivenamdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swisystemtimedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swie911modedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swipowersaveenterdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swipowersaveexitdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiexcessspcfailuredecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiruimstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swirssi_eciodecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swihdrnotificationstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swihdr_rssidecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swihdr_roam_statusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swihdr_srv_statedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swihdr_hybr_prefdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiidle_digital_modedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiddtm_prefdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swihdr_powersave_enterdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swihdr_powersave_exitdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swihdr_acqdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swibytecntdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swigsmcelldecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swisimstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swinvstoredecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swinvupdatefromfiledecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swicurradiobanddecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiradiopowerdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiradiotempdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiradiobatterydecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE  swigsmdefprofchangedecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE  swigsmprofilechangedecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE  swiplmnmodedecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE  swiregistrationdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE  swiplmnreadydecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE  swiplmnavaildecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE   swiserviceicondecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE   swinetstatusdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE   swidevicetime( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE   swimodemdisabled( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiomadmconfigdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiomadmstatedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiomadmnialertdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiomadmuialertdisplaydecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiomadmuialertconfirmdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiomadmuialertinputdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiomadmuialertchoicedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swidmdlprogressdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swidmdlalertdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swidmdlinstallstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swilbspagpslockdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swilbspaptlmmodedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swilbspaprivacydecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swilbspanetaccessdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swilbspabsinfodecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE   swilbspawarmstatusdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE   swilbspawarmbegindecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE   swilbspawarmdonedecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE   swilbsnireqdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swidecodeairchgnotif(
    struct amairserverparms *amairparmsp,
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swismsstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swismssendstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiiota_statusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiiota_wappushdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swicallnotificationstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swicallbytecounterdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swicallincomingdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swicallcalleriddecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swicallextendedcalleriddecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swicallconnecteddecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swicalldisconnecteddecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swicallerrordecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swicallconnectingdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swicalldormantdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swicallprivacydecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiotaspstatedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechsiddecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechniddecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechbsinfodecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechbandclassdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechregzonedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechpacketzonedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechpcsblockdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechcellchanneldecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiflashburndecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechhdrnotificationstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechhdrchanneldecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechhdranauthdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechhdrdrcvaldecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechhdrdrccoverdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechhdrrridecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechhdrstatsdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechhdrsufwdstatsdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechhdrsufwdslotcntdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechhdrsufwdslotcnt2decode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechhdrmufwdstatsdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechhdrpersonalitydecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechhdrmacindexdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechhdrprevdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechhdrsectordecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechhdrstatedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE switechhdrsessioninfodecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swimodemtoohotindicationdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
    
extern SWI_RCODE swipktsessioncalldecode(
    swi_uint8 *paramfldp,
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE  swigsmsystemstatusdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
    
extern SWI_RCODE  swirscpeciodecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swilbspddloadbegindecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swilbspddloaddatadecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swilbspddloaddonedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swilbspddloadenddecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swilbspddatadecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swilbspddonedecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swilbspdenddecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swilbspdbegindecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swilbspdupdatefailuredecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swilbspdgetstatusdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swilbssatinfodecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swilbspaipaddrdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swilbspaportiddecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
    
extern SWI_RCODE swilbsxtraassist(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swilbsxtradloadtrigger(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);

extern SWI_RCODE swiphonebookentrydecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
    
extern SWI_RCODE swiphonebookreadydecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
    
extern SWI_RCODE swistkcmdnotifdecode(
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
    
extern SWI_RCODE  swiaudioprofile( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
 
extern SWI_RCODE  swimissedcall( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE  swigsmcallprogressdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE  swigsmcallforwardeddecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE  swigsmcallstatusdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE  swigsmdisplaycficondecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE  swisspasswordstatusdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE  swissrespokdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE  swissfwdinforspdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE  swisscallbarrspdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE  swissinfodatarspdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern package SWI_RCODE  swissstatusrspdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE  swissfwdnumrspdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE  swissbslistrspdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE  swissfwdlistrspdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE  swissrawrspdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE  swisspwdrspdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE  swissclirrspdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE  swisserrorrspdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE  swissrejectrspdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE  swisscancelrspdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE  swissreleaserspdecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE  swissnotifydecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
extern SWI_RCODE  swivoicemessagedecode( 
    swi_uint8 *paramfldp, 
    struct SwiNotifyVariant *notifyvariantp);
/*
 * $Log: SwiIntDecodeNotify.h,v $
 */
