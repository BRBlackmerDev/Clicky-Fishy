
internal void
updateTurtle( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, flo32 dt ) {
    TURTLE_STATE * state = &appState->turtleState;
    
    if( state->isActive ) {
        flo32 maxSpeed = TURTLE_MOVE_SPEED;
        flo32 friction = TURTLE_MOVE_FRICTION;
        
        state->timer += dt;
        
        vec2 dir = {};
        switch( state->state ) {
            case fishState_idle: {
                dir = state->direction;
            } break;
        }
        
        vec2 accel = ( dir * maxSpeed - state->velocity ) * friction;
        state->position += accel * ( dt * dt * 0.5f ) + state->velocity * dt;
        state->velocity += accel * dt;
    }
}

internal rect
getTurtleCollision( TURTLE_STATE * state ) {
    Assert( state->isActive );
    
    vec2 dim = TURTLE_COLLISION_DIM;
    rect result = rectCD( state->position, dim );
    return result;
}

internal void
drawTurtle( RENDER_PASS * pass, APP_STATE * appState, TURTLE_STATE * state, boo32 showDebug = false ) {
    if( state->isActive ) {
        vec2 scale  = Vec2( TURTLE_BITMAP_SCALE_X,  TURTLE_BITMAP_SCALE_Y  );
        vec2 offset = Vec2( TURTLE_BITMAP_OFFSET_X, TURTLE_BITMAP_OFFSET_Y );
        if( state->isFacingLeft ) {
            scale.x  = -scale.x;
            offset.x = -offset.x;
        }
        
        drawQuad( pass, textureID_turtle_idle, state->position + offset, scale, COLOR_WHITE );
        
        if( showDebug ) {
            rect coll = getTurtleCollision( state );
            drawRectOutline( pass, coll, COLOR_RED );
        }
    }
}

