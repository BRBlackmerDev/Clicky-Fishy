
#define REGULAR_GAME ( 0 )
#define BOSS_FIGHT   ( 1 )

internal void
loadGameScenarios( MEMORY * tempMemory, APP_STATE * appState ) {
    // TODO: NOTE: Game Area will be locked to an aspect ratio of 1.65
    // TODO: NOTE: Regarding animation positions, visible Game Area will be expressed as Vec2( [0..1], [0..1.65] ). Any animation positions outside this range will be drawn off screen.
    // TODO: NOTE: This means, for example, 0.05 units = 54 pixels
    
    uint32 result = 1; // set to event 1 by default because event 0 is the sentinel 'start game' event
    
    GAME_CONTROL_STATE * controlState = &appState->gameControlState;
    MEMORY * mem = &controlState->memory;
    GAME_CONTROL__EVENT * event = 0;
    GAME_CONTROL__SPAWN * spawn = 0;
    controlState->world_scale = 1080.0f;
    
    ANIMATION_SYSTEM * state = &appState->animationSystem;
    ANIMATION    * animation    = 0;
    SUBANIMATION * subanimation = 0;
    
    WORLD_SYSTEM * world = &appState->worldSystem;
    
    // NOTE: This event is treated as a sentinel 'start game' event and should not be removed!
    event = nextEvent( controlState );
    event->type = eventType_beginGame;
    event->exit = exitCondition_instant;
    
    // TODO: music : village under attack
    
    { // init background
        vec2 atPos = Vec2( 1.0f, 0.6f );
        
        for( uint32 iter = 0; iter < 5; iter++ ) {
            BACKGROUND back = {};
            back.position  = atPos;
            back.scale     = Vec2( 1.0f, 1.0f );
            back.radians   = 0.0f;
            back.modelID   = modelID_rect;
            back.textureID = ( TEXTURE_ID )( ( uint32 )textureID_background_left + ( iter % 4 ) );
            back.modColor  = COLOR_WHITE;
            back.depth     = 1.0f;
            
            addBackground( controlState, world, back );
            
            atPos.x += 2.0f;
        }
    }
    
    firstEvent( controlState );
    
#if REGULAR_GAME
    nextEvent_newAnim( controlState );
    { // animation : peasant run away screaming
        animation = newAnimation( state, mem );
        animation->isCameraRel = true;
        
        vec2 offset = Vec2( 0.04f, 0.04f );
        vec2 posA = Vec2( 1.75f, 0.5f );
        vec2 posB = Vec2( -0.1f, 0.5f );
        vec2 posC = lerp( posA, 1.0f / 6.0f, posB ) + offset;
        vec2 posD = lerp( posA, 3.0f / 6.0f, posB ) + offset;
        vec2 posE = lerp( posA, 4.0f / 6.0f, posB ) + offset;
        vec2 posF = lerp( posA, 6.0f / 6.0f, posB ) + offset;
        
        subanimation = newSubAnimation( state, animation );
        addKeyframe( state, textureID_peasant_idle, 2.0f, posA, Vec2( -35.0f, 35.0f ), 0.0f, COLOR_WHITE );
        nextKey( state, 8.0f, posB );
        
        char * strA = add( &state->stringBuffer, "AAUUUGGH!!" );
        char * strB = add( &state->stringBuffer, "Swim away!!" );
        char * strC = add( &state->stringBuffer, "Hey! Hold it right there!" );
        
        subanimation = newSubAnimation( state, animation );
        addKeyframe( state, strA, textAlign_center, 3.0f, posC, Vec2( 1.5f, 1.5f ), 0.0f, COLOR_BLACK, true );
        nextKey( state, 5.0f,       posD, false );
        nextKey( state, 6.0f, strB, posE, true  );
        nextKey( state, 8.0f,       posF, false );
        
        // animation : single archer 'Hey! Hold it right there!'
        vec2 posG = Vec2( 1.62f, 0.75f );
        subanimation = newSubAnimation( state, animation );
        addHold( state, strC, textAlign_centerRight, 7.0f, 10.0f, posG, 1.5f );
        
        endAnimation( state );
    }
    
    nextEvent_moveCamerah( controlState, 10.0f, Vec2( 0.75f, 0.0f ) );
    nextEvent_spawnArcherh( controlState, 2, 0 );
    
    nextEvent_newAnimh( controlState );
    { // animation : archers 'Hey, what're you doing?!' 'Get him!'
        animation = newAnimation( state, mem );
        animation->isCameraRel = true;
        
        char * strA = add( &state->stringBuffer, "Hey!" );
        char * strB = add( &state->stringBuffer, "What're you doing?!" );
        char * strC = add( &state->stringBuffer, "Get him!" );
        
        vec2 posA = Vec2( 1.0f, 0.91f );
        vec2 posB = Vec2( 0.5f, 0.92f );
        vec2 posC = Vec2( 0.825f, 0.9f );
        
        subanimation = newSubAnimation( state, animation );
        addHold( state, strA, textAlign_center, 0.5f, 1.5f, posA, 1.5f );
        
        subanimation = newSubAnimation( state, animation );
        addHold( state, strB, textAlign_center, 1.25f, 2.25, posB, 1.5f );
        
        subanimation = newSubAnimation( state, animation );
        addHold( state, strC, textAlign_center, 2.0f, 3.5f, posC, 2.5f );
        endAnimation( state );
    }
    addCheckpoint( controlState );
    
    nextEvent_spawnArcherh( controlState, 3, 0 );
    addCheckpoint( controlState );
    
    nextEvent_spawnArcherh( controlState, 5, 0 );
    nextEvent_exit( controlState );
    addCheckpoint( controlState );
    
    nextEvent_spawnw( controlState );
    spawnNet( controlState, Vec2( 1.85f, 0.05f ) );
    endSpawn( controlState );
    
    event = newChain( controlState );
    uint32 netIntro_index = event->eventIndex;
    event->type = eventType_animation;
    event->exit = exitCondition_netsAtHPOrLower;
    event->atHP = 0.5f;
    { // animation : peasants trapped in net 'Help us!' 'Get us out!'
        animation = newAnimation( state, mem );
        animation->isCameraRel = true;
        
        char * strA = add( &state->stringBuffer, "Help us!" );
        char * strB = add( &state->stringBuffer, "Get us out!" );
        
        vec2 posA = Vec2( 0.7f, 0.25f );
        vec2 posB = Vec2( 0.55f, 0.24f );
        
        subanimation = newSubAnimation( state, animation );
        addHold( state, strA, textAlign_center, 0.5f, 2.0f, posA, 1.5f );
        
        subanimation = newSubAnimation( state, animation );
        addHold( state, strB, textAlign_center, 2.25f, 4.0f, posB, 1.5f );
        endAnimation( state );
    }
    
    event = nextEvent( controlState );
    event->type = eventType_animation;
    event->exit = exitCondition_targetTime;
    event->targetTime = 1.0f;
    { // animation : archers 'Stop him! Don't let him get away!'
        animation = newAnimation( state, mem );
        animation->isCameraRel = true;
        
        char * strA = add( &state->stringBuffer, "Stop him!" );
        char * strB = add( &state->stringBuffer, "Don't let him get away!" );
        
        vec2 posA = Vec2( 1.4f, 0.9f  );
        vec2 posB = Vec2( 1.3f, 0.88f );
        
        subanimation = newSubAnimation( state, animation );
        addHold( state, strA, textAlign_center, 0.5f, 1.5f, posA, 1.5f );
        
        subanimation = newSubAnimation( state, animation );
        addHold( state, strB, textAlign_center, 1.75f, 3.0f, posB, 2.0f );
        
        endAnimation( state );
    }
    
    nextEvent_spawnArcher( controlState, 4, 0 );
    endChain( controlState );
    
    nextEvent_moveCamerah( controlState, 1.0f, Vec2( 1.25f, -0.1f ) );
    event = nextEvent_noEnemies( controlState );
    event->subEventIndex = netIntro_index;
    
    nextEvent_spawnw( controlState );
    spawnNet( controlState, Vec2( 2.05f, 0.15f ) );
    endSpawn( controlState );
    
    nextEvent_moveCamerah ( controlState, 1.0f, Vec2( 1.0f, -0.05f ) );
    nextEvent_spawnArcher( controlState, 6, 0 );
    nextEvent_noEnemies( controlState );
    
    nextEvent_exit( controlState );
    addCheckpoint( controlState );
    
    nextEvent_spawnw( controlState );
    spawnNet( controlState, Vec2( 1.93f, 0.15f ) );
    spawnNet( controlState, Vec2( 3.04f, 0.15f ) );
    endSpawn( controlState );
    nextEvent_moveCamerah( controlState, 1.0f, Vec2( 1.65f, -0.05f ) );
    
    event = nextEvent_newAnim( controlState );
    event->exit = exitCondition_targetTime;
    event->targetTime = 3.0f;
    { // animation : peasants trapped in nets 'Yay! He's come to save us! Uh-oh!'
        animation = newAnimation( state, mem );
        animation->isCameraRel = true;
        
        char * strA = add( &state->stringBuffer, "Yay! He's come to save us!" );
        char * strB = add( &state->stringBuffer, "Uh-oh!" );
        
        vec2 posA = Vec2( 1.25f, 0.28f );
        vec2 posB = Vec2( 1.18f, 0.29f );
        
        subanimation = newSubAnimation( state, animation );
        addHold( state, strA, textAlign_center, 0.5f, 2.0f, posA, 1.5f );
        
        subanimation = newSubAnimation( state, animation );
        addHold( state, strB, textAlign_center, 2.25f, 2.75f, posB, 2.0f );
        endAnimation( state );
    }
    nextEvent_spawnArcher( controlState, 10, 0 );
    nextEvent_noEnemies( controlState );
    
    nextEvent_spawnw( controlState );
    spawnNet( controlState, Vec2( 2.8f, 0.12f ) );
    endSpawn( controlState );
    nextEvent_hold( controlState, 1.0f );
    nextEvent_moveCamera( controlState, 65.0f, Vec2( 3.2f, 0.0f ) );
    
    nextEvent_hold( controlState, 4.0f );
    nextEvent_spawnArcher( controlState, 6, 0 );
    nextEvent_hold( controlState, 16.0f );
    nextEvent_spawnArcher( controlState, 5, 0 );
    nextEvent_hold( controlState, 20.0f );
    nextEvent_spawnArcher( controlState, 7, 0 );
    nextEvent_hold( controlState, 24.0f );
    
    nextEvent_newAnim( controlState );
    { // animation : giant shadow passes overhead
        animation = newAnimation( state, mem );
        animation->isCameraRel = true;
        
        char * strA = add( &state->stringBuffer, "Enough of this!!" );
        char * strB = add( &state->stringBuffer, "Auuughh! Swim away!" );
        
        vec2 posA = Vec2( 0.825f, 0.1f );
        
        subanimation = newSubAnimation( state, animation );
        addHold( state, strA, textAlign_center, 0.5f, 2.5f, posA, 2.0f );
        addHold( state, strB, textAlign_center, 7.5f, 10.5f, posA, 2.0f );
        
        vec2 posB = Vec2(  2.65f, 0.5f );
        vec2 posC = Vec2( -0.86f, 0.5f );
        
        subanimation = newSubAnimation( state, animation );
        vec4 color = Vec4( 1.0f, 1.0f, 1.0f, 0.25f );
        addKeyframe( state, textureID_turtle_shadow, 8.0f, posB, Vec2( 1100.0f, 600.0f ), 0.0f, color );
        nextKey( state, 14.0f, posC );
        addCameraShake( state, 6.0f, 60.0f, Vec2( 15.0f, 15.0f ), 3.0f, 0.0f, 3.0f );
        // TODO: add rumble sound
        
        endAnimation( state );
    }
    
    nextEvent_hold( controlState, 2.0f );
    nextEvent_spawnArcher( controlState, 16, 4 );
    nextEvent_hold( controlState, 5.0f );
    
    nextEvent_exitEnemies( controlState, 5.5f );
    //firstEvent( controlState );
    
    vec2 basePos = {};
    setBasePos( controlState, Vec2( 1.4f, -1.9f ) );
    nextEvent_spawnw( controlState );
    { // cave 01
        uint32 nObs       = 0;
        uint32 maxObs     = GAME_CONTROL__OBSTACLE_MAX_COUNT;
        LINE_SEG * obs = _pushArray_clear( tempMemory, LINE_SEG, maxObs );
        
        obs[ nObs++ ] = LineSegQP( Vec2( 0.0f, 0.0f ), 1.0f, -PI * 33.0f / 64.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 0 ].P, 0.55f, -PI * 17.0f / 32.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 1 ].P, 0.25f, -PI / 3.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 2 ].P, 0.25f,  PI / 10.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 3 ].P, 1.0f,  -PI / 5.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 4 ].P, 1.0f,  -PI / 16.0f );
        basePos = obs[ 5 ].P;
        
        obs[ nObs++ ] = LineSeg( obs[ 0 ].Q + toVec2( 1.1f, -PI * 63.0f / 64.0f ), obs[ 1 ].Q + toVec2( 0.7f, -PI * 31.0f / 32.0f ) );
        obs[ nObs++ ] = LineSeg( obs[ 6 ].Q, 1.0f, -PI * 15.0f / 32.0f );
        obs[ nObs++ ] = LineSeg( obs[ 7 ].Q, 0.6f, -PI * 7.0f / 32.0f );
        obs[ nObs++ ] = LineSeg( obs[ 8 ].Q, obs[ 4 ].P + toVec2( 0.45f, -PI * 33.0f / 64.0f ) );
        obs[ nObs++ ] = LineSeg( obs[ 9 ].Q, obs[ 5 ].P + Vec2( 0.0f, -0.25f ) );
        
        obs[ nObs++ ] = LineSeg  ( obs[ 0 ].Q, 1.5f, PI / 16.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 6 ].P, 1.5f, PI * 15.0f / 16.0f );
        
        for( uint32 iObs = 0; iObs < nObs; iObs++ ) {
            addObstacle( controlState, world, Obstacle( obs[ iObs ], textureID_whiteTexture ) );
        }
        
        _popArray( tempMemory, LINE_SEG, maxObs );
    }
    { // cave 01 : background
        addBackgroundRect( controlState, world, textureID_background_sandToCave, Rect( -3.0f, -0.1f, 2.0f, 1.8f ), COLOR_GRAY( 0.3f ), 0.9999f );
        addCaveWall( controlState, world, Vec2( 1.584528f, -2.521376f ), Vec2( 0.469397f, 0.608123f ), 0.004095f );
        addCaveWall( controlState, world, Vec2( 0.617482f, -2.272327f ), Vec2( 0.517935f, 0.606577f ), 0.004095f );
        addCaveWall( controlState, world, Vec2( -0.264176f, -2.487426f ), Vec2( 0.473422f, 0.507936f ), 0.004095f );
        addCaveWall( controlState, world, Vec2( -0.404047f, -1.469934f ), Vec2( 0.512073f, 0.516733f ), 0.004095f );
        addCaveWall( controlState, world, Vec2( -0.486760f, -0.503439f ), Vec2( 0.641566f, 0.476297f ), 0.050052f );
        addCaveRock( controlState, world, Vec2( 0.325045f, -0.516571f ), Vec2( 0.500000f, 0.350000f ), 1.521709f );
        addCaveRock( controlState, world, Vec2( 1.049384f, -1.020863f ), Vec2( 0.528749f, 0.677831f ), 0.000000f );
        addCaveRock( controlState, world, Vec2( 0.275680f, -1.272378f ), Vec2( 0.309566f, 0.350000f ), 1.472621f );
        addCaveRock( controlState, world, Vec2( 0.073593f, -1.588542f ), Vec2( 0.125000f, 0.131717f ), 2.094395f );
        addCaveRock( controlState, world, Vec2( 0.104984f, -1.613474f ), Vec2( 0.125000f, 0.116242f ), -2.827433f );
        addCaveRock( controlState, world, Vec2( 0.816828f, -1.657489f ), Vec2( 0.565749f, 0.350000f ), 2.513274f );
        addCaveRock( controlState, world, Vec2( 1.642627f, -1.951294f ), Vec2( 0.500000f, 0.427654f ), 2.945243f );
        addCaveRock( controlState, world, Vec2( -1.252711f, -0.675812f ), Vec2( 0.536573f, 0.350000f ), -1.235637f );
        addCaveRock( controlState, world, Vec2( -0.741756f, -2.819204f ), Vec2( 0.698194f, 0.644551f ), -0.687223f );
        addCaveRock( controlState, world, Vec2( -1.030892f, -1.742574f ), Vec2( 0.643962f, 0.350000f ), -1.472622f );
        addCaveRock( controlState, world, Vec2( 0.331995f, -3.175631f ), Vec2( 0.741219f, 0.599999f ), -0.223270f );
        addCaveRock( controlState, world, Vec2( 1.463284f, -3.321030f ), Vec2( 0.588925f, 0.600196f ), 0.004355f );
        addCaveRock( controlState, world, Vec2( 0.803871f, -0.196957f ), Vec2( 0.750000f, 0.350000f ), 0.196350f );
        addCaveRock( controlState, world, Vec2( -1.969393f, -1.183928f ), Vec2( 0.738387f, 0.766343f ), 0.000000f );
        addCaveRock( controlState, world, Vec2( -1.873892f, -2.704964f ), Vec2( 0.829233f, 0.791965f ), 0.000000f );
        addCaveRock( controlState, world, Vec2( -1.902546f, -0.250931f ), Vec2( 0.750000f, 0.350000f ), -0.196350f );
    }
    spawnItem( controlState, itemID_heal, Vec2( 0.25f, -1.72f ) );
    endSpawn( controlState );
    
    nextEvent_setCameraBound( controlState, Rect( -2.0f, -20.0f, 20.0f, -1.0f ) );
    nextEvent_moveCamerah( controlState, 26.0f, Vec2( -0.475f, -1.74f ) - Vec2( 0.825f, 0.0f ), false );
    nextEvent_enableCameraSpring( controlState );
    setBasePos( controlState, basePos );
    
    nextEvent_spawnw( controlState );
    { // cave 02
        uint32 nObs       = 0;
        uint32 maxObs     = GAME_CONTROL__OBSTACLE_MAX_COUNT;
        LINE_SEG * obs = _pushArray_clear( tempMemory, LINE_SEG, maxObs );
        
        obs[ nObs++ ] = LineSegQP( Vec2( 0.0f, 0.0f ), 1.25f, -PI / 24.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 0 ].P,         1.25f,  PI / 24.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 1 ].P, 0.35f,  PI / 4.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 2 ].P, 0.85f,  PI / 24.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 3 ].P, 0.85f,  PI / 3.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 4 ].P, 1.5f,   PI / 24.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 5 ].P, 1.5f,  -PI / 12.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 6 ].P, 1.35f, -PI *  5.0f / 12.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 7 ].P, 0.15f, -PI * 11.0f / 12.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 8 ].P, 1.26f,  PI * 29.0f / 48.0f );
        obs[ nObs++ ] = LineSeg( obs[ 5 ].P + Vec2( 0.0f,  -0.25f ), obs[  9 ].P );
        obs[ nObs++ ] = LineSeg( obs[ 4 ].P + Vec2( 0.25f, -0.25f ), obs[ 10 ].P );
        obs[ nObs++ ] = LineSeg( obs[ 3 ].P + Vec2( 0.5f,  -0.1f ),  obs[ 11 ].P );
        
        
        obs[ nObs++ ] = LineSeg( obs[ 0 ].Q + Vec2( 0.0f, -0.25f ), obs[ 0 ].P + Vec2( -0.01f, -0.25f ) );
        obs[ nObs++ ] = LineSeg( obs[ 13 ].Q, obs[ 1 ].P + Vec2( 0.1f, -0.25f ) );
        obs[ nObs++ ] = LineSeg( obs[ 14 ].Q, 0.4f,  -PI / 4.0f );
        obs[ nObs++ ] = LineSeg( obs[ 15 ].Q, 1.1f,  -PI * 25.0f / 48.0f );
        obs[ nObs++ ] = LineSeg( obs[ 16 ].Q, 1.1f,  -PI * 3.0f / 4.0f );
        obs[ nObs++ ] = LineSeg( obs[ 17 ].Q, 1.15f, -PI * 23.0f / 48.0f );
        obs[ nObs++ ] = LineSeg( obs[ 18 ].Q, 0.15f, 0.0f );
        obs[ nObs++ ] = LineSeg( obs[ 19 ].Q, obs[ 18 ].P + Vec2( 0.2f, -0.1f ) );
        obs[ nObs++ ] = LineSeg( obs[ 20 ].Q, obs[ 17 ].P + Vec2( 0.2f, -0.2f ) );
        obs[ nObs++ ] = LineSeg( obs[ 21 ].Q, obs[ 16 ].P + Vec2( 0.25f, -0.05f ) );
        obs[ nObs++ ] = LineSeg( obs[ 22 ].Q, 0.3f, PI / 12.0f );
        obs[ nObs++ ] = LineSeg( obs[ 23 ].Q, 0.35f, PI / 4.0f );
        obs[ nObs++ ] = LineSeg( obs[ 24 ].Q, 0.35f, PI / 12.0f );
        obs[ nObs++ ] = LineSeg( obs[ 25 ].Q, 0.8f, -PI / 4.0f );
        obs[ nObs++ ] = LineSeg( obs[ 26 ].Q + Vec2( 0.25f, 0.25f ), obs[ 12 ].P );
        basePos = obs[ 27 ].P;
        
        for( uint32 iObs = 0; iObs < nObs; iObs++ ) {
            addObstacle( controlState, world, Obstacle( obs[ iObs ], textureID_whiteTexture ) );
        }
        
        _popArray( tempMemory, LINE_SEG, maxObs );
    }
    { // cave 02 : background
        addCaveWall( controlState, world, Vec2( 4.354612f, -0.296694f ), Vec2( 0.538608f, 0.510772f ), 0.630187f );
        addCaveWall( controlState, world, Vec2( 3.136102f, -0.167079f ), Vec2( 0.666974f, 0.525974f ), 0.000000f );
        addCaveWall( controlState, world, Vec2( 4.064370f, 0.762683f ), Vec2( 0.579339f, 0.599699f ), -0.296527f );
        addCaveWall( controlState, world, Vec2( 4.978241f, 1.081365f ), Vec2( 0.723098f, 0.465225f ), 0.148457f );
        addCaveWall( controlState, world, Vec2( 6.238274f, 1.000903f ), Vec2( 0.936531f, 0.465225f ), -0.226532f );
        addCaveWall( controlState, world, Vec2( 7.072541f, 0.000320f ), Vec2( 0.512483f, 0.929275f ), 0.231820f );
        addCaveWall( controlState, world, Vec2( 2.957599f, -1.211141f ), Vec2( 0.596988f, 0.665333f ), 0.000000f );
        addCaveWall( controlState, world, Vec2( 3.840644f, 0.061509f ), Vec2( 0.546000f, 0.528985f ), 0.000160f );
        addCaveWall( controlState, world, Vec2( 2.579502f, -2.046198f ), Vec2( 0.548090f, 0.694538f ), -0.790591f );
        addCaveWall( controlState, world, Vec2( 2.139786f, -3.100800f ), Vec2( 0.542632f, 0.848032f ), 0.000000f );
        addCaveWall( controlState, world, Vec2( 1.852536f, -0.224474f ), Vec2( 0.631376f, 0.465225f ), 0.000000f );
        addCaveWall( controlState, world, Vec2( 0.606540f, -0.217084f ), Vec2( 0.662631f, 0.465225f ), 0.000000f );
        addCaveRock( controlState, world, Vec2( 0.682837f, 0.398351f ), Vec2( 0.625000f, 0.484071f ), 3.010693f );
        addCaveRock( controlState, world, Vec2( 1.797920f, 0.382060f ), Vec2( 0.625000f, 0.467640f ), -3.010693f );
        addCaveRock( controlState, world, Vec2( 2.443526f, 0.547418f ), Vec2( 0.362273f, 0.411893f ), -2.356194f );
        addCaveRock( controlState, world, Vec2( 2.989235f, 0.724585f ), Vec2( 0.526842f, 0.438669f ), -3.010693f );
        addCaveRock( controlState, world, Vec2( 3.567681f, 1.056449f ), Vec2( 0.603925f, 0.350000f ), -2.094395f );
        addCaveRock( controlState, world, Vec2( 4.579131f, 1.569004f ), Vec2( 0.992453f, 0.393992f ), -3.010693f );
        addCaveRock( controlState, world, Vec2( 6.500548f, 1.419105f ), Vec2( 1.133788f, 0.388251f ), 2.879793f );
        addCaveRock( controlState, world, Vec2( 7.516973f, 0.426822f ), Vec2( 0.957209f, 0.444070f ), 1.832596f );
        addCaveRock( controlState, world, Vec2( 7.326171f, -0.861745f ), Vec2( 0.836388f, 0.456327f ), 0.261799f );
        addCaveRock( controlState, world, Vec2( 6.576070f, -0.066014f ), Vec2( 0.725723f, 0.408260f ), -1.243547f );
        addCaveRock( controlState, world, Vec2( 6.026535f, 0.555339f ), Vec2( 0.640582f, 0.350000f ), -0.226669f );
        addCaveRock( controlState, world, Vec2( 4.917120f, 0.596753f ), Vec2( 0.626282f, 0.350000f ), 0.156954f );
        addCaveRock( controlState, world, Vec2( 4.428728f, 0.470164f ), Vec2( 0.305845f, 0.284282f ), 1.280649f );
        addCaveRock( controlState, world, Vec2( 0.663300f, -0.691105f ), Vec2( 0.715570f, 0.350000f ), -0.131952f );
        addCaveRock( controlState, world, Vec2( 1.866402f, -0.688670f ), Vec2( 0.759720f, 0.350000f ), 0.120336f );
        addCaveRock( controlState, world, Vec2( 2.583441f, -0.528014f ), Vec2( 0.200000f, 0.193171f ), -0.785398f );
        addCaveRock( controlState, world, Vec2( 2.476232f, -1.058774f ), Vec2( 0.550000f, 0.350000f ), -1.636246f );
        addCaveRock( controlState, world, Vec2( 2.030427f, -1.894597f ), Vec2( 0.723507f, 0.350000f ), -2.356194f );
        addCaveRock( controlState, world, Vec2( 1.705811f, -3.092862f ), Vec2( 0.959498f, 0.350000f ), -1.505347f );
        addCaveRock( controlState, world, Vec2( 2.177666f, -3.905843f ), Vec2( 0.699177f, 0.350000f ), -0.000000f );
        addCaveRock( controlState, world, Vec2( 2.579800f, -3.256376f ), Vec2( 0.756712f, 0.350000f ), 1.594861f );
        addCaveRock( controlState, world, Vec2( 2.981241f, -2.301942f ), Vec2( 0.715744f, 0.350000f ), 0.716807f );
        addCaveRock( controlState, world, Vec2( 3.444865f, -1.341015f ), Vec2( 0.722144f, 0.405581f ), 1.473367f );
        addCaveRock( controlState, world, Vec2( 3.435157f, -0.691023f ), Vec2( 0.284674f, 0.188275f ), 0.261799f );
        addCaveRock( controlState, world, Vec2( 3.665180f, -0.521657f ), Vec2( 0.175000f, 0.198278f ), 0.785398f );
        addCaveRock( controlState, world, Vec2( 3.873760f, -0.421422f ), Vec2( 0.175000f, 0.216378f ), 0.261799f );
        addCaveRock( controlState, world, Vec2( 4.031993f, -0.687609f ), Vec2( 0.400000f, 0.336079f ), -0.785398f );
        addCaveRock( controlState, world, Vec2( 4.750460f, 0.033545f ), Vec2( 0.639338f, 0.326260f ), 2.351048f );
    }
    spawnItem( controlState, itemID_heal, Vec2( 2.17f, -3.52f ) );
    spawnItem( controlState, itemID_heal, Vec2( 7.2f,  -0.385f ) );
    endSpawn( controlState );
    
    setBasePos( controlState, basePos );
    nextEvent_spawnw( controlState );
    { // cave 03
        uint32 nObs       = 0;
        uint32 maxObs     = GAME_CONTROL__OBSTACLE_MAX_COUNT;
        LINE_SEG * obs = _pushArray_clear( tempMemory, LINE_SEG, maxObs );
        
        obs[ nObs++ ] = LineSegQP( Vec2( 0.0f, 0.0f ), 1.0f, -PI * 23.0f / 48.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 0 ].P,         0.8f, -PI / 4.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 1 ].P,         0.7f, -PI / 2.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 2 ].P,         0.6f, -PI * 35.0f / 48.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 3 ].P,         0.1f, -PI / 2.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 4 ].P,         0.6f, -PI * 3.0f / 16.0f );
        basePos = obs[ 5 ].P;
        
        obs[ nObs++ ] = LineSeg( obs[  0 ].Q + Vec2( -0.25f, -0.25f ), obs[ 0 ].P + Vec2( -0.25f, -0.02f ) );
        obs[ nObs++ ] = LineSeg( obs[  6 ].Q, 0.4f,  -PI * 3.0f / 4.0f );
        obs[ nObs++ ] = LineSeg( obs[  7 ].Q, 0.95f,  -PI * 25.0f / 48.0f );
        obs[ nObs++ ] = LineSeg( obs[  8 ].Q, 0.95f,  -PI * 23.0f / 48.0f );
        obs[ nObs++ ] = LineSeg( obs[  9 ].Q, 1.2f,  -PI * 5.0f / 6.0f );
        obs[ nObs++ ] = LineSeg( obs[ 10 ].Q, 1.2f,  -PI * 23.0f / 48.0f );
        obs[ nObs++ ] = LineSeg( obs[ 11 ].Q, 0.1f,  0.0f );
        obs[ nObs++ ] = LineSeg( obs[ 12 ].Q, obs[ 11 ].P + Vec2( 0.16f, -0.11f ) );
        obs[ nObs++ ] = LineSeg( obs[ 13 ].Q, obs[ 10 ].P + Vec2( 0.1f, -0.2f ) );
        obs[ nObs++ ] = LineSeg( obs[ 14 ].Q, 0.4f,  -PI * 3.0f / 16.0f );
        obs[ nObs++ ] = LineSeg( obs[ 15 ].Q, 0.4f,   PI * 3.0f / 16.0f );
        obs[ nObs++ ] = LineSeg( obs[ 16 ].Q, obs[ 5 ].P + Vec2( -0.04f, -0.17f ) );
        
        for( uint32 iObs = 0; iObs < nObs; iObs++ ) {
            addObstacle( controlState, world, Obstacle( obs[ iObs ], textureID_whiteTexture ) );
        }
        
        _popArray( tempMemory, LINE_SEG, maxObs );
    }
    { // cave 03 : background
        addCaveWall( controlState, world, Vec2( -1.409710f, -4.439894f ), Vec2( 0.405109f, 0.776954f ), 0.000000f );
        addCaveWall( controlState, world, Vec2( -0.943652f, -3.617914f ), Vec2( 0.614617f, 0.497720f ), 0.540180f );
        addCaveWall( controlState, world, Vec2( -0.094624f, -3.070391f ), Vec2( 0.686265f, 0.786037f ), 0.000000f );
        addCaveWall( controlState, world, Vec2( 0.386079f, -3.106164f ), Vec2( 0.403824f, 0.463086f ), -0.230944f );
        addCaveWall( controlState, world, Vec2( 0.005141f, -1.921571f ), Vec2( 0.888816f, 0.659056f ), 0.000000f );
        addCaveWall( controlState, world, Vec2( -0.049888f, -0.687852f ), Vec2( 0.556268f, 0.762313f ), 0.064649f );
        addCaveRock( controlState, world, Vec2( 0.470649f, -0.373521f ), Vec2( 0.596497f, 0.445212f ), 1.636246f );
        addCaveRock( controlState, world, Vec2( 0.696329f, -1.133810f ), Vec2( 0.542264f, 0.350000f ), 2.356194f );
        addCaveRock( controlState, world, Vec2( 0.970324f, -1.726429f ), Vec2( 0.576999f, 0.339235f ), 1.570796f );
        addCaveRock( controlState, world, Vec2( 0.770405f, -2.505605f ), Vec2( 0.509867f, 0.264346f ), 0.850848f );
        addCaveRock( controlState, world, Vec2( 0.422002f, -2.764648f ), Vec2( 0.050000f, 0.186521f ), 1.570796f );
        addCaveRock( controlState, world, Vec2( 0.600234f, -2.808743f ), Vec2( 0.300000f, 0.207556f ), 2.552544f );
        addCaveRock( controlState, world, Vec2( -0.566036f, -0.663633f ), Vec2( 0.385320f, 0.350000f ), -1.485826f );
        addCaveRock( controlState, world, Vec2( -0.668024f, -1.006312f ), Vec2( 0.333670f, 0.350000f ), -2.356194f );
        addCaveRock( controlState, world, Vec2( -0.838946f, -1.617366f ), Vec2( 0.609715f, 0.350000f ), -1.636246f );
        addCaveRock( controlState, world, Vec2( -0.854624f, -2.640773f ), Vec2( 0.579994f, 0.350000f ), -1.505347f );
        addCaveRock( controlState, world, Vec2( -1.347460f, -3.300568f ), Vec2( 0.814087f, 0.350000f ), -2.617994f );
        addCaveRock( controlState, world, Vec2( -1.811241f, -4.501205f ), Vec2( 1.113241f, 0.350000f ), -1.505347f );
        addCaveRock( controlState, world, Vec2( -1.351597f, -5.344065f ), Vec2( 0.725948f, 0.350000f ), -0.000000f );
        addCaveRock( controlState, world, Vec2( -0.983817f, -4.659863f ), Vec2( 0.759287f, 0.350000f ), 1.587792f );
        addCaveRock( controlState, world, Vec2( -0.696267f, -3.960356f ), Vec2( 0.552040f, 0.348083f ), 0.480162f );
        addCaveRock( controlState, world, Vec2( -0.183947f, -3.916957f ), Vec2( 0.441645f, 0.330690f ), -0.589049f );
        addCaveRock( controlState, world, Vec2( 0.147613f, -3.917884f ), Vec2( 0.414414f, 0.350000f ), 0.589049f );
        addCaveRock( controlState, world, Vec2( 0.564122f, -3.700629f ), Vec2( 0.202174f, 0.350000f ), 0.195742f );
    }
    spawnItem( controlState, itemID_heal, Vec2( 0.46f,  -2.375f ) );
    spawnItem( controlState, itemID_heal, Vec2( -1.38f, -4.895f ) );
    endSpawn( controlState );
    
    nextEvent_loadTrigger( controlState, rectTRD( Vec2( 0.2f, -0.2f ), Vec2( 0.6f, 0.5f ) ) );
    nextEvent_setCameraBound( controlState, Rect( -2.1f, -20.0f, 20.0f, 2.0f ) );
    
    setBasePos( controlState, basePos );
    event = newChain_spawnw( controlState );
    uint32 triggerID_spawnBubble_cave04 = event->eventIndex;
    spawnBubble( controlState, Vec2( 1.58f, -1.26f ) );
    endSpawn( controlState );
    endChain( controlState );
    
    nextEvent_spawnw( controlState );
    { // cave 04
        uint32 nObs       = 0;
        uint32 maxObs     = GAME_CONTROL__OBSTACLE_MAX_COUNT;
        LINE_SEG * obs = _pushArray_clear( tempMemory, LINE_SEG, maxObs );
        
        obs[ nObs++ ] = LineSegQP( Vec2( 0.0f, 0.0f ), 0.8f, PI / 48.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 0 ].P,         0.6f, PI / 4.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 1 ].P,         0.5f, PI / 48.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 2 ].P,         0.5f, -PI / 6.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 3 ].P,         0.8f, -PI / 2.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 4 ].P,         0.7f, -PI * 3.0f / 4.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 5 ].P,         0.55f, -PI / 4.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 6 ].P,         0.15f, -PI * 17.0f / 32.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 7 ].P,         0.15f, -PI * 5.0f / 8.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 8 ].P,         0.55f, -PI / 4.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 9 ].P,         0.55f, -PI * 7.0f / 16.0f );
        basePos = obs[ 10 ].P;
        
        obs[ nObs++ ] = LineSeg( obs[ 0 ].Q + Vec2( -0.04f, -0.17f ), obs[ 0 ].P + toVec2( 0.17f, -PI * 31.0f / 64.0f ) );
        obs[ nObs++ ] = LineSeg( obs[ 11 ].Q, 0.5f, -PI / 4.0f );
        obs[ nObs++ ] = LineSeg( obs[ 12 ].Q, obs[ 5 ].P + toVec2( 0.15f, -PI ) );
        obs[ nObs++ ] = LineSeg( obs[ 13 ].Q, 0.8f, -PI * 7.0f / 8.0f );
        obs[ nObs++ ] = LineSeg( obs[ 14 ].Q, 0.15f, -PI / 2.0f );
        obs[ nObs++ ] = LineSeg( obs[ 15 ].Q, 0.8f, -PI / 6.0f );
        obs[ nObs++ ] = LineSeg( obs[ 16 ].Q, 0.6f, -PI * 3.0f / 8.0f );
        obs[ nObs++ ] = LineSeg( obs[ 17 ].Q, obs[ 10 ].P + toVec2( 0.35f, -PI * 15.0f / 16.0f ) );
        
        for( uint32 iObs = 0; iObs < nObs; iObs++ ) {
            addObstacle( controlState, world, Obstacle( obs[ iObs ], textureID_whiteTexture ) );
        }
        
        _popArray( tempMemory, LINE_SEG, maxObs );
    }
    { // cave 04 : background
        addCaveWall( controlState, world, Vec2( 1.962444f, -2.255481f ), Vec2( 0.509706f, 0.574914f ), 0.355639f );
        addCaveWall( controlState, world, Vec2( 1.421181f, -1.461442f ), Vec2( 0.444398f, 0.740358f ), 1.595425f );
        addCaveWall( controlState, world, Vec2( 1.598776f, -0.653598f ), Vec2( 0.401076f, 0.667102f ), 1.595425f );
        addCaveWall( controlState, world, Vec2( 0.412682f, -0.071097f ), Vec2( 0.331993f, 0.469207f ), 1.595425f );
        addCaveWall( controlState, world, Vec2( 1.544568f, 0.156463f ), Vec2( 0.445698f, 0.692459f ), 1.595425f );
        addCaveRock( controlState, world, Vec2( 0.369977f, 0.375001f ), Vec2( 0.406289f, 0.350000f ), -3.076143f );
        addCaveRock( controlState, world, Vec2( 0.855313f, 0.604323f ), Vec2( 0.430646f, 0.350000f ), -2.356194f );
        addCaveRock( controlState, world, Vec2( 1.427946f, 0.840800f ), Vec2( 0.493807f, 0.350000f ), -3.076143f );
        addCaveRock( controlState, world, Vec2( 2.200422f, 0.636916f ), Vec2( 0.553161f, 0.350000f ), 2.617994f );
        addCaveRock( controlState, world, Vec2( 2.504493f, 0.012238f ), Vec2( 0.643420f, 0.350000f ), 1.570796f );
        addCaveRock( controlState, world, Vec2( 2.060355f, -0.941548f ), Vec2( 0.350000f, 0.216868f ), 0.785398f );
        addCaveRock( controlState, world, Vec2( 2.497312f, -1.072610f ), Vec2( 0.618518f, 0.566301f ), 2.356194f );
        addCaveRock( controlState, world, Vec2( 2.384454f, -1.583668f ), Vec2( 0.125371f, 0.350000f ), 1.472622f );
        addCaveRock( controlState, world, Vec2( 2.281922f, -1.687968f ), Vec2( 0.139571f, 0.272967f ), 1.178097f );
        addCaveRock( controlState, world, Vec2( 2.356772f, -1.720913f ), Vec2( 0.275000f, 0.263038f ), 2.356195f );
        addCaveRock( controlState, world, Vec2( 2.712782f, -2.054583f ), Vec2( 0.528505f, 0.350000f ), 1.767146f );
        addCaveRock( controlState, world, Vec2( 0.408300f, -0.581264f ), Vec2( 0.426284f, 0.438235f ), 0.061963f );
        addCaveRock( controlState, world, Vec2( 0.735918f, -0.541737f ), Vec2( 0.250000f, 0.350000f ), -0.785398f );
        addCaveRock( controlState, world, Vec2( 1.178194f, -0.850274f ), Vec2( 0.331993f, 0.184212f ), -1.016764f );
        addCaveRock( controlState, world, Vec2( 0.983271f, -1.064553f ), Vec2( 0.497236f, 0.174717f ), -2.748894f );
        addCaveRock( controlState, world, Vec2( 0.420415f, -1.426701f ), Vec2( 0.465180f, 0.350000f ), -1.570796f );
        addCaveRock( controlState, world, Vec2( 0.766708f, -1.893839f ), Vec2( 0.602207f, 0.350000f ), -0.523599f );
        addCaveRock( controlState, world, Vec2( 1.239451f, -2.443063f ), Vec2( 0.423632f, 0.417696f ), -1.178097f );
        addCaveRock( controlState, world, Vec2( 1.571532f, -2.853429f ), Vec2( 0.403236f, 0.411454f ), -0.542221f );
        addCaveRock( controlState, world, Vec2( 0.943342f, -0.947150f ), Vec2( 0.331993f, 0.184212f ), -1.340718f );
        addCaveRock( controlState, world, Vec2( 1.020066f, -0.812392f ), Vec2( 0.331993f, 0.162000f ), 0.726395f );
    }
    spawnTrigger( controlState, triggerID_spawnBubble_cave04, rectCD( Vec2( 1.58f, -1.04f ), Vec2( 1.0f, 1.0f ) ) );
    endSpawn( controlState );
    
    nextEvent_loadTrigger( controlState, rectTLD( Vec2( 0.2f, 0.2f ), Vec2( 0.5f, 0.6f ) ) );
    nextEvent_setCameraBound( controlState, Rect( -2.1f, -20.0f, 20.0f, 1.69f ) );
    
    setBasePos( controlState, basePos );
    event = newChain_spawnw( controlState );
    uint32 triggerID_spawnBubble_cave05 = event->eventIndex;
    spawnBubble( controlState, Vec2( 3.85f, -2.1f  ) );
    endSpawn( controlState );
    endChain( controlState );
    
    nextEvent_spawnw( controlState );
    { // cave 05
        uint32 nObs       = 0;
        uint32 maxObs     = GAME_CONTROL__OBSTACLE_MAX_COUNT;
        LINE_SEG * obs = _pushArray_clear( tempMemory, LINE_SEG, maxObs );
        
        obs[ nObs++ ] = LineSegQP( Vec2( 0.0f, 0.0f ), 1.0f,   PI * 3.0f / 32.0f );
        obs[ nObs++ ] = LineSegQP( obs[  0 ].P,        0.35f, -PI / 4.0f );
        obs[ nObs++ ] = LineSegQP( obs[  1 ].P,        0.75f,  PI / 16.0f );
        obs[ nObs++ ] = LineSegQP( obs[  2 ].P,        0.5f,  -PI / 5.0f );
        obs[ nObs++ ] = LineSegQP( obs[  3 ].P,        0.6f,  -PI * 17.0f / 32.0f );
        obs[ nObs++ ] = LineSegQP( obs[  4 ].P,        0.35f, -PI * 5.0f / 12.0f );
        obs[ nObs++ ] = LineSegQP( obs[  5 ].P,        0.3f,  -PI * 9.0f / 16.0f );
        obs[ nObs++ ] = LineSegQP( obs[  6 ].P,        0.45f, -PI * 11.0f / 32.0f );
        obs[ nObs++ ] = LineSegQP( obs[  7 ].P,        0.55f,  PI / 64.0f );
        obs[ nObs++ ] = LineSegQP( obs[  8 ].P,        0.5f,   PI * 7.0f / 16.0f );
        obs[ nObs++ ] = LineSegQP( obs[  9 ].P,        0.45f,  PI * 5.0f / 8.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 10 ].P,        0.6f,   PI / 3.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 11 ].P,        0.4f,   PI * 2.0f / 3.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 12 ].P,        0.6f,   PI * 5.0f / 16.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 13 ].P,        0.8f,   PI / 16.0f );
        basePos = obs[ 14 ].P;
        
        obs[ nObs++ ] = LineSeg( toVec2( 0.35f, -PI * 15.0f / 16.0f ), 1.0f, -PI * 17.0f / 32.0f );
        obs[ nObs++ ] = LineSeg( obs[ 15 ].Q,        0.3f,  -PI / 4.0f );
        obs[ nObs++ ] = LineSeg( obs[ 16 ].Q,        0.85f,  -PI * 15.0f / 32.0f );
        obs[ nObs++ ] = LineSeg( obs[ 17 ].Q,        0.8f,   PI / 32.0f );
        obs[ nObs++ ] = LineSeg( obs[ 18 ].Q,        0.3f,  -PI * 15.0f / 32.0f );
        obs[ nObs++ ] = LineSeg( obs[ 19 ].Q,        0.6f,   PI / 8.0f );
        obs[ nObs++ ] = LineSeg( obs[ 20 ].Q,        0.65f, -PI * 3.0f / 32.0f );
        obs[ nObs++ ] = LineSeg( obs[ 21 ].Q,        0.6f,  -PI / 16.0f );
        obs[ nObs++ ] = LineSeg( obs[ 22 ].Q,        0.4f,   PI * 1.0f / 8.0f );
        obs[ nObs++ ] = LineSeg( obs[ 23 ].Q,        0.65f, -PI * 3.0f / 32.0f );
        obs[ nObs++ ] = LineSeg( obs[ 24 ].Q,        0.8f,   PI * 1.0f / 32.0f );
        obs[ nObs++ ] = LineSeg( obs[ 25 ].Q,        0.5f,   PI * 9.0f / 16.0f );
        obs[ nObs++ ] = LineSeg( obs[ 26 ].Q,        0.6f,   PI * 1.0f / 16.0f );
        obs[ nObs++ ] = LineSeg( obs[ 27 ].Q,        0.3f,   PI * 19.0f / 32.0f );
        obs[ nObs++ ] = LineSeg( obs[ 28 ].Q,        0.45f,  PI * 1.0f / 4.0f );
        obs[ nObs++ ] = LineSeg( obs[ 29 ].Q,        0.4f,   PI * 31.0f / 64.0f );
        obs[ nObs++ ] = LineSeg( obs[ 30 ].Q,        0.5f,   PI * 17.0f / 32.0f );
        obs[ nObs++ ] = LineSeg( obs[ 31 ].Q,        0.55f,  PI * 63.0f / 64.0f );
        obs[ nObs++ ] = LineSeg( obs[ 32 ].Q,        0.15f,  PI * 1.0f / 2.0f );
        obs[ nObs++ ] = LineSeg( obs[ 33 ].Q,        0.55f,  0.0f );
        obs[ nObs++ ] = LineSeg( obs[ 34 ].Q,        0.45f,  PI * 7.0f / 16.0f );
        obs[ nObs++ ] = LineSeg( obs[ 35 ].Q, obs[ 14 ].P + Vec2( 0.5f, -PI * 1.0f / 32.0f ) );
        
        obs[ nObs++ ] = LineSeg( Vec2( 0.2f, -0.6f ), 0.45f, PI * 1.0f / 32.0f );
        obs[ nObs++ ] = LineSeg( obs[ 37 ].Q, 0.5f,  -PI * 1.0f / 32.0f );
        obs[ nObs++ ] = LineSeg( obs[ 38 ].Q, 0.15f, -PI * 1.0f / 4.0f );
        obs[ nObs++ ] = LineSeg( obs[ 39 ].Q, 0.2f,  -PI * 5.0f / 8.0f );
        obs[ nObs++ ] = LineSeg( obs[ 40 ].Q, 0.2f,  -PI * 7.0f / 8.0f );
        obs[ nObs++ ] = LineSeg( obs[ 41 ].Q, 0.15f, -PI );
        obs[ nObs++ ] = LineSeg( obs[ 42 ].Q, 0.12f,  PI * 33.0f / 64.0f );
        obs[ nObs++ ] = LineSeg( obs[ 43 ].Q, 0.35f,  PI * 63.0f / 64.0f );
        obs[ nObs++ ] = LineSeg( obs[ 44 ].Q, 0.3f,   PI * 31.0f / 32.0f );
        obs[ nObs++ ] = LineSeg( obs[ 45 ].Q, obs[ 37 ].P );
        
        obs[ nObs++ ] = LineSeg( Vec2( 1.6f, -1.55f ), 0.2f, 0.0f );
        obs[ nObs++ ] = LineSeg( obs[ 47 ].Q, 0.2f,  -PI * 1.0f / 2.0f );
        obs[ nObs++ ] = LineSeg( obs[ 48 ].Q, 0.22f, -PI * 15.0f / 16.0f );
        obs[ nObs++ ] = LineSeg( obs[ 49 ].Q, 0.15f,  PI * 5.0f / 8.0f );
        obs[ nObs++ ] = LineSeg( obs[ 50 ].Q, obs[ 47 ].P );
        
        obs[ nObs++ ] = LineSeg( Vec2( 3.5f, 0.1f ), 0.2f, PI * 1.0f / 64.0f );
        obs[ nObs++ ] = LineSeg( obs[ 52 ].Q, 0.2f,       -PI * 1.0f / 32.0f );
        obs[ nObs++ ] = LineSeg( obs[ 53 ].Q, 0.12f,      -PI * 9.0f / 16.0f );
        obs[ nObs++ ] = LineSeg( obs[ 54 ].Q, 0.18f,       PI * 31.0f / 32.0f );
        obs[ nObs++ ] = LineSeg( obs[ 55 ].Q, 0.18f,      -PI * 63.0f / 64.0f );
        obs[ nObs++ ] = LineSeg( obs[ 56 ].Q, obs[ 52 ].P );
        
        for( uint32 iObs = 0; iObs < nObs; iObs++ ) {
            addObstacle( controlState, world, Obstacle( obs[ iObs ], textureID_whiteTexture ) );
        }
        
        _popArray( tempMemory, LINE_SEG, maxObs );
    }
    { // cave 05 : background
        addCaveWall( controlState, world, Vec2( 4.701629f, -0.251326f ), Vec2( 0.891530f, 0.611340f ), -1.570882f );
        addCaveWall( controlState, world, Vec2( 4.645120f, -1.230241f ), Vec2( 0.712324f, 0.662842f ), -1.570882f );
        addCaveWall( controlState, world, Vec2( 3.692660f, -0.077013f ), Vec2( 0.874347f, 0.824757f ), -1.570882f );
        addCaveWall( controlState, world, Vec2( 3.319474f, -1.353322f ), Vec2( 0.796137f, 0.824757f ), -1.570882f );
        addCaveWall( controlState, world, Vec2( 3.589550f, -2.145992f ), Vec2( 0.601853f, 0.824757f ), -1.570882f );
        addCaveWall( controlState, world, Vec2( 2.000964f, -1.915907f ), Vec2( 0.758940f, 0.851871f ), -1.570882f );
        addCaveWall( controlState, world, Vec2( 0.378251f, -1.733524f ), Vec2( 0.704962f, 0.824757f ), -1.570882f );
        addCaveWall( controlState, world, Vec2( 0.210082f, -0.388471f ), Vec2( 0.763534f, 0.824757f ), -1.570882f );
        addCaveWall( controlState, world, Vec2( 1.797299f, -0.484206f ), Vec2( 0.796137f, 0.824757f ), -1.570882f );
        addCaveRock( controlState, world, Vec2( 0.576878f, 0.540743f ), Vec2( 0.709007f, 0.350000f ), -2.847068f );
        addCaveRock( controlState, world, Vec2( 1.179069f, 0.563131f ), Vec2( 0.385863f, 0.350000f ), 2.356194f );
        addCaveRock( controlState, world, Vec2( 1.607061f, 0.539748f ), Vec2( 0.491847f, 0.408852f ), -2.945243f );
        addCaveRock( controlState, world, Vec2( 2.373666f, 0.492074f ), Vec2( 0.626758f, 0.499992f ), 2.513274f );
        addCaveRock( controlState, world, Vec2( 2.663435f, -0.437639f ), Vec2( 0.300000f, 0.350000f ), 1.472622f );
        addCaveRock( controlState, world, Vec2( 2.604898f, -0.877975f ), Vec2( 0.252698f, 0.262733f ), 1.832596f );
        addCaveRock( controlState, world, Vec2( 2.690313f, -1.255362f ), Vec2( 0.150000f, 0.350000f ), 1.374447f );
        addCaveRock( controlState, world, Vec2( 2.732511f, -1.367642f ), Vec2( 0.225000f, 0.350000f ), 2.061670f );
        addCaveRock( controlState, world, Vec2( 2.787398f, -1.367991f ), Vec2( 0.275000f, 0.350000f ), -3.092505f );
        addCaveRock( controlState, world, Vec2( 2.869555f, -1.407469f ), Vec2( 0.250000f, 0.263522f ), -1.767146f );
        addCaveRock( controlState, world, Vec2( 2.858891f, -0.993971f ), Vec2( 0.324640f, 0.209616f ), -1.178097f );
        addCaveRock( controlState, world, Vec2( 2.907372f, -0.438669f ), Vec2( 0.337468f, 0.263817f ), -2.094395f );
        addCaveRock( controlState, world, Vec2( 2.851207f, -0.193060f ), Vec2( 0.300524f, 0.350000f ), -1.047197f );
        addCaveRock( controlState, world, Vec2( 2.866487f, 0.644691f ), Vec2( 0.521322f, 0.518309f ), -2.159845f );
        addCaveRock( controlState, world, Vec2( 3.497743f, 0.988895f ), Vec2( 0.659012f, 0.402146f ), -2.945243f );
        addCaveRock( controlState, world, Vec2( -0.813499f, -0.661970f ), Vec2( 0.636919f, 0.409768f ), -1.668971f );
        addCaveRock( controlState, world, Vec2( -0.774019f, -1.253258f ), Vec2( 0.401071f, 0.369476f ), -0.785398f );
        addCaveRock( controlState, world, Vec2( -0.621360f, -2.070620f ), Vec2( 0.752751f, 0.468237f ), -1.472622f );
        addCaveRock( controlState, world, Vec2( -0.081141f, -2.507505f ), Vec2( 0.708955f, 0.393445f ), 0.098175f );
        addCaveRock( controlState, world, Vec2( 0.434064f, -2.355838f ), Vec2( 0.290046f, 0.245852f ), -1.472622f );
        addCaveRock( controlState, world, Vec2( 0.843451f, -2.652659f ), Vec2( 0.567740f, 0.350000f ), 0.392699f );
        addCaveRock( controlState, world, Vec2( 1.514167f, -2.692614f ), Vec2( 0.436602f, 0.474260f ), -0.294524f );
        addCaveRock( controlState, world, Vec2( 1.994464f, -2.821405f ), Vec2( 0.485052f, 0.483675f ), -0.196350f );
        addCaveRock( controlState, world, Vec2( 2.557178f, -2.818951f ), Vec2( 0.449438f, 0.413750f ), 0.392699f );
        addCaveRock( controlState, world, Vec2( 2.999299f, -2.773674f ), Vec2( 0.325000f, 0.433286f ), -0.294524f );
        addCaveRock( controlState, world, Vec2( 3.981781f, -2.793205f ), Vec2( 0.902644f, 0.391670f ), 0.098175f );
        addCaveRock( controlState, world, Vec2( 4.606833f, -2.045563f ), Vec2( 0.250000f, 0.431672f ), 1.767146f );
        addCaveRock( controlState, world, Vec2( 4.515745f, -2.262556f ), Vec2( 0.300000f, 0.445054f ), 0.196350f );
        addCaveRock( controlState, world, Vec2( 5.079675f, -1.737106f ), Vec2( 0.374382f, 0.350000f ), 1.865320f );
        addCaveRock( controlState, world, Vec2( 3.533961f, 0.044234f ), Vec2( 0.059905f, 0.025973f ), 1.708690f );
        addCaveRock( controlState, world, Vec2( 5.238580f, -1.482328f ), Vec2( 0.424708f, 0.427371f ), 0.785398f );
        addCaveRock( controlState, world, Vec2( 5.409085f, -1.061427f ), Vec2( 0.403166f, 0.449323f ), 1.521709f );
        addCaveRock( controlState, world, Vec2( 5.406318f, -0.493830f ), Vec2( 0.367650f, 0.456699f ), 1.668972f );
        addCaveRock( controlState, world, Vec2( 4.651142f, -0.232802f ), Vec2( 0.275000f, 0.018864f ), 3.092505f );
        addCaveRock( controlState, world, Vec2( 4.678797f, -0.163149f ), Vec2( 0.075000f, 0.303249f ), 1.570796f );
        addCaveRock( controlState, world, Vec2( 4.650548f, -0.140491f ), Vec2( 0.275000f, 0.052342f ), -0.000000f );
        addCaveRock( controlState, world, Vec2( 5.454098f, 0.088805f ), Vec2( 0.385784f, 0.483872f ), 1.374447f );
        addCaveRock( controlState, world, Vec2( 5.365429f, 0.503246f ), Vec2( 0.552801f, 0.350000f ), 2.390188f );
        addCaveRock( controlState, world, Vec2( 0.431042f, -0.650297f ), Vec2( 0.225000f, 0.072701f ), 0.098175f );
        addCaveRock( controlState, world, Vec2( 0.887634f, -0.671727f ), Vec2( 0.250000f, 0.091772f ), -0.098175f );
        addCaveRock( controlState, world, Vec2( 1.127300f, -0.729092f ), Vec2( 0.075000f, 0.100633f ), -0.785398f );
        addCaveRock( controlState, world, Vec2( 1.121948f, -0.765547f ), Vec2( 0.100000f, 0.098796f ), -1.963495f );
        addCaveRock( controlState, world, Vec2( 1.043445f, -0.839563f ), Vec2( 0.100000f, 0.102230f ), -2.748894f );
        addCaveRock( controlState, world, Vec2( 0.911661f, -0.908695f ), Vec2( 0.060000f, 0.074516f ), 1.619884f );
        addCaveRock( controlState, world, Vec2( 0.915179f, -0.879267f ), Vec2( 0.075000f, 0.093012f ), -3.141593f );
        addCaveRock( controlState, world, Vec2( 0.728761f, -0.769512f ), Vec2( 0.240529f, 0.077634f ), 3.092505f );
        addCaveRock( controlState, world, Vec2( 0.343256f, -0.741135f ), Vec2( 0.150000f, 0.079797f ), 3.043418f );
        addCaveRock( controlState, world, Vec2( 0.283535f, -0.709006f ), Vec2( 0.103155f, 0.090661f ), 1.503647f );
        addCaveRock( controlState, world, Vec2( 1.700000f, -1.607823f ), Vec2( 0.100000f, 0.057823f ), -0.000000f );
        addCaveRock( controlState, world, Vec2( 1.741470f, -1.650000f ), Vec2( 0.100000f, 0.058530f ), -1.570796f );
        addCaveRock( controlState, world, Vec2( 1.682130f, -1.721271f ), Vec2( 0.110000f, 0.051172f ), -2.945243f );
        addCaveRock( controlState, world, Vec2( 1.603284f, -1.703847f ), Vec2( 0.075000f, 0.051692f ), 1.963495f );
        addCaveRock( controlState, world, Vec2( 1.633934f, -1.651628f ), Vec2( 0.063720f, 0.086136f ), 0.959179f );
        addCaveRock( controlState, world, Vec2( 3.576587f, 0.078451f ), Vec2( 0.027727f, 0.025741f ), -1.604014f );
        addCaveRock( controlState, world, Vec2( 3.606572f, 0.069762f ), Vec2( 0.095040f, 0.035431f ), 0.049087f );
        addCaveRock( controlState, world, Vec2( 3.822321f, 0.072762f ), Vec2( 0.027727f, 0.025741f ), -1.579186f );
        addCaveRock( controlState, world, Vec2( 3.861844f, 0.036385f ), Vec2( 0.060000f, 0.025741f ), -1.767146f );
        addCaveRock( controlState, world, Vec2( 3.788495f, 0.060659f ), Vec2( 0.094034f, 0.040220f ), -0.098175f );
        addCaveRock( controlState, world, Vec2( 3.772307f, 0.018617f ), Vec2( 0.107100f, 0.035775f ), 3.043418f );
        addCaveRock( controlState, world, Vec2( 3.619298f, 0.014941f ), Vec2( 0.104355f, 0.028528f ), -3.092505f );
    }
    spawnBubble( controlState, Vec2( 0.73f, -0.95f ) );
    spawnBubble( controlState, Vec2( 2.7f,  -1.85f ) );
    spawnBubble( controlState, Vec2( 3.63f, -0.12f ) );
    spawnBubble( controlState, Vec2( 3.75f, -0.12f ) );
    spawnTrigger( controlState, triggerID_spawnBubble_cave05, rectCD( Vec2( 3.0f, -2.05f ), Vec2( 0.2f, 1.0f ) ) );
    endSpawn( controlState );
    
    nextEvent_loadTrigger( controlState, rectTRD( Vec2( 0.2f, 0.2f ), Vec2( 1.0f, 0.6f ) ) );
    nextEvent_setCameraBound( controlState, Rect( -1.14f, -20.0f, 20.0f, 3.0f ) );
    
    setBasePos( controlState, basePos );
    //firstEvent( controlState, Vec2( 0.3f, -0.05f ) );
    nextEvent_spawnw( controlState );
    { // cave 06
        uint32 nObs       = 0;
        uint32 maxObs     = GAME_CONTROL__OBSTACLE_MAX_COUNT;
        LINE_SEG * obs = _pushArray_clear( tempMemory, LINE_SEG, maxObs );
        
        obs[ nObs++ ] = LineSegQP( Vec2( 0.0f, 0.0f ), 0.6f,    PI * 17.0f / 32.0f );
        obs[ nObs++ ] = LineSegQP( obs[  0 ].P,        0.35f,   PI * 3.0f  / 8.0f );
        obs[ nObs++ ] = LineSegQP( obs[  1 ].P,        0.35f,   PI * 1.0f  / 64.0f );
        obs[ nObs++ ] = LineSegQP( obs[  2 ].P,        0.75f,  -PI * 1.0f  / 8.0f );
        obs[ nObs++ ] = LineSegQP( obs[  3 ].P,        0.6f,   PI * 7.0f  / 32.0f );
        obs[ nObs++ ] = LineSegQP( obs[  4 ].P,        0.1f,   0.0f );
        obs[ nObs++ ] = LineSegQP( obs[  5 ].P,        0.1f,  -PI * 15.0f / 32.0f );
        obs[ nObs++ ] = LineSegQP( obs[  6 ].P,        0.7f,  -PI *  1.0f / 32.0f );
        basePos = obs[ 7 ].P;
        
        obs[ nObs++ ] = LineSeg( obs[  0 ].Q + Vec2( 0.5f, -PI * 1.0f / 32.0f ), 0.2f, PI / 4.0f );
        obs[ nObs++ ] = LineSeg( obs[  8 ].Q,         0.23f,   PI * 1.0f / 2.0f );
        obs[ nObs++ ] = LineSeg( obs[  9 ].Q,         0.6f,  PI * 1.0f / 8.0f );
        obs[ nObs++ ] = LineSeg( obs[ 10 ].Q,         0.5f,   PI * 7.0f / 32.0f );
        obs[ nObs++ ] = LineSeg( obs[ 11 ].Q,         0.1f,   PI * 13.0f / 32.0f );
        obs[ nObs++ ] = LineSeg( obs[ 12 ].Q,         0.1f,  -PI *  5.0f / 16.0f );
        obs[ nObs++ ] = LineSeg( obs[ 13 ].Q,         0.15f, -PI *  1.0f / 32.0f );
        obs[ nObs++ ] = LineSeg( obs[ 14 ].Q,         0.2f,  -PI *  1.0f / 4.0f );
        obs[ nObs++ ] = LineSeg( obs[ 15 ].Q, obs[ 7 ].P + toVec2( 0.35f, -PI * 17.0f / 32.0f ) );
        
        for( uint32 iObs = 0; iObs < nObs; iObs++ ) {
            addObstacle( controlState, world, Obstacle( obs[ iObs ], textureID_whiteTexture ) );
        }
        
        _popArray( tempMemory, LINE_SEG, maxObs );
    }
    { // cave 06 : background
        addCaveWall( controlState, world, Vec2( 0.376951f, 0.470054f ), Vec2( 0.565104f, 0.753243f ), 1.495921f );
        addCaveWall( controlState, world, Vec2( 1.673114f, 0.746459f ), Vec2( 0.486244f, 0.753243f ), 1.495921f );
        addCaveRock( controlState, world, Vec2( -0.387077f, 0.359258f ), Vec2( 0.395469f, 0.350000f ), -1.472621f );
        addCaveRock( controlState, world, Vec2( -0.261426f, 1.022548f ), Vec2( 0.476109f, 0.350000f ), -1.963495f );
        addCaveRock( controlState, world, Vec2( 0.110671f, 1.272637f ), Vec2( 0.490341f, 0.350000f ), -3.092505f );
        addCaveRock( controlState, world, Vec2( 0.843938f, 1.142828f ), Vec2( 0.441202f, 0.350000f ), 2.748893f );
        addCaveRock( controlState, world, Vec2( 1.127482f, 1.111501f ), Vec2( 0.300000f, 0.350000f ), -2.454369f );
        addCaveRock( controlState, world, Vec2( 1.562679f, 1.381266f ), Vec2( 0.432723f, 0.350000f ), -3.141593f );
        addCaveRock( controlState, world, Vec2( 2.011876f, 1.246927f ), Vec2( 0.282232f, 0.350000f ), 1.668971f );
        addCaveRock( controlState, world, Vec2( 2.073846f, 1.245756f ), Vec2( 0.350000f, 0.350000f ), 3.043418f );
        addCaveRock( controlState, world, Vec2( 0.722680f, -0.081997f ), Vec2( 0.168898f, 0.146019f ), 0.785398f );
        addCaveRock( controlState, world, Vec2( 0.991421f, 0.106949f ), Vec2( 0.166298f, 0.350000f ), 1.570796f );
        addCaveRock( controlState, world, Vec2( 1.060219f, 0.130590f ), Vec2( 0.332326f, 0.292064f ), 0.392699f );
        addCaveRock( controlState, world, Vec2( 1.534401f, 0.419058f ), Vec2( 0.345684f, 0.279615f ), 0.687223f );
        addCaveRock( controlState, world, Vec2( 1.610648f, 0.863690f ), Vec2( 0.050000f, 0.014504f ), 1.276273f );
        addCaveRock( controlState, world, Vec2( 1.627445f, 0.866412f ), Vec2( 0.050000f, 0.013971f ), -0.981748f );
        addCaveRock( controlState, world, Vec2( 1.707106f, 0.712737f ), Vec2( 0.098178f, 0.115339f ), -0.098175f );
        addCaveRock( controlState, world, Vec2( 1.767375f, 0.525417f ), Vec2( 0.172349f, 0.241282f ), -0.785398f );
        addCaveRock( controlState, world, Vec2( 2.036687f, 0.266129f ), Vec2( 0.366079f, 0.350000f ), -0.387565f );
    }
    spawnBubble( controlState, Vec2( 1.65f, 0.96f ) );
    spawnBubble( controlState, Vec2( 1.68f, 0.89f ) );
    spawnBubble( controlState, Vec2( 1.75f, 0.86f ) );
    endSpawn( controlState );
    
    
    setBasePos( controlState, basePos );
    //firstEvent( controlState, Vec2( -0.1f, -0.1f ) );
    nextEvent_enableMovementBound( controlState, Rect( 1.3f, -1.45f, 1.8f, -1.2f ) );
    nextEvent_spawnw( controlState );
    { // cave 07
        uint32 nObs       = 0;
        uint32 maxObs     = GAME_CONTROL__OBSTACLE_MAX_COUNT;
        LINE_SEG * obs = _pushArray_clear( tempMemory, LINE_SEG, maxObs );
        
        obs[ nObs++ ] = LineSegQP( Vec2( 0.0f, 0.0f ), 0.50f,   PI *  4.0f / 64.0f );
        obs[ nObs++ ] = LineSegQP( obs[  0 ].P,        0.15f,  -PI * 24.0f / 64.0f );
        obs[ nObs++ ] = LineSegQP( obs[  1 ].P,        1.00f,  -PI * 38.0f / 64.0f );
        obs[ nObs++ ] = LineSegQP( obs[  2 ].P,        0.10f,  -PI * 24.0f / 64.0f );
        obs[ nObs++ ] = LineSegQP( obs[  3 ].P,        1.00f,   PI * 10.0f / 64.0f );
        obs[ nObs++ ] = LineSegQP( obs[  4 ].P,        0.30f,  -PI * 22.0f / 64.0f );
        obs[ nObs++ ] = LineSegQP( obs[  5 ].P,        0.40f,  -PI * 10.0f / 64.0f );
        obs[ nObs++ ] = LineSegQP( obs[  6 ].P,        0.75f,  0.0f );
        basePos = obs[ 7 ].P;
        
        obs[ nObs++ ] = LineSeg( obs[ 0 ].Q + toVec2( 0.35f, -PI * 34.0f / 64.0f ), 0.25f, -PI * 4.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[  8 ].Q,        0.85f,  -PI * 38.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[  9 ].Q, obs[ 3 ].P + toVec2( 0.13f, -PI * 33.0f / 64.0f ) );
        obs[ nObs++ ] = LineSeg(   obs[ 10 ].Q,        0.96f,   PI * 10.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[ 11 ].Q,        0.10f,  -PI * 39.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[ 12 ].Q,        0.34f,  -PI * 33.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[ 13 ].Q,        0.30f,  -PI * 31.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[ 14 ].Q,        0.50f,  -PI *  2.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[ 15 ].Q,        0.05f,   PI * 16.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[ 16 ].Q,        0.30f,   PI *  2.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[ 17 ].Q,        0.485f,   0.0f );
        obs[ nObs++ ] = LineSeg(   obs[ 18 ].Q, obs[ 7 ].P + Vec2( 0.0f, -0.20f ) );
        
        for( uint32 iObs = 0; iObs < nObs; iObs++ ) {
            addObstacle( controlState, world, Obstacle( obs[ iObs ], textureID_whiteTexture ) );
        }
        
        _popArray( tempMemory, LINE_SEG, maxObs );
    }
    { // cave 07 : background
        addCaveWall( controlState, world, Vec2( 0.281408f, -0.160613f ), Vec2( 0.339191f, 0.329569f ), -0.064694f );
        addCaveWall( controlState, world, Vec2( 0.255363f, -0.928534f ), Vec2( 0.265964f, 0.529000f ), -0.240223f );
        addCaveWall( controlState, world, Vec2( 0.849546f, -0.876541f ), Vec2( 0.566505f, 0.329569f ), 0.467590f );
        addCaveWall( controlState, world, Vec2( 1.381377f, -1.291592f ), Vec2( 0.389739f, 0.354705f ), -0.010827f );
        addCaveWall( controlState, world, Vec2( 2.068128f, -1.323417f ), Vec2( 0.418958f, 0.340181f ), 0.000973f );
        addCaveRock( controlState, world, Vec2( 0.412321f, 0.438873f ), Vec2( 0.490019f, 0.350000f ), -2.945243f );
        addCaveRock( controlState, world, Vec2( 0.894221f, 0.037211f ), Vec2( 0.664981f, 0.350000f ), 1.963495f );
        addCaveRock( controlState, world, Vec2( 0.486496f, -0.442038f ), Vec2( 0.598471f, 0.069289f ), 1.276272f );
        addCaveRock( controlState, world, Vec2( 0.317190f, -1.027377f ), Vec2( 0.050000f, 0.043886f ), 1.963495f );
        addCaveRock( controlState, world, Vec2( 0.711623f, -0.807678f ), Vec2( 0.500000f, 0.053280f ), -2.650719f );
        addCaveRock( controlState, world, Vec2( 1.469647f, -0.422688f ), Vec2( 0.335481f, 0.350000f ), 2.061670f );
        addCaveRock( controlState, world, Vec2( 1.660492f, -0.669152f ), Vec2( 0.200000f, 0.350000f ), 2.650719f );
        addCaveRock( controlState, world, Vec2( 2.046888f, -0.722103f ), Vec2( 0.375000f, 0.350000f ), -3.141593f );
        addCaveRock( controlState, world, Vec2( 0.034781f, -0.463415f ), Vec2( 0.159786f, 0.099410f ), -0.196350f );
        addCaveRock( controlState, world, Vec2( -0.030830f, -0.943168f ), Vec2( 0.592734f, 0.072793f ), -1.865321f );
        addCaveRock( controlState, world, Vec2( 0.079845f, -1.564101f ), Vec2( 0.357232f, 0.350000f ), -0.029882f );
        addCaveRock( controlState, world, Vec2( 0.645272f, -1.566178f ), Vec2( 0.418472f, 0.472875f ), 0.490874f );
        addCaveRock( controlState, world, Vec2( 1.099760f, -0.826175f ), Vec2( 0.067311f, 0.014452f ), -1.914408f );
        addCaveRock( controlState, world, Vec2( 1.040125f, -1.028970f ), Vec2( 0.170000f, 0.053954f ), -1.619884f );
        addCaveRock( controlState, world, Vec2( 0.938632f, -1.242147f ), Vec2( 0.266529f, 0.148862f ), -1.521709f );
        addCaveRock( controlState, world, Vec2( 1.253729f, -1.867847f ), Vec2( 0.592073f, 0.350000f ), -0.098175f );
        addCaveRock( controlState, world, Vec2( 1.725265f, -1.917754f ), Vec2( 0.219998f, 0.350000f ), 0.785398f );
        addCaveRock( controlState, world, Vec2( 1.816924f, -1.848316f ), Vec2( 0.150000f, 0.350000f ), 0.098175f );
        addCaveRock( controlState, world, Vec2( 2.508066f, -1.835299f ), Vec2( 0.576170f, 0.350000f ), -0.000000f );
        addCaveRock( controlState, world, Vec2( 2.762851f, -1.662149f ), Vec2( 0.381957f, 0.350000f ), 1.547387f );
        addCaveRock( controlState, world, Vec2( 0.891221f, -0.912162f ), Vec2( 0.266529f, 0.018741f ), 0.549900f );
        addCaveRock( controlState, world, Vec2( 0.954404f, -0.911731f ), Vec2( 0.166621f, 0.021776f ), 0.549900f );
        addCaveRock( controlState, world, Vec2( 0.977642f, -0.950367f ), Vec2( 0.104814f, 0.034977f ), 0.549900f );
        addCaveRock( controlState, world, Vec2( 0.642072f, -0.734429f ), Vec2( 0.303551f, 0.067594f ), 0.480367f );
        addCaveRock( controlState, world, Vec2( 0.690330f, -0.570091f ), Vec2( 0.303551f, 0.067594f ), 0.480367f );
        addCaveRock( controlState, world, Vec2( 0.738934f, -0.409705f ), Vec2( 0.303551f, 0.067594f ), 0.480367f );
        addCaveRock( controlState, world, Vec2( 0.785999f, -0.226522f ), Vec2( 0.298755f, 0.087145f ), 0.480367f );
    }
    spawnBubble( controlState, Vec2( 1.08f, -0.74f ) );
    spawnBubble( controlState, Vec2( 1.20f, -0.78f ) );
    spawnBubble( controlState, Vec2( 1.16f, -0.87f ) );
    spawnBubble( controlState, Vec2( 1.25f, -0.93f ) );
    spawnBubble( controlState, Vec2( 1.18f, -0.99f ) );
    spawnBubble( controlState, Vec2( 1.29f, -1.05f ) );
    spawnBubble( controlState, Vec2( 1.18f, -1.11f ) );
    spawnBubble( controlState, Vec2( 1.30f, -1.17f ) );
    endSpawn( controlState );
    
    nextEvent_loadTrigger( controlState, Rect( 0.75f, -1.5f, 1.0f, -0.5f ) );
    nextEvent_spawnw( controlState );
    spawnlgArcher( controlState, Vec2( 1.6f, -1.3f ) );
    endSpawn( controlState );
    nextEvent_disableCameraSpring( controlState );
    nextEvent_moveCamerah( controlState, 1.0f, Vec2( 1.55f, -1.08f ) - Vec2( 0.825f, 0.5f ), false, false );
    nextEvent_setCameraBound( controlState, Rect( 1.55f, -10.0f, 20.0f, 10.0f ) );
    nextEvent_noEnemies( controlState );
    
    setBasePos( controlState, basePos );
    //firstEvent( controlState, Vec2( -0.1f, -0.1f ) );
    nextEvent_enableCameraSpring( controlState );
    nextEvent_spawnw( controlState );
    { // cave 08
        uint32 nObs       = 0;
        uint32 maxObs     = GAME_CONTROL__OBSTACLE_MAX_COUNT;
        LINE_SEG * obs = _pushArray_clear( tempMemory, LINE_SEG, maxObs );
        
        obs[ nObs++ ] = LineSegQP( Vec2( 0.0f, 0.0f ), 0.40f,   0.0f );
        obs[ nObs++ ] = LineSegQP( obs[  0 ].P,        0.40f,   PI *  8.0f / 64.0f );
        obs[ nObs++ ] = LineSegQP( obs[  1 ].P,        0.29f,   PI * 41.0f / 64.0f );
        obs[ nObs++ ] = LineSegQP( obs[  2 ].P,        0.15f,   PI * 30.0f / 64.0f );
        obs[ nObs++ ] = LineSegQP( obs[  3 ].P,        0.25f,   PI *  2.0f / 64.0f );
        obs[ nObs++ ] = LineSegQP( obs[  4 ].P,        0.35f,  -PI *  4.0f / 64.0f );
        obs[ nObs++ ] = LineSegQP( obs[  5 ].P,        0.10f,   0.0f );
        obs[ nObs++ ] = LineSegQP( obs[  6 ].P,        0.10f,  -PI * 32.0f / 64.0f );
        obs[ nObs++ ] = LineSegQP( obs[  7 ].P,        0.10f,   PI );
        obs[ nObs++ ] = LineSegQP( obs[  8 ].P,        0.45f,   PI * 63.0f / 64.0f );
        obs[ nObs++ ] = LineSegQP( obs[  9 ].P,        0.16f,  -PI * 18.0f / 64.0f );
        obs[ nObs++ ] = LineSeg( obs[  1 ].P + toVec2( 0.15f, PI * 8.0f / 64.0f ), obs[ 10 ].P );
        obs[ nObs++ ] = LineSegQP( obs[ 11 ].P,        0.40f,   PI *  8.0f / 64.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 12 ].P,        0.35f,   0.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 13 ].P,        0.40f,   PI *  8.0f / 64.0f );
        obs[ nObs++ ] = LineSegQP( obs[ 14 ].P,        0.25f,   0.0f );
        basePos = obs[ 15 ].P;
        
        obs[ nObs++ ] = LineSeg( obs[ 0 ].Q + Vec2( 0.0f, -0.2f ), 0.37f, PI * 2.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[ 16 ].Q,        0.13f,   PI * 16.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[ 17 ].Q,        0.88f,   PI *  8.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[ 18 ].Q,        0.37f,   PI *  1.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[ 19 ].Q,        0.37f,   PI *  8.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[ 20 ].Q, obs[ 15 ].P + Vec2( 0.0f, -0.08f ) );
        
        for( uint32 iObs = 0; iObs < nObs; iObs++ ) {
            addObstacle( controlState, world, Obstacle( obs[ iObs ], textureID_whiteTexture ) );
        }
        
        _popArray( tempMemory, LINE_SEG, maxObs );
    }
    { // cave 08 : background
        addCaveWall( controlState, world, Vec2( 1.840898f, 0.410441f ), Vec2( 0.435063f, 0.300569f ), 0.010496f );
        addCaveWall( controlState, world, Vec2( 0.991182f, 0.485007f ), Vec2( 0.488186f, 0.263701f ), 0.010496f );
        addCaveWall( controlState, world, Vec2( 1.259933f, 0.127831f ), Vec2( 0.483269f, 0.271084f ), 0.010496f );
        addCaveWall( controlState, world, Vec2( 0.407005f, 0.015510f ), Vec2( 0.431378f, 0.300569f ), 0.010496f );
        addCaveRock( controlState, world, Vec2( 0.200000f, 0.350000f ), Vec2( 0.200000f, 0.350000f ), -3.141593f );
        addCaveRock( controlState, world, Vec2( 0.559099f, 0.138526f ), Vec2( 0.200000f, 0.067097f ), -2.748893f );
        addCaveRock( controlState, world, Vec2( 0.620392f, 0.242926f ), Vec2( 0.145000f, 0.096422f ), -1.129010f );
        addCaveRock( controlState, world, Vec2( 0.374037f, 0.561035f ), Vec2( 0.429623f, 0.284508f ), -1.668971f );
        addCaveRock( controlState, world, Vec2( 0.702197f, 0.920332f ), Vec2( 0.314017f, 0.350000f ), -3.043418f );
        addCaveRock( controlState, world, Vec2( 1.101490f, 0.907592f ), Vec2( 0.223418f, 0.350000f ), 2.945243f );
        addCaveRock( controlState, world, Vec2( 1.450358f, 0.870731f ), Vec2( 0.198024f, 0.350000f ), -3.141593f );
        addCaveRock( controlState, world, Vec2( 1.495798f, 0.456840f ), Vec2( 0.063890f, 0.143464f ), 1.570796f );
        addCaveRock( controlState, world, Vec2( 1.302334f, 0.392056f ), Vec2( 0.050000f, 0.028675f ), -0.000000f );
        addCaveRock( controlState, world, Vec2( 1.044428f, 0.416989f ), Vec2( 0.242528f, 0.013939f ), -0.049087f );
        addCaveRock( controlState, world, Vec2( 0.871126f, 0.384822f ), Vec2( 0.088124f, 0.015970f ), 2.258020f );
        addCaveRock( controlState, world, Vec2( 0.922413f, 0.269994f ), Vec2( 0.058990f, 0.016326f ), 1.605336f );
        addCaveRock( controlState, world, Vec2( 1.093705f, 0.306751f ), Vec2( 0.208288f, 0.017932f ), -2.748893f );
        addCaveRock( controlState, world, Vec2( 1.487977f, 0.387394f ), Vec2( 0.139709f, 0.023844f ), -3.141593f );
        addCaveRock( controlState, world, Vec2( 1.732397f, 0.785760f ), Vec2( 0.258314f, 0.350000f ), -2.748893f );
        addCaveRock( controlState, world, Vec2( 1.930398f, 0.866623f ), Vec2( 0.316839f, 0.350000f ), -3.141593f );
        addCaveRock( controlState, world, Vec2( 0.332097f, -0.518985f ), Vec2( 0.299232f, 0.350000f ), 0.098175f );
        addCaveRock( controlState, world, Vec2( 0.560871f, -0.466055f ), Vec2( 0.207547f, 0.350000f ), 0.785398f );
        addCaveRock( controlState, world, Vec2( 1.000588f, -0.226787f ), Vec2( 0.440000f, 0.350000f ), 0.392699f );
        addCaveRock( controlState, world, Vec2( 1.583220f, -0.070238f ), Vec2( 0.293244f, 0.350000f ), 0.049087f );
        addCaveRock( controlState, world, Vec2( 1.896681f, 0.009468f ), Vec2( 0.240079f, 0.350000f ), 0.392699f );
        addCaveRock( controlState, world, Vec2( 2.131761f, 0.081021f ), Vec2( 0.131481f, 0.350000f ), 0.045357f );
        addCaveRock( controlState, world, Vec2( 1.024034f, 0.333216f ), Vec2( 0.107976f, 0.048601f ), 0.398131f );
        addCaveRock( controlState, world, Vec2( 1.193809f, 0.370638f ), Vec2( 0.068678f, 0.022205f ), 0.398131f );
        addCaveRock( controlState, world, Vec2( 1.139526f, 0.372853f ), Vec2( 0.068678f, 0.022205f ), 0.398131f );
        addCaveRock( controlState, world, Vec2( 1.086565f, 0.373410f ), Vec2( 0.068678f, 0.022205f ), 0.398131f );
        addCaveRock( controlState, world, Vec2( 0.987451f, 0.376151f ), Vec2( 0.086763f, 0.024925f ), 0.398131f );
        addCaveRock( controlState, world, Vec2( 0.942034f, 0.389033f ), Vec2( 0.065150f, 0.022205f ), 0.151895f );
        addCaveRock( controlState, world, Vec2( 0.903922f, 0.406539f ), Vec2( 0.042606f, 0.024925f ), 0.010496f );
    }
    endSpawn( controlState );
    
    setBasePos( controlState, basePos );
    nextEvent_spawnw( controlState );
    { // cave 09
        uint32 nObs       = 0;
        uint32 maxObs     = GAME_CONTROL__OBSTACLE_MAX_COUNT;
        LINE_SEG * obs = _pushArray_clear( tempMemory, LINE_SEG, maxObs );
        
        obs[ nObs++ ] = LineSegQP( Vec2( 0.0f, 0.0f ), 0.65f,  -PI *  2.0f / 64.0f );
        obs[ nObs++ ] = LineSegQP( obs[  0 ].P,        0.30f,  -PI *  3.0f / 64.0f );
        basePos = obs[ 1 ].P;
        
        obs[ nObs++ ] = LineSeg( obs[ 0 ].Q + Vec2( 0.0f, -0.08f ), 0.50f,   -PI * 33.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[  2 ].Q,        0.70f,  -PI *  1.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[  3 ].Q,        0.15f,   PI * 29.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[  4 ].Q,        0.22f,   PI *  4.0f / 64.0f );
        
        obs[ nObs++ ] = LineSegQP( obs[  1 ].P,        0.70f,   PI * 30.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[  5 ].Q,        0.72f,  -PI * 33.0f / 64.0f );
        
        for( uint32 iObs = 0; iObs < nObs; iObs++ ) {
            addObstacle( controlState, world, Obstacle( obs[ iObs ], textureID_whiteTexture ) );
        }
        
        _popArray( tempMemory, LINE_SEG, maxObs );
    }
    { // cave 09 : background
        addCaveWall( controlState, world, Vec2( 0.413759f, -0.311076f ), Vec2( 0.497946f, 0.373502f ), -0.097121f );
        addCaveRock( controlState, world, Vec2( 0.431486f, 0.309196f ), Vec2( 0.497946f, 0.350000f ), 3.043418f );
        addCaveRock( controlState, world, Vec2( 0.804696f, -0.022018f ), Vec2( 0.150000f, 0.064400f ), 2.994330f );
        addCaveRock( controlState, world, Vec2( -0.101917f, -0.372804f ), Vec2( 0.297452f, 0.087427f ), -1.619884f );
        addCaveRock( controlState, world, Vec2( 0.287568f, -0.945152f ), Vec2( 0.573400f, 0.350000f ), -0.049087f );
        addCaveRock( controlState, world, Vec2( 0.782969f, -0.553996f ), Vec2( 0.075000f, 0.098406f ), 1.423534f );
        addCaveRock( controlState, world, Vec2( 0.811494f, -0.478976f ), Vec2( 0.110000f, 0.035755f ), 0.196350f );
        addCaveRock( controlState, world, Vec2( 0.921450f, 0.246147f ), Vec2( 0.350000f, 0.056752f ), -1.668971f );
        addCaveRock( controlState, world, Vec2( 0.876533f, -0.781121f ), Vec2( 0.360000f, 0.018230f ), -1.619884f );
    }
