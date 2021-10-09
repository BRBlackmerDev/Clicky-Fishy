
//----------
// URCHIN
//----------

internal void
addUrchin( URCHIN_STATE * state, URCHIN urchin ) {
    Assert( state->nUrchin < URCHIN_MAX_COUNT );
    
    for( uint32 index = 0; index < URCHIN_MAX_COUNT; index++ ) {
        URCHIN fish = state->urchin[ index ];
        if( !fish.isActive ) {
            state->urchin[ index ] = urchin;
            state->nUrchin++;
            break;
        }
    }
}

internal void
shootUrchinSpine( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, vec2 position, vec2 direction ) {
    URCHIN_SPINE_STATE * spineState = &appState->urchinSpineState;
    spawnUrchinSpine( spineState, position, direction );
    
    uint32 index = RandomU32InRange( 0, 1 );
    AUDIO_ID audioID = ( AUDIO_ID )( audioID_swordfish_stab01 + index );
    playAudio( audioSystem, appState->audio, audioID, 0.15f );
}

internal void
updateUrchin( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, flo32 dt ) {
    URCHIN_STATE * state = &appState->urchinState;
    
    // NOTE: player cannot bite or tailwhip an urchin directly without taking damage
    // NOTE: maybe the player can tailwhip the urchin directly using a item?
    // NOTE: should the player be able to move the urchin a little bit with tailwhip?
    
    for( uint32 iFish = 0; iFish < URCHIN_MAX_COUNT; iFish++ ) {
        URCHIN * fish = state->urchin + iFish;
        if( fish->isActive ) {
            flo32 maxSpeed = URCHIN_MOVE_SPEED;
            flo32 friction = URCHIN_MOVE_FRICTION;
            
            fish->timer += dt;
            
            vec2 dir = {};
            switch( fish->state ) {
                case fishState_idle_gameOver: {
                    
                } break;
                
                case fishState_idle: {
                    PLAYER_STATE * player = &appState->playerState;
                    
                    if( fish->position.x < player->position.x ) {
                        dir.x =  1.0f;
                    } else {
                        dir.x = -1.0f;
                    }
                    
                    if( fish->timer >= URCHIN_FIRE_SPINE_TARGET_TIME ) {
                        fish->timer = 0.0f;
                        
                        vec2 direction = getNormal( player->position - fish->position );
                        shootUrchinSpine( audioSystem, appState, fish->position, direction );
                    }
                } break;
                
                case fishState_spawning: {
                    if( fish->timer < URCHIN_SPAWN_MOVE_TARGET_TIME ) {
                        dir.x = 1.0f;
                    }
                } break;
            }
            
            vec2 accel = ( dir * maxSpeed - fish->velocity ) * friction;
            fish->position += accel * ( dt * dt * 0.5f ) + fish->velocity * dt;
            fish->velocity += accel * dt;
        }
    }
}

internal void
drawUrchin( RENDER_PASS * pass, APP_STATE * appState, URCHIN * fish, boo32 showDebug = false ) {
    vec2 scale  = Vec2( URCHIN_BITMAP_SCALE_X,  URCHIN_BITMAP_SCALE_Y  );
    vec2 offset = Vec2( URCHIN_BITMAP_OFFSET_X, URCHIN_BITMAP_OFFSET_Y );
    
    vec2 pos = fish->position + offset;
    drawQuad( pass, textureID_urchin_idle, pos, scale, COLOR_WHITE );
    
    if( showDebug ) {
        drawCircleOutline( pass, fish->position, URCHIN_SPINE_COLLISION__URCHIN_RADIUS, COLOR_RED );
    }
}

internal void
drawUrchin( RENDER_PASS * pass, APP_STATE * appState, boo32 showDebug = false ) {
    URCHIN_STATE * state = &appState->urchinState;
    
    for( uint32 iFish = 0; iFish < URCHIN_MAX_COUNT; iFish++ ) {
        URCHIN * fish = state->urchin + iFish;
        if( fish->isActive ) {
            drawUrchin( pass, appState, fish, showDebug );
        }
    }
}

internal void
finalizeUrchin( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
    URCHIN_STATE * state = &appState->urchinState;
    
    for( uint32 iFish = 0; iFish < URCHIN_MAX_COUNT; iFish++ ) {
        URCHIN * fish = state->urchin + iFish;
        if( fish->isActive ) {
            switch( fish->state ) {
                case fishState_idle: {
                    
                } break;
                
                case fishState_spawning: {
                    if( fish->timer >= URCHIN_SPAWN_TARGET_TIME ) {
                        PLAYER_STATE * player = &appState->playerState;
                        vec2 dir = getNormal( player->position - fish->position );
                        
                        shootUrchinSpine( audioSystem, appState, fish->position, dir );
                        
                        fish->state = fishState_idle;
                        fish->timer = 0.0f;
                    }
                } break;
            }
            
            if( appState->gameOver ) {
                fish->state = fishState_idle_gameOver;
                fish->timer = 0.0f;
            }
        }
    }
}

internal void
spawnUrchin( APP_STATE * appState, uint32 nUrchin = 8 ) {
    URCHIN_STATE * state = &appState->urchinState;
    
    for( uint32 iFish = 0; iFish < nUrchin; iFish++ ) {
        URCHIN fish = {};
        fish.isActive = true;
        fish.state    = fishState_spawning;
        fish.health   = URCHIN_HEALTH;
        fish.position = getBL( appState->cameraBound ) + Vec2( -200.0f, 100.0f );
        
        addUrchin( state, fish );
    }
}
