struct QmiQosRelResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;
};

enum eQCWWANError PkQmiQosRel (
    WORD    *pMlength,
    BYTE    *pParamField,
    swiQosIds *pQosIds
    );

enum eQCWWANError UpkQmiQosRel(
    BYTE    *pMdmResp,
    struct  QmiQosRelResp *pApiResp );