#if 0
    { // cave 10
        uint32 nObs       = 0;
        uint32 maxObs     = GAME_CONTROL__OBSTACLE_MAX_COUNT;
        LINE_SEG * obs = _pushArray_clear( tempMemory, LINE_SEG, maxObs );
        
        obs[ nObs++ ] = LineSeg( Vec2( 0.0f, 0.0f ) + Vec2( 0.4f, -0.4f ), 0.45f,  -PI *  2.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[  0 ].Q,        0.30f,  -PI * 31.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[  1 ].Q,        0.50f,   PI * 63.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[  2 ].Q, obs[ 0 ].P );
        
        for( uint32 iObs = 0; iObs < nObs; iObs++ ) {
            addObstacle( controlState, world, Obstacle( obs[ iObs ], textureID_whiteTexture ) );
        }
        _popArray( tempMemory, LINE_SEG, maxObs );
    }
#endif
    {
        orect R = orectCD( Vec2( 0.65f, -0.35f ), Vec2( 0.03f, 0.65f ), -PI * 1.5f / 64.0f );
        spawnBreakable( controlState, textureID_breakable_plank, R, 10 );
        
        orect S = orectCD( Vec2( 0.9f, -0.25f ), Vec2( 0.06f, 0.4f ), -PI * 1.5f / 64.0f );
        spawnBreakable( controlState, textureID_breakable_plank, S, 10 );
    }
    endSpawn( controlState );
    
    nextEvent_newAnim( controlState );
    { // animation : crate hiding parrotfish
        animation = newAnimation( state, mem );
        animation->shouldLoop  = true;
        animation->isCameraRel = false;
        
        vec2 posA   = toWorldPos( controlState, Vec2( 0.6f, -0.55f ) );
        vec2 scaleA = Vec2( 160.0f, 160.0f );
        
        subanimation = newSubAnimation( state, animation );
        addHold( state, textureID_background_crate, 0.0f, 4.0f, posA, scaleA, -PI * 1.0f / 32.0f, COLOR_WHITE, true );
        
        endAnimation( state );
    }
    
    nextEvent_loadTrigger( controlState, Rect( 0.0f, -0.5f, 0.5f, 0.5f ) );
    nextEvent_enableMovementBound( controlState, Rect( 0.20f, -0.5f, 0.55f, -0.2f ) );
    nextEvent_spawnw( controlState );
    spawnlgArcher( controlState, Vec2( 0.22f, -0.45f ) );
    spawnlgArcher( controlState, Vec2( 0.30f, -0.30f ) );
    spawnlgArcher( controlState, Vec2( 0.45f, -0.22f ) );
    spawnlgArcher( controlState, Vec2( 0.48f, -0.46f ) );
    endSpawn( controlState );
    
    nextEvent_disableCameraSpring( controlState );
    nextEvent_moveCamerah( controlState, 1.0f, Vec2( 0.75f, -0.27f ) - Vec2( 0.825f, 0.5f ), false, false );
    nextEvent_noEnemies( controlState );
    nextEvent_hold( controlState, 1.0f );
    nextEvent_endAnimation( controlState );
    
    nextEvent_newAnim( controlState );
    { // animation : crate hiding parrotfish breaks
        animation = newAnimation( state, mem );
        animation->isCameraRel = false;
        
        vec2 posA   = toWorldPos( controlState, Vec2( 0.6f, -0.55f ) );
        vec2 posB   = toWorldPos( controlState, Vec2( 0.65f, -0.95f ) );
        vec2 scaleA = Vec2( 160.0f, 160.0f );
        
        subanimation = newSubAnimation( state, animation );
        addHold( state, textureID_background_crate, 0.0f, 2.0f, posA, scaleA, -PI * 1.0f / 32.0f, COLOR_WHITE, true );
        addKeyframe( state, textureID_background_crate, 3.0f, posB, scaleA * 1.1f, -PI * 3.0f / 4.0f, Vec4( 0.5f ) );
        
        addAnimShake( state, 0.0f, 60.0f, Vec2( 10.0f, 10.0f ), 1.0f, 1.0f, 0.1f );
        addAudio( state, 2.0f, audioID_swordfish_hitPlayer, 0.5f );
        
        endAnimation( state );
    }
    
    nextEvent_hold( controlState, 2.0f );
    nextEvent_spawnw( controlState );
    spawnParrotfish( controlState, Vec2( 0.55f, -0.55f ) );
    endSpawn( controlState );
    
    nextEvent_loadTrigger( controlState, Rect( 0.91f, -0.5f, 1.11f, 0.0f ) );
    setBasePos( controlState, basePos );
    //firstEvent( controlState, Vec2( 0.1f, -0.1f ) );
    nextEvent_exit( controlState );
    addCheckpoint( controlState );
    
    { // gen background after cave exit
        rect R = Rect( -0.2f, -0.65f, 2.6f, 1.0f );
        addBackgroundRect( controlState, world, textureID_background_cave_exit, R, COLOR_GRAY( 0.6f ), 0.9999f );
        
        vec2 atPos = getTL( R ) + Vec2( 1.0f, 1.0f );
        for( uint32 iter = 0; iter < 5; iter++ ) {
            BACKGROUND back = {};
            back.position  = atPos;
            back.scale     = Vec2( 1.0f, 1.0f );
            back.radians   = 0.0f;
            back.modelID   = modelID_rect;
            back.textureID = ( TEXTURE_ID )( ( uint32 )textureID_background_left + ( iter % 4 ) );
            back.modColor  = COLOR_WHITE;
            back.depth     = 1.0f;
            
            addBackground( controlState, world, back );
            
            atPos.x += 2.0f;
        }
    }
    
    setBasePos( controlState, Vec2( 1.4f, 0.0f ) );
    nextEvent_spawnw( controlState );
    { // cave 11
        uint32 nObs       = 0;
        uint32 maxObs     = GAME_CONTROL__OBSTACLE_MAX_COUNT;
        LINE_SEG * obs = _pushArray_clear( tempMemory, LINE_SEG, maxObs );
        
        obs[ nObs++ ] = LineSeg( Vec2( 1.2f, 0.0f ), 0.50f,  PI * 34.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[  0 ].Q,      0.05f,  PI *  3.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[  1 ].Q,      0.70f,  PI * 33.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[  2 ].Q,      0.05f,  PI *  6.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[  3 ].Q,      0.10f,  PI * 31.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[  4 ].Q,      0.17f,  PI *  1.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[  5 ].Q,      0.10f,  PI * 28.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[  6 ].Q,      0.25f, -PI *  2.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[  7 ].Q,      0.30f,  PI *  2.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[  8 ].Q,      0.12f,  PI * 16.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[  9 ].Q,      0.50f, -PI *  3.0f / 64.0f );
        obs[ nObs++ ] = LineSeg(   obs[ 10 ].Q,      0.65f, -PI * 16.0f / 64.0f );
        
        obs[ nObs++ ] = LineSegQP( obs[  0 ].P,      1.00f, -PI * 36.0f / 64.0f );
        
        for( uint32 iObs = 0; iObs < nObs; iObs++ ) {
            addObstacle( controlState, world, Obstacle( obs[ iObs ], textureID_whiteTexture ) );
        }
        _popArray( tempMemory, LINE_SEG, maxObs );
    }
    { // cave 11 : background
        addCaveRock( controlState, world, Vec2( 1.527110f, 0.249605f ), Vec2( 0.283659f, 0.350000f ), 1.668971f );
        addCaveRock( controlState, world, Vec2( 1.196299f, 0.362532f ), Vec2( 0.025000f, 0.140246f ), 0.147262f );
        addCaveRock( controlState, world, Vec2( 1.535199f, 0.823970f ), Vec2( 0.397769f, 0.350000f ), 1.619884f );
        addCaveRock( controlState, world, Vec2( 1.202633f, 1.169786f ), Vec2( 0.025000f, 0.043427f ), 0.294524f );
        addCaveRock( controlState, world, Vec2( 1.268722f, 1.227704f ), Vec2( 0.088220f, 0.054259f ), 1.521709f );
        addCaveRock( controlState, world, Vec2( 1.307530f, 1.245794f ), Vec2( 0.085000f, 0.076949f ), 0.049088f );
        addCaveRock( controlState, world, Vec2( 1.728613f, 1.241881f ), Vec2( 0.116985f, 0.350000f ), 1.374446f );
        addCaveRock( controlState, world, Vec2( 1.571552f, 1.332931f ), Vec2( 0.171618f, 0.075510f ), -0.098175f );
        addCaveRock( controlState, world, Vec2( 1.887524f, 1.071410f ), Vec2( 0.219517f, 0.350000f ), 0.098175f );
        addCaveRock( controlState, world, Vec2( 2.215832f, 1.195145f ), Vec2( 0.101853f, 0.350000f ), 0.785399f );
        addCaveRock( controlState, world, Vec2( 2.236304f, 1.131759f ), Vec2( 0.250000f, 0.350000f ), -0.147262f );
        addCaveRock( controlState, world, Vec2( 2.517276f, 0.963991f ), Vec2( 0.325000f, 0.350000f ), -0.785398f );
        addCaveRock( controlState, world, Vec2( 1.462360f, -0.475071f ), Vec2( 0.585241f, 0.350000f ), 1.374447f );
    }
    endSpawn( controlState );
    
    nextEvent_moveCamerah( controlState,  7.0f, Vec2( 0.55f,  0.15f ) - Vec2( 0.825f, 0.5f ) );
    nextEvent_killItems  ( controlState );
    nextEvent_killBubbles( controlState );
    nextEvent_moveCamerah( controlState,  9.0f, Vec2( 0.025f, 1.0f ) );
    nextEvent_moveCamerah( controlState,  5.0f, Vec2( 0.65f,  0.65f ) );
    nextEvent_moveCamerah( controlState,  7.0f, Vec2( 0.8f, -0.02f ) );
    
    nextEvent_spawnw( controlState );
    spawnCage( controlState, Vec2( 2.0f, 0.35f ) );
    endSpawn( controlState );
    
    nextEvent_moveCamerah( controlState,  6.0f, Vec2( 0.8f,  0.1f ) );
    
    nextEvent_spawnIf( controlState, &spawn );
    spawn->nParrotfish = 1;
    
    nextEvent_spawn( controlState, &spawn );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 1.95f, 0.25f );
    nextEvent_moveCamerah( controlState, 6.0f, Vec2( 0.75f, 0.1f ) );
    // TODO: animation : 'Help! Get us out of here!'
    
    nextEvent_noCages( controlState );
    
    nextEvent_moveCamerah( controlState, 1.0f, Vec2( 1.0f, 0.0f ) );
    nextEvent_spawnArcherh( controlState, 4, 1 );
    nextEvent_moveCamerah( controlState, 1.0f, Vec2( 1.0f, 0.0f ) );
    nextEvent_spawnArcherh( controlState, 6, 2 );
    nextEvent_spawn( controlState, &spawn );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 1.8f, 0.1f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 2.4f, 0.65f );
    nextEvent_moveCamerah( controlState, 1.0f, Vec2( 1.1f, -0.02f ) );
    // TODO: animation : 'Free us!'
    // TODO: gameplay : kill parrotfish
    nextEvent_exitEnemies( controlState, 6.0f );
    
    addCheckpoint( controlState );
    nextEvent_exit( controlState );
    
    nextEvent_spawnIf( controlState, &spawn );
    spawn->nParrotfish = 2;
    nextEvent_moveCamerah( controlState, 1.0f, Vec2( 1.0f, 0.0f ) );
    nextEvent_spawnArcherh( controlState, 6, 3 );
    nextEvent_moveCamerah( controlState, 1.0f, Vec2( 1.0f, 0.0f ) );
    nextEvent_spawnArcherh( controlState, 8, 4 );
    nextEvent_spawn( controlState, &spawn );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 1.85f, 0.15f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 2.5f, 0.2f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 3.0f, 0.13f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 2.05f, 0.4f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 2.85f, 0.45f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 1.88f, 0.75f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 2.4f, 0.8f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 3.05f, 0.75f );
    // TODO: add collision that blocks parrotfish, not destructible
    nextEvent_moveCamerah( controlState, 1.0f, Vec2( 1.65f, 0.0f ) );
    nextEvent_noCages( controlState );
    // TODO: gameplay : kill parrotfish
    nextEvent_exitEnemies( controlState, 6.0f );
    
    // TODO: gameplay : this should be secret room with 2-3 parrotfish
    nextEvent_spawn( controlState, &spawn );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 1.85f, 0.15f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 2.5f, 0.2f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 3.0f, 0.13f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 2.05f, 0.4f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 2.85f, 0.45f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 1.88f, 0.75f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 2.4f, 0.8f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 3.05f, 0.75f );
    
    addCheckpoint( controlState );
    nextEvent_exit( controlState );
    
