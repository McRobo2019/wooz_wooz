/******************************************************************************
 *  app.h (for LEGO Mindstorms EV3)
 *  Created on: 2015/01/25
 *  Definition of the Task main_task
 *  Author: Kazuhiro.Kawachi
 *  Copyright (c) 2015 Embedded Technology Software Design Robot Contest
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "ev3api.h"

/*
 *  各タスクの優先度の定義
 */
#define MAIN_PRI    (TMIN_APP_TPRI + 2)  /* メインタスクの優先度 */
#define TRACER_PRI  (TMIN_APP_TPRI + 1)
#define LOGGER_PRI  (TMIN_APP_TPRI + 3)

/*
 *  ターゲットに依存する可能性のある定数の定義
 */
#ifndef STACK_SIZE
#define STACK_SIZE (4096)        /* タスクのスタックサイズ */
#endif /* STACK_SIZE */

/*
 *  関数のプロトタイプ宣言
 */
#ifndef TOPPERS_MACRO_ONLY

extern void main_task(intptr_t exinf);
extern void tracer_task(intptr_t exinf);
extern void logger_task(intptr_t exinf);
extern void ev3_cyc_tracer(intptr_t exinf);

#endif /* TOPPERS_MACRO_ONLY */

#ifdef __cplusplus
}
#endif
