
//----------
// SHARED
//----------

enum FISH_TYPE {
    fishType_null,
    
    fishType_player,
    fishType_rainbow,
    fishType_goldfish,
    fishType_dogfish,
    fishType_swordfish,
    fishType_urchin,
    fishType_archer,
    fishType_lgArcher,
    fishType_cage,
    fishType_net,
    
    fishType_count,
};

enum COLOR_ID {
    colorID_blue,
    colorID_green,
    colorID_orange,
    colorID_purple,
    colorID_red,
    colorID_yellow,
    
    colorID_count,
};

enum FISH_STATE {
    fishState_idle,
    fishState_moving,
    fishState_spawning,
    fishState_lungePrep,
    fishState_lunging,
    fishState_biting,
    fishState_exiting,
    fishState_stunnedByEscape,
    fishState_chasing,
    fishState_stunnedByBite,
    fishState_dying,
    fishState_idle_gameOver,
    fishState_shooting,
    fishState_resting,
    fishState_charging,
    fishState_stabbing,
    fishState_stunned,
    fishState_dodging,
    fishState_slashPrep,
    fishState_slashWarmup,
    fishState_slashing,
    fishState_introAnimation,
    fishState_headbutt,
};

struct FACING_DIRECTION {
    boo32 isFacingLeft;
    flo32 transitionX;
};

struct SPAWN {
    vec2  position;
    vec2  direction;
    vec2  velocity;
    flo32 targetTime;
    flo32 speed;
    FACING_DIRECTION facingDir;
};

//----------
// ITEMS
//----------

#define ITEM_BITE_RADIUS  ( 40.0f )
#define ITEM_SLAP_RADIUS  ( 40.0f )
#define ITEM_BUBBLE_BITMAP_SCALE  Vec2( 56.0f, 56.0f )
#define ITEM_HEAL_BITMAP_SCALE    Vec2( 22.0f, 22.0f )
#define ITEM_HEAL_BITMAP_OFFSET   Vec2( 0.0f, -3.0f )

enum ITEM_ID {
    itemID_empty,
    
    itemID_pointMultiplier,
    itemID_slowDownTime,
    itemID_strongBite,
    
    itemID_heal,
    
    itemID_count,
};

struct INVENTORY_ITEM {
    ITEM_ID   itemID;
    uint32   nItem;
    uint32 maxItem;
};

#define ITEM_INVENTORY__SLOT_MAX_COUNT  ( 6 )
struct ITEM_INVENTORY {
    uint32 nSlots;
    uint32 activeSlot;
    
    boo32  charge_isActive;
    flo32  charge_timer;
    flo32  charge_targetTime;
    // TODO: handle multi-use with faster target time
    
#define ITEM_INVENTORY__TO_INVENTORY_ICON_TARGET_TIME  ( 0.2f )
    flo32 toInventory_timer;
    
    INVENTORY_ITEM item[ ITEM_INVENTORY__SLOT_MAX_COUNT ];
};

struct ITEM_STORAGE {
    uint32 nItem[ itemID_count ];
};

struct GAME_ITEM {
    ITEM_ID itemID;
    vec2    position;
};

#define ITEM_STATE__GAME_ITEM_MAX_COUNT  ( 16 )
struct ITEM_STATE {
    uint32   nItem;
    GAME_ITEM item[ ITEM_STATE__GAME_ITEM_MAX_COUNT ];
};

internal INVENTORY_ITEM
InventoryItem( ITEM_ID itemID, uint32 nItem ) {
    INVENTORY_ITEM result = {};
    result.itemID  = itemID;
    switch( itemID ) {
        case itemID_empty:           { result.maxItem = 0; } break;
        case itemID_pointMultiplier: { result.maxItem = 3; } break;
        case itemID_slowDownTime:    { result.maxItem = 3; } break;
        case itemID_strongBite:      { result.maxItem = 3; } break;
        case itemID_heal:            { result.maxItem = 3; } break;
        
        default: { InvalidCodePath; } break;
    }
    result.nItem = minValue( nItem, result.maxItem );
    return result;
}

//----------
// SPRITE
//----------

struct SPRITE {
    vec2  position;
    vec2  halfDim;
    flo32 radians;
};

//----------
// RAINBOW
//----------

#define RAINBOW_BITMAP_SCALE_X   ( 36.0f )
#define RAINBOW_BITMAP_SCALE_Y   ( 24.0f )
#define RAINBOW_BITMAP_OFFSET_X  ( -5.0f )
#define RAINBOW_BITMAP_OFFSET_Y  ( -5.0f )
#define RAINBOW_MAX_SPEED  ( 500.0f )
#define RAINBOW_FRICTION   (   5.0f )
#define RAINBOW_EAT_RADIUS  ( 34.0f )
#define RAINBOW_MIN_DIST_TO_TRAVEL  ( 180.0f )
#define RAINBOW_MAX_DIST_TO_TRAVEL  ( 300.0f )
#define RAINBOW_REPEL_MAX_STRENGTH  ( 4.0f )
#define RAINBOW_REPEL_MAX_DISTANCE  ( 14.0f )

struct RAINBOW {
    boo32 isActive;
    FISH_STATE state;
    vec2 position;
    vec2 velocity;
    vec2 direction;
    vec2 repel;
    FACING_DIRECTION facingDir;
    flo32 timer;
    flo32 targetTime;
    flo32 spawnSpeed;
    COLOR_ID colorID;
};

#define RAINBOW_MAX_COUNT  ( 64 )
struct RAINBOW_STATE {
    uint32 nRainbow;
    RAINBOW rainbow[ RAINBOW_MAX_COUNT ];
};

//----------
// GOLDFISH
//----------

#define GOLDFISH_BITMAP_SCALE_X   ( 20.0f )
#define GOLDFISH_BITMAP_SCALE_Y   ( 20.0f )
#define GOLDFISH_BITMAP_OFFSET_X  ( 0.0f )
#define GOLDFISH_BITMAP_OFFSET_Y  ( 0.0f )
#define GOLDFISH_MAX_SPEED        ( 1700.0f )
#define GOLDFISH_FRICTION         ( 12.0f )
#define GOLDFISH_EAT_RADIUS       ( 26.0f )
#define GOLDFISH_MOVE_RADIUS               ( 100.0f )
#define GOLDFISH_MOVE_MIN_DISTANCE         ( 130.0f )
#define GOLDFISH_MOVE_DISTANCE_LO          ( 260.0f )
#define GOLDFISH_MOVE_DISTANCE_HI          ( 300.0f )
#define GOLDFISH_MOVE_OFFSET_TO_DIRECTION  ( 80.0f )
#define GOLDFISH_PLAY_BOUND_MARGIN         ( 40.0f )
#define GOLDFISH_BOB_ACCEL            ( 0.0125f )
#define GOLDFISH_BOB_HEIGHT           ( 10.0f )
#define GOLDFISH_BOB_REVS_PER_SECOND_LO  ( 0.4f )
#define GOLDFISH_BOB_REVS_PER_SECOND_HI  ( 0.3f )
#define GOLDFISH_MOVE_WARMUP_TARGET_TIME  ( 0.1f )
#define GOLDFISH_POINTS  ( 1000 )