#if 0    
    nextEvent_newAnim( controlState );
    { // animation : peasants trapped in cage 'Get us out of here!'
        animation = newAnimation( state, mem );
        
        char * strA = add( &state->stringBuffer, "Get us out of here!" );
        
        vec2 posA = Vec2( 1.25f, 0.34f );
        
        subanimation = newSubAnimation( state, animation );
        addHold( state, strA, textAlign_center, 0.5f, 2.0f, posA, 1.5f );
        
        endAnimation( state );
    }
#endif
    
    nextEvent_hold( controlState, 3.0f );
    nextEvent_spawnArcherh( controlState, 0, 4 );
    addCheckpoint( controlState );
    
    nextEvent_spawnArcherh( controlState, 8, 2 );
    nextEvent_spawnArcherh( controlState, 10, 3 );
    
    nextEvent_exit( controlState );
    addCheckpoint( controlState );
    
    nextEvent_spawn( controlState, &spawn );
    spawn->tower_bound[ spawn->nTower++ ] = rectCD( Vec2( 2.5f, 0.5f ), Vec2( 0.5f, 1.2f ) );
    
    nextEvent_moveCamerah( controlState, 0.5f, Vec2( 1.05f, -0.05f ) );
    nextEvent_hold( controlState, 1.0f );
    nextEvent_spawnArcher( controlState, 10, 0 );
    
    event = nextEvent( controlState );
    event->type = eventType_respawnEnemies;
    event->exit = exitCondition_towersAtHPOrLower;
    event->atHP = 0.0f;
    spawn = _pushType( mem, GAME_CONTROL__SPAWN );
    spawn->nArcher = 8;
    
    nextEvent_newAnim( controlState );
    { // animation : tower collapsing 'It's coming down!!'
        animation = newAnimation( state, mem );
        
        char * strA = add( &state->stringBuffer, "It's coming down!!" );
        
        vec2 posA = Vec2( 0.825f, 0.2f );
        
        subanimation = newSubAnimation( state, animation );
        addHold( state, strA, textAlign_center, 0.5f, 2.0f, posA, 2.0f );
        endAnimation( state );
    }
    
    nextEvent_hold( controlState, 2.0f );
    nextEvent_exitEnemies( controlState, 6.0f );
    
