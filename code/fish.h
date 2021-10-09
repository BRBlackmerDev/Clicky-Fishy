
typedef DIRECTX_12_RENDERER    RENDERER;
typedef DIRECTX_12_RENDER_PASS RENDER_PASS;

struct PLATFORM {
    boo32 isRunning;
    
    flo32 targetSec;
    
    KEYBOARD_STATE keyboard;
    MOUSE_STATE    mouse;
    
    MEMORY permMemory;
    MEMORY tempMemory;
    
    AUDIO_SYSTEM audioSystem;
    
    RENDERER renderer;
    
    READ_FILE  * readFile;
    WRITE_FILE * writeFile;
};

//----------
// GAME CONTROL
//----------

struct GAME_CONTROL__TURTLE {
    boo32 enable;
    flo32 timeBetweenSpawn_lo;
    flo32 timeBetweenSpawn_hi;
    boo32 firstAppearOnRight;
    flo32 firstAppearAtY;
};

internal GAME_CONTROL__TURTLE
GameControl_turtle( flo32 timeBetweenSpawn_lo, flo32 timeBetweenSpawn_hi, boo32 firstAppearOnRight, flo32 firstAppearAtY ) {
    GAME_CONTROL__TURTLE result = {};
    result.enable = true;
    result.timeBetweenSpawn_lo = timeBetweenSpawn_lo;
    result.timeBetweenSpawn_hi = timeBetweenSpawn_hi;
    result.firstAppearOnRight  = firstAppearOnRight;
    result.firstAppearAtY      = firstAppearAtY;
    return result;
}

enum GAME_CONTROL__EVENT_TYPE {
    eventType_holdUntil,
    eventType_spawnEnemies,
    eventType_animation,
    eventType_winGame,
    eventType_exitRainbow,
    eventType_setCheckpoint,
    eventType_moveCamera,
    eventType_respawnEnemies,
    eventType_exitEnemies,
    eventType_endAnimation,
    eventType_spawnIfNeeded,
    eventType_killChain,
    eventType_scrollingSpawn,
    
    eventType_enableCameraSpring,
    eventType_disableCameraSpring,
    eventType_setCameraBound,
    
    eventType_spawn_relativeToBasePos,
    
    eventType_unloadObstacle,
    eventType_enableMovementBound,
    
    eventType_killBubbles,
    eventType_killItems,
    
    eventType_dummy,
    eventType_beginGame,
};

enum GAME_CONTROL__EXIT_CONDITION {
    exitCondition_instant,
    exitCondition_noEnemies,
    exitCondition_noAnimation,
    exitCondition_exitBound,
    exitCondition_targetTime,
    
    exitCondition_loadTrigger,
    
    exitCondition_noNets,
    exitCondition_netsAtHPOrLower,
    
    exitCondition_noCages,
    
    exitCondition_noTowers,
    exitCondition_towersAtHPOrLower,
    
    exitCondition_noHeal,
    
    exitCondition_none, // TODO: temp!
};

enum GAME_CONTROL__SPAWN_TYPE {
    spawnType_background,
    spawnType_breakable,
    spawnType_bubble,
    spawnType_cage,
    spawnType_item,
    spawnType_lgArcher,
    spawnType_net,
    spawnType_obstacle,
    spawnType_parrotfish,
    spawnType_trigger,
    
    spawnType_end,
};

struct GAME_CONTROL__SPAWN {
    uint32 nRainbow;
    uint32 nGoldfish;
    uint32 nDogfish;
    uint32 nSwordfish;
    uint32 nUrchin;
    uint32 nArcher;
    uint32 nlgArcher;
    uint32 nParrotfish;
    uint32 nsmTurtle;
    
    boo32 enableBubblonGen;
    
    GAME_CONTROL__TURTLE turtle;
    
#define GAME_CONTROL__CAGE_MAX_COUNT  ( 16 )
    uint32 nCage;
    vec2    cage_position[ GAME_CONTROL__CAGE_MAX_COUNT ];
    
#define GAME_CONTROL__NET_MAX_COUNT  ( 16 )
    uint32 nNet;
    vec2    net_position[ GAME_CONTROL__NET_MAX_COUNT ];
    
#define GAME_CONTROL__TOWER_MAX_COUNT  ( 8 )
    uint32 nTower;
    rect   tower_bound[ GAME_CONTROL__TOWER_MAX_COUNT ];
    
#define GAME_CONTROL__HEAL_MAX_COUNT  ( 4 )
    uint32 nHeal;
    rect   heal_bound[ GAME_CONTROL__HEAL_MAX_COUNT ];
    