struct FISH_TARGET {
    FISH_TYPE type;
    void *    target;
};

struct GOLDFISH {
    boo32 isActive;
    
    FISH_STATE state;
    vec2 position;
    vec2 velocity;
    vec2 direction;
    FACING_DIRECTION facingDir;
    flo32 timer;
    flo32 targetTime;
    flo32 spawnSpeed;
    flo32 lifeTimer;
    flo32 bob_timer;
    flo32 bob_revsPerSec;
};

#define GOLDFISH_MAX_COUNT  ( 64 )
struct GOLDFISH_STATE {
    uint32  nGoldfish;
    GOLDFISH goldfish[ GOLDFISH_MAX_COUNT ];
};

//----------
// DOGFISH
//----------

#define DOGFISH_BITMAP_SCALE_X   ( 105.0f )
#define DOGFISH_BITMAP_SCALE_Y   ( 60.0f )
#define DOGFISH_BITMAP_OFFSET_X  ( -34.0f )
#define DOGFISH_BITMAP_OFFSET_Y  ( 6.0f )
#define DOGFISH_MAX_SPEED        ( 360.0f )
#define DOGFISH_FRICTION         ( 12.0f )
#define DOGFISH_SPAWN_MARGIN     ( 40.0f )
#define DOGFISH_LUNGE_RADIUS     ( 280.0f )
#define DOGFISH_LUNGE_FRICTION   ( 5.0f )
#define DOGFISH_LUNGE_WARMUP_TARGET_TIME  ( 0.1f )
#define DOGFISH_LUNGE_SPEED               ( 2400.0f )
#define DOGFISH_LUNGE_TARGET_TIME         ( 1.2f )
#define DOGFISH_LUNGE_BITE_TARGET_TIME    ( 0.25f )
#define DOGFISH_BITE_RADIUS       ( 26.0f )
#define DOGFISH_BITE_TARGET_TIME  ( 3.0f )
#define DOGFISH_BITE_INITIAL_DAMAGE  ( 0.075f )
#define DOGFISH_BITE_DAMAGE_RATE     ( 1.0f / 10.0f )
#define DOGFISH_BITE_DAMAGE_WARMUP_TARGET_TIME  ( 0.4f )
#define DOGFISH_ESCAPE_VALUE             ( 3.0f )
#define DOGFISH_ESCAPE_EMPTY_RATE        ( 2.0f )
#define DOGFISH_ESCAPE_STUN_TARGET_TIME  ( 1.0f )
#define DOGFISH_ESCAPE_BITMAP_OFFSET     ( 26.0f )
#define DOGFISH_NO_CHASE_AFTER_ESCAPE_TARGET_TIME  ( 0.5f )
#define DOGFISH_NO_CHASE_AFTER_FAIL_TARGET_TIME    ( 2.0f )
#define DOGFISH_IDLE_TARGET_TIME_LO  ( 1.0f )
#define DOGFISH_IDLE_TARGET_TIME_HI  ( 4.0f )
#define DOGFISH_IDLE_SPEED_LO        ( 40.0f )
#define DOGFISH_IDLE_SPEED_HI        ( 80.0f )
#define DOGFISH_IDLE_FRICTION        ( 2.0f )
#define DOGFISH_SIGHT_RADIUS_LO      ( 100.0f )
#define DOGFISH_SIGHT_RADIUS_MID     ( 220.0f )
#define DOGFISH_SIGHT_RADIUS_HI      ( 540.0f )
#define DOGFISH_SIGHT_FILL_RATE_LO   ( 0.3f )
#define DOGFISH_SIGHT_FILL_RATE_MID  ( 0.75f )
#define DOGFISH_SIGHT_FILL_RATE_HI   ( 1.25f )
#define DOGFISH_SIGHT_EMPTY_RATE     ( 0.5f )
#define DOGFISH_BITE_DIM_X  ( 60.0f )
#define DOGFISH_BITE_DIM_Y  ( 64.0f )
#define DOGFISH_BITE_OFFSET_X  ( 60.0f )
#define DOGFISH_BITE_OFFSET_Y  (  2.0f )
#define DOGFISH_HEALTH  ( 10 )
#define DOGFISH_STUNNED_BY_BITE_TARGET_TIME    ( 0.5f )
#define DOGFISH_STUNNED_BY_BITE_BITMAP_OFFSET  ( 3.0f )
#define DOGFISH_LAST_BITE_TARGET_TIME  ( 0.25f )
#define DOGFISH_DYING_TARGET_TIME  ( 2.0f )
#define DOGFISH_REPEL_MAX_STRENGTH  ( 4.0f )
#define DOGFISH_REPEL_MAX_DISTANCE  ( 14.0f )

struct DOGFISH {
    boo32 isActive;
    
    FISH_STATE state;
    vec2 position;
    vec2 velocity;
    vec2 direction;
    vec2 repel;
    FACING_DIRECTION facingDir;
    flo32 timer;
    flo32 targetTime;
    
    flo32 radians;
    flo32 speed;
    flo32 targetRadians;
    flo32 targetSpeed;
    
    flo32 dir_timer;
    flo32 dir_targetTime;
    flo32 speed_timer;
    flo32 speed_targetTime;
    
    uint32    chase_nLunges;
    boo32     chase_ateTarget;
    uint32    chase_index;
    FISH_TYPE chase_type;
    uint32    chase_prevIndex;
    FISH_TYPE chase_prevType;
    flo32     chase_value;
    
    flo32 escape;
    vec2  escapeDir;
    
    flo32 lastBit_timer;
    int32 health;
};

#define DOGFISH_MAX_COUNT  ( 32 )
struct DOGFISH_STATE {
    uint32  nDogfish;
    DOGFISH dogfish[ DOGFISH_MAX_COUNT ];
};

