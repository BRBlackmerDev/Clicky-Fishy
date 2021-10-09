
internal void
addGoldfish( GOLDFISH_STATE * state, GOLDFISH goldfish ) {
    Assert( state->nGoldfish < GOLDFISH_MAX_COUNT );
    
    for( uint32 index = 0; index < GOLDFISH_MAX_COUNT; index++ ) {
        GOLDFISH fish = state->goldfish[ index ];
        if( !fish.isActive ) {
            state->goldfish[ index ] = goldfish;
            state->nGoldfish++;
            break;
        }
    }
}

internal void
addGoldfishToDebug( DEBUG_STATE * debug, GOLDFISH_STATE * state ) {
    for( uint32 iFish = 0; iFish < GOLDFISH_MAX_COUNT; iFish++ ) {
        GOLDFISH goldfish = state->goldfish[ iFish ];
        if( goldfish.isActive ) {
            DEBUG__LAST_BITE_FRAME__FISH fish = {};
            fish.type       = fishType_goldfish;
            fish.pos        = goldfish.position;
            fish.facingLeft = goldfish.facingDir.isFacingLeft;
            addToDebug( debug->lastBite.frame + debug->lastBite.atIndex, fish );
        }
    }
}

internal void
updateGoldfish( APP_STATE * appState, flo32 dt ) {
    PLAYER_STATE * playerState = &appState->playerState;
    GOLDFISH_STATE * state = &appState->goldfishState;
    FISH_ATE_LIST  * list  = &appState->fishAteList;
    
    flo32 maxSpeed = GOLDFISH_MAX_SPEED;
    flo32 friction = GOLDFISH_FRICTION;
    
    if( list->attemptBite ) {
        //addGoldfishToDebug( &appState->debugState, state );
        
        flo32 radiusSq = GOLDFISH_EAT_RADIUS * GOLDFISH_EAT_RADIUS;
        for( uint32 iFish = 0; iFish < GOLDFISH_MAX_COUNT; iFish++ ) {
            GOLDFISH * fish = state->goldfish + iFish;
            if( fish->isActive ) {
                flo32 distSq = getLengthSq( fish->position - playerState->position );
                if( distSq <= radiusSq ) {
                    fish->isActive = false;
                    state->nGoldfish--;
                    addGoldfish( list, *fish );
                    
                    removeThisTargetAndSetIdle( &appState->dogfishState, fishType_goldfish, iFish );
                }
            }
        }
    }
    
    for( uint32 iFish = 0; iFish < GOLDFISH_MAX_COUNT; iFish++ ) {
        GOLDFISH * fish = state->goldfish + iFish;
        if( fish->isActive ) {
            if( ( appState->gameOver ) && ( fish->state == fishState_idle ) ) {
                fish->state = fishState_idle_gameOver;
            }
            
            fish->timer += dt;
            
            vec2 dir = {};
            switch( fish->state ) {
                case fishState_idle_gameOver: {
                    if( fish->timer >= fish->targetTime ) {
                        DOGFISH_STATE * dogfishState = &appState->dogfishState;
                        
                        flo32 radiusSq = GOLDFISH_MOVE_RADIUS * GOLDFISH_MOVE_RADIUS;
                        
                        vec2  dodgeDirection = {};
                        flo32 minDistSq      = 1000000000.0f;
                        for( uint32 iDogfish = 0; iDogfish < DOGFISH_MAX_COUNT; iDogfish++ ) {
                            DOGFISH dogfish = dogfishState->dogfish[ iDogfish ];
                            if( dogfish.isActive ) {
                                vec2  V      = fish->position - dogfish.position;
                                flo32 distSq = getLengthSq( V );
                                if( distSq < minDistSq ) {
                                    dodgeDirection = getNormal( V );
                                    minDistSq      = distSq;
                                }
                            }
                        }
                        
                        rect bound = addRadius( appState->cameraBound, -GOLDFISH_PLAY_BOUND_MARGIN );
                        vec2 direction = {};
                        
                        if( minDistSq <= radiusSq ) {
                            fish->timer = 0.0f;
                            fish->state = fishState_moving;
                            
                            flo32 distance = RandomF32InRange( GOLDFISH_MOVE_DISTANCE_LO, GOLDFISH_MOVE_DISTANCE_HI );
                            flo32 radians = atan2f( dodgeDirection.y, dodgeDirection.x );
                            
                            flo32 range  = PI / 4.0f;
                            flo32 offset = RandomF32InRange( -range, range );
                            radians += offset;
                            
                            direction = Vec2( cosf( radians ), sinf( radians ) );
                            vec2 destination = fish->position + direction * distance;
                            
                            if( !isInBound( destination, bound ) ) {
                                flo32 distToBound = getDistToBound( bound, fish->position, direction );
                                if( distToBound >= GOLDFISH_MOVE_MIN_DISTANCE ) {
                                    distance = RandomF32InRange( GOLDFISH_MOVE_MIN_DISTANCE, distToBound );
                                } else {
                                    direction = getPerp( direction );
                                    if( ( rand() % 2 ) == 0 ) {
                                        direction = -direction;
                                    }
                                    destination = fish->position + direction * distance;
                                    if( ( destination.x <= bound.left ) || ( destination.x >= bound.right ) ) {
                                        direction.x = -direction.x;
                                    }
                                    if( ( destination.y <= bound.bottom ) || ( destination.y >= bound.top ) ) {
                                        direction.y = -direction.y;
                                    }
                                }
                            }
                            
                            flo32 decelDist = GOLDFISH_MAX_SPEED / GOLDFISH_FRICTION;
                            flo32 length    = maxValue( distance - decelDist, 0.0f );
                            
                            fish->direction   = direction;
                            fish->targetTime  = length / GOLDFISH_MAX_SPEED;
                        }
                    }
                } break;
                
                case fishState_idle: {
                    fish->lifeTimer += dt;
                    fish->bob_timer += dt;
                    
                    flo32 t = 0.5f / fish->bob_revsPerSec;
                    flo32 acc = GOLDFISH_BOB_HEIGHT / ( 0.5f * t * t * maxSpeed );
                    
                    flo32 bob_radians = fish->bob_timer * 2.0f * PI * fish->bob_revsPerSec;
                    dir.y = sinf( bob_radians ) * acc;
                } break;
                
                case fishState_moving: {
                    fish->lifeTimer += dt;
                    
                    dir = fish->direction;
                    
                    if( fish->timer >= fish->targetTime ) {
                        fish->timer      = 0.0f;
                        fish->targetTime = GOLDFISH_MOVE_WARMUP_TARGET_TIME;
                        fish->state      = fishState_idle;
                        fish->bob_revsPerSec = RandomF32InRange( GOLDFISH_BOB_REVS_PER_SECOND_LO, GOLDFISH_BOB_REVS_PER_SECOND_HI );
                        
                        flo32 t = 1.0f / fish->bob_revsPerSec;
                        fish->bob_timer = t * 0.5f * ( dir.y * 0.5f + 0.5f ) + t * 0.75f;
                    }
                } break;
                
                case fishState_spawning: {
                    dir      = fish->direction;
                    maxSpeed = fish->spawnSpeed;
                    if( fish->timer >= fish->targetTime ) {
                        fish->timer = 0.0f;
                        fish->state = fishState_idle;
                        fish->targetTime = GOLDFISH_MOVE_WARMUP_TARGET_TIME;
                        fish->bob_revsPerSec = RandomF32InRange( GOLDFISH_BOB_REVS_PER_SECOND_LO, GOLDFISH_BOB_REVS_PER_SECOND_HI );
                        
                        flo32 t = 1.0f / fish->bob_revsPerSec;
                        fish->bob_timer = t * 0.5f * ( dir.y * 0.5f + 0.5f ) + t * 0.75f;
                    }
                } break;
            }
            
            vec2 accel = ( dir * maxSpeed - fish->velocity ) * friction;
            fish->position += accel * ( dt * dt * 0.5f ) + fish->velocity * dt;
            fish->velocity += accel * dt;
        }
    }
    
    for( uint32 iFish = 0; iFish < GOLDFISH_MAX_COUNT; iFish++ ) {
        GOLDFISH * fish = state->goldfish + iFish;
        if( fish->isActive ) {
            updateFacingDir( &fish->facingDir, fish->position.x );
        }
    }
}

