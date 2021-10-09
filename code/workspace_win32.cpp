
// #ifdef	STD_INCLUDE_DECL
// #endif	// STD_INCLUDE_DECL
// #ifdef	STD_INCLUDE_FUNC
// #endif	// STD_INCLUDE_FUNC

#include <math.h> // sqrtf, cosf, sinf
#include <stdint.h> // type definitions
#include <stdlib.h> // rand
#include <float.h> // FLT_MAX
#include <time.h> // time
#include <cstdio> // sprintf
#include <windows.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "typeDef.h"
#include "math.cpp"

#define 	STD_INCLUDE_DECL
#include "vector.cpp"
#include "memory.cpp"
#include "font.cpp"
#include "userInput.cpp"
#undef	STD_INCLUDE_DECL

#define	STD_INCLUDE_FUNC
#include "vector.cpp"
#include "memory.cpp"
#include "font.cpp"
#include "userInput.cpp"
#undef	STD_INCLUDE_FUNC

#include "d3d12.cpp"
#include "win32_platform.h"

#define APP_targetFPS  ( 60.0f )

#include <dsound.h>
#include "audioSystem.cpp"

#include "fish_config.h"

#include "entity.h"
#include "fish.h"

#include "debug_win32.cpp"

#include "intersect.cpp"
#include "tools.cpp"
#include "camera.cpp"
#include "item.cpp"
#include "player.cpp"
#include "rainbow.cpp"
#include "goldfish.cpp"
#include "dogfish.cpp"
#include "swordfish.cpp"
#include "urchinSpine.cpp"
#include "urchin.cpp"
#include "bubblet.cpp"
#include "archer.cpp"
#include "bubble.cpp"
#include "lgarcher.cpp"
#include "bubblon.cpp"
#include "breakable.cpp"
#include "parrotfish.cpp"
#include "turtle.cpp"
#include "smturtle.cpp"
#include "cage.cpp"
#include "net.cpp"
#include "defendZone.cpp"
#include "heal.cpp"
#include "tower.cpp"
#include "obstacle.cpp"
#include "background.cpp"
#include "eventTrigger.cpp"
#include "animationSystem.cpp"
#include "worldSystem.cpp"
#include "gameControl.cpp"
#include "gameplay.cpp"
#include "home.cpp"

#include "bmp.cpp"
#include "wav.cpp"

#include "debugFrameSystem.cpp"

#include "editor.cpp"
#include "fish.cpp"
#include "win32_platform.cpp"