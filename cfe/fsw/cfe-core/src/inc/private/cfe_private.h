/*
 * cfe_private.h
 *
 *  Created on: Dec 3, 2013
 *      Author: jphickey
 */

#ifndef CFE_PRIVATE_H_
#define CFE_PRIVATE_H_

#include "common_types.h"
#include "cfe.h"

#include "cfe_platform_cfg.h" /* Define platform configuration parameters */


/*****************************************************************************/
/** cFE Core task entry point prototypes *************************************/
/*****************************************************************************/
/*
 * All cFE core apps should define an entry point and the prototype should be here
 * These may also be called from the unit test
 */

/*****************************************************************************/
/**
** \brief Entry Point for cFE Core Application
**
** \par Description
**        This is the entry point to the cFE TIME Core Application.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval None
******************************************************************************/
extern void CFE_TIME_TaskMain(void);

/*****************************************************************************/
/**
** \brief Entry Point for cFE Core Application
**
** \par Description
**        This is the entry point to the cFE SB Core Application.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval None
******************************************************************************/
extern void CFE_SB_TaskMain(void);

/*****************************************************************************/
/**
** \brief Entry Point for cFE Core Application
**
** \par Description
**        This is the entry point to the cFE EVS Core Application.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval None
******************************************************************************/
extern void CFE_EVS_TaskMain(void);

/*****************************************************************************/
/**
** \brief Entry Point for cFE Core Application
**
** \par Description
**        This is the entry point to the cFE ES Core Application.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval None
******************************************************************************/
extern void CFE_ES_TaskMain(void);

/*****************************************************************************/
/**
** \brief Entry Point for cFE Table Services Core Application
**
** \par Description
**        This is the entry point to the cFE Table Services Core Application.
**        This Application provides the ground interface to the cFE Table
**        Services.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval None
******************************************************************************/
extern void CFE_TBL_TaskMain(void);




/*****************************************************************************/
/** cFE Core task early init prototypes **************************************/
/*****************************************************************************/
/*
 * cFE core apps may define an early init function and the prototype should be here
 * These may also be called from the unit test
 */

/*****************************************************************************/
/**
** \brief Initializes the cFE core module API Library
**
** \par Description
**        Initializes the cFE core module API Library
**
** \par Assumptions, External Events, and Notes:
**        -# This function MUST be called before any module API's are called.
**
******************************************************************************/
extern int32 CFE_EVS_EarlyInit(void);

/*****************************************************************************/
/**
** \brief Initializes the cFE core module API Library
**
** \par Description
**        Initializes the cFE core module API Library
**
** \par Assumptions, External Events, and Notes:
**        -# This function MUST be called before any module API's are called.
**
******************************************************************************/
extern int32 CFE_SB_EarlyInit(void);

/*****************************************************************************/
/**
** \brief Initializes the cFE core module API Library
**
** \par Description
**        Initializes the cFE core module API Library
**
** \par Assumptions, External Events, and Notes:
**        -# This function MUST be called before any module API's are called.
**
******************************************************************************/
extern int32 CFE_TIME_EarlyInit(void);

/*****************************************************************************/
/**
** \brief Initializes the Table Services API Library
**
** \par Description
**        Initializes the Table Services API Library
**
** \par Assumptions, External Events, and Notes:
**        -# This function MUST be called before any TBL API's are called.
**
******************************************************************************/
extern int32 CFE_TBL_EarlyInit(void);

/*****************************************************************************/
/**
** \brief Initializes the cFE core module API Library
**
** \par Description
**        Initializes the cFE core module API Library
**
** \par Assumptions, External Events, and Notes:
**        -# This function MUST be called before any module API's are called.
**
******************************************************************************/
extern int32 CFE_ES_CDS_EarlyInit(void);

/*****************************************************************************/
/**
** \brief Initializes the cFE core module API Library
**
** \par Description
**        Initializes the cFE core module API Library
**
** \par Assumptions, External Events, and Notes:
**        -# This function MUST be called before any module API's are called.
**
******************************************************************************/
extern int32 CFE_FS_EarlyInit(void);



/*****************************************************************************/
/** cFE Core task clean up prototypes ****************************************/
/*****************************************************************************/
/*
 * cFE core apps may define a clean up function and the prototype should be here
 * These may also be called from the unit test
 */