internal void
removeThisTargetAndSetIdle( DOGFISH_STATE * state, FISH_TYPE type, uint32 index, int32 skipIndex = -1 );

//----------
// SWORDFISH
//----------

#define SWORDFISH_PRIMARY_COLLISION_OFFSET    ( 20.0f  )
#define SWORDFISH_PRIMARY_COLLISION_DIM_X     ( 215.0f )
#define SWORDFISH_PRIMARY_COLLISION_DIM_Y     ( 60.0f  )
#define SWORDFISH_SECONDARY_COLLISION_OFFSET  ( 0.0f )
#define SWORDFISH_SECONDARY_COLLISION_DIM_X   ( 240.0f )
#define SWORDFISH_SECONDARY_COLLISION_DIM_Y   ( 120.0f )

#define SWORDFISH_BITMAP_SCALE_X   ( 340.0f )
#define SWORDFISH_BITMAP_SCALE_Y   ( 132.0f )
#define SWORDFISH_BITMAP_OFFSET_X  ( -34.0f )
#define SWORDFISH_BITMAP_OFFSET_Y  ( 6.0f )
#define SWORDFISH_MAX_SPEED        ( 1000.0f )
#define SWORDFISH_FRICTION         ( 3.0f ) // 12.0f
#define SWORDFISH_ATTACK_RADIUS    ( 280.0f )
#define SWORDFISH_EXIT_SPEED       ( 1400.0f )
#define SWORDFISH_EXIT_FRICTION    ( 4.0f )
#define SWORDFISH_EXIT_OFFSET      ( 400.0f )

#define SWORDFISH_CHARGE_SPEED       ( 4400.0f )
#define SWORDFISH_CHARGE_FRICTION    ( 5.0f )
#define SWORDFISH_CHARGE_INIT_DELAY  ( 1.0f )
#define SWORDFISH_CHARGE_COLLISION_DIM_X     ( 215.0f )  
#define SWORDFISH_CHARGE_COLLISION_DIM_Y     ( 60.0f )
#define SWORDFISH_CHARGE_COLLISION_OFFSET_X  ( 120.0f )
#define SWORDFISH_CHARGE_COLLISION_OFFSET_Y  ( 8.0f )
#define SWORDFISH_CHARGE_POSITION_OFFSET_Y   ( 100.0f )

#define SWORDFISH_SPAWN_SPEED      ( 1400.0f )
#define SWORDFISH_SPAWN_FRICTION   ( 5.0f )
#define SWORDFISH_SPAWN_END_MOVEMENT_TARGET_TIME     ( 0.4f )
#define SWORDFISH_SPAWN_BEGIN_ANIMATION_TARGET_TIME  ( 1.8f )
#define SWORDFISH_SPAWN_END_ANIMATION_TARGET_TIME    ( 3.8f )
#define SWORDFISH_SPAWN_TARGET_TIME                  ( 4.2f )

#define SWORDFISH_STAB_WARMUP_BITMAP_SCALE_X   ( 235.0f )
#define SWORDFISH_STAB_WARMUP_BITMAP_SCALE_Y   ( 140.0f )
#define SWORDFISH_STAB_WARMUP_BITMAP_OFFSET_X  ( -72.0f )
#define SWORDFISH_STAB_WARMUP_BITMAP_OFFSET_Y  ( 6.0f )
#define SWORDFISH_STAB_BITMAP_SCALE_X   ( 440.0f )
#define SWORDFISH_STAB_BITMAP_SCALE_Y   ( 112.0f )
#define SWORDFISH_STAB_BITMAP_OFFSET_X  ( 28.0f )
#define SWORDFISH_STAB_BITMAP_OFFSET_Y  ( 6.0f )
#define SWORDFISH_STAB_COLLISION_DIM_X     ( 280.0f )
#define SWORDFISH_STAB_COLLISION_DIM_Y     ( 50.0f )
#define SWORDFISH_STAB_COLLISION_OFFSET_X  ( 200.0f )
#define SWORDFISH_STAB_COLLISION_OFFSET_Y  ( 5.0f )
#define SWORDFISH_STAB_INIT_RADIUS    ( 96.0f )
#define SWORDFISH_STAB_INIT_OFFSET_X  ( 260.0f )
#define SWORDFISH_STAB_INIT_OFFSET_Y  ( 8.0f )
#define SWORDFISH_CHASE_OFFSET_FOR_FACING_DIR  ( 160.0f )

#define SWORDFISH_STUNNED_BY_BITE_BITMAP_OFFSET_LO  ( 3.0f )
#define SWORDFISH_STUNNED_BY_BITE_BITMAP_OFFSET_HI  ( 8.0f )
#define SWORDFISH_STUNNED_BY_BITE_TARGET_TIME       ( 0.15f )

#define SWORDFISH_BUMP_SPEED  ( 900.0f )
// TODO: probably adjust bump speed based on friction of the current state
#define SWORDFISH_STUNNED_FRICTION     ( 5.0f )
#define SWORDFISH_STUNNED_LO_MAX_SPEED    ( 600.0f )
#define SWORDFISH_STUNNED_LO_TARGET_TIME  ( 1.4f )
#define SWORDFISH_STUNNED_HI_MAX_SPEED    ( 1200.0f )
#define SWORDFISH_STUNNED_HI_TARGET_TIME  ( 3.0f )
#define SWORDFISH_STUNNED_DRIFT_SPEED  ( 35.0f )
#define SWORDFISH_STUNNED_HIT_FRAME_TARGET_TIME  ( 0.1f )

#define SWORDFISH_DODGE_DIM_X  ( 220.0f )
#define SWORDFISH_DODGE_DIM_Y  ( 60.0f )
#define SWORDFISH_DODGE_OFFSET_X  ( -110.0f )
#define SWORDFISH_DODGE_OFFSET_Y  ( 5.0f )
#define SWORDFISH_DODGE_DISTANCE  ( 200.0f )
#define SWORDFISH_DODGE_MAX_SPEED    ( 1800.0f )
#define SWORDFISH_DODGE_FRICTION     ( 5.0f )
#define SWORDFISH_DODGE_TARGET_TIME  ( 0.4f )
#define SWORDFISH_DODGE_RADIUS       ( 600.0f )

#define SWORDFISH_REST_SPEED        ( 10.0f )
#define SWORDFISH_REST_FRICTION     ( 5.0f )
#define SWORDFISH_REST_TARGET_TIME  ( 24.0f / 60.0f )

