#ifdef __c_plus_plus
extern "C" {
#endif

#ifndef IMPL
#pragma once
#endif

#include "types.h"

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
#define QUAL extern     // no static name is needed elsewhere
#define WINDOW window_t // made to avoid name conflict
#endif

QUAL void fn(init_ui)(void);    // boot up and loading everything needed
QUAL void fn(close_ui)(void);   // close everything and free space if allocated
QUAL void fn(refresh_ui)(void); // new frame signal

QUAL const char *fn(get_ui_info)(void); // infos about the ui in a string

QUAL void fn(set_ui_map)(
    const map_chunk_t
        *p_map_chunk); // change the map to draw (pointer stored in ui)
QUAL void fn(set_ui_entity_stack)(
    const entity_t *p_buffer,
    uint16_t count); // change the entity buffer pointer on the ui, first
                     // element is player
QUAL void fn(set_ui_input_queue)(
    input_queue_t
        *input_queue); // change the map to draw (pointer stored in ui)

struct WINDOW typedef WINDOW;

QUAL WINDOW *fn(newwin_ui)(uint32_t height, uint32_t with);
QUAL void fn(delwin_ui)(WINDOW *win);
QUAL void fn(resizewin_ui)(WINDOW *win, uint32_t new_height,
                           uint32_t new_width);
QUAL void fn(addsubwin_ui)(WINDOW *root, WINDOW *subwin, uint32_t y,
                           uint32_t x);
QUAL void fn(movesubwin_ui)(WINDOW *root, WINDOW *subwin, uint32_t new_y,
                            uint32_t new_x);
QUAL void fn(rmsubwin_ui)(WINDOW *root, WINDOW *subwin);
QUAL void fn(getwinyx)(WINDOW *win, uint32_t *y, uint32_t *x);
QUAL void fn(getwinhw)(WINDOW *win, uint32_t *height, uint32_t *width);

#undef fn
#undef QUAL
#undef WINDOW

#ifdef __c_plus_plus
}
#endif
