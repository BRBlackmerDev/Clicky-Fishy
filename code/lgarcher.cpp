
//----------
// LG ARCHER FISH
//----------

internal void
addlgArcher( LGARCHER_STATE * state, LGARCHER lgArcher ) {
    Assert( state->nlgArcher < LGARCHER_MAX_COUNT );
    
    for( uint32 index = 0; index < LGARCHER_MAX_COUNT; index++ ) {
        LGARCHER fish = state->lgArcher[ index ];
        if( !fish.isActive ) {
            state->lgArcher[ index ] = lgArcher;
            state->nlgArcher++;
            break;
        }
    }
}

internal void
shootBubble( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, vec2 position, vec2 direction ) {
    BUBBLE_STATE * bubbleState = &appState->bubbleState;
    spawnBubble( bubbleState, position, direction );
    
    uint32 index = RandomU32InRange( 0, 1 );
    AUDIO_ID audioID = ( AUDIO_ID )( audioID_bubble_spit01 + index );
    playAudio( audioSystem, appState->audio, audioID, 0.5f );
}

internal circ
getlgArcherBiteCollision( LGARCHER * fish ) {
    vec2 offset = PLAYER__LGARCHER_BITE_COLLISION_OFFSET;
    
    if( fish->facingDir.isFacingLeft ) {
        offset.x = -offset.x;
    }
    
    circ result = {};
    result.center = fish->position + offset;
    result.radius = PLAYER__LGARCHER_BITE_COLLISION_RADIUS;
    return result;
}

internal void
findClosestEnemyPos( APP_STATE * appState, LGARCHER * lgArcher ) {
    PLAYER_STATE * player = &appState->playerState;
    
    vec2  closestPos = player->position;
    flo32 minDistSq  = getLengthSq( lgArcher->position - closestPos );
    
    SMTURTLE_STATE * smTurtleState = &appState->smTurtleState;
    for( uint32 iFish = 0; iFish < SMTURTLE_MAX_COUNT; iFish++ ) {
        SMTURTLE * fish = smTurtleState->smTurtle + iFish;
        if( fish->isActive ) {
            rect bound = appState->cameraBound;
            // TODO: maybe add a margin
            
            boo32 canTarget = ( fish->state != fishState_stunned ) && ( isInBound( fish->position, bound ) );
            if( canTarget ) {
                flo32 distSq = getLengthSq( lgArcher->position - fish->position );
                if( distSq < minDistSq ) {
                    closestPos = fish->position;
                    minDistSq  = distSq;
                }
            }
        }
    }
    
    lgArcher->targetPos = closestPos;
}

