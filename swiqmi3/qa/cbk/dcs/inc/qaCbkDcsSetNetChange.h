#include "qaQmiNotify.h"
#include "qmudefs.h"

struct QmiCbkDcsSetNetChangeReq
{
    ULONG   hiMark;
    ULONG   loMark;
    ULONG   period;
};

struct QmiCbkDcsSetNetChangeCbkResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

extern enum eQCWWANError PkQmiCbkDcsSetNetChange(
    WORD                  *pMlength,
    ULONG                  hiMark,
    ULONG                  loMark,
    ULONG                  period,
    BYTE                  *pBuffer );

extern enum eQCWWANError UpkQmiCbkDcsSetNetChange(
    BYTE   *mdmresp,
    struct QmiCbkDcsSetNetChangeCbkResp *apiresp);

