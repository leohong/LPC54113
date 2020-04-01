#ifndef COMMONDEF_H
#define COMMONDEF_H
// ==============================================================================
// FILE NAME: COMMONDEF.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 15/03/2013, Leohong written
// --------------------
// ==============================================================================


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdarg.h>
#include "Platform.h"
#include "TaskConfig.h"

#define __DEBUG__
#define DEBUG_ENABLE

//#define CLI_CALIBRE 1
//#define CLI_OPTOMA 2
//#define ACTIVE_CLI_PROTOCOL CLI_OPTOMA
//#define ACTIVE_CLI_PROTOCOL CLI_CALIBRE

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#endif // COMMONDEF_H