internal void
updatelgArcher( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, flo32 dt ) {
    PLAYER_STATE   * player = &appState->playerState;
    LGARCHER_STATE * state  = &appState->lgArcherState;
    FISH_ATE_LIST  * list   = &appState->fishAteList;
    
    // NOTE: can player move/stun lgArcher with a tailwhip? there's really no reason to
    if( list->attemptBite ) {
        for( uint32 iFish = 0; iFish < LGARCHER_MAX_COUNT; iFish++ ) {
            LGARCHER * fish = state->lgArcher + iFish;
            if( fish->isActive ) {
                circ bite = getlgArcherBiteCollision( fish );
                if( isInBound( player->position, bite ) ) {
                    fish->health = maxValue( fish->health - 1, 0 );
                    addBit( list, *fish );
                    
                    if( fish->health <= 0 ) {
                        fish->isActive = false;
                        state->nlgArcher--;
                        
                        addAte( list, *fish );
                    }
                }
            }
        }
    }
    
    for( uint32 iFish = 0; iFish < LGARCHER_MAX_COUNT; iFish++ ) {
        LGARCHER * fish = state->lgArcher + iFish;
        if( fish->isActive ) {
            PARROTFISH_STATE * parrotfishState = &appState->parrotfishState;
            for( uint32 iParrotfish = 0; iParrotfish < PARROTFISH_MAX_COUNT; iParrotfish++ ) {
                PARROTFISH * parrotfish = parrotfishState->parrotfish + iParrotfish;
                if( ( parrotfish->isActive ) && ( parrotfish->state == fishState_stunned ) && ( parrotfish->timer < PARROTFISH_CAN_STUN_OTHER_FISH_TARGET_TIME ) ) {
                    vec2 V = fish->position - parrotfish->position;
                    
                    flo32 radius   = PARROTFISH_LGARCHER_STUN_RADIUS;
                    flo32 radiusSq = radius * radius;
                    flo32 distSq   = getLengthSq( V );
                    if( distSq <= radiusSq ) {
                        fish->state = fishState_stunned;
                        fish->timer = 0.0f;
                        
                        vec2 dir = getNormal( V );
                        fish->velocity = dir * LGARCHER_STUNNED_SPEED;
                        
                        vec2 particlePos = lerp( parrotfish->position, 1.4f, fish->position );
                        addSmExplosion( &appState->explosionList, particlePos );
                        
                        appState->audioManager.nSlap++;
                    }
                }
            }
            
            flo32 maxSpeed = LGARCHER_MOVE_SPEED;
            flo32 friction = LGARCHER_MOVE_FRICTION;
            
            fish->timer += dt;
            
            vec2 dir = {};
            switch( fish->state ) {
                case fishState_idle_gameOver: {
                    
                } break;
                
                case fishState_idle:
                case fishState_shooting: {
                    findClosestEnemyPos( appState, fish );
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
                    maxSpeed = LGARCHER_EXIT_SPEED;
                } break;
                
                case fishState_dying:
                case fishState_stunned: {
                    maxSpeed = LGARCHER_STUNNED_SPEED;
                    friction = LGARCHER_STUNNED_FRICTION;
                } break;
            }
            
            vec2 accel = ( dir * maxSpeed - fish->velocity ) * friction;
            fish->position += accel * ( dt * dt * 0.5f ) + fish->velocity * dt;
            fish->velocity += accel * dt;
            
            if( appState->camera_isMoving ) {
                fish->position += appState->camera_dPos;
            }
            
            if( fish->state != fishState_stunned ) {
                if( ( fish->state == fishState_idle ) || ( fish->state == fishState_shooting ) ) {
                    fish->facingDir.isFacingLeft = ( fish->targetPos.x < fish->position.x );
                    fish->facingDir.transitionX  = fish->position.x;
                    
                    flo32 range = ( PI / 12.0f );
                    flo32 rangeA0 = range;
                    flo32 rangeA1 = TAU - range;
                    flo32 rangeB0 = PI - range;
                    flo32 rangeB1 = PI + range;
                    
                    flo32 radians = toRadians( fish->targetPos - fish->position );
                    if( fish->facingDir.isFacingLeft ) {
                        radians = clamp( radians, rangeB0, rangeB1 );
                    } else {
                        radians = clout( radians, rangeA0, rangeA1 );
                    }
                    fish->radians = radians;
                } else {
                    updateFacingDir( &fish->facingDir, fish->position.x );
                    fish->radians = 0.0f;
                }
            }
            
            DISPLAY_VALUE( boo32, appState->entityUpdate_isEnabled );
            DISPLAY_VALUE( vec2, fish->position );
        }
    }
}