internal void
finalizeTurtle( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
    TURTLE_STATE * state = &appState->turtleState;
    
    if( ( state->isActive ) && ( !appState->gameOver ) ) {
        // TODO: Currently, turtle kills everything in its path, no matter what state its in and uses the same collision bound for everything. Should this be customized for each enemy?
        
        rect coll = getTurtleCollision( state );
        { // ARCHER COLLISION
            ARCHER_STATE * archerState = &appState->archerState;
            for( uint32 iFish = 0; iFish < ARCHER_MAX_COUNT; iFish++ ) {
                ARCHER * fish = archerState->archer + iFish;
                if( ( fish->isActive ) && ( isInBound( fish->position, coll ) ) ) {
                    fish->isActive = false;
                    archerState->nArcher--;
                    
                    // TODO: death sound
                    addPop( &appState->explosionList, fish->position, fish->position );
                    
                    uint32 index = rand() % 2;
                    AUDIO_ID audioID = ( AUDIO_ID )( audioID_pop01 + index );
                    playAudio( audioSystem, appState->audio, audioID, 0.85f );
                }
            }
        }
        { // LGARCHER COLLISION
            LGARCHER_STATE * lgArcherState = &appState->lgArcherState;
            for( uint32 iFish = 0; iFish < LGARCHER_MAX_COUNT; iFish++ ) {
                LGARCHER * fish = lgArcherState->lgArcher + iFish;
                if( ( fish->isActive ) && ( isInBound( fish->position, coll ) ) ) {
                    fish->isActive = false;
                    lgArcherState->nlgArcher--;
                    
                    // TODO: death sound
                    addPop( &appState->explosionList, fish->position, fish->position );
                    
                    uint32 index = rand() % 2;
                    AUDIO_ID audioID = ( AUDIO_ID )( audioID_pop01 + index );
                    playAudio( audioSystem, appState->audio, audioID, 0.85f );
                }
            }
        }
        { // BUBBLET COLLISION
            BUBBLET_STATE * bubbletState = &appState->bubbletState;
            for( uint32 iFish = 0; iFish < BUBBLET_MAX_COUNT; iFish++ ) {
                BUBBLET * fish = bubbletState->bubblet + iFish;
                if( ( fish->isActive ) && ( isInBound( fish->position, coll ) ) ) {
                    fish->isActive = false;
                    bubbletState->nBubblet--;
                    
                    addPop( &appState->explosionList, fish->position, fish->position );
                    
                    uint32 index = rand() % 2;
                    AUDIO_ID audioID = ( AUDIO_ID )( audioID_pop01 + index );
                    playAudio( audioSystem, appState->audio, audioID, 0.85f );
                }
            }
        }
        { // BUBBLE COLLISION
            BUBBLE_STATE * bubbleState = &appState->bubbleState;
            for( uint32 iFish = 0; iFish < BUBBLE_MAX_COUNT; iFish++ ) {
                BUBBLE * fish = bubbleState->bubble + iFish;
                if( ( fish->isActive ) && ( isInBound( fish->position, coll ) ) ) {
                    fish->isActive = false;
                    bubbleState->nBubble--;
                    
                    addPop( &appState->explosionList, fish->position, fish->position );
                    
                    uint32 index = rand() % 2;
                    AUDIO_ID audioID = ( AUDIO_ID )( audioID_pop01 + index );
                    playAudio( audioSystem, appState->audio, audioID, 0.85f );
                }
            }
        }
        { // BUBBLON COLLISION
            BUBBLON_STATE * bubblonState = &appState->bubblonState;
            for( uint32 iFish = 0; iFish < BUBBLON_MAX_COUNT; iFish++ ) {
                BUBBLON * fish = bubblonState->bubblon + iFish;
                if( ( fish->isActive ) && ( isInBound( fish->position, coll ) ) ) {
                    fish->isActive = false;
                    bubblonState->nBubblon--;
                    
                    addPop( &appState->explosionList, fish->position, fish->position );
                    
                    uint32 index = rand() % 2;
                    AUDIO_ID audioID = ( AUDIO_ID )( audioID_pop01 + index );
                    playAudio( audioSystem, appState->audio, audioID, 0.85f );
                }
            }
        }
        { // PARROTFISH COLLISION
            PARROTFISH_STATE * parrotfishState = &appState->parrotfishState;
            for( uint32 iFish = 0; iFish < PARROTFISH_MAX_COUNT; iFish++ ) {
                PARROTFISH * fish = parrotfishState->parrotfish + iFish;
                if( ( fish->isActive ) && ( isInBound( fish->position, coll ) ) ) {
                    fish->isActive = false;
                    parrotfishState->nParrotfish--;
                    
                    // TODO: death sound
                    addPop( &appState->explosionList, fish->position, fish->position );
                    
                    uint32 index = rand() % 2;
                    AUDIO_ID audioID = ( AUDIO_ID )( audioID_pop01 + index );
                    playAudio( audioSystem, appState->audio, audioID, 0.85f );
                }
            }
        }
        
        PLAYER_STATE * player = &appState->playerState;
        if( ( isInBound( player->position, coll ) ) && ( !player->isStunnedHi ) ) {
            rect bound = addRadius( coll, 20.0f );
            
            flo32 dist = fabsf( player->position.x - bound.left   );
            vec2  pos  = Vec2( bound.left, player->position.y );
            
            flo32 distA = fabsf( player->position.y - bound.bottom );
            if( distA < dist ) {
                dist = distA;
                pos  = Vec2( player->position.x, bound.bottom );
            }
            
            flo32 distB = fabsf( player->position.y - bound.top    );
            if( distB < dist ) {
                dist = distB;
                pos  = Vec2( player->position.x, bound.top );
            }
            
            player->initStunnedHi    = true;
            player->stun_position    = pos;
            player->stun_direction   = getNormal( pos - getCenter( bound ) );
            player->stun_damage      = TURTLE_DAMAGE;
            player->stun_particlePos = player->position;
        }
        
        if( state->timer >= state->targetTime ) {
            // TODO: spawn bound depends on play space, if area contains ground or not
            flo32 camera_height = getHeight( appState->cameraBound );
            flo32 atY = RandomF32InRange( 0.1f, 0.9f ) * camera_height;
            state->position.y = atY;
            
            state->isFacingLeft = !state->isFacingLeft;
            state->direction.x  = -state->direction.x;
            state->velocity     = state->direction * TURTLE_MOVE_SPEED;
            state->timer        = 0.0f;
            state->targetTime   = RandomF32InRange( state->targetTime_lo, state->targetTime_hi );
            if( state->isFacingLeft ) {
                state->position.x = appState->cameraBound.right + 1.0f * camera_height;
            } else {
                state->position.x = appState->cameraBound.left  - 1.0f * camera_height;
            }
            
            uint32 breakHere = 10;
        }
    }
}

internal void
spawnTurtle( APP_STATE * appState ) {
    TURTLE_STATE * state = &appState->turtleState;
    state->isActive  = true;
    state->state     = fishState_idle;
    state->position  = getBR( appState->cameraBound ) + Vec2( 1000.0f, 1000.0f );
    state->direction = Vec2( 1.0f, 0.0f );
    state->velocity  = state->direction * TURTLE_MOVE_SPEED;
    
    state->targetTime = RandomF32InRange( 30.0f, 40.0f );
}

internal void
spawnTurtle( APP_STATE * appState, GAME_CONTROL__TURTLE turtle, vec2 camera_base, flo32 camera_height ) {
    vec2 offset = Vec2( -1.0f, turtle.firstAppearAtY );
    vec2 dir    = Vec2(  1.0f, 0.0f );
    if( turtle.firstAppearOnRight ) {
        offset.x = -offset.x + 1.65f;
        dir.x    = -dir.x;
    }
    
    TURTLE_STATE * state = &appState->turtleState;
    state->isActive  = true;
    state->state     = fishState_idle;
    state->position  = camera_base + offset * camera_height;
    state->direction = dir;
    state->velocity  = state->direction * TURTLE_MOVE_SPEED;
    
    state->isFacingLeft = turtle.firstAppearOnRight;
    
    flo32 time = ( 3.65f * camera_height ) / TURTLE_MOVE_SPEED;
    state->timer = 0.0f;
    state->targetTime_lo = time + turtle.timeBetweenSpawn_lo;
    state->targetTime_hi = time + turtle.timeBetweenSpawn_hi;
    state->targetTime = RandomF32InRange( state->targetTime_lo, state->targetTime_hi );
    
    uint32 breakHere = 10;
}