#define SWORDFISH_LUNGE_RADIUS       ( 600.0f )
#define SWORDFISH_LUNGE_SPEED        ( 2500.0f )
#define SWORDFISH_LUNGE_FRICTION     ( 5.0f )
#define SWORDFISH_LUNGE_TARGET_TIME  ( 0.8f )
#define SWORDFISH_LUNGE_COLLISION_DIM_X     ( 215.0f )
#define SWORDFISH_LUNGE_COLLISION_DIM_Y     ( 60.0f )
#define SWORDFISH_LUNGE_COLLISION_OFFSET_X  ( 150.0f )
#define SWORDFISH_LUNGE_COLLISION_OFFSET_Y  ( 0.0f )

#define SWORDFISH_LAST_BIT_TARGET_TIME    ( 0.2f )
#define SWORDFISH_LAST_BIT_BITMAP_OFFSET  ( 4.0f )
#define SWORDFISH_DYING_BITMAP_OFFSET  ( 6.0f )
#define SWORDFISH_DYING_TARGET_TIME    ( 3.0f )
#define SWORDFISH_DYING_START_FADE_TARGET_TIME  ( 0.6f )

#define SWORDFISH_INTRO_ANIMATION_WAIT        ( 2.75f )
#define SWORDFISH_INTRO_ANIMATION_BACKGROUND  ( 5.3f )
#define SWORDFISH_INTRO_ANIMATION_TARGET_TIME ( 8.0f )
#define SWORDFISH_INTRO_ANIMATION_FRICTION  ( 4.0f )
#define SWORDFISH_INTRO_ANIMATION_SPEED_01  ( 950.0f )
#define SWORDFISH_INTRO_ANIMATION_SPEED_02  ( 1050.0f )

#define SWORDFISH_SLASH_SPEED         ( 4000.0f )
#define SWORDFISH_SLASH_FRICTION      ( 4.0f )
#define SWORDFISH_SLASH_TARGET_TIME   ( 0.4f )
#define SWORDFISH_SLASH_WARMUP_FRICTION     ( 6.0f )
#define SWORDFISH_SLASH_WARMUP_TARGET_TIME  ( 30.0f / 60.0f )
#define SWORDFISH_SLASH_RADIUS        ( 108.0f )
#define SWORDFISH_SLASH_PREP_SPEED    ( 1400.0f )
#define SWORDFISH_SLASH_PREP_FRICTION ( 4.0f )
#define SWORDFISH_SLASH_PREP_MIN_DIST ( 100.0f )
#define SWORDFISH_INIT_SLASH_TARGET_TIME  ( 5.0f )

#define SWORDFISH_STAB_DAMAGE_HI   ( 0.2f )
#define SWORDFISH_STAB_DAMAGE_LO   ( 0.08f )
#define SWORDFISH_CHARGE_DAMAGE ( 0.33f )
#define SWORDFISH_LUNGE_DAMAGE  ( 0.25f )
#define SWORDFISH_MAX_HEALTH        ( 50 )
#define SWORDFISH_CHARGE_HEALTH_01  ( 34 )
#define SWORDFISH_CHARGE_HEALTH_02  ( 16 )

#define SWORDFISH_STAB_STATS  \
flo32 framesA = 13.0f / 60.0f;\
flo32 framesB =  6.0f / 60.0f;\
flo32 framesC =  4.0f / 60.0f;\
flo32 loA = 0.0f;\
flo32 hiA = loA + framesA;\
flo32 loB = hiA;\
flo32 hiB = loB + framesB;\
flo32 loC = hiB;\
flo32 hiC = loC + framesC;\
flo32 max = hiC;\
flo32 rad = PI / 18.0f;\
flo32 rad_offYA = 30.0f;\
flo32 rad_offYB = 50.0f;

#define SWORDFISH_DYING_STATS \
flo32 targetA = 1.0f;\
flo32 targetB = 1.6f;\
flo32 targetC = 3.6f;\
flo32 targetD = 3.8f;

struct SWORDFISH {
    boo32      isActive;
    FISH_STATE state;
    vec2       position;
    vec2       velocity;
    vec2       direction;
    vec2       destination;
    flo32      timer;
    flo32      targetTime;
    FACING_DIRECTION facingDir;
    
    uint32 nAttacks;
    
    boo32  initStun;
    boo32  initSlash_dontCountAttack;
    uint32 initSlash_nAttacks;
    uint32 initSlash_nTargetAttacks;
    
    vec2  headDir;
    boo32 transitionToStunnedByBite;
    boo32 charge_targetPlayer[ 2 ];
    
    flo32 lastBit_timer;
    
    boo32 doCharge;
    boo32 didCharge[ 2 ];
    boo32 doStabAudio;
    
    boo32 initCollision;
    boo32 collision_isActive;
    
    uint32 explosion_index;
    
    boo32 toggleBackground;
    flo32 toggleTimer;
    
    int32 health;
    
    boo32 slash_show;
    boo32 slash_flip;
    vec2  slash_pos;
    
    flo32 speed;
    flo32 radians;
    flo32 dir_timer;
    flo32 dir_targetTime;
    flo32 speed_timer;
    flo32 speed_targetTime;
    flo32 targetSpeed;
    flo32 targetRadians;
};

#define SWORDFISH_MAX_COUNT  ( 8 )
struct SWORDFISH_STATE {
    uint32 nSwordfish;
    SWORDFISH swordfish[ SWORDFISH_MAX_COUNT ];
};

//----------
// URCHIN
//----------

#define URCHIN_BITMAP_SCALE_X   ( 120.0f )
#define URCHIN_BITMAP_SCALE_Y   ( 120.0f )
#define URCHIN_BITMAP_OFFSET_X  ( 0.0f )
#define URCHIN_BITMAP_OFFSET_Y  ( 0.0f )
#define URCHIN_MOVE_SPEED       ( 140.0f )
#define URCHIN_MOVE_FRICTION    ( 4.0f )

#define URCHIN_SPAWN_MOVE_TARGET_TIME  ( 3.0f )
#define URCHIN_SPAWN_TARGET_TIME       ( 5.0f )

#define URCHIN_FIRE_SPINE_TARGET_TIME  ( 3.0f )

#define URCHIN_HEALTH  ( 2 )

struct URCHIN {
    boo32      isActive;
    FISH_STATE state;
    vec2       position;
    vec2       velocity;
    vec2       direction;
    vec2       destination;
    flo32      timer;
    flo32      targetTime;
    FACING_DIRECTION facingDir;
    
    int32 health;
};