internal void
drawGoldfish( RENDER_PASS * pass, APP_STATE * appState, GOLDFISH * fish, boo32 showDebug = false ) {
    vec2 scale  = Vec2( GOLDFISH_BITMAP_SCALE_X, GOLDFISH_BITMAP_SCALE_Y );
    vec2 offset = Vec2( GOLDFISH_BITMAP_OFFSET_X, GOLDFISH_BITMAP_OFFSET_Y );
    
    boo32 facingLeft = fish->facingDir.isFacingLeft;
    if( facingLeft ) {
        scale.x  = -scale.x;
        offset.x = -offset.x;
    }
    
    drawQuad( pass, textureID_goldfish, fish->position + offset, scale, COLOR_WHITE );
    
    if( showDebug ) {
        drawCircleOutline( pass, fish->position, GOLDFISH_EAT_RADIUS,  COLOR_RED  );
        drawCircleOutline( pass, fish->position, GOLDFISH_MOVE_RADIUS, COLOR_CYAN );
    }
}

internal void
drawGoldfish( RENDER_PASS * pass, APP_STATE * appState, boo32 showDebug = false ) {
    GOLDFISH_STATE * state = &appState->goldfishState;
    
    for( uint32 iFish = 0; iFish < GOLDFISH_MAX_COUNT; iFish++ ) {
        GOLDFISH * fish = state->goldfish + iFish;
        if( fish->isActive ) {
            drawGoldfish( pass, appState, fish, showDebug );
        }
    }
}

