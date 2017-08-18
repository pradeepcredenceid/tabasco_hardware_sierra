struct QmiQosGetFlowStatusResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* Response parameters */
    BYTE    *pStatus;
};

extern enum eQCWWANError PkQmiQosGetFlowStatus (
    WORD    *pMlength,
    BYTE    *pParamField,
    ULONG    id);

extern enum eQCWWANError UpkQmiQosGetFlowStatus (
    BYTE     *pMdmResp,
    struct   QmiQosGetFlowStatusResp *pAipResp );