#if 0
    addCheckpoint( controlState );
    nextEvent_moveCamerah( controlState, 10.0f, Vec2( 0.75f, -0.15f ) );
    
    nextEvent_spawn( controlState, &spawn );
    spawn->nParrotfish = 1;
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 2.95f, 0.15f );
    
    nextEvent_moveCamerah( controlState, 24.0f, Vec2( 1.8f, -0.05f ) );
    
    event = nextEvent_hold( controlState, 0.0f );
    event->exit = exitCondition_noCages;
    
    addCheckpoint( controlState );
    
    nextEvent_spawnIf( controlState, &spawn );
    spawn->nParrotfish = 1;
    
    event = newChain( controlState );
    event->type = eventType_holdUntil;
    event->exit = exitCondition_targetTime;
    event->targetTime = 60.0f;
    uint32 scrollParrotfish_index = event->eventIndex;
    
    nextEvent_spawnArcher( controlState, 4, 0 );
    nextEvent_hold( controlState, 30.0f );
    nextEvent_spawnArcher( controlState, 6, 0 );
    nextEvent_hold( controlState, 20.0f );
    nextEvent_spawnArcher( controlState, 6, 1 );
    nextEvent_hold( controlState, 20.0f );
    nextEvent_spawnArcher( controlState, 8, 2 );
    nextEvent_hold( controlState, 20.0f );
    nextEvent_spawnArcher( controlState, 8, 2 );
    nextEvent_hold( controlState, 20.0f );
    nextEvent_spawnArcher( controlState, 12, 3 );
    endChain( controlState );
    
    _nextEvent_spawn( scrollParrotfish_index );
    event->exit = exitCondition_instant;
    // TODO: spawn more nets and cages!!
    spawn->cage_position[ spawn->nCage++ ] = Vec2(  2.45f, 0.15f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2(  3.45f, 0.10f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2(  4.25f, 0.6f  );
    spawn->net_position [ spawn->nNet++  ] = Vec2(  4.45f, 0.15f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2(  5.25f, 0.3f  );
    spawn->cage_position[ spawn->nCage++ ] = Vec2(  6.45f, 0.15f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2(  7.45f, 0.10f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2(  8.25f, 0.6f  );
    spawn->net_position [ spawn->nNet++  ] = Vec2(  8.45f, 0.15f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2(  9.25f, 0.3f  );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 10.45f, 0.15f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 11.45f, 0.10f );
    spawn->cage_position[ spawn->nCage++ ] = Vec2( 12.25f, 0.6f  );
    spawn->net_position [ spawn->nNet++  ] = Vec2( 12.45f, 0.15f );
    
    event = nextEvent( controlState );
    event->type = eventType_moveCamera;
    event->exit = exitCondition_targetTime;
    event->targetTime = 180;
    _writem( mem, vec2, Vec2( 13.0f, 0.0f ) );
    
    
    event = nextEvent( controlState );
    event->type = eventType_animation;
    event->exit = exitCondition_targetTime;
    event->targetTime = 1.25f;
    { // animation : giant shadow passes overhead
        animation = newAnimation( state, mem );
        
        char * strA = add( &state->stringBuffer, "Auuughh! Swim away!" );
        
        vec2 posA = Vec2( 0.825f, 0.1f );
        
        subanimation = newSubAnimation( state, animation );
        addKeyframe( state, strA, textAlign_center, 0.5f, posA, Vec2( 2.0f, 2.0f ), 0.0f, COLOR_BLACK, true );
        addKeyframe( state, strA, textAlign_center, 2.5f, posA, Vec2( 2.0f, 2.0f ), 0.0f, COLOR_BLACK, false );
        
        vec2 posB = Vec2( -0.66f, 0.5f );
        vec2 posC = Vec2(  2.45f, 0.5f );
        
        subanimation = newSubAnimation( state, animation );
        vec4 color = Vec4( 1.0f, 1.0f, 1.0f, 0.10f );
        addKeyframe( state, textureID_turtle_shadow, 2.0f, posB, Vec2( 1100.0f, 600.0f ), 0.0f, color );
        addKeyframe( state, textureID_turtle_shadow, 8.0f, posC, Vec2( 1100.0f, 600.0f ), 0.0f, color );
        addCameraShake( state, 0.0f, 60.0f, Vec2( 10.0f, 10.0f ), 3.0f, 0.0f, 3.0f );
        
        vec2 posD = Vec2( -0.66f, 1.12f );
        vec2 posE = Vec2(  2.45f, 1.12f );
        
        subanimation = newSubAnimation( state, animation );
        addKeyframe( state, textureID_turtle_idle, 2.0f, posD, Vec2( 800.0f, 300.0f ), 0.0f, COLOR_GRAY( 0.35f ) );
        addKeyframe( state, textureID_turtle_idle, 8.0f, posE, Vec2( 800.0f, 300.0f ), 0.0f, COLOR_GRAY( 0.35f ) );
        // TODO: add rumble sound
        endAnimation( state );
    }
    
    nextEvent_exitEnemies( controlState, 6.0f );
#endif
    
    addCheckpoint( controlState );
    nextEvent_exit( controlState );
    
    event = nextEvent_newAnimh( controlState );
    { // animation : injured peasant A
        animation = newAnimation( state, mem );
        
        char * strA = add( &state->stringBuffer, "Help!" );
        char * strB = add( &state->stringBuffer, "Oh, please! Won't someone help us!" );
        
        vec2 posA = Vec2( 1.6f, 0.7f );
        
        subanimation = newSubAnimation( state, animation );
        addHold( state, strA, textAlign_centerRight, 0.5f, 2.0f, posA, 2.0f );
        
        subanimation = newSubAnimation( state, animation );
        addHold( state, strB, textAlign_centerRight, 2.5f, 5.0f, posA, 1.5f );
        
        endAnimation( state );
    }
    
    event = newChain_newAnimh( controlState );
    uint32 healPeasant_index = event->eventIndex;
    { // animation : injured peasant B
        animation = newAnimation( state, mem );
        
        char * strA = add( &state->stringBuffer, "Oh, thank goodness!" );
        char * strB = add( &state->stringBuffer, "Help us! He's terribly injured!" );
        char * strC = add( &state->stringBuffer, "*cough*" );
        char * strD = add( &state->stringBuffer, "Don't bother! I'm done for!" );
        char * strE = add( &state->stringBuffer, "Please, isn't there some way you could HEAL him?" );
        
        vec2 posA = Vec2( 1.1f, 0.2f  );
        vec2 posB = Vec2( 1.0f, 0.15f );
        vec2 posC = posA + Vec2(  0.05f, 0.05f );
        vec2 posD = posB + Vec2( -0.05f, 0.05f );
        
        flo32 targetTime = 13.0f;
        
        subanimation = newSubAnimation( state, animation );
        addKeyframe( state, textureID_peasant_idle, 0.0f, posA + Vec2( 1.65f, 0.0f ), Vec2( -35.0f, 35.0f ), 0.0f, COLOR_WHITE );
        nextKey( state, 1.0f, posA );
        nextKey( state, targetTime, posA );
        
        subanimation = newSubAnimation( state, animation );
        addKeyframe( state, textureID_peasant_stunned, 0.0f, posB + Vec2( 1.65f, 0.0f ), Vec2( 35.0f, 35.0f ), 0.0f, COLOR_WHITE );
        nextKey( state, 1.0f, posB );
        nextKey( state, targetTime, posB );
        
        subanimation = newSubAnimation( state, animation );
        addHold( state, strA, textAlign_centerLeft,   1.5f, 3.0f, posC, 1.5f );
        addHold( state, strB, textAlign_centerLeft,   3.5f, 5.5f, posC, 1.5f );
        addHold( state, strC, textAlign_centerRight,  6.0f, 7.0f, posD, 1.5f );
        addHold( state, strD, textAlign_centerRight,  7.5f, 9.5f, posD, 1.5f );
        addHold( state, strE, textAlign_centerLeft,  10.0f, targetTime, posC, 1.5f );
        
        endAnimation( state );
    }
    
    nextEvent_newAnim( controlState );
    { // animation : injured peasant B
        animation = newAnimation( state, mem );
        animation->shouldLoop = true;
        
        char * strA = add( &state->stringBuffer, "Please, HEAL him!" );
        
        vec2 posA = Vec2( 1.1f, 0.2f  );
        vec2 posB = Vec2( 1.0f, 0.15f );
        vec2 posC = posA + Vec2(  0.05f, 0.05f );
        vec2 posD = posB + Vec2( -0.05f, 0.05f );
        
        subanimation = newSubAnimation( state, animation );
        addKeyframe( state, textureID_peasant_idle, 0.0f, posA, Vec2( -35.0f, 35.0f ), 0.0f, COLOR_WHITE, true );
        
        subanimation = newSubAnimation( state, animation );
        addKeyframe( state, textureID_peasant_stunned, 0.0f, posB, Vec2( 35.0f, 35.0f ), 0.0f, COLOR_WHITE, true );
        
        subanimation = newSubAnimation( state, animation );
        addHold( state, strA, textAlign_centerLeft, 5.0f, 7.0f, posC, 1.5f );
        nextKey( state, 13.0f, posC );
        
        endAnimation( state );
    }
    endChain( controlState );
    
    event = nextEvent_moveCamerah( controlState, 1.0f, Vec2( 1.65f, 0.0f ) );
    event->subEventIndex = healPeasant_index;
    
    nextEvent_spawn( controlState, &spawn );
    spawn->heal_bound[ spawn->nHeal++ ] = rectCD( Vec2( 1.0f, 0.15f ), Vec2( 0.1f, 0.1f ) );
    
    event = nextEvent( controlState );
    event->type = eventType_holdUntil;
    event->exit = exitCondition_noHeal;
    
    nextEvent_killChain( controlState, healPeasant_index );
    nextEvent_endAnimation( controlState );
    
    nextEvent_newAnimh( controlState );
    { // animation : injured peasant C
        animation = newAnimation( state, mem );
        
        char * strA = add( &state->stringBuffer, "Wow! I feel great!" );
        char * strB = add( &state->stringBuffer, "Oh, thank you so much!" );
        char * strC = add( &state->stringBuffer, "I feel ready to take on the world!" );
        char * strD = add( &state->stringBuffer, "Nothing can stop me! There's nothing that can-" );
        char * strE = add( &state->stringBuffer, "Aaauggh! Soldiers! Swim away!" );
        
        vec2 posA = Vec2( 1.1f, 0.2f  );
        vec2 posB = Vec2( 1.0f, 0.15f );
        vec2 posC = Vec2( 0.99f, 0.19f );
        vec2 posD = posA + Vec2(  0.05f, 0.05f );
        vec2 posE = posC + Vec2( -0.05f, 0.05f );
        vec2 posF = Vec2( -0.1f, 0.2f );
        vec2 posG = Vec2( -0.08f, 0.22f );
        
        flo32 endTime = 12.0f;
        
        subanimation = newSubAnimation( state, animation );
        addHold( state, textureID_peasant_idle, 0.0f, endTime, posA, Vec2( -35.0f, 35.0f ), 0.0f, COLOR_WHITE, true );
        addKeyframe( state, textureID_peasant_idle, endTime + 1.0f, posF, Vec2( -35.0f, 35.0f ), 0.0f, COLOR_WHITE );
        
        subanimation = newSubAnimation( state, animation );
        addKeyframe( state, textureID_peasant_idle, 0.0f, posB, Vec2( 35.0f, 35.0f ), 0.0f, COLOR_WHITE );
        nextKey( state, 0.5f, posC );
        nextKey( state, endTime, posC );
        nextKey( state, endTime, posC, Vec2( -35.0f, 35.0f ) );
        nextKey( state, endTime + 1.0f, posG );
        
        subanimation = newSubAnimation( state, animation );
        addHold( state, strA, textAlign_centerRight, 1.5f, 3.0f, posE, 1.5f );
        addHold( state, strB, textAlign_centerLeft,  3.5f, 5.0f, posD, 1.5f );
        addHold( state, strC, textAlign_centerRight, 5.5f, 7.0f, posE, 1.5f );
        addHold( state, strD, textAlign_centerRight, 7.5f, 9.0f, posE, 1.5f );
        addHold( state, strE, textAlign_centerRight, 9.5f, endTime, posE, 1.5f );
        
        endAnimation( state );
    }
    
    event = newChain_hold( controlState, 1.0f );
    uint32 turtleIntro_index = event->eventIndex;
    
    nextEvent_newAnim( controlState );
    { // animation : shadowed turtle in background
        animation = newAnimation( state, mem );
        
        vec2 posA = Vec2( -0.25f, 0.5f );
        vec2 posB = Vec2( 1.9f, 0.5f );
        
        subanimation = newSubAnimation( state, animation );
        addKeyframe( state, textureID_turtle_idle, 1.0f, posA, Vec2( 200.0f, 200.0f ), 0.0f, COLOR_GRAY( 0.2f ) );
        nextKey( state, 4.5f, posB );
        
        addCameraShake( state, 0.0f, 60.0f, Vec2( 6.0f, 6.0f ), 1.5f, 3.0f, 1.0f );
        
        endAnimation( state );
    }
    
    nextEvent_hold( controlState, 5.0f );
    nextEvent_spawn( controlState, &spawn );
    spawn->turtle = GameControl_turtle( 8.0f, 10.0f, true, 0.5f );
    
    endChain( controlState );
    
    event = nextEvent_spawnh( controlState, &spawn );
    event->subEventIndex = turtleIntro_index;
    spawn->nParrotfish = 3;
    spawn->nArcher     = 8;
    spawn->nlgArcher   = 4;
    
    event = nextEvent_spawnh( controlState, &spawn );
    event->exit = exitCondition_none;
    spawn->nsmTurtle = 1;
    spawn->nlgArcher = 24;
    
    event = newChain_spawnIf( controlState, &spawn );
    spawn->nsmTurtle = 3;
    uint32 turtleCharge_index = event->eventIndex;
    
    nextEvent_moveCamerah( controlState, 30.0f, Vec2( 2.1f,  0.0f ) );
    nextEvent_moveCamerah( controlState,  6.0f, Vec2( 0.5f, -0.3f ) );
    nextEvent_moveCamerah( controlState, 30.0f, Vec2( 2.1f,  0.0f ) );
    nextEvent_moveCamerah( controlState, 20.0f, Vec2( 1.4f,  0.1f ) );
    endChain( controlState );
    
    event = nextEvent( controlState );
    event->type = eventType_scrollingSpawn;
    event->exit = exitCondition_targetTime;
    event->targetTime = 86.0f;
    event->subEventIndex = turtleCharge_index;
    spawn = _pushType( mem, GAME_CONTROL__SPAWN );
    spawn->nArcher   = 24;
    spawn->nlgArcher = 10;
    
    
    // TODO: animation : slowly continually advance camera
    // TODO: gameplay : 4 lgarcher, 10 archers continually respawn, turtle continually crosses screen
    
    // TODO: animation : turtle attacks player
    //  SOLDIER #1 : Waaaaa-ait!!!
    //  SOLDIER #1 : You guys are on the same side!
    // TODO: animation : soldiers enter
    //  SOLDIER #1 : This [player] has been helping us!
    //  TURTLE : Oooh, sorry if I hurt you!
    //  SOLDIER #1 : This is [turtle name], the guardian of our city!
    //  TURTLE : Sorry! Sometimes I get carried away...
    //  TURTLE : ...when I'm taking out the trash! Raaawwwr!
    // TODO: animation : turtle exits
    //  SOLDIER #1 : You heard him, men! Take it to 'em!
    // TODO: should I explain healing them when they go unconscious?
    // TODO: gameplay : 4 soldiers, unlimited health drops, 6 lgarchers, 12 archers continually respawn
    
    // TODO: gameplay : 1 cage, 1 humphead
    // TODO: animation : camera scrolls slowly
    // TODO: gameplay : 3 humphead, random nets and cages scroll across the screen
    // TODO: gameplay : turtle continually crosses screen until humphead are dead
    
    // TODO: gameplay : 1 humphead, 2 lgarcher, 10 archers
    // TODO: gameplay : 1 humphead (from previous), 6 lgarcher, 16 archers
    // TODO: how do I get rid of the humphead?
    
    // TODO: gameplay : starfish
    // TODO: gameplay : sentry gun on tower, 10 archers, lure bubblets to destroy tower
    // TODO: gameplay : 4 starfish on giant peasant cage, 6 lgarchers, 14 archers
    
    // TODO: gameplay : bubblon gen, canopy, peasants crossing screen for bonus xp, 1 lgarchers, 4 archers, lgarchers and archers continually respawn
    // TODO: animation : turtle attacked by giant cannon
    // TODO: gameplay : protect turtle from bubblon, 2 lgarcher, 8 archers, lgarchers and archers continually respawn
    
    // TODO: gameplay : squid gunner
    // TODO: gameplay : squid gunner
    // TODO: gameplay : squid gunner
    // TODO: gameplay : squid gunner
    // TODO: gameplay : boss fish : bubblon cannon
    
    
#if 0    
    _nextEvent_spawn( -1 );
    spawn->nRainbow = 1; // RandomU32InRange( 2, 3 );
    
    addCheckpoint( controlState );
    
    event = nextEvent( controlState );
    event->type = eventType_animation;
    event->exit = exitCondition_noAnimation;
    {
        animation = newAnimation( state, mem );
        
        vec2 scale = Vec2( 200.0f, 100.0f );
        vec2 posA  = getTL( appState->cameraBound ) + Vec2( -100.0f, -100.0f );
        vec2 posB  = getTL( appState->cameraBound ) + Vec2(  200.0f, -120.0f );
        vec2 posC  = Vec2(  800.0f, 860.0f );
        vec2 posD  = Vec2( 1460.0f, 680.0f );
        vec2 posE  = getBR( appState->cameraBound ) + Vec2(  1400.0f,  200.0f );
        
        subanimation = newSubAnimation( state, animation );
        addKeyframe( state, textureID_explosion01, 2.0f, posB + Vec2( 50.0f, 10.0f ), Vec2( 50.0f, 50.0f ), 0.0f, COLOR_WHITE );
        addKeyframe( state, textureID_explosion01, 2.1f, posB + Vec2( 50.0f, 10.0f ), Vec2( 60.0f, 60.0f ), 1.0f, COLOR_WHITE ); 
        addAudio( state, 2.0f, audioID_swordfish_hitPlayer, 0.8f );
        
        subanimation = newSubAnimation( state, animation );
        addKeyframe( state, textureID_turtle_idle,  0.0f, posA, scale * 0.25f, 0.0f, COLOR_BLACK );
        addKeyframe( state, textureID_turtle_idle,  2.0f, posB, scale * 0.26f, 0.0f, COLOR_BLACK );
        addKeyframe( state, textureID_turtle_idle,  5.0f, posC, scale * 0.48f, 0.0f, COLOR_GRAY( 0.25f ) );
        addKeyframe( state, textureID_turtle_idle,  8.0f, posD, scale * 1.5f,  0.0f, COLOR_GRAY( 0.5f ) );
        addKeyframe( state, textureID_turtle_idle, 11.0f, posE, scale * 3.5f,  0.0f, COLOR_WHITE );
        addKeyframe( state, textureID_turtle_idle, 13.0f, posE, scale * 3.5f,  0.0f, COLOR_WHITE );
        addCameraShake( state, 10.0f, 60.0f, Vec2( 40.0f, 40.0f ), 0.0f, 0.0f, 1.5f );
        
        vec2 offsetA = Vec2( 200.0f, 100.0f );
        vec2 offsetB = Vec2( 300.0f, 50.0f );
        vec2 offsetC = Vec2( 400.0f, 100.0f );
        
        subanimation = newSubAnimation( state, animation );
        addKeyframe( state, textureID_whiteTexture,  5.0f, posC + offsetA, Vec2( 150.0f, 40.0f ), 0.0f, COLOR_WHITE );
        addKeyframe( state, textureID_whiteTexture,  8.0f, posD + offsetB, Vec2( 150.0f, 40.0f ), 0.0f, COLOR_WHITE );
        addKeyframe( state, textureID_whiteTexture, 11.0f, posE + offsetC, Vec2( 150.0f, 40.0f ), 0.0f, COLOR_WHITE );
        
        char * strA = add( &state->stringBuffer, "AAAAUUUUUGGH!!!!" );
        
        subanimation = newSubAnimation( state, animation );
        addKeyframe( state, strA, textAlign_center,  5.0f, posC + offsetA, Vec2( 2.0f, 2.0f ), 0.0f, COLOR_BLACK );
        addKeyframe( state, strA, textAlign_center,  8.0f, posD + offsetB, Vec2( 2.0f, 2.0f ), 0.0f, COLOR_BLACK );
        addKeyframe( state, strA, textAlign_center, 11.0f, posE + offsetC, Vec2( 2.0f, 2.0f ), 0.0f, COLOR_BLACK );
        endAnimation( state );
    }
    
    event = newChain( controlState );
    event->type = eventType_animation;
    event->exit = exitCondition_noAnimation;
    uint32 turtleCrash_index = event->eventIndex;
    {
        animation = newAnimation( state, mem );
        
        rect bound = rectBCD( getBC( appState->cameraBound ) + Vec2( 200.0f, 0.0f ), Vec2( 650.0f, 140.0f ) );
        vec2 offset = Vec2( 25.0f, -40.0f );
        vec2 scale  = Vec2( TURTLE_BITMAP_SCALE_X, TURTLE_BITMAP_SCALE_Y );
        vec2 posA = getCenter( bound ) + offset + Vec2( 1800.0f, 0.0f );
        vec2 posB = getCenter( bound ) + offset;
        
        subanimation = newSubAnimation( state, animation );
        addKeyframe( state, textureID_turtle_idle, 0.0f, posA, scale, 0.0f, COLOR_WHITE );
        addKeyframe( state, textureID_turtle_idle, 1.0f, posB, scale, 0.0f, COLOR_WHITE );
        endAnimation( state );
    }
    endChain( controlState );
    
    event = nextEvent( controlState );
    event->type = eventType_moveCamera;
    event->exit = exitCondition_targetTime;
    event->subEventIndex = turtleCrash_index;
    event->targetTime = 1.0f;
    _writem( mem, vec2, Vec2( 400.0f, 0.0f ) );
    
    
    // RAINBOW SCHOOL
    event = newChain( controlState );
    event->type = eventType_holdUntil;
    event->exit = exitCondition_targetTime;
    event->targetTime = 6.0f; // 22.0f;
    int32 rainbowSchool_index = event->eventIndex;
    
    event = nextEvent( controlState );
    event->type = eventType_exitRainbow;
    event->exit = exitCondition_instant;
    endChain( controlState );
    // END RAINBOW SCHOOL
    
    // temp test : single instant event
    event = newChain( controlState );
    event->type = eventType_spawnEnemies;
    event->exit = exitCondition_instant;
    spawn = _pushType( mem, GAME_CONTROL__SPAWN );
    spawn->nUrchin = 1;
    uint32 tempEvent_index = event->eventIndex;
    endChain( controlState );
    // end temp test
    
    _nextEvent_spawn( rainbowSchool_index );
    spawn->nRainbow = RandomU32InRange( 16, 20 );
    
    addCheckpoint( controlState );
    
    _nextEvent_spawn( tempEvent_index );
    spawn->nArcher   = RandomU32InRange( 6, 8 );
    spawn->nlgArcher = RandomU32InRange( 6, 8 );
    
    addCheckpoint( controlState );
#endif
    
    
    //_nextEvent_spawn( -1 );
    //spawn->net[ spawn->nNet++ ] = SpawnNet( Vec2( 1500.0f, 160.0f ), Vec2( 250.0f, 175.0f ) );
    
    event = nextEvent( controlState );
    event->type = eventType_winGame;
    event->exit = exitCondition_none;
    
    uint32 breakHere = 10;
    
#if 0
    // default exit_state = waveSuccess_killAllEnemies
    // default exit_bound = rectRCD( getRC( appState->cameraBound ), Vec2( 340.0f, 340.0f ) );
    wave = newWave( appState );
    wave->nRainbow = 3;
    
    wave = newWave( appState );
    wave->nRainbow = RandomU32InRange( 6, 8 );
    
    wave = newWave( appState );
    wave->nRainbow  = RAINBOW_MAX_COUNT;
    wave->hasSchool = true;
    
    wave = newWave( appState );
    wave->nRainbow = RandomU32InRange( 6, 8 );
    
    wave = newWave( appState );
    wave->nRainbow  = RAINBOW_MAX_COUNT;
    wave->hasSchool = true;
    
    wave = newWave( appState );
    wave->nDogfish = 1;
    
    wave = newWave( appState );
    wave->nGoldfish = 1;
    wave->nDogfish  = 1;
    
    wave = newWave( appState );
    wave->nGoldfish = 4;
    
    wave = newWave( appState );
    wave->nRainbow  = RAINBOW_MAX_COUNT;
    wave->hasSchool = true;
    wave->nDogfish  = 2;
    
    wave = newWave( appState );
    wave->nDogfish = 6;
    
    wave = newWave( appState );
    wave->nUrchin  = 1;
    
    wave = newWave( appState );
    wave->nArcher   = 4;
    wave->nlgArcher = 4;
    wave->enableBubblonGen       = true;
    wave->disableBubblonGenAtEnd = true;
    wave->enableTurtle           = true;
    wave->disableTurtleAtEnd     = true;
    wave->cage[ wave->nCage++ ] = WaveCage( Vec2( 1500.0f, 160.0f ), Vec2( 150.0f, 100.0f ) );
    
    wave = newWave( appState );
    {
        wave->exit_state = waveSuccess_defendZone;
        
        DEFEND_ZONE_STATE state = {};
        rect bound = rectBCD( getBC( appState->cameraBound ) + Vec2( 200.0f, 0.0f ), Vec2( 650.0f, 140.0f ) );
        state.isActive  = true;
        state.position  = getCenter( bound );
        state.halfDim   = getHalfDim( bound );
        state.maxHealth = 40;
        state.health    = state.maxHealth;
        state.textureID = textureID_turtle_idle;
        state.texture_offset = Vec2( 25.0f, -40.0f );
        state.texture_scale  = Vec2( TURTLE_BITMAP_SCALE_X, TURTLE_BITMAP_SCALE_Y );
        wave->defendZone = state;
        
        wave->nArcher = 6;
    }
    
    wave = newWave( appState );
    wave->nRainbow  = RAINBOW_MAX_COUNT;
    wave->nGoldfish = GOLDFISH_MAX_COUNT;
    wave->nDogfish  = DOGFISH_MAX_COUNT;
    wave->nArcher   = ARCHER_MAX_COUNT;
    wave->nlgArcher = LGARCHER_MAX_COUNT;
    
    restartAtWave = true;
    atWave        = 12;
#endif // create waves *** remove this code ***
    
#endif
    
#if BOSS_FIGHT
    //wave = newWave( appState );
    //wave->nSwordfish = 1;
    
    event = nextEvent( controlState );
    event->type = eventType_spawnEnemies;
    event->exit = exitCondition_none;
    spawn = _pushType( mem, GAME_CONTROL__SPAWN );
    spawn->nSwordfish = 1;
#endif // BOSS_FIGHT
}

internal void
startGame( MEMORY * tempMemory, APP_STATE * appState, AUDIO_SYSTEM * audioSystem, boo32 startFromCheckpoint ) {
    appState->gameMode = gameMode_game;
    appState->gameOver = false;
    appState->gameOver_timer = 0.0f;
    appState->restartGame_timer    = 0.0f;
    appState->restartGame_isActive = false;
    appState->restartCheckpoint_timer    = 0.0f;
    appState->restartCheckpoint_isActive = false;
    
    appState->pointsList.nParticles = 0;
    appState->colorChain.count = 0;
    
    // TODO: this currently assumes that the player is starting the game from the beginning! this will change for checkpoints and loading a save game!
    //memset( &appState->itemState,      0, sizeof( ITEM_STATE     ) );
    //memset( &appState->item_storage,   0, sizeof( ITEM_STORAGE   ) );
    //memset( &appState->item_inventory, 0, sizeof( ITEM_INVENTORY ) );
    // TODO: item toolbar
    ITEM_INVENTORY * inventory = &appState->item_inventory;
    inventory->nSlots     = 1;
    inventory->activeSlot = 0;
    
    CAMERA_STATE * cameraState = &appState->cameraState;
    memset( cameraState, 0, sizeof( CAMERA_STATE ) );
    // TODO: init_cameraDim is temporary. These will actually be set as part of gameplay. This will also also us to include camera zooming.
    cameraState->dim  = appState->init_cameraDim;
    cameraState->targetTime = 1.0f;
    appState->cameraBound = rectCD( cameraState->at, cameraState->dim );
    appState->unitScale   = getHeight( appState->cameraBound );
    
    // TODO: temp! Add camera and background pos to gameplay and checkpoints
    appState->backgroundPos = {};
    
    PLAYER_STATE * player = &appState->playerState;
    memset( player, 0, sizeof( PLAYER_STATE ) );
    player->health = 1.0f;
    
    memset( &appState->rainbowState,     0, sizeof( RAINBOW_STATE      ) );
    memset( &appState->goldfishState,    0, sizeof( GOLDFISH_STATE     ) );
    memset( &appState->dogfishState,     0, sizeof( DOGFISH_STATE      ) );
    memset( &appState->swordfishState,   0, sizeof( SWORDFISH_STATE    ) );
    memset( &appState->urchinState,      0, sizeof( URCHIN_STATE       ) );
    memset( &appState->urchinSpineState, 0, sizeof( URCHIN_SPINE_STATE ) );
    memset( &appState->archerState,      0, sizeof( ARCHER_STATE       ) );
    memset( &appState->bubbletState,     0, sizeof( BUBBLET_STATE      ) );
    memset( &appState->lgArcherState,    0, sizeof( LGARCHER_STATE     ) );
    memset( &appState->bubbleState,      0, sizeof( BUBBLE_STATE       ) );
    memset( &appState->bubblonState,     0, sizeof( BUBBLON_STATE      ) );
    memset( &appState->bubblonGen,       0, sizeof( BUBBLON_GEN        ) );
    memset( &appState->parrotfishState,  0, sizeof( PARROTFISH_STATE   ) );
    memset( &appState->turtleState,      0, sizeof( TURTLE_STATE       ) );
    memset( &appState->defendZoneState,  0, sizeof( DEFEND_ZONE_STATE  ) );
    memset( &appState->towerState,       0, sizeof( TOWER_STATE        ) );
    memset( &appState->obstacleState,    0, sizeof( OBSTACLE_STATE     ) );
    memset( &appState->cageState,        0, sizeof( CAGE_STATE         ) );
    memset( &appState->netState,         0, sizeof( NET_STATE          ) );
    memset( &appState->smTurtleState,    0, sizeof( SMTURTLE_STATE     ) );
    
    audioSystem->nAudio = 0;
    playAudio( audioSystem, appState->audio, appState->musicPlaying, 0.30, 1.0f, true );
    
    //spawnDogfish ( appState, DOGFISH_MAX_COUNT );
    //spawnGoldfish( appState, GOLDFISH_MAX_COUNT );
    
    reset( &appState->animationSystem );
    reset( &appState->gameControlState );
    reset( &appState->worldSystem );
    
    loadGameScenarios( tempMemory, appState );
    initWorldSystem( tempMemory, appState );
    GAME_CONTROL_STATE * controlState = &appState->gameControlState;
    controlState->event[ 0 ].nextEventIndex = controlState->firstEvent_index;
    
    // TODO: init_cameraDim is temporary. These will actually be set as part of gameplay. This will also also us to include camera zooming.
    cameraState->at   = controlState->firstEvent_pos;
    cameraState->pos  = controlState->firstEvent_pos;
    cameraState->dest = controlState->firstEvent_pos;
    player->position  = controlState->firstEvent_pos;
    
    cameraState->springIsEnabled = controlState->firstEvent_springIsEnabled;
    appState->world_cameraBound  = controlState->firstEvent_cameraBound;
    appState->world_basePos      = controlState->firstEvent_basePos;
    
    cameraState->dim  = appState->init_cameraDim;
    cameraState->targetTime = 1.0f;
    appState->cameraBound = rectCD( cameraState->at, cameraState->dim );
    appState->unitScale   = getHeight( appState->cameraBound );
    
    // TODO: temp! Add camera and background pos to gameplay and checkpoints
    appState->backgroundPos = controlState->firstEvent_pos;
    
    if( startFromCheckpoint ) {
        controlState->event[ 0 ].nextEventIndex = appState->checkpoint_eventIndex;
        appState->cameraBound   = appState->checkpoint_cameraBound;
        appState->backgroundPos = appState->checkpoint_backgroundPos;
        
        appState->score = 0;
        
        appState->nBites     = 0;
        appState->nThingsAte = 0;
        
        memset( &appState->secretState, 0, sizeof( SECRET_STATE ) );
    }
    
    controlState->nChain     = 1;
    controlState->chain[ 0 ] = controlState->event[ 0 ];
    Assert( controlState->event[ 0 ].subEventIndex == -1 );
    
    appState->entityUpdate_isEnabled = true;
    updateGameControl( appState, 0.0f );
    updateWorldSystem( appState, 0.0f );
}