    // TODO: i should store any spawn data in a separate buffer and just store a pointer to the range
#define GAME_CONTROL__OBSTACLE_MAX_COUNT  ( 256 )
    uint32 nObstacle;
    OBSTACLE obstacle[ GAME_CONTROL__OBSTACLE_MAX_COUNT ];
    
    DEFEND_ZONE_STATE defendZone;
};

struct GAME_CONTROL__EVENT {
    int32 chainID;
    boo32 shouldKill;
    
    int32  eventIndex;
    GAME_CONTROL__EVENT_TYPE type;
    uint32 dataOffset;
    
    flo32 timer;
    union {
        flo32 targetTime;
        flo32 atHP; // NOTE: percent
        int32 chainIDToKill;
    };
    
    GAME_CONTROL__EXIT_CONDITION exit; // TODO: This might not be necessary.
    int32  nextEventIndex;
    int32  subEventIndex;
};

struct GAME_CONTROL_STATE {
#define GAME_CONTROL__CHAIN_MAX_COUNT  ( 4 )
    uint32 nChain;
    GAME_CONTROL__EVENT chain[ GAME_CONTROL__CHAIN_MAX_COUNT ];
    
    int32 chainID;
    int32 prevEventIndex;// NOTE: Only used during event creation in game reset! Do NOT use during the game!
    int32 tempValueForSubChain;
    vec2  world_basePos;
    flo32 world_scale;
    
    boo32 camera_springIsEnabled;
    rect  camera_bound;
    
    uint32 atGroupID;
    uint32 firstEvent_index;
    vec2   firstEvent_pos;
    boo32  firstEvent_springIsEnabled;
    rect   firstEvent_cameraBound;
    vec2   firstEvent_basePos;
    
#define GAME_CONTROL__EVENT_MAX_COUNT  ( 256 )
    uint32             nEvent;
    GAME_CONTROL__EVENT event[ GAME_CONTROL__EVENT_MAX_COUNT ];
    MEMORY              memory;
};

//----------
// STRING BUFFER
//----------

struct STRING_BUFFER {
    uint32 nStr;
    uint32 nStrMax;
    char ** str;
    MEMORY  memory;
};

internal STRING_BUFFER
StringBuffer( MEMORY * parent_memory, uint32 maxString, uint32 maxSize ) {
    STRING_BUFFER result = {};
    result.str     = _pushArray_clear( parent_memory, char *, maxString );
    result.memory  = subArena( parent_memory, maxSize );
    result.nStrMax = maxString;
    return result;
}

internal char *
add( STRING_BUFFER * s, char * string ) {
    Assert( s->nStr < s->nStrMax );
    
    uint32 length = ( uint32 )strlen( string );
    char * dest = ( char * )_pushSize( &s->memory, length + 1 );
    strcpy( dest, string );
    
    s->str[ s->nStr++ ] = dest;
    
    return dest;
}

//----------
// ANIMATION
//----------

enum KEYFRAME_TYPE {
    keyframeType_sprite,
    keyframeType_text,
};

struct ANIMATION_KEYFRAME {
    boo32 isVisible;
    KEYFRAME_TYPE type;
    flo32 targetTime;
    vec2  position;
    vec2  scale;
    flo32 radians;
    vec4  modColor;
    
    union {
        struct {
            TEXTURE_ID textureID;
            uint32 padding[ 2 ];
        };
        struct {
            const char *   str;
            TEXT_ALIGNMENT align;
        };
    };
};

enum ANIMATION_EVENT_TYPE {
    animEventType_audio,
    animEventType_cameraShake,
    animEventType_animShake,
};

struct ANIMATION_EVENT {
    ANIMATION_EVENT_TYPE type;
    flo32                targetTime;
    
