/********************** BEGIN LICENSE BLOCK ************************************
 *
 * JZ4740  mobile_tv  Project  V1.0.0
 * INGENIC CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Copyright (c) Ingenic Semiconductor Co. Ltd 2005. All rights reserved.
 * 
 * This file, and the files included with this file, is distributed and made 
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
 * 
 * http://www.ingenic.cn 
 *
 ********************** END LICENSE BLOCK **************************************
 *
 *  Author:  <dsqiu@ingenic.cn>  <jgao@ingenic.cn> 
 *
 *  Create:   2008-06-26, by dsqiu
 *            
 *  Maintain: 2008-06-26, by jgao
 *            
 *
 *******************************************************************************
 */
 


#ifndef __FRMWRK_H__
#define __FRMWRK_H__

#include "sysdefs.h"

/* ********************************************************************* */
/* Module configuration */

#ifndef _SYS_STKSIZE
#define _SYS_STKSIZE 20*1024
#endif
#ifndef _EXC_STKSIZ
#define _EXC_STKSIZE 20*1024
#endif


/* ********************************************************************* */
/* Interface macro & data definition */

#define C_abRTMem

/* ********************************************************************* */

#ifdef __cplusplus
}
#endif

#endif /*__FRMWRK_H__*/

