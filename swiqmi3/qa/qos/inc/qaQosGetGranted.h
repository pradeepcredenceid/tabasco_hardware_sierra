struct QmiQosGetGrantedResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* Response parameters */
    swiQosGranted   *pGranted;
};

enum eQCWWANError PkQmiQosGetGranted (
    WORD    *pMlength,
    BYTE    *pParamField,
    ULONG    id
    );

enum eQCWWANError UpkQmiQosGetGranted(
    BYTE    *pMdmResp,
    struct  QmiQosGetGrantedResp *pApiResp );