    union {
        struct {
            AUDIO_ID audioID;
            flo32    volume;
            flo32    padding[ 4 ];
        };
        struct {
            flo32 cam_upTime;
            flo32 cam_holdTime;
            flo32 cam_downTime;
            flo32 cam_frequency;
            vec2  cam_offsetInPixels;
        };
    };
};

struct ANIMATION {
    boo32  shouldLoop;
    boo32  isCameraRel;
    uint32 bSubAnimation;
    uint32 nSubAnimation;
    uint32 bEvent;
    uint32 nEvent;
};

struct SUBANIMATION {
    uint32 bKeyframe;
    uint32 nKeyframe;
};

struct ACTIVE_SUBANIMATION {
    uint32 firstKeyframe;
    uint32 atKeyframe;
    uint32 lastKeyframe;
    
    ANIMATION_KEYFRAME keyframeA;
    ANIMATION_KEYFRAME keyframeB;
};

struct CAMERA_SHAKE {
    vec2 P;
    vec2 Q;
    
    flo32 timer;
    flo32 upTime;
    flo32 holdTime;
    flo32 downTime;
    
    flo32 t;
    flo32 targetT;
    
    flo32 strength;
    vec2  offset;
};

#define ANIMATION_SYSTEM__ACTIVE_SUBANIMATION_COUNT  ( 8 )
struct ACTIVE_ANIMATION {
    boo32 isActive;
    flo32 timer;
    boo32 shouldLoop;
    boo32 isCameraRel;
    
    boo32        shake_isActive;
    CAMERA_SHAKE shake;
    
    uint32             nSubAnim;
    ACTIVE_SUBANIMATION subAnim[ ANIMATION_SYSTEM__ACTIVE_SUBANIMATION_COUNT ];
    
    uint32 firstEvent;
    uint32 atEvent;
    uint32 lastEvent;
};

#define ANIMATION_SYSTEM__ACTIVE_ANIMATION_MAX_COUNT  ( 8 )
#define ANIMATION_SYSTEM__ANIMATION_MAX_COUNT     ( 128 )
#define ANIMATION_SYSTEM__SUBANIMATION_MAX_COUNT  ( 128 )
#define ANIMATION_SYSTEM__KEYFRAME_MAX_COUNT   ( 1024 )
#define ANIMATION_SYSTEM__EVENT_MAX_COUNT      ( 1024 )
struct ANIMATION_SYSTEM {
    uint32          nAnim;
    ACTIVE_ANIMATION anim[ ANIMATION_SYSTEM__ACTIVE_ANIMATION_MAX_COUNT ];
    
    ANIMATION    * curr_animation;
    SUBANIMATION * curr_subanimation;
    
    uint32 nAnimation;
    uint32 nSubAnimation;
    uint32 nKeyframe;
    uint32 nEvent;
    ANIMATION          animation   [ ANIMATION_SYSTEM__ANIMATION_MAX_COUNT ];
    SUBANIMATION       subanimation[ ANIMATION_SYSTEM__SUBANIMATION_MAX_COUNT ];
    ANIMATION_KEYFRAME keyframe    [ ANIMATION_SYSTEM__KEYFRAME_MAX_COUNT  ];
    ANIMATION_EVENT    event       [ ANIMATION_SYSTEM__EVENT_MAX_COUNT     ];
    
    STRING_BUFFER stringBuffer;
};

//----------
// WORLD SYSTEM
//----------

#define WORLD_SYSTEM__CELL_WIDTH   ( 8192.0f )
#define WORLD_SYSTEM__CELL_HEIGHT  ( 8192.0f )

#define WORLD_SYSTEM__OBSTACLE_MAX_COUNT          ( 2048 )
#define WORLD_SYSTEM__OBSTACLE_GROUP_MAX_COUNT    (  512 )
#define WORLD_SYSTEM__BACKGROUND_MAX_COUNT        ( 2048 )
#define WORLD_SYSTEM__BACKGROUND_GROUP_MAX_COUNT  (  512 )
struct WORLD_SYSTEM {
    vec2   grid_offset;
    uint32 grid_maxX;
    uint32 grid_maxY;
    
    uint32 camera_cellID;
    