/*****************************************************************************/
/**
** \brief Removes TBL resources associated with specified Application
**
** \par Description
**        This function is called by cFE Executive Services to cleanup after
**        an Application has been terminated.  It frees TBL services resources
**        that have been allocated to the specified Application.
**
** \par Assumptions, External Events, and Notes:
**        -# This function DOES NOT remove any critical tables associated with
**           the specified application from the Critical Data Store.
**
******************************************************************************/
extern int32 CFE_TBL_CleanUpApp(uint32 AppId);

/*****************************************************************************/
/**
** \brief Removes SB resources associated with specified Application
**
** \par Description
**        This function is called by cFE Executive Services to cleanup after
**        an Application has been terminated.  It frees resources
**        that have been allocated to the specified Application.
**
******************************************************************************/
extern int32 CFE_SB_CleanUpApp(uint32 AppId);

/*****************************************************************************/
/**
** \brief Removes EVS resources associated with specified Application
**
** \par Description
**        This function is called by cFE Executive Services to cleanup after
**        an Application has been terminated.  It frees resources
**        that have been allocated to the specified Application.
**
******************************************************************************/
extern int32 CFE_EVS_CleanUpApp(uint32 AppId);

/*****************************************************************************/
/**
** \brief Removes TIME resources associated with specified Application
**
** \par Description
**        This function is called by cFE Executive Services to cleanup after
**        an Application has been terminated.  It frees resources
**        that have been allocated to the specified Application.
**
******************************************************************************/
extern int32 CFE_TIME_CleanUpApp(uint32 AppId);


/*****************************************************************************/
/** cFE Core task other function call prototypes *****************************/
/*****************************************************************************/
/*
 * Functions that are used in a cFE core module other than the module that
 * actually defines the function should be prototyped here.
 *
 * Including the prototype here allows the function to be called properly
 * without redefining it at the point of use.
 */

/*****************************************************************************/
/**
** \brief Reserve space (or re-obtain previously reserved space) in the Critical Data Store (CDS)
**
** \par Description
**        This routine is identical to #CFE_ES_RegisterCDS except it identifies the contents
**        of the CDS as a critical table.  This is crucial because a critical table CDS must
**        only be deleted by cFE Table Services, not via an ES delete CDS command.  Otherwise,
**        Table Services may be out of sync with the contents of the CDS.
**
** \par Assumptions, External Events, and Notes:
**        -# This function assumes input parameters are error free and have met size/value restrictions.
**        -# The calling function is responsible for issuing any event messages associated with errors.
**
** \param[in]   HandlePtr   Pointer Application's variable that will contain the CDS Memory Block Handle.
**
** \param[in]   BlockSize   The number of bytes needed in the CDS.
**
** \param[in]   Name        Pointer to character string containing the Application's local name for
**                          the CDS.
**
** \param[in]   CriticalTbl   Indicates whether the CDS is to be used as a Critical Table or not
**
** \param[out]  *HandlePtr  The handle of the CDS block that can be used in #CFE_ES_CopyToCDS and #CFE_ES_RestoreFromCDS.
**
** \return See return codes for #CFE_ES_RegisterCDS
**
******************************************************************************/
int32 CFE_ES_RegisterCDSEx(CFE_ES_CDSHandle_t *HandlePtr, int32 BlockSize, const char *Name, boolean CriticalTbl);

/*****************************************************************************/
/**
** \brief Deletes the specified CDS from the CDS Registry and frees CDS Memory
**
** \par Description
**        Removes the record of the specified CDS from the CDS Registry and
**        frees the associated CDS memory for future use.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  CDSName - Pointer to character string containing complete
**                       CDS Name (of the format "AppName.CDSName").
**
** \param[in]  CalledByTblServices - Flag that identifies whether the CDS is supposed to
**                       be a Critical Table Image or not.
**
** \return #CFE_SUCCESS                     \copydoc CFE_SUCCESS
** \return #CFE_ES_CDS_WRONG_TYPE_ERR       \copydoc CFE_ES_CDS_WRONG_TYPE_ERR
** \return #CFE_ES_CDS_OWNER_ACTIVE_ERR     \copydoc CFE_ES_CDS_OWNER_ACTIVE_ERR
** \return #CFE_ES_CDS_NOT_FOUND_ERR        \copydoc CFE_ES_CDS_NOT_FOUND_ERR
** \return Any of the return values from #CFE_ES_UpdateCDSRegistry
** \return Any of the return values from #CFE_ES_PutCDSBlock
**
******************************************************************************/
int32  CFE_ES_DeleteCDS(const char *CDSName, boolean CalledByTblServices);




#endif /* CFE_PRIVATE_H_ */
