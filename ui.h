#ifndef IMPL
#pragma once
#endif

#include "types.h"

#ifdef CORE
#define fn(f) (*f)

#ifdef IMPL
#define QUAL
#else
#define QUAL extern
#endif

#else
#define fn(f) f
#define QUAL
#endif


QUAL void fn(init_ui)(void);    // boot up and loading everything needed
QUAL void fn(close_ui)(void);   // close everything and free space if allocated
QUAL void fn(refresh_ui)(void); // new frame signal

QUAL const char* fn(get_ui_info)(void); // infos about the ui in a string

QUAL void fn(set_ui_map)(const map_chunk_t* p_map_chunk);                    // change the map to draw (pointer stored in ui)
QUAL void fn(set_ui_entity_stack)(const entity_t* p_buffer, uint16_t count); // change the entity buffer pointer on the ui, first element is player
QUAL void fn(set_ui_input_queue)(input_queue_t* input_queue);                // change the map to draw (pointer stored in ui)




