    uint32  nObs;
    OBSTACLE obs[ WORLD_SYSTEM__OBSTACLE_MAX_COUNT ];
    
    uint32        nObsGroup;
    OBSTACLE_GROUP obsGroup[ WORLD_SYSTEM__OBSTACLE_GROUP_MAX_COUNT ];
    
    uint32    nBack;
    BACKGROUND back[ WORLD_SYSTEM__BACKGROUND_MAX_COUNT ];
    
    uint32          nBackGroup;
    BACKGROUND_GROUP backGroup[ WORLD_SYSTEM__BACKGROUND_GROUP_MAX_COUNT ];
};

#if 0
//----------
// WAVE
//----------

struct WAVE_CAGE {
    vec2 position;
    vec2 halfDim;
};

internal WAVE_CAGE
WaveCage( vec2 position, vec2 halfDim ) {
    WAVE_CAGE result = {};
    result.position = position;
    result.halfDim  = halfDim;
    return result;
}

enum WAVE_SUCCESS {
    waveSuccess_killAllEnemies,
    waveSuccess_surviveTime,
    waveSuccess_defendZone,
};

struct WAVE {
    boo32 isSpawned;
    
    WAVE_SUCCESS exit_success;
    rect         exit_bound;
    
    boo32  hasSchool; // temp
    uint32 nRainbow;
    uint32 nGoldfish;
    uint32 nDogfish;
    uint32 nSwordfish;
    uint32 nUrchin;
    uint32 nArcher;
    uint32 nlgArcher;
    
    boo32 enableBubblonGen;
    boo32 disableBubblonGenAtEnd;
    boo32 enableTurtle;
    boo32 disableTurtleAtEnd;
    
#define WAVE__CAGE_MAX_COUNT  ( 8 )
    uint32   nCage;
    WAVE_CAGE cage[ WAVE__CAGE_MAX_COUNT ];
    
    DEFEND_ZONE_STATE defendZone;
};

#define WAVE_MAX_COUNT  ( 32 )
struct WAVE_STATE {
    boo32 isSpawned;
    boo32 exit_isActive;
    flo32 exit_timer;
    
    uint32 atWave;
    uint32 nWaves;
    WAVE   wave[ WAVE_MAX_COUNT ];
    flo32  wave_timer;
};

internal WAVE
Wave( uint32 nRainbow, uint32 nGoldfish, uint32 nDogfish, boo32 hasSchool = false ) {
    WAVE wave = {};
    wave.hasSchool = hasSchool;
    wave.nRainbow  = nRainbow;
    wave.nGoldfish = nGoldfish;
    wave.nDogfish  = nDogfish;
    return wave;
}
#endif

struct FISH_ATE {
    FISH_TYPE type;
    vec2      position;
    COLOR_ID  colorID;
    uint32    points;
    uint32    health;
    flo32     timer;
};

#define FISH_ATE_LIST_COUNT  ( 64 )
struct FISH_ATE_LIST {
    boo32  attemptBite;
    uint32 nFishAte;
    uint32 nFishBit;
    FISH_ATE fishAte[ FISH_ATE_LIST_COUNT ];
    FISH_ATE fishBit[ FISH_ATE_LIST_COUNT ];
    uint32   nBit[ fishType_count ];
    uint32   nAte[ fishType_count ];
};

struct SCORE_PARTICLE {
    uint32 value;
    flo32  timer;
    vec2   position;
};

#define SCORE_PARTICLE_LIST__PARTICLE_COUNT  ( 128 )
struct SCORE_PARTICLE_LIST {
    uint32 nParticles;
    uint32 maxParticles;
    
    SCORE_PARTICLE particle[ SCORE_PARTICLE_LIST__PARTICLE_COUNT ];
};

internal void
addParticle( SCORE_PARTICLE_LIST * particleList, uint32 value, vec2 position ) {
    SCORE_PARTICLE particle = {};
    particle.value    = value;
    particle.position = position;
    
    Assert( particleList->nParticles < SCORE_PARTICLE_LIST__PARTICLE_COUNT );
    particleList->particle[ particleList->nParticles++ ] = particle;
}