internal void
drawlgArcher( RENDER_PASS * pass, APP_STATE * appState, LGARCHER * fish, boo32 showDebug = false ) {
    vec2 scale  = Vec2( LGARCHER_BITMAP_SCALE_X,  LGARCHER_BITMAP_SCALE_Y  );
    vec2 offset = Vec2( LGARCHER_BITMAP_OFFSET_X, LGARCHER_BITMAP_OFFSET_Y );
    
    boo32 facingLeft = fish->facingDir.isFacingLeft;
    
    TEXTURE_ID textureID = textureID_archer_idle;
    
    vec4 modColor = Vec4( 1.0f, 0.65f, 0.65f, 1.0f );
    
    flo32 radians = fish->radians;
    switch( fish->state ) {
        case fishState_shooting: {
            flo32 t   = clamp01( fish->timer / fish->targetTime );
            t = sqrtf( t );
            scale.x  *= lerp( 1.0f, t, 0.8f );
            scale.y  *= lerp( 1.0f, t, 1.5f );
            offset.x *= lerp( 1.0f, t, 1.3f );
        } // fall through
        
        case fishState_idle: {
            if( facingLeft ) {
                offset.y = -offset.y;
                scale.y  = -scale.y;
            }
        } break;
        
        case fishState_stunned: {
            radians = 0.0f;
            //textureID = textureID_archer_stunned;
            if( facingLeft ) {
                offset.x = -offset.x;
                scale.x  = -scale.x;
            }
            
            modColor = Vec4( 0.65f, 1.0f, 0.65f, 1.0f );
        } break;
        
        case fishState_dying: {
            if( fish->timer >= LGARCHER_DYING_FADE_DELAY_TARGET_TIME ) {
                flo32 numer = LGARCHER_DYING_FADE_DELAY_TARGET_TIME - fish->timer;
                flo32 denom = LGARCHER_DYING_FADE_DELAY_TARGET_TIME - LGARCHER_DYING_TARGET_TIME;
                flo32 alpha = 1.0f - clamp01( numer / denom );
                modColor *= alpha;
            }
        } break;
        
        default: {
            if( facingLeft ) {
                offset.x = -offset.x;
                scale.x  = -scale.x;
            }
        } break;
    }
    
    vec2 xAxis = toDirection2D( radians );
    vec2 yAxis = getPerp( xAxis );
    xAxis *= scale.x;
    yAxis *= scale.y;
    
    modColor *= COLOR_GRAY( 0.65f );
    
    vec2 pos = fish->position + offset;
    drawQuad( pass, textureID, pos, xAxis, yAxis, modColor );
    
    vec2 health_dim  = Vec2( 34.0f, 4.0f );
    rect health_back = rectTCD( fish->position + Vec2( 0.0f, -10.0f ), health_dim );
    drawHealthBar( pass, health_back, fish->health, LGARCHER_HEALTH );
    
    if( showDebug ) {
        circ c = getlgArcherBiteCollision( fish );
        drawCircleOutline( pass, c.center, c.radius, COLOR_RED );
        
        if( ( fish->state == fishState_idle ) || ( fish->state == fishState_shooting ) ) {
            drawLine( pass, fish->position, fish->targetPos, COLOR_GREEN );
        }
    }
    if( fish->useSmBoundBehavior ) {
        drawRectOutline( pass, appState->movementBound, COLOR_GREEN );
    }
}

internal void
drawlgArcher( RENDER_PASS * pass, APP_STATE * appState, boo32 showDebug = false ) {
    LGARCHER_STATE * state = &appState->lgArcherState;
    
    for( uint32 iFish = 0; iFish < LGARCHER_MAX_COUNT; iFish++ ) {
        LGARCHER * fish = state->lgArcher + iFish;
        if( fish->isActive ) {
            drawlgArcher( pass, appState, fish, showDebug );
        }
    }
}

