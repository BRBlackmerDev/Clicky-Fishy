
internal void
addRainbowToDebug( DEBUG_STATE * debug, RAINBOW_STATE * state ) {
    for( uint32 iFish = 0; iFish < RAINBOW_MAX_COUNT; iFish++ ) {
        RAINBOW rainbow = state->rainbow[ iFish ];
        
        if( rainbow.isActive ) {
            DEBUG__LAST_BITE_FRAME__FISH fish = {};
            fish.type       = fishType_rainbow;
            fish.pos        = rainbow.position;
            fish.facingLeft = rainbow.facingDir.isFacingLeft;
            fish.colorID    = rainbow.colorID;
            addToDebug( debug->lastBite.frame + debug->lastBite.atIndex, fish );
        }
    }
}

internal void
updateRainbow( APP_STATE * appState, flo32 dt ) {
    PLAYER_STATE * playerState = &appState->playerState;
    RAINBOW_STATE * state = &appState->rainbowState;
    FISH_ATE_LIST * list  = &appState->fishAteList;
    
    flo32 maxSpeed = RAINBOW_MAX_SPEED;
    flo32 friction = RAINBOW_FRICTION;
    
    if( list->attemptBite ) {
        //addRainbowToDebug( &appState->debugState, state );
        
        flo32 radiusSq = RAINBOW_EAT_RADIUS * RAINBOW_EAT_RADIUS;
        for( uint32 iFish = 0; iFish < RAINBOW_MAX_COUNT; iFish++ ) {
            RAINBOW * fish = state->rainbow + iFish;
            if( fish->isActive ) {
                
                flo32 distSq = getLengthSq( fish->position - playerState->position );
                if( distSq <= radiusSq ) {
                    addRainbow( list, *fish );
                    fish->isActive = false;
                    state->nRainbow--;
                }
            }
        }
    }
    
    for( uint32 iFish = 0; iFish < RAINBOW_MAX_COUNT; iFish++ ) {
        RAINBOW * fish = state->rainbow + iFish;
        if( fish->isActive ) {
            
            fish->timer += dt;
            
            vec2 dir = {};
            switch( fish->state ) {
                case fishState_idle: {
                } break;
                
                case fishState_moving: {
                    dir = fish->direction;
                } break;
                
                case fishState_spawning: {
                    dir      = fish->direction;
                    maxSpeed = fish->spawnSpeed;
                } break;
                
                case fishState_exiting: {
                    dir = fish->direction;
                } break;
            }
            
            vec2 repel_dir = fish->repel;
            for( uint32 iter = iFish + 1; iter < RAINBOW_MAX_COUNT; iter++ ) {
                RAINBOW * fish0 = state->rainbow + iter;
                if( fish0->isActive ) {
                    vec2  V = getRepel( fish->position, fish0->position, RAINBOW_REPEL_MAX_STRENGTH, RAINBOW_REPEL_MAX_DISTANCE );
                    
                    repel_dir     -= V;
                    fish0->repel  += V;
                }
            }
            
            vec2 accel = ( dir * maxSpeed - fish->velocity + repel_dir ) * friction;
            fish->position += accel * ( dt * dt * 0.5f ) + fish->velocity * dt;
            fish->velocity += accel * dt;
            fish->repel = {};
        }
    }
    
    for( uint32 iFish = 0; iFish < RAINBOW_MAX_COUNT; iFish++ ) {
        RAINBOW * fish = state->rainbow + iFish;
        if( fish->isActive ) {
            updateFacingDir( &fish->facingDir, fish->position.x );
        }
    }
}

internal void
drawRainbow( RENDER_PASS * pass, APP_STATE * appState, RAINBOW * fish, boo32 showDebug = false ) {
    vec2 scale  = Vec2( RAINBOW_BITMAP_SCALE_X, RAINBOW_BITMAP_SCALE_Y );
    vec2 offset = Vec2( RAINBOW_BITMAP_OFFSET_X, RAINBOW_BITMAP_OFFSET_Y );
    
    boo32 facingLeft = fish->facingDir.isFacingLeft;
    if( facingLeft ) {
        scale.x  = -scale.x;
        offset.x = -offset.x;
    }
    
    vec4 color = {};
    switch( fish->colorID ) {
        case colorID_blue:   { color = Vec4( 0.1f, 0.1f, 0.8f, 1.0f ); } break;
        case colorID_green:  { color = Vec4( 0.0f, 0.6f, 0.0f, 1.0f ); } break;
        case colorID_orange: { color = Vec4( 0.9f, 0.4f, 0.0f, 1.0f ); } break;
        case colorID_purple: { color = Vec4( 0.6f, 0.0f, 0.8f, 1.0f ); } break;
        case colorID_red:    { color = Vec4( 0.8f, 0.0f, 0.0f, 1.0f ); } break;
        case colorID_yellow: { color = Vec4( 0.7f, 0.7f, 0.0f, 1.0f ); } break;
    }
    
    drawQuad( pass, textureID_fish,     fish->position + offset, scale, color );
    drawQuad( pass, textureID_fish_eye, fish->position + offset, scale, COLOR_WHITE );
    
    if( showDebug ) {
        drawCircleOutline( pass, fish->position, RAINBOW_EAT_RADIUS, COLOR_RED );
    }
}