struct EXPLOSION_PARTICLE {
    TEXTURE_ID textureID;
    vec2  position;
    vec2  drift;
    vec2  scale;
    flo32 radians;
    flo32 timer;
};

#define EXPLOSION_PARTICLE_LIST__PARTICLE_COUNT  ( 128 )
struct EXPLOSION_PARTICLE_LIST {
    uint32 nParticles;
    uint32 maxParticles;
    
    EXPLOSION_PARTICLE particle[ EXPLOSION_PARTICLE_LIST__PARTICLE_COUNT ];
};

struct SCORE_MULTIPLIER {
    uint32 value;
    flo32  timer;
};

struct COLOR_CHAIN {
    COLOR_ID color;
    uint32   count;
};

enum SECRET_ID {
    secretID__eat_3_or_more_fish_in_one_bite,
    secretID__eat_1_of_each_color_fish_in_a_row,
    secretID__eat_all_fish_of_one_color_during_school_wave,
    
    secretID__eat_a_goldfish_immediately_after_it_spawns,
    secretID__eat_a_second_goldfish_immediately_after_eating_one,
    secretID__snipe_a_goldfish,
    
    secretID__bite_two_or_more_dogfish_in_one_bite,
    secretID__dodge_20_dogfish_in_20_seconds,
    
    secretID_count,
};

struct SECRET_STATE {
    boo32 wasFound[ secretID_count ];
    
#define SECRET_STATE__SHOW_COUNT  ( 8 )
    uint32        nShow;
    flo32          show_timer;
    SECRET_ID show[ SECRET_STATE__SHOW_COUNT ];
    
    uint32   rainbowChain_nFishAte;
    uint32   rainbowChain_atIndex;
    COLOR_ID rainbowChain_color[ colorID_count ];
    
    boo32  school_isActive;
    uint32 school_nFishOfColor[ colorID_count ];
    uint32 school_nFishAte    [ colorID_count ];
    
    flo32 goldfish_secondTimer;
    flo32 goldfish_snipeTimer;
};

#if 0
struct ITEM_PANEL {
    rect bound;
    
    uint32 nSlots;
    uint32 nItem;
#define ITEM_PANEL_SLOT_COUNT  ( 4 )
    ITEM_ID item[ ITEM_PANEL_SLOT_COUNT ];
};

struct ITEM_TOOLBAR {
#define ITEM_TOOLBAR_ITEM_COUNT  ( 64 )
    uint32 nItem;
    ITEM item[ ITEM_TOOLBAR_ITEM_COUNT ];
};
#endif

struct DEBUG__LAST_BITE_FRAME__PLAYER {
    vec2  pos;
    boo32 facingLeft;
};

struct DEBUG__LAST_BITE_FRAME__FISH {
    FISH_TYPE type;
    vec2      pos;
    boo32     facingLeft;
    COLOR_ID  colorID;
    flo32     distToPlayerSq;
};

struct DEBUG__LAST_BITE_FRAME {
    DEBUG__LAST_BITE_FRAME__PLAYER player;
    
#define DEBUG__LAST_BITE_FRAME__FISH_COUNT  ( 8 )
    uint32                      nFish;
    DEBUG__LAST_BITE_FRAME__FISH fish[ DEBUG__LAST_BITE_FRAME__FISH_COUNT ];
};

#define DEBUG__LAST_BITE_FRAME_COUNT  ( 12 )
struct DEBUG__LAST_BITE {
    boo32  show;
    uint32 show_index;
    uint32 atIndex;
    DEBUG__LAST_BITE_FRAME frame[ DEBUG__LAST_BITE_FRAME_COUNT ];
};

#define DEBUG__SWORDFISH__FRAME_COUNT  ( 30 * 60 )
struct DEBUG__SWORDFISH {
    boo32  show;
    uint32 show_index;
    boo32  pause;
    
    uint32 atIndex;
    SWORDFISH swordfish[ DEBUG__SWORDFISH__FRAME_COUNT ];
    PLAYER_STATE player[ DEBUG__SWORDFISH__FRAME_COUNT ];
};

struct DEBUG_STATE {
    DEBUG__LAST_BITE lastBite;
    DEBUG__SWORDFISH swordfish;
};