internal void
finalizelgArcher( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
    LGARCHER_STATE * state  = &appState->lgArcherState;
    PLAYER_STATE   * player = &appState->playerState;
    
    for( uint32 iFish = 0; iFish < LGARCHER_MAX_COUNT; iFish++ ) {
        LGARCHER * fish = state->lgArcher + iFish;
        if( fish->isActive ) {
            switch( fish->state ) {
                case fishState_idle: {
                    if( fish->timer >= fish->targetTime ) {
                        fish->timer = 0.0f;
                        
                        DISPLAY_VALUE( boo32, fish->useSmBoundBehavior );
                        DISPLAY_VALUE( rect, appState->movementBound );
                        
                        if( fish->useSmBoundBehavior ) {
                            rect bound = appState->movementBound;
                            if( !isInBound( fish->position, bound ) ) {
                                vec2 dest = getRandomPoint( bound );
                                vec2 V = dest - fish->position;
                                fish->direction  = getNormal( V );
                                fish->targetTime = getLength( V ) / LGARCHER_MOVE_SPEED;
                                
                                fish->state = fishState_moving;
                            } else {
                                boo32 shootBubble = ( ( rand() % 3 ) != 0 );
                                
                                if( shootBubble ) {
                                    fish->state      = fishState_shooting;
                                    fish->targetTime = 2.8f;
                                } else {
                                    vec2 dest = getRandomPoint( bound );
                                    vec2 V = dest - fish->position;
                                    fish->direction  = getNormal( V );
                                    fish->targetTime = getLength( V ) / LGARCHER_MOVE_SPEED;
                                    
                                    fish->state = fishState_moving;
                                }
                            }
                        } else {
                            rect bound = addDim( appState->cameraBound, Vec2( -20.0f, -20.0f ) );
                            if( !isInBound( fish->position, bound ) ) {
                                vec2 P   = clampToBound( fish->position, bound );
                                vec2 dim = getDim( bound ) * 0.6667f;
                                
                                rect destBound = getIntersect( rectCD( P, dim ), bound );
                                vec2 dest = getRandomPoint( destBound );
                                
                                vec2 V = dest - fish->position;
                                fish->direction  = getNormal( V );
                                fish->targetTime = getLength( V ) / LGARCHER_MOVE_SPEED;
                                
                                fish->state = fishState_moving;
                            } else {
                                flo32 radiusSq = squareValue( LGARCHER_MOVEMENT_DECISION_RADIUS );
                                flo32 distSq   = getLengthSq( player->position - fish->position );
                                
                                boo32 isInRange   = ( distSq <= radiusSq );
                                boo32 shootBubble = false;
                                if( isInRange ) {
                                    shootBubble = ( ( rand() % 3 ) != 0 );
                                } else {
                                    shootBubble = ( ( rand() % 3 ) == 0 );
                                }
                                
                                if( shootBubble ) {
                                    fish->state      = fishState_shooting;
                                    fish->targetTime = 2.8f;
                                } else {
                                    fish->state = fishState_moving;
                                    
                                    if( isInRange ) {
                                        vec2  dir    = RandomDirection2D();
                                        flo32 length = RandomF32InRangeQuadratic( LGARCHER_MAX_DIST_TO_TRAVEL, LGARCHER_MIN_DIST_TO_TRAVEL );
                                        
                                        fish->direction  = adjustDirToBound( fish->position, dir, length, bound );
                                        fish->targetTime = length / LGARCHER_MOVE_SPEED;
                                    } else {
                                        vec2  dir    = RandomDirection2D();
                                        flo32 length = RandomF32InRange( 100.0f, 300.0f );
                                        
                                        dir = adjustDirToBound( player->position, dir, length, bound );
                                        vec2 dest = player->position + dir * length;
                                        
                                        vec2 V = dest - fish->position;
                                        flo32 distFromDest = getLength( V );
                                        flo32 dist = RandomF32InRangeQuadratic( distFromDest, distFromDest * 0.5f );
                                        dist = minValue( dist, 400.0f );
                                        
                                        fish->direction  = getNormal( V );
                                        fish->targetTime = dist / LGARCHER_MOVE_SPEED;
                                    }
                                }
                            }
                        }
                    }
                } break;
                
                case fishState_shooting: {
                    if( fish->timer >= fish->targetTime ) {
                        BUBBLE_STATE * bubble = &appState->bubbleState;
                        vec2 dir = getNormal( fish->targetPos - fish->position );
                        
                        shootBubble( audioSystem, appState, fish->position, dir );
                        
                        fish->timer = 0.0f;
                        fish->state = fishState_idle;
                        fish->targetTime = RandomF32InRange( 0.8f, 1.8f );
                    }
                } break;
                
                case fishState_moving: {
                    if( fish->timer >= fish->targetTime ) {
                        fish->timer = 0.0f;
                        fish->state = fishState_idle;
                        fish->targetTime = RandomF32InRange( 0.8f, 1.8f );
                    }
                } break;
                
                case fishState_spawning: {
                    if( fish->timer >= fish->targetTime ) {
                        fish->state = fishState_idle;
                        fish->timer = 0.0f;
                        fish->targetTime = RandomF32InRange( 0.8f, 1.8f );
                    }
                } break;
                
                case fishState_stunned: {
                    if( fish->timer >= LGARCHER_STUNNED_TARGET_TIME ) {
                        fish->state = fishState_idle;
                        fish->timer = 0.0f;
                        fish->targetTime = RandomF32InRange( 0.8f, 1.8f );
                    }
                } break;
                
                case fishState_exiting: {
                    rect exit_bound = addRadius( appState->cameraBound, SPAWN_AND_EXIT_MARGIN );
                    if( !isInBound( fish->position, exit_bound ) ) {
                        fish->isActive = false;
                        state->nlgArcher--;
                    }
                } break;
                
                case fishState_dying: {
                    if( fish->timer >= LGARCHER_DYING_TARGET_TIME ) {
                        fish->isActive = false;
                        state->nlgArcher--;
                    }
                } break;
            }
            
            if( appState->gameOver ) {
                fish->state = fishState_idle_gameOver;
                fish->timer = 0.0f;
            }
        }
    }
    
    if( appState->respawnEnemyForScrolling ) {
        flo32 left  = cameraRel( appState, -0.08f );
        flo32 right = cameraRel( appState, 1.8f );
        for( uint32 iFish = 0; iFish < LGARCHER_MAX_COUNT; iFish++ ) {
            LGARCHER * fish = state->lgArcher + iFish;
            if( ( fish->isActive ) && ( fish->position.x < left ) ) {
                LGARCHER lgArcher = {};
                lgArcher.isActive = true;
                lgArcher.state    = fishState_idle;
                lgArcher.health   = LGARCHER_HEALTH;
                lgArcher.position.x = right;
                *fish = lgArcher;
            }
        }
    }
}

