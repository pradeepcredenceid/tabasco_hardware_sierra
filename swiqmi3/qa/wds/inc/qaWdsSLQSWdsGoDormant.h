/*
 * \ingroup wds
 *
 * \file qaWdsSLQSWdsGoDormant.h
 *
 * \brief This file contains definitions, enumerations, structures and
 *        forward declarations for qaWdsSLQSGoDormant.c
 *
 * Copyright: © 2014 Sierra Wireless, Inc. all rights reserved
 *
 */
#ifndef __QA_WDS_SLQS_WDS_GO_DORMANT_H__
#define __QA_WDS_SLQS_WDS_GO_DORMANT_H__

/*
 * This structure contains the GetDormancyState response parameters.
 *
 * \sa qaGobiApiWds.h for parameter descriptions
 *
 */
struct QmiWdsGoDormantResp
{
    /* Every response message must have a results structure */
    struct qmTlvResult results;

    /* GoDormat response parameters */
    /* None */
};

/*
 * Prototypes
 */
extern enum eQCWWANError PkQmiWdsGoDormant(
    WORD    *pMlength,
    BYTE    *pParamField);

extern enum eQCWWANError UpkQmiWdsGoDormant(
    BYTE   *pMdmResp,
    struct QmiWdsGoDormantResp *pApiResp);

#endif /* __QA_WDS_SLQS_WDS_GO_DORMANT_H__ */