struct DEBUG_FRAME {
    uint32 frame_counter;
    
    uint8 * memory_at;
    uint8 nDefendZone;
    uint8 nObstacle;
    uint8 nTurtle;
    uint8 nTower;
    uint8 nCage;
    uint8 nNet;
    uint8 nPlayer;
    uint8 nRainbow;
    uint8 nGoldfish;
    uint8 nDogfish;
    uint8 nSwordfish;
    uint8 nUrchin;
    uint8 nUrchinSpine;
    uint8 nArcher;
    uint8 nBubblet;
    uint8 nlgArcher;
    uint8 nBubble;
    uint8 nBubblon;
    uint8 nParrotfish;
    uint8 nsmTurtle;
};

#define DEBUG__FRAME_COUNT  ( 30 * 60 )
struct DEBUG_FRAME_SYSTEM {
    boo32 show;
    boo32 show_atFrame;
    boo32 pause;
    
    boo32 showLine;
    vec2  showLine_P;
    
    uint32    atFrame;
    DEBUG_FRAME frame[ DEBUG__FRAME_COUNT ];
    MEMORY memory;
};

//----------
// CAMERA
//----------

struct CAMERA_STATE {
    // TODO: This system currently treats the background quad as the camera. Eventually, update this to include a 3d background and actual camera scrolling!
    vec2 pos;
    vec2 dest;
    vec2 dim;
    
    vec2 at;
    vec2 vel;
    
    boo32 springIsEnabled;
    
    flo32 timer;
    flo32 targetTime;
    
    CAMERA_SHAKE cameraShake;
};

struct AUDIO_MANAGER {
    uint32 nSlap;
    uint32 nParrotfishBounce;
    
    uint32 nPlayerStun;
    flo32   playerStun_timer;
};

#define HOME__OPEN_STORAGE_TARGET_TIME  ( 0.8f )
#define HOME__EXIT_BOUND_TARGET_TIME    ( 1.5f )
struct HOME_STATE {
    flo32 exit_timer;
    boo32 storage_isActive;
    flo32 storage_timer;
    
    vec2  cursor_pos;
    boo32  isHolding_inventory;
    boo32  isHolding_storage;
    uint32 isHolding_index;
    INVENTORY_ITEM isHolding_item;
};

enum GAME_MODE {
    gameMode_home,
    gameMode_game,
};

enum EDITOR_MODE {
    editorMode_default,
    editorMode_scale,
    editorMode_translate,
    editorMode_rotate,
    editorMode_order,
    editorMode_texture,
};

enum SCALE_MODE {
    scaleMode_bothAxes,
    scaleMode_xAxis,
    scaleMode_yAxis,
};

#define EDITOR_STATE__OBSTACLE_MAX_COUNT    ( 256 )
#define EDITOR_STATE__BACKGROUND_MAX_COUNT  ( 512 )
struct EDITOR_STATE {
    boo32 isInitialized;
    EDITOR_MODE mode;
    
    boo32 hasUnsavedChanges;
    
    uint32    nBack;
    BACKGROUND back           [ EDITOR_STATE__BACKGROUND_MAX_COUNT ];
    BACKGROUND back_temp      [ EDITOR_STATE__BACKGROUND_MAX_COUNT ];
    boo32      back_isSelected[ EDITOR_STATE__BACKGROUND_MAX_COUNT ];
    uint32    nSelected;
    
    BACKGROUND lastCreated;
    
    vec2 cursor_pos;
    vec2 cursor_dPos;
    vec2 camera_pos;
    vec2 camera_dim;
    boo32 camera_left;
    boo32 camera_right;
    boo32 camera_down;
    boo32 camera_up;
    boo32 camera_in;
    boo32 camera_out;
    
    boo32 scale_isActive;
    int32 scale_sBack;
    int32 scale_sSide;
    vec2  scale_bPos;
    vec2  scale_bAxis;
    
    vec2 modify_basePos;
    vec2 modify_pos;
    
    uint32  nObs;
    LINE_SEG obs[ EDITOR_STATE__OBSTACLE_MAX_COUNT ];
};

struct APP_STATE {
	boo32 isRunning;
	boo32 isInitialized;
    boo32 isPaused;
    uint32 frame_counter;
	
