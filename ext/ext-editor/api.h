// -----------------------------------------------------------------------------
// in-game editor
// - rlyeh, public domain.

// current obj impl provides 19 available flags starting from bit-19
#define IS_SELECTED  (1ull<<0)
#define HAS_CHILDREN (1ull<<1) // whether this object has children or not
#define IS_OPEN      (1ull<<2) // whether this object is being open in editor tree or not
#define HAS_TICK     (1ull<<3)
#define IS_TICKING   (1ull<<4)
#define HAS_DRAW     (1ull<<5)
#define IS_DRAWING   (1ull<<6)
#define HAS_VIEW     (1ull<<7)
#define IS_COMPONENT (1ull<<8) // whether this object acts as a component to its parent's eyes
//#define IS_CHANGED   (1ull<<2)
//#define IS_CACHED    (1ull<<X) // whether objects will be cached to ram
//#define IS_PERSIST   (1ull<<X) // whether objects will be saved to disk
//#define IS_BREAK     breakpoint: request to break on any given node
//#define IS_DEBUG     debug: request to monitor/isolate any specific node

// attaches X to obj, and marks X as a component
#define obj_attach_component(obj,x) ( obj_attach((obj),(x)), obj_flag((x),|=,IS_COMPONENT), (x) )

#define HAS_EDITOR \
    ( optioni("--editor", flag("--editor")) || strstr(argvcmdline(), "editor") )

#include "editor_bindings2.h"
#include "editor_gizmo2.h"
#include "editor_pick2.h"
#include "editor_ui2.h"
#include "editor_ui_toolbar2.h"
#include "editor_ui_controlbar2.h"
#include "editor_ui_tree2.h"
#include "editor_events2.h"
#include "editor_groups2.h"
#include "editor_label2.h"
#include "editor_fps2.h"
#include "editor_prop2.h"
#include "editor_sub2.h"
#include "editor_sub_browser2.h"
#include "editor_sub_console2.h"
#include "editor_sub_text2.h"

// #include "demo_types2.h"

API void igAssetsBrowser(unsigned library); // 0:all, 1:cache

#include "editor_obj2.h"
