
internal void
addBubblon( BUBBLON_STATE * state, BUBBLON bubblon ) {
    if( state->nBubblon < BUBBLON_MAX_COUNT ) {
        for( uint32 index = 0; index < BUBBLON_MAX_COUNT; index++ ) {
            BUBBLON fish = state->bubblon[ index ];
            if( !fish.isActive ) {
                state->bubblon[ index ] = bubblon;
                state->nBubblon++;
                break;
            }
        }
    }
}

internal void
updateBubblon( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, flo32 dt ) {
    BUBBLON_STATE * state  = &appState->bubblonState;
    PLAYER_STATE  * player = &appState->playerState;
    FISH_ATE_LIST * list   = &appState->fishAteList;
    
    if( list->attemptBite ) {
        // TODO: tune collision for bite
        circ collision = getBubblonCollision( player );
        for( uint32 iFish = 0; iFish < BUBBLON_MAX_COUNT; iFish++ ) {
            BUBBLON * fish = state->bubblon + iFish;
            if( ( fish->isActive ) && ( fish->type == bubblonType_bite ) && ( isInBound( fish->position, collision ) ) ) {
                fish->isActive = false;
                state->nBubblon--;
            }
        }
    }
    
    if( player->defend_isActive ) {
        // TODO: tune collision for stun
        circ collision = getBubblonCollision( player );
        for( uint32 iFish = 0; iFish < BUBBLON_MAX_COUNT; iFish++ ) {
            BUBBLON * fish = state->bubblon + iFish;
            if( ( fish->isActive ) && ( fish->type == bubblonType_stun ) && ( isInBound( fish->position, collision ) ) ) {
                fish->isActive = false;
                state->nBubblon--;
            }
        }
    }
    
    for( uint32 iFish = 0; iFish < BUBBLON_MAX_COUNT; iFish++ ) {
        BUBBLON * fish = state->bubblon + iFish;
        if( fish->isActive ) {
            flo32 maxSpeed = BUBBLON_MOVE_SPEED;
            flo32 friction = BUBBLON_MOVE_FRICTION;
            
            vec2 dir = Vec2( 0.0f, -1.0f );
            
            vec2 accel = ( dir * maxSpeed - fish->velocity ) * friction;
            fish->position += accel * ( dt * dt * 0.5f ) + fish->velocity * dt;
            fish->velocity += accel * dt;
        }
    }
}

internal void
drawBubblon( RENDER_PASS * pass, APP_STATE * appState, BUBBLON * fish, boo32 showDebug = false ) {
    vec2 scale  = Vec2( BUBBLON_BITMAP_SCALE_X,  BUBBLON_BITMAP_SCALE_Y  );
    vec2 offset = Vec2( BUBBLON_BITMAP_OFFSET_X, BUBBLON_BITMAP_OFFSET_Y );
    
    vec4 color = COLOR_WHITE;
    if( fish->type == bubblonType_bite ) {
        color = BUBBLON_BITE_COLOR;
    }
    if( fish->type == bubblonType_stun ) {
        color = BUBBLON_STUN_COLOR;
    }
    
    drawQuad( pass, textureID_bubblet_idle, fish->position + offset, scale, color );
}

internal void
drawBubblon( RENDER_PASS * pass, APP_STATE * appState, boo32 showDebug = false ) {
    BUBBLON_STATE * state  = &appState->bubblonState;
    PLAYER_STATE  * player = &appState->playerState;
    
    //DISPLAY_VALUE( uint32, state->nBubblon );
    for( uint32 iFish = 0; iFish < BUBBLON_MAX_COUNT; iFish++ ) {
        BUBBLON * fish = state->bubblon + iFish;
        if( fish->isActive ) {
            drawBubblon( pass, appState, fish, showDebug );
        }
    }
}

internal void
finalizeBubblon( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
    BUBBLON_STATE * state = &appState->bubblonState;
    
    // TODO: think about game over state
    if( !appState->gameOver ) {
        PLAYER_STATE * player = &appState->playerState;
        circ collision = getBubblonCollision( player );
        
        for( uint32 iFish = 0; iFish < BUBBLON_MAX_COUNT; iFish++ ) {
            BUBBLON * fish = state->bubblon + iFish;
            if( fish->isActive ) {
                // TODO: collision against objects (ground is probably an object)
                
                // TODO: temp!
                if( fish->position.y <= ( appState->cameraBound.bottom - 100.0f ) ) {
                    fish->isActive = false;
                    state->nBubblon--;
                    // TODO: do I want to keep track of how many of each type are active?
                }
                
                if( ( fish->type == bubblonType_dodge ) && ( isInBound( fish->position, collision ) ) ) {
                    player->health = maxValue( player->health - ( flo32 )BUBBLON_DAMAGE, 0.0f );
                    
                    fish->isActive = false;
                    state->nBubblon--;
                    
                    addPop( &appState->explosionList, fish->position, player->position );
                    
                    uint32 index = rand() % 2;
                    AUDIO_ID audioID = ( AUDIO_ID )( audioID_pop01 + index );
                    playAudio( audioSystem, appState->audio, audioID, 0.85f );
                }
            }
        }
    }
}

internal void
spawnBubblon( BUBBLON_STATE * state, vec2 position, vec2 direction ) {
    BUBBLON fish = {};
    fish.isActive  = true;
    fish.position  = position;
    fish.velocity  = direction * BUBBLON_MOVE_SPEED;
    fish.type = ( BUBBLON_TYPE )( rand() % bubblonType_count );
    
    addBubblon( state, fish );
}

internal void
updateBubblonGen( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, flo32 dt ) {
    BUBBLON_GEN * gen = &appState->bubblonGen;
    if( gen->isActive ) {
        gen->timer += dt;
        if( gen->timer >= gen->targetTime ) {
            gen->timer      = 0.0f;
            gen->targetTime = RandomF32InRange( 0.2f, 0.6f );
            
            rect bound = addRadius( appState->cameraBound, 100.0f );
            flo32 x0 = RandomF32InRange( bound.left, bound.right );
            flo32 x1 = RandomF32InRange( bound.left, bound.right );
            
            vec2 pos  = Vec2( x0, bound.top );
            vec2 dest = Vec2( x1, bound.bottom );
            // TODO: pass in dest if we want to focus on a particular area (probably pass in pos also)
            vec2 dir  = getNormal( dest - pos );
            
            spawnBubblon( &appState->bubblonState, pos, dir );
        }
    }
}