#define URCHIN_MAX_COUNT  ( 16 )
struct URCHIN_STATE {
    uint32 nUrchin;
    URCHIN  urchin[ URCHIN_MAX_COUNT ];
};

//----------
// URCHIN SPINE
//----------

#define URCHIN_SPINE_BITMAP_SCALE_X   ( 140.0f )
#define URCHIN_SPINE_BITMAP_SCALE_Y   ( 140.0f )
#define URCHIN_SPINE_BITMAP_OFFSET_X  ( 0.0f )
#define URCHIN_SPINE_BITMAP_OFFSET_Y  ( 0.0f )
#define URCHIN_SPINE_MOVE_SPEED       ( 400.0f )
#define URCHIN_SPINE_MOVE_FRICTION    ( 4.0f )

#define URCHIN_SPINE_DAMAGE  ( 0.2f )

#define URCHIN_SPINE_DEFLECT_RADIUS   ( 140.0f )
#define URCHIN_SPINE_COLLISION__URCHIN_RADIUS  ( 80.0f )
#define URCHIN_SPINE_COLLISION__PLAYER_RADIUS  ( 60.0f )

struct URCHIN_SPINE {
    boo32      isActive;
    FISH_STATE state;
    vec2       position;
    vec2       velocity;
    vec2       direction;
    vec2       destination;
    flo32      timer;
    flo32      targetTime;
    FACING_DIRECTION facingDir;
    
    int32 health;
    boo32 isDeflected;
    vec2  deflectDir;
};

#define URCHIN_SPINE_MAX_COUNT  ( 16 )
struct URCHIN_SPINE_STATE {
    uint32       nUrchinSpine;
    URCHIN_SPINE  urchinSpine[ URCHIN_SPINE_MAX_COUNT ];
};

//----------
// ARCHER
//----------

#define ARCHER_BITMAP_SCALE_X   ( 75.0f )
#define ARCHER_BITMAP_SCALE_Y   ( 88.0f )
#define ARCHER_BITMAP_OFFSET_X  ( 0.0f )
#define ARCHER_BITMAP_OFFSET_Y  ( 0.0f )
#define ARCHER_EXIT_SPEED       ( 1200.0f )
#define ARCHER_MOVE_SPEED       ( 220.0f )
#define ARCHER_MOVE_FRICTION    ( 4.0f )
#define ARCHER_MIN_DIST_TO_TRAVEL  ( 180.0f )
#define ARCHER_MAX_DIST_TO_TRAVEL  ( 300.0f )

#define ARCHER_STUNNED_SPEED        ( 1800.0f )
#define ARCHER_STUNNED_FRICTION     ( 8.0f )
#define ARCHER_STUNNED_TARGET_TIME  ( 3.0f )

#define ARCHER_FIRE_SPINE_TARGET_TIME  ( 3.0f )

#define ARCHER_HEALTH  ( 2 )

#define ARCHER_DYING_FADE_DELAY_TARGET_TIME  ( 1.0f )
#define ARCHER_DYING_TARGET_TIME             ( 2.0f )

struct ARCHER {
    boo32      isActive;
    FISH_STATE state;
    vec2       position;
    vec2       velocity;
    vec2       direction;
    vec2       destination;
    flo32      timer;
    flo32      targetTime;
    FACING_DIRECTION facingDir;
    vec2 targetPos;
    
    flo32 spawnSpeed;
    flo32 radians;
    
    int32 health;
};

#define ARCHER_MAX_COUNT  ( 64 )
struct ARCHER_STATE {
    uint32 nArcher;
    ARCHER  archer[ ARCHER_MAX_COUNT ];
};

//----------
// LG ARCHER
//----------

#define LGARCHER_BITMAP_SCALE_X   ( 125.0f )
#define LGARCHER_BITMAP_SCALE_Y   ( 138.0f )
#define LGARCHER_BITMAP_OFFSET_X  ( 0.0f )
#define LGARCHER_BITMAP_OFFSET_Y  ( 0.0f )
#define LGARCHER_MOVE_SPEED       ( 200.0f )
#define LGARCHER_MOVE_FRICTION    ( 4.0f )
#define LGARCHER_EXIT_SPEED       ( 1000.0f )
#define LGARCHER_MIN_DIST_TO_TRAVEL  ( 100.0f )
#define LGARCHER_MAX_DIST_TO_TRAVEL  ( 300.0f )

#define LGARCHER_MOVEMENT_DECISION_RADIUS  ( 600.0f )

#define LGARCHER_STUNNED_SPEED        ( 1800.0f )
#define LGARCHER_STUNNED_FRICTION     ( 8.0f )
#define LGARCHER_STUNNED_TARGET_TIME  ( 3.0f )

#define LGARCHER_HEALTH  ( 5 )

#define LGARCHER_DYING_FADE_DELAY_TARGET_TIME  ( 1.0f )
#define LGARCHER_DYING_TARGET_TIME             ( 2.0f )

struct LGARCHER {
    boo32      isActive;
    FISH_STATE state;
    vec2       position;
    vec2       velocity;
    vec2       direction;
    vec2       destination;
    flo32      timer;
    flo32      targetTime;
    FACING_DIRECTION facingDir;
    vec2 targetPos;
    
    flo32 spawnSpeed;
    flo32 radians;
    
    int32 health;
    boo32 useSmBoundBehavior;
};

#define LGARCHER_MAX_COUNT  ( 32 )
struct LGARCHER_STATE {
    uint32  nlgArcher;
    LGARCHER lgArcher[ LGARCHER_MAX_COUNT ];
};

//----------
// BUBBLET
//----------

#define BUBBLET_BITMAP_SCALE_X   ( 30.0f )
#define BUBBLET_BITMAP_SCALE_Y   ( 30.0f )
#define BUBBLET_BITMAP_OFFSET_X  ( 0.0f )
#define BUBBLET_BITMAP_OFFSET_Y  ( 0.0f )
#define BUBBLET_MOVE_SPEED       ( 400.0f )
#define BUBBLET_MOVE_FRICTION    ( 4.0f )

#define BUBBLET_DAMAGE            ( 0.05f )

struct BUBBLET {
    boo32      isActive;
    vec2       position;
    vec2       velocity;
    vec2       direction;
};

#define BUBBLET_MAX_COUNT  ( 128 )
struct BUBBLET_STATE {
    uint32  nBubblet;
    BUBBLET  bubblet[ BUBBLET_MAX_COUNT ];
};

//----------
// BUBBLE
//----------

