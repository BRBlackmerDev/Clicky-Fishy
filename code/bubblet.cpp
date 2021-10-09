
internal void
addBubblet( BUBBLET_STATE * state, BUBBLET bubblet ) {
    Assert( state->nBubblet < BUBBLET_MAX_COUNT );
    
    for( uint32 index = 0; index < BUBBLET_MAX_COUNT; index++ ) {
        BUBBLET fish = state->bubblet[ index ];
        if( !fish.isActive ) {
            state->bubblet[ index ] = bubblet;
            state->nBubblet++;
            break;
        }
    }
}

internal void
updateBubblet( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, flo32 dt ) {
    BUBBLET_STATE * state = &appState->bubbletState;
    
    for( uint32 iFish = 0; iFish < BUBBLET_MAX_COUNT; iFish++ ) {
        BUBBLET * fish = state->bubblet + iFish;
        if( fish->isActive ) {
            flo32 maxSpeed = BUBBLET_MOVE_SPEED;
            flo32 friction = BUBBLET_MOVE_FRICTION;
            
            vec2 dir = fish->direction;
            
            vec2 accel = ( dir * maxSpeed - fish->velocity ) * friction;
            fish->position += accel * ( dt * dt * 0.5f ) + fish->velocity * dt;
            fish->velocity += accel * dt;
        }
    }
}

internal void
drawBubblet( RENDER_PASS * pass, APP_STATE * appState, BUBBLET * fish, boo32 showDebug = false ) {
    vec2 scale  = Vec2( BUBBLET_BITMAP_SCALE_X,  BUBBLET_BITMAP_SCALE_Y  );
    vec2 offset = Vec2( BUBBLET_BITMAP_OFFSET_X, BUBBLET_BITMAP_OFFSET_Y );
    
    drawQuad( pass, textureID_bubblet_idle, fish->position + offset, scale, COLOR_WHITE );
}

internal void
drawBubblet( RENDER_PASS * pass, APP_STATE * appState, boo32 showDebug = false ) {
    BUBBLET_STATE * state  = &appState->bubbletState;
    PLAYER_STATE  * player = &appState->playerState;
    
    for( uint32 iFish = 0; iFish < BUBBLET_MAX_COUNT; iFish++ ) {
        BUBBLET * fish = state->bubblet + iFish;
        if( fish->isActive ) {
            drawBubblet( pass, appState, fish, showDebug );
        }
    }
}

internal void
popBubblet( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, BUBBLET * bubblet ) {
    BUBBLET_STATE * state = &appState->bubbletState;
    
    bubblet->isActive = false;
    state->nBubblet--;
    
    addPop( &appState->explosionList, bubblet->position, bubblet->position );
    
    uint32 index = rand() % 2;
    AUDIO_ID audioID = ( AUDIO_ID )( audioID_pop01 + index );
    playAudio( audioSystem, appState->audio, audioID, 0.85f );
}

internal void
finalizeBubblet( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
    BUBBLET_STATE * state = &appState->bubbletState;
    
    // spawn urchin spine outside hit radius, so it doesn't intersect with parent urchin upon creation
    // can it just friendly fire enemies? maybe some, but it should probably be deflected
    
    if( !appState->gameOver ) {
        PLAYER_STATE * player = &appState->playerState;
        circ collision = getBubbletCollision( player );
        
        for( uint32 iFish = 0; iFish < BUBBLET_MAX_COUNT; iFish++ ) {
            BUBBLET * fish = state->bubblet + iFish;
            if( fish->isActive ) {
                rect bound = addRadius( appState->cameraBound, 100.0f );
                if( !isInBound( fish->position, bound ) ) {
                    fish->isActive = false;
                    state->nBubblet--;
                } else { // PLAYER COLLISION
                    if( fish->isActive ) { // TOWER COLLISION
                        TOWER_STATE * towerState = &appState->towerState;
                        for( uint32 iTower = 0; iTower < TOWER_MAX_COUNT; iTower++ ) {
                            TOWER * tower = towerState->tower + iTower;
                            if( ( tower->isActive ) && ( tower->collision_isActive ) && ( isInBound( fish->position, tower->collision_bound ) ) ) {
                                tower->health = maxValue( tower->health - 1, 0 );
                                popBubblet( audioSystem, appState, fish );
                            }
                        }
                    }
                    if( fish->isActive ) { // PARROTFISH COLLISION
                        PARROTFISH_STATE * parrotfishState = &appState->parrotfishState;
                        for( uint32 iParrotfish = 0; iParrotfish < PARROTFISH_MAX_COUNT; iParrotfish++ ) {
                            PARROTFISH * parrotfish = parrotfishState->parrotfish + iParrotfish;
                            if( parrotfish->isActive ) {
                                flo32 radius   = PARROTFISH_BUBBLET_COLLISION_RADIUS;
                                flo32 radiusSq = radius * radius;
                                flo32 distSq = getLengthSq( fish->position - parrotfish->position );
                                if( distSq <= radiusSq ) {
                                    popBubblet( audioSystem, appState, fish );
                                }
                            }
                        }
                    }
                    if( isInBound( fish->position, collision ) ) {
                        player->health           = maxValue( player->health - ( flo32 )BUBBLET_DAMAGE, 0.0f );
                        player->wasDamaged_timer = PLAYER_WAS_DAMAGED_TARGET_TIME;
                        popBubblet( audioSystem, appState, fish );
                    }
                }
            }
        }
    }
}

internal void
killBubblet( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, BUBBLET * fish, vec2 pop_position ) {
    Assert( fish->isActive );
    
    BUBBLET_STATE * state = &appState->bubbletState;
    state->nBubblet--;
    
    fish->isActive = false;
    
    addPop( &appState->explosionList, pop_position );
    
    uint32 index = rand() % 2;
    AUDIO_ID audioID = ( AUDIO_ID )( audioID_pop01 + index );
    playAudio( audioSystem, appState->audio, audioID, 0.85f );
}


internal void
spawnBubblet( BUBBLET_STATE * state, vec2 position, vec2 direction ) {
    BUBBLET fish = {};
    fish.isActive  = true;
    fish.position  = position;
    fish.direction = direction;
    fish.velocity  = direction * BUBBLET_MOVE_SPEED;
    
    addBubblet( state, fish );
}
