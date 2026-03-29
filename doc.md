# general documentation

## global types
### terrain_t (struct)
representation of a map tile
- uint8_t floor: floor type
- uint8_t roof: roof type
- uint8_t obstacle: obstacle type
- int8_t temperature: didn't knwo what to put here

### map_t (struct)
terrain container
- terrain_t **terrain: pointers to the tiles
- uint16_t height: number of pointers
- uint16_t width: length of the data in the pointers

### entity_t (struct)
bunch of random crap
- uint16_t hp:
- uint16_t stamina:
- int16_t x:
- int16_t y:
- uint16_t type:

### input_queue_t (struct)
queue with loop back by overflow
currently not in use
- uint8_t data[256]:
- uint8_t in:
- uint8_t out:

### enum window_type_e : uint8_t
- represents window types, names are quite explicits
dgb

### WINDOW*
opaque struct that represent ui windows

### enum input_code_e : uint8_t
- numbers used for intercation between the main file and the ui

### window_input_callback_pfn:
 - typedef for void (*)(WINDOW *const, enum input_code_e const,void *data)
callback called when the window recieves inputs

## functions

### init_ui
void init_ui(void)
boot up and load everything needed

### close_ui
void close_ui(void)
close everything and delete buffers

### refresh_ui
void refresh_ui(void)
force refresh ui

### log_ui_info
void log_ui_info(void)
log infos about the ui in stderr

### newwin_ui
WINDOW \*newwin_ui(window_create_info_t const * const)
ask for a new window, in case it fails return NULL and log debug infos

### delwin_ui
void delwin_ui(WINDOW \*win)
delete the window

### resizewin_ui
void resizewin_ui(WINDOW \*win, uint32_t new_height, uint32_t new_width)
explicitly ask a new size for the window (may be ignored)

### addsubwin_ui
void addsubwin_ui(WINDOW \*root, WINDOW \*subwin, uint32_t y, uint32_t x)
set a window as subwindow of another one

### rmsubwin_ui
void rmsubwin_ui(WINDOW \*root, WINDOW \*subwin)
unbinds subwindow from root window

### getwinyx_ui
void getwinyx_ui(const WINDOW \*win, uint32_t \*y, uint32_t \*x)

### getwinhw_ui
void getwinhw_ui(const WINDOW \*win, uint32_t \*height, uint32_t \*width)

### bdbfwin_ui
void \*bdbfwin_ui(WINDOW \*win, void \*new_buffer)
change the buffer atached to the window, return the former one

### bdwininpclbk_ui
void bdwininpclbk_ui(WINDOW \*win, window_input_callback_pfn clbk)
set or change the input callback

## global variables
