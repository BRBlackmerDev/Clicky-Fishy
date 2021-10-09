
#define DEBUG_FRAME_CAPTURE_STATE( TYPE, Type ) \
internal void \
addStateToDebug( DEBUG_FRAME * frame, MEMORY * memory, TYPE##_STATE * state ) { \
++frame->n##Type; \
TYPE##_STATE * at = _pushType( memory, TYPE##_STATE ); \
memcpy( at, state, sizeof( TYPE##_STATE ) ); \
}

#define DEBUG_FRAME_CAPTURE_FUNC( TYPE, Type, type ) \
internal void \
addToDebug( DEBUG_FRAME * frame, MEMORY * memory, TYPE##_STATE * state ) { \
for( uint32 iFish = 0; iFish < TYPE##_MAX_COUNT; iFish++ ) { \
TYPE * fish = state->##type + iFish; \
if( fish->isActive ) { \
++frame->n##Type; \
TYPE * at = _pushType( memory, TYPE ); \
memcpy( at, fish, sizeof( TYPE ) ); \
} \
} \
}

#define DEBUG_FRAME_CAPTURE_FUNCN( TYPE, Type, type ) \
internal void \
addToDebug( DEBUG_FRAME * frame, MEMORY * memory, TYPE##_STATE * state ) { \
for( uint32 iFish = 0; iFish < state->n##Type; iFish++ ) { \
TYPE * fish = state->##type + iFish; \
++frame->n##Type; \
TYPE * at = _pushType( memory, TYPE ); \
memcpy( at, fish, sizeof( TYPE ) ); \
} \
}

//DISPLAY_VALUE( uint8, frame.n##Type );
#define DEBUG_FRAME_DRAW_LOOP( TYPE, Type ) \
for( uint32 iFish = 0; iFish < frame.n##Type; iFish++ ) { \
TYPE * fish = _addr( at, TYPE ); \
draw##Type( pass_game, appState, fish, true ); \
}

//DISPLAY_VALUE( uint8, frame.n##Type );
#define DEBUG_FRAME_DRAW_STATE( TYPE, Type ) \
if( frame.n##Type > 0 ) { \
TYPE##_STATE * fish = _addr( at, TYPE##_STATE ); \
draw##Type( pass_game, appState, fish, true ); \
}

DEBUG_FRAME_CAPTURE_STATE( DEFEND_ZONE, DefendZone );
DEBUG_FRAME_CAPTURE_STATE( TURTLE, Turtle );
DEBUG_FRAME_CAPTURE_FUNCN( OBSTACLE, Obstacle, obstacle );
DEBUG_FRAME_CAPTURE_FUNC( TOWER, Tower, tower );
DEBUG_FRAME_CAPTURE_FUNC( CAGE, Cage, cage );
DEBUG_FRAME_CAPTURE_FUNC( NET, Net, net );
DEBUG_FRAME_CAPTURE_STATE( PLAYER, Player );
DEBUG_FRAME_CAPTURE_FUNC( RAINBOW,      Rainbow,     rainbow     );
DEBUG_FRAME_CAPTURE_FUNC( GOLDFISH,     Goldfish,    goldfish    );
DEBUG_FRAME_CAPTURE_FUNC( DOGFISH,      Dogfish,     dogfish     );
DEBUG_FRAME_CAPTURE_FUNC( SWORDFISH,    Swordfish,   swordfish   );
DEBUG_FRAME_CAPTURE_FUNC( URCHIN,       Urchin,      urchin      );
DEBUG_FRAME_CAPTURE_FUNC( URCHIN_SPINE, UrchinSpine, urchinSpine );
DEBUG_FRAME_CAPTURE_FUNC( ARCHER,       Archer,      archer      );
DEBUG_FRAME_CAPTURE_FUNC( BUBBLET,      Bubblet,     bubblet     );
DEBUG_FRAME_CAPTURE_FUNC( LGARCHER,     lgArcher,    lgArcher    );
DEBUG_FRAME_CAPTURE_FUNC( BUBBLE,       Bubble,      bubble      );
DEBUG_FRAME_CAPTURE_FUNC( BUBBLON,      Bubblon,     bubblon     );
DEBUG_FRAME_CAPTURE_FUNC( PARROTFISH,   Parrotfish,  parrotfish  );
DEBUG_FRAME_CAPTURE_FUNC( SMTURTLE,     smTurtle,    smTurtle    );

internal void
captureDebugFrame( APP_STATE * appState ) {
    DEBUG_FRAME_SYSTEM * debug = &appState->debugFrameSystem;
    if( !debug->show ) {
        DEBUG_FRAME * frame = debug->frame + debug->atFrame;
        memset( frame, 0, sizeof( DEBUG_FRAME ) );
        
        frame->memory_at = getUsed( &debug->memory );
        
        frame->frame_counter = appState->frame_counter;
        
        addStateToDebug( frame, &debug->memory, &appState->defendZoneState );
        addStateToDebug( frame, &debug->memory, &appState->turtleState      );
        addToDebug( frame, &debug->memory, &appState->obstacleState    );
        addToDebug( frame, &debug->memory, &appState->towerState       );
        addToDebug( frame, &debug->memory, &appState->cageState        );
        addToDebug( frame, &debug->memory, &appState->netState         );
        addStateToDebug( frame, &debug->memory, &appState->playerState      );
        addToDebug( frame, &debug->memory, &appState->rainbowState     );
        addToDebug( frame, &debug->memory, &appState->goldfishState    );
        addToDebug( frame, &debug->memory, &appState->dogfishState     );
        addToDebug( frame, &debug->memory, &appState->swordfishState   );
        addToDebug( frame, &debug->memory, &appState->urchinState      );
        addToDebug( frame, &debug->memory, &appState->urchinSpineState );
        addToDebug( frame, &debug->memory, &appState->archerState      );
        addToDebug( frame, &debug->memory, &appState->bubbletState     );
        addToDebug( frame, &debug->memory, &appState->lgArcherState    );
        addToDebug( frame, &debug->memory, &appState->bubbleState      );
        addToDebug( frame, &debug->memory, &appState->bubblonState     );
        addToDebug( frame, &debug->memory, &appState->parrotfishState  );
        addToDebug( frame, &debug->memory, &appState->smTurtleState  );
        
        debug->atFrame = ( debug->atFrame + 1 ) % DEBUG__FRAME_COUNT;
        if( debug->atFrame == 0 ) {
            debug->memory.used = 0;
        }
    }
    
    //DISPLAY_VALUE( uint32, debug->atFrame );
}

internal void
drawDebugFrame( RENDERER * renderer, APP_STATE * appState, MOUSE_STATE * mouse, KEYBOARD_STATE * keyboard ) {
    RENDER_PASS * pass_game = &renderer->pass_game;
    RENDER_PASS * pass_ui   = &renderer->pass_ui;
    
    DEBUG_FRAME_SYSTEM * debug = &appState->debugFrameSystem;
    //DISPLAY_VALUE( uint32, debug->atFrame );
    
    if( debug->show ) {
        drawRect( pass_game, textureID_whiteTexture, appState->cameraBound, COLOR_WHITE );
        
        if( wasPressed( mouse, mouseButton_left ) ) {
            debug->showLine = true;
            debug->showLine_P = mouse->position;
        }
        if( wasReleased( mouse, mouseButton_left ) ) {
            debug->showLine = false;
        }
        if( wasPressed( mouse, mouseButton_right ) ) {
            debug->pause = !debug->pause;
        }
        
        if( !debug->pause ) {
            flo32 t = clamp01( mouse->position.x / appState->app_dim.x );
            debug->show_atFrame = ( ( uint32 )( t * ( flo32 )DEBUG__FRAME_COUNT ) + debug->atFrame ) % DEBUG__FRAME_COUNT;
            DISPLAY_VALUE( uint32, debug->show_atFrame );
        }
        
        uint32 index = debug->show_atFrame;
        
        DEBUG_FRAME frame = debug->frame[ index ];
        uint8 * at = frame.memory_at;
        
        // NOTE: Very important! DEBUG_FRAME expects all entities to be drawn in the same order that they are added to the debug system! Otherwise, errors will occur!
        DEBUG_FRAME_DRAW_STATE( DEFEND_ZONE, DefendZone );
        DEBUG_FRAME_DRAW_STATE( TURTLE, Turtle );
        DEBUG_FRAME_DRAW_LOOP( OBSTACLE,     Obstacle    );
        DEBUG_FRAME_DRAW_LOOP( TOWER,        Tower       );
        DEBUG_FRAME_DRAW_LOOP( CAGE,         Cage        );
        DEBUG_FRAME_DRAW_LOOP( NET,          Net         );
        DEBUG_FRAME_DRAW_STATE( PLAYER, Player );
        DEBUG_FRAME_DRAW_LOOP( RAINBOW,      Rainbow     );
        DEBUG_FRAME_DRAW_LOOP( GOLDFISH,     Goldfish    );
        DEBUG_FRAME_DRAW_LOOP( DOGFISH,      Dogfish     );
        DEBUG_FRAME_DRAW_LOOP( SWORDFISH,    Swordfish   );
        DEBUG_FRAME_DRAW_LOOP( URCHIN,       Urchin      );
        DEBUG_FRAME_DRAW_LOOP( URCHIN_SPINE, UrchinSpine );
        DEBUG_FRAME_DRAW_LOOP( ARCHER,       Archer      );
        DEBUG_FRAME_DRAW_LOOP( BUBBLET,      Bubblet     );
        DEBUG_FRAME_DRAW_LOOP( LGARCHER,     lgArcher    );
        DEBUG_FRAME_DRAW_LOOP( BUBBLE,       Bubble      );
        DEBUG_FRAME_DRAW_LOOP( BUBBLON,      Bubblon     );
        DEBUG_FRAME_DRAW_LOOP( PARROTFISH,   Parrotfish  );
        DEBUG_FRAME_DRAW_LOOP( SMTURTLE,     smTurtle    );
        
        //SWORDFISH fish = debug->swordfish[ index ];
        //DISPLAY_VALUE( vec2, fish.position );
        
        //drawSwordfish( pass, appState, debug->swordfish[ index ], true, mouse );
        
        //PLAYER_STATE * player = _addr( at, PLAYER_STATE );
        //drawPlayer( pass, appState, player, true );
        
        rect bound = rectCD( mouse->position, Vec2( 4.0f, 4.0f ) );
        drawRect( pass_ui, textureID_whiteTexture, bound, COLOR_GREEN );
        
        DISPLAY_VALUE( uint32, frame.frame_counter );
        DISPLAY_VALUE( vec2, mouse->position );
        if( debug->showLine ) {
            drawLine( pass_ui, debug->showLine_P, mouse->position, COLOR_CYAN );
            DISPLAY_VALUE( vec2, debug->showLine_P );
            
            vec2  dPos   = mouse->position - debug->showLine_P;
            flo32 length = getLength( dPos );
            
            char str[ 128 ] = {};
            sprintf( str, "( %f, %f ) : %f", dPos.x, dPos.y, length );
            DISPLAY_STRING( str );
        }
    }
}