	vec2 app_dim;
	vec2 app_halfDim;
	rect app_bound;
    
    FONT * font;
    
#define GAME_OVER_BACKGROUND_COLOR_TARGET_TIME        ( 4.0f )
#define GAME_OVER_BACKGROUND_COLOR_DELAY_TARGET_TIME  ( 1.0f )
#define GAME_OVER_SLOW_TARGET_TIME                    ( 3.0f )
#define GAME_OVER_DELAY_STATS_TARGET_TIME             ( 4.0f )
#define GAME_OVER_FADE_IN_STATS_TARGET_TIME           ( 4.0f )
    boo32 gameOver;
    flo32 gameOver_timer;
    flo32 restartGame_timer;
    boo32 restartGame_isActive;
    flo32 restartCheckpoint_timer;
    boo32 restartCheckpoint_isActive;
    
    GAME_MODE gameMode;
#define EXIT_TO_HOME__TARGET_TIME  ( 1.2f )
    boo32 exitToHome_isActive;
    flo32 exitToHome_timer;
    
    uint32 score;
    SCORE_PARTICLE_LIST pointsList;
    EXPLOSION_PARTICLE_LIST explosionList;
    
    COLOR_CHAIN     colorChain;
    
    SECRET_STATE    secretState;
    ITEM_STATE     itemState;
    ITEM_INVENTORY item_inventory;
    ITEM_STORAGE   item_storage;
    
    //ITEM_TOOLBAR itemToolbar;
    FISH_ATE_LIST   fishAteList;
    
    uint32 nBites;
    uint32 nThingsAte;
    
    vec2 play_dim;
    rect play_bound;
    rect panel_bound;
    
    vec2 backgroundPos; // TEMP
    vec2 init_cameraDim; // TEMP
    
    rect  movementBound;
    
    boo32 entityUpdate_isEnabled;
    flo32 unitScale;
    rect cameraBound;
    CAMERA_STATE       cameraState;
    boo32 camera_isMoving;
    vec2  camera_dPos;
    vec2  world_basePos;
    rect  world_cameraBound;
    
    PLAYER_STATE       playerState;
    HOME_STATE         homeState;
    RAINBOW_STATE      rainbowState;
    GOLDFISH_STATE     goldfishState;
    DOGFISH_STATE      dogfishState;
    SWORDFISH_STATE    swordfishState;
    URCHIN_STATE       urchinState;
    URCHIN_SPINE_STATE urchinSpineState;
    ARCHER_STATE       archerState;
    BUBBLET_STATE      bubbletState;
    LGARCHER_STATE     lgArcherState;
    BUBBLE_STATE       bubbleState;
    BUBBLON_STATE      bubblonState;
    BUBBLON_GEN        bubblonGen;
    PARROTFISH_STATE   parrotfishState;
    TURTLE_STATE       turtleState;
    SMTURTLE_STATE     smTurtleState;
    CAGE_STATE         cageState;
    NET_STATE          netState;
    DEFEND_ZONE_STATE  defendZoneState;
    TOWER_STATE        towerState;
    OBSTACLE_STATE     obstacleState;
    BACKGROUND_STATE   backgroundState;
    BREAKABLE_STATE    breakableState;
    HEAL_STATE         healState;
    
    EVENT_TRIGGER_STATE eventTriggerState;
    
    GAME_CONTROL_STATE gameControlState;
    AUDIO_MANAGER audioManager;
    
    uint32 checkpoint_eventIndex;
    rect   checkpoint_cameraBound;
    vec2   checkpoint_backgroundPos;
    
    ANIMATION_SYSTEM animationSystem;
    WORLD_SYSTEM     worldSystem;
    
    boo32 resetMousePos;
    vec2  mouse_base;
    int32 mouse_baseX;
    int32 mouse_baseY;
    
    AUDIO_ID   musicPlaying;
    AUDIO_DATA audio[ audioID_count ];
    
    boo32 respawnEnemyForScrolling;
    
    DEBUG_FRAME_SYSTEM debugFrameSystem;
    //DEBUG_STATE debugState;
    
    boo32        editor_isActive;
    EDITOR_STATE editorState;
};