internal void
finalizeGoldfish( APP_STATE * appState ) {
    GOLDFISH_STATE * state = &appState->goldfishState;
    PLAYER_STATE * playerState = &appState->playerState;
    
    for( uint32 iFish = 0; iFish < GOLDFISH_MAX_COUNT; iFish++ ) {
        GOLDFISH * fish = state->goldfish + iFish;
        if( fish->isActive ) {
            switch( fish->state ) {
                case fishState_idle: {
                    if( fish->timer >= fish->targetTime ) {
                        DOGFISH_STATE * dogfishState = &appState->dogfishState;
                        
                        flo32 radiusSq = GOLDFISH_MOVE_RADIUS * GOLDFISH_MOVE_RADIUS;
                        
                        boo32 dodgeDogfish   = false;
                        vec2  dodgeDirection = fish->position - playerState->position;
                        flo32 minDistSq = getLengthSq( fish->position - playerState->position );
                        for( uint32 iDogfish = 0; iDogfish < DOGFISH_MAX_COUNT; iDogfish++ ) {
                            DOGFISH dogfish = dogfishState->dogfish[ iDogfish ];
                            if( dogfish.isActive ) {
                                vec2  V      = fish->position - dogfish.position;
                                flo32 distSq = getLengthSq( V );
                                if( distSq < minDistSq ) {
                                    dodgeDirection = dogfish.direction;
                                    minDistSq      = distSq;
                                    dodgeDogfish = true;
                                }
                            }
                        }
                        
                        rect bound = addRadius( appState->cameraBound, -GOLDFISH_PLAY_BOUND_MARGIN );
                        vec2 direction = {};
                        
                        if( minDistSq <= radiusSq ) {
                            fish->timer = 0.0f;
                            fish->state = fishState_moving;
                            
                            flo32 distance = RandomF32InRange( GOLDFISH_MOVE_DISTANCE_LO, GOLDFISH_MOVE_DISTANCE_HI );
                            flo32 radians = atan2f( dodgeDirection.y, dodgeDirection.x );
                            
                            if( dodgeDogfish ) {
                                flo32 range  = PI / 6.0f;
                                flo32 offset = RandomF32InRange( -range, range );
                                
                                radians = ( ( rand() % 2 ) == 0 ) ? radians + ( PI * 0.5f ) : radians - ( PI * 0.5f );
                                radians += offset;
                                
                                direction = Vec2( cosf( radians ), sinf( radians ) );
                                vec2 destination = fish->position + direction * distance;
                                if( ( destination.x <= bound.left ) || ( destination.x >= bound.right ) ) {
                                    direction.x = -direction.x;
                                }
                                if( ( destination.y <= bound.bottom ) || ( destination.y >= bound.top ) ) {
                                    direction.y = -direction.y;
                                }
                            } else {
                                flo32 range  = PI / 4.0f;
                                flo32 offset = RandomF32InRange( -range, range );
                                radians += offset;
                                
                                direction = Vec2( cosf( radians ), sinf( radians ) );
                                vec2 destination = fish->position + direction * distance;
                                
                                if( !isInBound( destination, bound ) ) {
                                    flo32 distToBound = getDistToBound( bound, fish->position, direction );
                                    if( distToBound >= GOLDFISH_MOVE_MIN_DISTANCE ) {
                                        distance = RandomF32InRange( GOLDFISH_MOVE_MIN_DISTANCE, distToBound );
                                    } else {
                                        direction = getPerp( direction );
                                        if( ( rand() % 2 ) == 0 ) {
                                            direction = -direction;
                                        }
                                        destination = fish->position + direction * distance;
                                        if( ( destination.x <= bound.left ) || ( destination.x >= bound.right ) ) {
                                            direction.x = -direction.x;
                                        }
                                        if( ( destination.y <= bound.bottom ) || ( destination.y >= bound.top ) ) {
                                            direction.y = -direction.y;
                                        }
                                    }
                                }
                            }
                            
                            flo32 decelDist = GOLDFISH_MAX_SPEED / GOLDFISH_FRICTION;
                            flo32 length    = maxValue( distance - decelDist, 0.0f );
                            
                            fish->direction   = direction;
                            fish->targetTime  = length / GOLDFISH_MAX_SPEED;
                        }
                    }
                } break;
                
                case fishState_moving:
                case fishState_spawning: {
                    if( fish->timer >= fish->targetTime ) {
                        fish->timer      = 0.0f;
                        fish->targetTime = GOLDFISH_MOVE_WARMUP_TARGET_TIME;
                        fish->state      = fishState_idle;
                        if( appState->gameOver ) {
                            fish->state = fishState_idle_gameOver;
                        }
                        fish->bob_revsPerSec = RandomF32InRange( GOLDFISH_BOB_REVS_PER_SECOND_LO, GOLDFISH_BOB_REVS_PER_SECOND_HI );
                        
                        flo32 t = 1.0f / fish->bob_revsPerSec;
                        fish->bob_timer = t * 0.5f * ( fish->direction.y * 0.5f + 0.5f ) + t * 0.75f;
                    }
                } break;
            }
        }
    }
}

internal void
spawnGoldfish( APP_STATE * appState, uint32 nGoldfish = 8 ) {
    GOLDFISH_STATE * state = &appState->goldfishState;
    
    for( uint32 iFish = 0; iFish < nGoldfish; iFish++ ) {
        SPAWN spawn = RandomSpawn( appState->cameraBound, GOLDFISH_FRICTION );
        
        GOLDFISH fish = {};
        fish.isActive   = true;
        fish.state      = fishState_spawning;
        fish.position   = spawn.position;
        fish.direction  = spawn.direction;
        fish.velocity   = spawn.velocity;
        fish.spawnSpeed = spawn.speed;
        fish.targetTime = spawn.targetTime;
        fish.facingDir  = spawn.facingDir;
        
        addGoldfish( state, fish );
    }
}