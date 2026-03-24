#ifdef __c_plus_plus
extern "C" {
#endif

#ifndef IMPL
#pragma once
#endif

#ifdef CORE
#define fn(f) (*f)
#define WINDOW WINDOW

#ifdef IMPL
#define QUAL
#else
#define QUAL extern
#endif

#else /*! CORE */
#define fn(f) f
#define QUAL            // no static name is needed elsewhere
#define WINDOW window_t // made to avoid name conflict
#endif

#include "types.h"

QUAL void fn(init_ui)(void);    // boot up and loading everything needed
QUAL void fn(close_ui)(void);   // close everything and free space if allocated
QUAL void fn(refresh_ui)(void); // new frame signal

QUAL void fn(log_ui_info)(void); // infos about the ui in a string

QUAL WINDOW *fn(newwin_ui)(window_create_info_t const *const);

QUAL void fn(delwin_ui)(WINDOW *win);

QUAL void fn(resizewin_ui)(WINDOW *win, uint32_t new_height,
                           uint32_t new_width);

QUAL void fn(addsubwin_ui)(WINDOW *root, WINDOW *subwin, uint32_t y,
                           uint32_t x);

QUAL void fn(rmsubwin_ui)(WINDOW *root, WINDOW *subwin);

QUAL void fn(getwinyx_ui)(const WINDOW *win, uint32_t *y, uint32_t *x);

QUAL void fn(getwinhw_ui)(const WINDOW *win, uint32_t *height, uint32_t *width);

QUAL void *fn(bdbfwin_ui)(WINDOW *win, void *new_buffer);

QUAL void fn(bdwininpclbk_ui)(WINDOW *win,
                              window_input_callback_pfn pfnCallback);

#undef fn
#undef QUAL
#undef WINDOW

#ifdef __c_plus_plus
}
#endif