#define BUBBLE_BITMAP_SCALE_X   ( 140.0f )
#define BUBBLE_BITMAP_SCALE_Y   ( 140.0f )
#define BUBBLE_BITMAP_OFFSET_X  ( 0.0f )
#define BUBBLE_BITMAP_OFFSET_Y  ( 0.0f )
#define BUBBLE_SPAWN_SPEED      ( 600.0f )
#define BUBBLE_MOVE_SPEED       ( 100.0f )
#define BUBBLE_MOVE_FRICTION    ( 2.0f )
#define BUBBLE_HOLD_RADIUS      ( 50.0f )
#define BUBBLE_HOLD_MOVE_SPEED  ( 20.0f )
#define BUBBLE_ESCAPE_VALUE             ( 2.0f )
#define BUBBLE_ESCAPE_BITMAP_OFFSET     ( 16.0f )

#define BUBBLE_REPEL_MAX_STRENGTH  ( 4.0f )  // px / sec
#define BUBBLE_REPEL_MAX_DISTANCE  ( 64.0f )

#define BUBBLE_DAMAGE            ( 0.05f )

struct BUBBLE {
    boo32 isActive;
    vec2  position;
    vec2  velocity;
    vec2  holdOffset;
    vec2  repel;
};

#define BUBBLE_MAX_COUNT  ( 128 )
struct BUBBLE_STATE {
    uint32  nBubble;
    BUBBLE  bubble[ BUBBLE_MAX_COUNT ];
};

//----------
// BUBBLON
//----------

#define BUBBLON_BITMAP_SCALE_X   ( 80.0f )
#define BUBBLON_BITMAP_SCALE_Y   ( 80.0f )
#define BUBBLON_BITMAP_OFFSET_X  ( 0.0f )
#define BUBBLON_BITMAP_OFFSET_Y  ( 0.0f )
#define BUBBLON_MOVE_SPEED       ( 200.0f )
#define BUBBLON_MOVE_FRICTION    ( 1.0f )

#define BUBBLON_DAMAGE            ( 0.05f )

#define BUBBLON_BITE_COLOR  ( Vec4( 1.0f,  0.35f, 0.35f, 1.0f ) )
#define BUBBLON_STUN_COLOR  ( Vec4( 0.35f, 0.35f, 1.0f,  1.0f ) )

enum BUBBLON_TYPE {
    bubblonType_dodge,
    bubblonType_bite,
    bubblonType_stun,
    
    bubblonType_count,
};

struct BUBBLON {
    boo32        isActive;
    BUBBLON_TYPE type;
    vec2         position;
    vec2         velocity;
};

struct BUBBLON_GEN {
    boo32      isActive;
    FISH_STATE state;
    flo32      timer;
    flo32      targetTime;
};

#define BUBBLON_MAX_COUNT  ( 128 )
struct BUBBLON_STATE {
    uint32  nBubblon;
    BUBBLON  bubblon[ BUBBLON_MAX_COUNT ];
};

//----------
// TURTLE
//----------

#define TURTLE_BITMAP_SCALE_X   ( 600.0f )
#define TURTLE_BITMAP_SCALE_Y   ( 300.0f )
#define TURTLE_BITMAP_OFFSET_X  ( 0.0f )
#define TURTLE_BITMAP_OFFSET_Y  ( 40.0f )
#define TURTLE_MOVE_SPEED       ( 400.0f )
#define TURTLE_MOVE_FRICTION    ( 2.0f )

#define TURTLE_COLLISION_DIM  ( Vec2( 600.0f, 360.0f ) )

#define TURTLE_DAMAGE            ( 0.2f )

struct TURTLE_STATE {
    boo32      isActive;
    FISH_STATE state;
    vec2       position;
    vec2       velocity;
    vec2       direction;
    flo32      timer;
    flo32      targetTime;
    flo32      targetTime_lo;
    flo32      targetTime_hi;
    boo32      isFacingLeft;
};

//----------
// SM TURTLE
//----------

#define SMTURTLE_SLAP_COLLISION_RADIUS  ( 146.0f )
#define SMTURTLE_WINDUP_BITMAP_RADIUS  ( 540.0f )
#define SMTURTLE_HEADBUTT_RADIUS       ( 70.0f )
#define SMTURTLE_BUBBLE_COLLISION_RADIUS  ( 112.0f )
#define SMTURTLE_BUBBLET_COLLISION_RADIUS  ( 68.0f )

#define SMTURTLE_LGARCHER_STUN_RADIUS     ( 112.0f )
#define SMTURTLE_ARCHER_STUN_RADIUS       ( 82.0f )
#define SMTURTLE_CAN_STUN_OTHER_FISH_TARGET_TIME  ( 0.5f )
#define SMTURTLE_CAN_RICOCHET_TARGET_TIME         ( 0.35f )

#define SMTURTLE_STUNNED_SPEED        ( 1600.0f )
#define SMTURTLE_STUNNED_FRICTION     ( 4.0f )
#define SMTURTLE_STUNNED_TARGET_TIME  ( 1.6f )

#define SMTURTLE_EXIT_SPEED       ( 800.0f )

#define SMTURTLE_CAGE_COLLISION_DIM  ( Vec2( 320.0f, 220.0f ) )
#define SMTURTLE_NET_COLLISION_DIM   ( Vec2( 100.0f, 100.0f ) )
#define SMTURTLE_RICOCHET_DAMAGE     ( 10 )
#define SMTURTLE_RICOCHET_SPEED      ( 1000.0f )

#define SMTURTLE_BOUNCE_RADIUS  ( 120.0f )
#define SMTURTLE_BOUNCE_SPEED   ( 500.0f )

#define SMTURTLE_BUBBLET_DAMAGE  ( 1 )
#define SMTURTLE_BUBBLE_DAMAGE   ( 2 )

#define SMTURTLE_ATTACK_COOLDOWN_TARGET_TIME  ( 0.2f )
#define SMTURTLE_ATTACK_ARCHER_RADIUS         ( 82.0f )
#define SMTURTLE_ATTACK_LGARCHER_RADIUS         ( 112.0f )

#define SMTURTLE_HEALTH  ( 14 )
struct SMTURTLE {
    boo32 isActive;
    FISH_STATE state;
    
    vec2 position;
    vec2 velocity;
    vec2 direction;
    FACING_DIRECTION facingDir;
    
    flo32 timer;
    flo32 targetTime;
    
    int32 health;
    boo32 ricochet_canStunPlayer;
    
    boo32 chase_isValid;
    vec2  chase_position;
};