internal void
spawnlgArcher( APP_STATE * appState, uint32 nlgArcher = 8 ) {
    LGARCHER_STATE * state = &appState->lgArcherState;
    
    for( uint32 iFish = 0; iFish < nlgArcher; iFish++ ) {
        LGARCHER fish = {};
        fish.isActive = true;
        fish.state    = fishState_spawning;
        fish.health   = LGARCHER_HEALTH;
        
        SPAWN spawn = RandomSpawn( appState->cameraBound, LGARCHER_MOVE_FRICTION );
        
        fish.position   = spawn.position;
        fish.direction  = spawn.direction;
        fish.velocity   = spawn.velocity;
        fish.spawnSpeed = spawn.speed;
        fish.targetTime = spawn.targetTime;
        fish.facingDir  = spawn.facingDir;
        
        addlgArcher( state, fish );
    }
}

internal void
spawnlgArcherAt( APP_STATE * appState, vec2 position ) {
    LGARCHER_STATE * state = &appState->lgArcherState;
    
    LGARCHER fish = {};
    fish.isActive = true;
    fish.state    = fishState_idle;
    boo32 shootBubble = ( ( rand() % 3 ) != 0 );
    if( shootBubble ) {
        fish.state      = fishState_shooting;
        fish.targetTime = RandomF32InRange( 0.8f, 2.8f );
    }
    fish.health   = LGARCHER_HEALTH;
    fish.position = position;
    fish.useSmBoundBehavior = true;
    
    addlgArcher( state, fish );
}

internal void
exitlgArcher( APP_STATE * appState ) {
    LGARCHER_STATE * state = &appState->lgArcherState;
    
    rect outerBound = addRadius( appState->cameraBound, SPAWN_AND_EXIT_MARGIN );
    vec2 outerDim   = getDim( outerBound );
    
    for( uint32 iFish = 0; iFish < LGARCHER_MAX_COUNT; iFish++ ) {
        LGARCHER * fish = state->lgArcher + iFish;
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