internal void
drawRainbow( RENDER_PASS * pass, APP_STATE * appState, boo32 showDebug = false ) {
    RAINBOW_STATE * state = &appState->rainbowState;
    
    for( uint32 iFish = 0; iFish < RAINBOW_MAX_COUNT; iFish++ ) {
        RAINBOW * fish = state->rainbow + iFish;
        if( fish->isActive ) {
            drawRainbow( pass, appState, fish, showDebug );
        }
    }
}

internal void
finalizeRainbow( APP_STATE * appState ) {
    RAINBOW_STATE * state = &appState->rainbowState;
    
    for( uint32 iFish = 0; iFish < RAINBOW_MAX_COUNT; iFish++ ) {
        RAINBOW * fish = state->rainbow + iFish;
        if( fish->isActive ) {
            switch( fish->state ) {
                case fishState_idle: {
                    if( fish->timer >= fish->targetTime ) {
                        fish->timer = 0.0f;
                        fish->state = fishState_moving;
                        
                        rect bound = addDim( appState->cameraBound, Vec2( -20.0f, -20.0f ) );
                        flo32 radians   = RandomF32() * 2.0f * PI;
                        vec2  direction = Vec2( cosf( radians ), sinf( radians ) );
                        
                        flo32 distToBound = minDistToBound( bound, fish->position, &direction );
                        flo32 maxLength   = RandomF32InRange( RAINBOW_MIN_DIST_TO_TRAVEL, RAINBOW_MAX_DIST_TO_TRAVEL );
                        if( ( rand() % 100 ) == 0 ) {
                            maxLength *= 4.0f;
                        }
                        flo32 length      = minValue( distToBound, maxLength );
                        fish->direction   = direction;
                        fish->targetTime  = length / RAINBOW_MAX_SPEED;
                    }
                } break;
                
                case fishState_moving: {
                    if( fish->timer >= fish->targetTime ) {
                        fish->timer = 0.0f;
                        fish->state = fishState_idle;
                        fish->targetTime = RandomF32InRange( 2.0f, 4.0f );
                    }
                } break;
                
                case fishState_spawning: {
                    if( fish->timer >= fish->targetTime ) {
                        fish->timer = 0.0f;
                        fish->state = fishState_idle;
                        fish->targetTime = RandomF32InRange( 0.5f, 2.0f );
                    }
                } break;
                
                case fishState_exiting: {
                    rect exit_bound = addRadius( appState->cameraBound, SPAWN_AND_EXIT_MARGIN );
                    if( !isInBound( fish->position, exit_bound ) ) {
                        fish->isActive = false;
                        state->nRainbow--;
                    }
                } break;
            }
        }
    }
}

internal void
addRainbow( RAINBOW_STATE * state, RAINBOW rainbow ) {
    Assert( state->nRainbow < RAINBOW_MAX_COUNT );
    
    for( uint32 iFish = 0; iFish < RAINBOW_MAX_COUNT; iFish++ ) {
        RAINBOW fish = state->rainbow[ iFish ];
        if( !fish.isActive ) {
            state->rainbow[ iFish ] = rainbow;
            state->nRainbow++;
            break;
        }
    }
}

internal void
spawnRainbow( APP_STATE * appState, uint32 nRainbow ) {
    RAINBOW_STATE * state = &appState->rainbowState;
    
    for( uint32 iFish = 0; iFish < nRainbow; iFish++ ) {
        SPAWN spawn = RandomSpawn( appState->cameraBound, RAINBOW_FRICTION );
        
        RAINBOW fish = {};
        fish.isActive   = true;
        fish.state      = fishState_spawning;
        fish.position   = spawn.position;
        fish.direction  = spawn.direction;
        fish.velocity   = spawn.velocity;
        fish.spawnSpeed = spawn.speed;
        fish.targetTime = spawn.targetTime;
        fish.facingDir  = spawn.facingDir;
        fish.colorID    = ( COLOR_ID )( rand() % colorID_count );
        
        addRainbow( state, fish );
    }
}

internal void
exitRainbow( APP_STATE * appState ) {
    RAINBOW_STATE * state = &appState->rainbowState;
    
    rect outerBound = addRadius( appState->cameraBound, SPAWN_AND_EXIT_MARGIN );
    vec2 outerDim   = getDim( outerBound );
    
    for( uint32 iFish = 0; iFish < RAINBOW_MAX_COUNT; iFish++ ) {
        RAINBOW * fish = state->rainbow + iFish;
        if( fish->isActive ) {
            vec2 exit = {};
            uint32 exit_side = rand() % 3;
            switch( exit_side ) {
                case 0: {
                    exit.y = RandomF32() * outerDim.y;
                } break;
                
                case 1: {
                    exit.x = RandomF32() * outerDim.x;
                    exit.y = outerDim.y;
                } break;
                
                case 2: {
                    exit.x = outerDim.x;
                    exit.y = RandomF32() * outerDim.y;
                } break;
            }
            exit += getBL( outerBound );
            
            fish->state     = fishState_exiting;
            fish->direction = getNormal( exit - fish->position );
        }
    }
}