#define SMTURTLE_MAX_COUNT  ( 8 )
struct SMTURTLE_STATE {
    uint32   nsmTurtle;
    SMTURTLE  smTurtle[ SMTURTLE_MAX_COUNT ];
};

//----------
// CAGE
//----------

#define CAGE_BITMAP_SCALE_X   ( 80.0f )
#define CAGE_BITMAP_SCALE_Y   ( 80.0f )
#define CAGE_BITMAP_OFFSET_X  ( -14.0f )
#define CAGE_BITMAP_OFFSET_Y  (  14.0f )

#define CAGE_SHAKE_TARGET_TIME    ( 0.35f )
#define CAGE_SHAKE_BITMAP_OFFSET  ( 5.0f )

#define CAGE_COLLISION_HALF_DIM  ( Vec2( 80.0f, 80.0f ) )

#define CAGE_FISH_MOVE_SPEED     ( 800.0f )
#define CAGE_FISH_MOVE_FRICTION  ( 4.0f )
#define CAGE_FISH_ESCAPE_DELAY_TARGET_TIME  ( 1.0f )

#define CAGE_HEALTH  ( 50 )

struct CAGE {
    boo32 isActive;
    vec2  position;
    vec2  velocity;
    vec2  halfDim;
    flo32 timer;
    
    int32 health;
};

#define CAGE_MAX_COUNT  ( 16 )
struct CAGE_STATE {
    uint32 nCage;
    CAGE    cage[ CAGE_MAX_COUNT ];
};

//----------
// NET
//----------

#define NET_FISH_MOVE_SPEED     ( 800.0f )
#define NET_FISH_MOVE_FRICTION  ( 4.0f )
#define NET_FISH_ESCAPE_DELAY_TARGET_TIME  ( 1.0f )

#define NET_BLOCK_HEALTH  ( 12 )

struct NET_COLLISION {
    rect A;
    rect B;
};

struct NET_BLOCK {
    flo32 timer;
    vec2  position;
    int32 health;
};

struct NET {
    boo32 isActive;
    
    NET_BLOCK blockA;
    NET_BLOCK blockB;
    
    SPRITE net;
    flo32  timer;
    
    boo32 isFree;
    vec2  position;
    vec2  velocity;
    vec2  halfDim;
};

#define NET_MAX_COUNT  ( 16 )
struct NET_STATE {
    uint32 nNet;
    NET    net[ NET_MAX_COUNT ];
};

//----------
// DEFEND ZONE
//----------

struct DEFEND_ZONE_STATE {
    boo32 isActive;
    
    // TODO: Currently, just treating DEFEND_ZONES as rects. Expand this to include any arbitrary polygon and multiple polygons!
    vec2  position;
    vec2  halfDim;
    
    int32 maxHealth;
    int32 health;
    
    TEXTURE_ID textureID;
    vec2       texture_offset;
    vec2       texture_scale;
};

//----------
// TOWER
//----------

#define TOWER_HEALTH  ( 50 )

struct TOWER {
    boo32 isActive;
    boo32 collision_isActive;
    rect  collision_bound;
    
    flo32 timer;
    
    int32 maxHealth;
    int32 health;
};

#define TOWER_MAX_COUNT  ( 8 )
struct TOWER_STATE {
    uint32 nTower;
    TOWER   tower[ TOWER_MAX_COUNT ];
};

//----------
// HEAL
//----------

struct HEAL {
    boo32 isActive;
    rect  bound;
};

#define HEAL_MAX_COUNT  ( 4 )
struct HEAL_STATE {
    uint32 nHeal;
    HEAL   heal[ HEAL_MAX_COUNT ];
};

//----------
// PARROTFISH
//----------

#define PARROTFISH_SLAP_COLLISION_RADIUS  ( 146.0f )
#define PARROTFISH_WINDUP_BITMAP_RADIUS  ( 540.0f )
#define PARROTFISH_HEADBUTT_RADIUS       ( 70.0f )
#define PARROTFISH_BUBBLE_COLLISION_RADIUS  ( 112.0f )
#define PARROTFISH_BUBBLET_COLLISION_RADIUS  ( 68.0f )

#define PARROTFISH_LGARCHER_STUN_RADIUS     ( 112.0f )
#define PARROTFISH_ARCHER_STUN_RADIUS       ( 82.0f )
#define PARROTFISH_CAN_STUN_OTHER_FISH_TARGET_TIME  ( 0.5f )
#define PARROTFISH_CAN_RICOCHET_TARGET_TIME         ( 0.35f )

#define PARROTFISH_STUNNED_SPEED        ( 1600.0f )
#define PARROTFISH_STUNNED_FRICTION     ( 4.0f )
#define PARROTFISH_STUNNED_TARGET_TIME  ( 1.6f )

#define PARROTFISH_EXIT_SPEED       ( 800.0f )

#define PARROTFISH_CAGE_COLLISION_DIM  ( Vec2( 320.0f, 220.0f ) )
#define PARROTFISH_NET_COLLISION_DIM   ( Vec2( 100.0f, 100.0f ) )
#define PARROTFISH_RICOCHET_DAMAGE     ( 10 )
#define PARROTFISH_RICOCHET_SPEED      ( 1000.0f )

#define PARROTFISH_BOUNCE_RADIUS  ( 120.0f )
#define PARROTFISH_BOUNCE_SPEED   ( 500.0f )

struct PARROTFISH {
    boo32 isActive;
    FISH_STATE state;
    
    vec2 position;
    vec2 velocity;
    vec2 direction;
    FACING_DIRECTION facingDir;
    
    flo32 timer;
    flo32 targetTime;
    
    int32 health;
    boo32 ricochet_canStunPlayer;
};

#define PARROTFISH_MAX_COUNT  ( 8 )
struct PARROTFISH_STATE {
    uint32     nParrotfish;
    PARROTFISH  parrotfish[ PARROTFISH_MAX_COUNT ];
};

//----------
// OBSTACLE
//----------

struct OBSTACLE {
    LINE_SEG   bound;
    TEXTURE_ID textureID;
};

struct OBSTACLE_GROUP {
    uint32 obsID; // TODO: temp! this is cellID!
    uint32 nObs;
    uint32 bObs;
};

#define OBSTACLE_MAX_COUNT  ( 256 )
struct OBSTACLE_STATE {
    uint32  nObstacle;
    OBSTACLE obstacle[ OBSTACLE_MAX_COUNT ];
};

//----------
// BREAKABLE
//----------

