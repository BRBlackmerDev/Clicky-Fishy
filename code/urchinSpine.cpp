
internal void
addUrchinSpine( URCHIN_SPINE_STATE * state, URCHIN_SPINE urchinSpine ) {
    Assert( state->nUrchinSpine < URCHIN_SPINE_MAX_COUNT );
    
    for( uint32 index = 0; index < URCHIN_SPINE_MAX_COUNT; index++ ) {
        URCHIN_SPINE fish = state->urchinSpine[ index ];
        if( !fish.isActive ) {
            state->urchinSpine[ index ] = urchinSpine;
            state->nUrchinSpine++;
            break;
        }
    }
}

internal void
deflectUrchinSpine( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
    URCHIN_SPINE_STATE * state  = &appState->urchinSpineState;
    PLAYER_STATE       * player = &appState->playerState;
    
    if( player->defend_isActive ) {
        for( uint32 iFish = 0; iFish < URCHIN_SPINE_MAX_COUNT; iFish++ ) {
            URCHIN_SPINE * fish = state->urchinSpine + iFish;
            if( fish->isActive ) {
                switch( fish->state ) {
                    case fishState_idle: {
                        vec2 V = fish->position - player->position;
                        // TODO: spine collision should probably be a rectangle, not a circle
                        flo32 radiusSq = URCHIN_SPINE_DEFLECT_RADIUS * URCHIN_SPINE_DEFLECT_RADIUS;
                        flo32 distSq = getLengthSq( V );
                        if( distSq <= radiusSq ) {
                            // TODO: deflect audio and particles
                            vec2 deflect = getNormal( V );
                            fish->direction = deflect;
                            fish->velocity  = deflect * URCHIN_SPINE_MOVE_SPEED;
                            fish->isDeflected = true;
                            
                            uint32 index = RandomU32InRange( 0, 2 );
                            AUDIO_ID audioID = ( AUDIO_ID )( audioID_swordfish_block01 + index );
                            playAudio( audioSystem, appState->audio, audioID, 0.2f );
                            
                            addSmExplosion( &appState->explosionList, player->position, fish->position );
                        }
                    } break;
                }
            }
        }
    }
}

internal void
updateUrchinSpine( APP_STATE * appState, flo32 dt ) {
    URCHIN_SPINE_STATE * state = &appState->urchinSpineState;
    PLAYER_STATE * player = &appState->playerState;
    
    for( uint32 iFish = 0; iFish < URCHIN_SPINE_MAX_COUNT; iFish++ ) {
        URCHIN_SPINE * fish = state->urchinSpine + iFish;
        if( fish->isActive ) {
            flo32 maxSpeed = URCHIN_SPINE_MOVE_SPEED;
            flo32 friction = URCHIN_SPINE_MOVE_FRICTION;
            
            fish->timer += dt;
            
            vec2 dir = {};
            switch( fish->state ) {
                case fishState_idle: {
                    dir = fish->direction;
                } break;
            }
            
            vec2 accel = ( dir * maxSpeed - fish->velocity ) * friction;
            fish->position += accel * ( dt * dt * 0.5f ) + fish->velocity * dt;
            fish->velocity += accel * dt;
            
            fish->deflectDir = getNormal( fish->position - player->position );
        }
    }
}

internal void
drawUrchinSpine( RENDER_PASS * pass, APP_STATE * appState, URCHIN_SPINE * fish, boo32 showDebug = false ) {
    {
        vec2 scale  = Vec2( URCHIN_SPINE_BITMAP_SCALE_X,  URCHIN_SPINE_BITMAP_SCALE_Y  );
        vec2 offset = Vec2( URCHIN_SPINE_BITMAP_OFFSET_X, URCHIN_SPINE_BITMAP_OFFSET_Y );
        
        vec2 xAxis = fish->direction;
        vec2 yAxis = getPerp( xAxis );
        
        xAxis *= scale.x;
        yAxis *= scale.y;
        
        vec2 pos = fish->position + offset;
        drawQuad( pass, textureID_urchinSpine_idle, pos, xAxis, yAxis, COLOR_WHITE );
        
        if( showDebug ) {
            drawCircleOutline( pass, fish->position, URCHIN_SPINE_DEFLECT_RADIUS, COLOR_RED );
        }
    }
    
    {
        vec2 deflect = fish->deflectDir;
        
        orect bound = {};
        bound.halfDim = Vec2( 100.0f, 2.0f );
        bound.center  = fish->position + deflect * 200.0f;
        bound.xAxis   = deflect;
        bound.yAxis   = getPerp( deflect );
        
        drawORect( pass, bound, COLOR_RED );
    }
    
}