#define BREAKABLE_SHAKE_TARGET_TIME    ( 0.25f )
#define BREAKABLE_SHAKE_BITMAP_OFFSET  ( 5.0f )
#define BREAKABLE_FADE_TARGET_TIME   ( 1.0f  )

#define BREAKABLE_FADE_FRICTION      ( 4.0f )

struct BREAKABLE {
    boo32 isActive;
    flo32 timer;
    vec2  velocity;
    
    TEXTURE_ID textureID;
    
    uint32 maxHealth;
    uint32 health;
    orect  collision_bound;
    boo32  collision_isActive;
};

#define BREAKABLE_MAX_COUNT  ( 8 )
struct BREAKABLE_STATE {
    uint32   nBreakable;
    BREAKABLE breakable[ BREAKABLE_MAX_COUNT ];
};

//----------
// BACKGROUND
//----------

struct BACKGROUND {
    vec2       position;
    vec2       scale;
    flo32      radians;
    MODEL_ID   modelID;
    TEXTURE_ID textureID;
    vec4       modColor;
    flo32      depth;
};

struct BACKGROUND_GROUP {
    uint32 backID;
    uint32 nBack;
    uint32 bBack;
};

#define BACKGROUND_MAX_COUNT  ( 512 )
struct BACKGROUND_STATE {
    uint32    nBackground;
    BACKGROUND background[ BACKGROUND_MAX_COUNT ];
};

//----------
// EVENT TRIGGER
//----------

struct EVENT_TRIGGER {
    uint32 eventID;
    rect   bound;
};

#define EVENT_TRIGGER_MAX_COUNT  ( 128 )
struct EVENT_TRIGGER_STATE {
    uint32       nEventTrigger;
    EVENT_TRIGGER eventTrigger[ EVENT_TRIGGER_MAX_COUNT ];
};

//----------
// PLAYER
//----------

#define EXIT_BOUND_TARGET_TIME  ( 1.2f )
#define EXIT_BOUND_DIM          ( Vec2( 0.3f,  0.3f ) )
#define EXIT_BOUND_POS          ( Vec2( 1.45f, 0.5f ) )

#define PLAYER_BITMAP_SCALE_X   ( 80.0f )
#define PLAYER_BITMAP_SCALE_Y   ( 80.0f )
#define PLAYER_BITMAP_OFFSET_X  ( -14.0f )
#define PLAYER_BITMAP_OFFSET_Y  (  14.0f )
#define PLAYER_BITMAP_OFFSET  ( Vec2( -14.0f, 14.0f ) )
#define PLAYER_ANIM_BITE_OPEN_TARGET_TIME   (  6.0f / 60.0f )
#define PLAYER_ANIM_BITE_CLOSE_TARGET_TIME  ( 15.0f / 60.0f )
//#define PLAYER_PARRY_TARGET_TIME      ( 18.0f / 60.0f )
#define PLAYER_DEFEND_TARGET_TIME     ( 18.0f / 60.0f )
#define PLAYER_DEFEND_ANIMATION_TIME  ( 27.0f / 60.0f )
#define PLAYER_PARRY_COOLDOWN_TIME    (  6.0f / 60.0f )
#define PLAYER_MAX_MOVEMENT_DIST_FOR_DEFEND_AND_STUN  ( 4.0f )

#define PLAYER_HEAL_AMOUNT              ( 0.4f )
#define PLAYER_FIRST_HEAL_TARGET_TIME   ( 3.4f )
#define PLAYER_MULTI_HEAL_TARGET_TIME   ( 2.4f )
#define PLAYER_SMTURTLE_HEAL_RADIUS     ( 120.0f )
#define PLAYER_WAS_DAMAGED_TARGET_TIME  ( 0.25f )

#define PLAYER_STUNNED_FRICTION        ( 8.0f )
#define PLAYER_STUNNED_HI_SPEED        ( 2000.0f )
#define PLAYER_STUNNED_HI_TARGET_TIME  ( 48.0f / 60.0f )
#define PLAYER_STUNNED_LO_TARGET_TIME  ( 24.0f / 60.0f )

#define PLAYER__BUBBLET_HIT_COLLISION_OFFSET  Vec2( -16.0f, 8.0f )
#define PLAYER__BUBBLET_HIT_COLLISION_RADIUS  ( 44.0f )

#define PLAYER__BUBBLON_HIT_COLLISION_OFFSET  Vec2( -16.0f, 8.0f )
#define PLAYER__BUBBLON_HIT_COLLISION_RADIUS  ( 44.0f )

#define PLAYER__ARCHER_BITE_COLLISION_OFFSET  Vec2( 0.0f, 0.0f )
#define PLAYER__ARCHER_BITE_COLLISION_RADIUS  ( 48.0f )

#define PLAYER__LGARCHER_BITE_COLLISION_OFFSET  Vec2( 0.0f, 0.0f )
#define PLAYER__LGARCHER_BITE_COLLISION_RADIUS  ( 68.0f )

#define PLAYER_FLASH_LOW_HEALTH_THRESHOLD    ( 0.25f )
#define PLAYER_FLASH_LOW_HEALTH_TARGET_TIME  ( 0.6f )

#define EXIT_ARROW_TARGET_TIME  ( 2.0f )

struct PLAYER_STATE {
    flo32 timer;
    vec2  position;
    vec2  direction;
    vec2  velocity;
    FACING_DIRECTION facingDir;
    
    vec2 dPos;
    
    boo32 doSlap;
    
    boo32 bite_isActive;
    //boo32 parry_isActive;
    boo32 defend_isActive;
    boo32 defendAnim_isActive;
    boo32 useItem_isActive;
    
    boo32 bitByDogfish;
    DOGFISH * dogfish;
    flo32     dogfish_escape;
    vec2      dogfish_escapeDir;
    flo32     dogfish_timer;
    
    boo32 heldByBubble;
    BUBBLE * bubble;
    flo32    bubble_escape;
    vec2     bubble_escapeDir;
    
    flo32   health;
    boo32   attemptHeal;
    flo32   health_prev;
    flo32   health_timer;
    
    flo32  dodge_timer;
    uint32 nDodges;
    
    boo32 initStunnedHi;
    boo32 isStunnedHi;
    
    boo32 initStunnedLo;
    boo32 isStunnedLo;
    
    flo32 wasDamaged_timer;
    
    vec2  stun_position;
    vec2  stun_direction;
    vec2  stun_particlePos;
    flo32 stun_damage;
};

// TODO: player should not be able to move while defending (or at least not very much)