internal void
drawUrchinSpine( RENDER_PASS * pass, APP_STATE * appState, boo32 showDebug = false ) {
    URCHIN_SPINE_STATE * state  = &appState->urchinSpineState;
    
    for( uint32 iFish = 0; iFish < URCHIN_SPINE_MAX_COUNT; iFish++ ) {
        URCHIN_SPINE * fish = state->urchinSpine + iFish;
        if( fish->isActive ) {
            drawUrchinSpine( pass, appState, fish, showDebug );
        }
    }
}

internal void
finalizeUrchinSpine( APP_STATE * appState ) {
    URCHIN_SPINE_STATE * state = &appState->urchinSpineState;
    
    // spawn urchin spine outside hit radius, so it doesn't intersect with parent urchin upon creation
    // can it just friendly fire enemies? maybe some, but it should probably be deflected
    
    if( !appState->gameOver ) {
        for( uint32 iFish = 0; iFish < URCHIN_SPINE_MAX_COUNT; iFish++ ) {
            URCHIN_SPINE * fish = state->urchinSpine + iFish;
            if( fish->isActive ) {
                switch( fish->state ) {
                    case fishState_idle: {
                        rect bound = addRadius( appState->cameraBound, 100.0f );
                        if( !isInBound( fish->position, bound ) ) {
                            fish->isActive = false;
                            state->nUrchinSpine--;
                        } else if( fish->isDeflected ) {
                            { // URCHIN COLLISION
                                flo32 radiusSq = URCHIN_SPINE_COLLISION__URCHIN_RADIUS * URCHIN_SPINE_COLLISION__URCHIN_RADIUS;
                                URCHIN_STATE * urchinState = &appState->urchinState;
                                for( uint32 iter = 0; ( fish->isActive ) && ( iter < URCHIN_MAX_COUNT ); iter++ ) {
                                    URCHIN * urchin = urchinState->urchin + iter;
                                    if( urchin->isActive ) {
                                        flo32 distSq = getLengthSq( urchin->position - fish->position );
                                        if( distSq <= radiusSq ) {
                                            fish->isActive = false;
                                            state->nUrchinSpine--;
                                            
                                            urchin->health = maxValue( urchin->health - 1, 0 );
                                            if( urchin->health == 0 ) {
                                                // TODO: urchin death
                                                urchin->isActive = false;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        
                        if( fish->isActive ) { // PLAYER COLLISION
                            PLAYER_STATE * player = &appState->playerState;
                            
                            flo32 radiusSq = URCHIN_SPINE_COLLISION__PLAYER_RADIUS * URCHIN_SPINE_COLLISION__PLAYER_RADIUS;
                            flo32 distSq = getLengthSq( player->position - fish->position );
                            if( distSq <= radiusSq ) {
                                player->health = maxValue( player->health - ( flo32 )URCHIN_SPINE_DAMAGE, 0.0f );
                                
                                fish->isActive = false;
                                state->nUrchinSpine--;
                            }
                        }
                    } break;
                }
            }
        }
        
        //DISPLAY_VALUE( uint32, state->nUrchinSpine );
    }
}

internal void
spawnUrchinSpine( URCHIN_SPINE_STATE * state, vec2 position, vec2 direction ) {
    URCHIN_SPINE fish = {};
    fish.isActive  = true;
    fish.state     = fishState_idle;
    fish.position  = position;
    fish.direction = direction;
    fish.velocity  = direction * URCHIN_SPINE_MOVE_SPEED;
    
    addUrchinSpine( state, fish );
}
