
//----------
// ARCHER FISH
//----------

internal void
addArcher( ARCHER_STATE * state, ARCHER archer ) {
    Assert( state->nArcher < ARCHER_MAX_COUNT );
    
    for( uint32 index = 0; index < ARCHER_MAX_COUNT; index++ ) {
        ARCHER fish = state->archer[ index ];
        if( !fish.isActive ) {
            state->archer[ index ] = archer;
            state->nArcher++;
            break;
        }
    }
}

internal void
shootBubblet( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, vec2 position, vec2 direction ) {
    BUBBLET_STATE * bubbletState = &appState->bubbletState;
    spawnBubblet( bubbletState, position, direction );
    
    uint32 index = rand() % 3;
    AUDIO_ID audioID = ( AUDIO_ID )( audioID_spit01 + index );
    playAudio( audioSystem, appState->audio, audioID, 0.85f );
}

internal circ
getArcherBiteCollision( ARCHER * fish ) {
    vec2 offset = PLAYER__ARCHER_BITE_COLLISION_OFFSET;
    
    if( fish->facingDir.isFacingLeft ) {
        offset.x = -offset.x;
    }
    
    circ result = {};
    result.center = fish->position + offset;
    result.radius = PLAYER__ARCHER_BITE_COLLISION_RADIUS;
    return result;
}

internal void
findClosestEnemyPos( APP_STATE * appState, ARCHER * archer ) {
    PLAYER_STATE * player = &appState->playerState;
    
    vec2  closestPos = player->position;
    flo32 minDistSq  = getLengthSq( archer->position - closestPos );
    
    SMTURTLE_STATE * smTurtleState = &appState->smTurtleState;
    for( uint32 iFish = 0; iFish < SMTURTLE_MAX_COUNT; iFish++ ) {
        SMTURTLE * fish = smTurtleState->smTurtle + iFish;
        if( fish->isActive ) {
            rect bound = appState->cameraBound;
            // TODO: maybe add a margin
            
            boo32 canTarget = ( fish->state != fishState_stunned ) && ( isInBound( fish->position, bound ) );
            if( canTarget ) {
                flo32 distSq = getLengthSq( archer->position - fish->position );
                if( distSq < minDistSq ) {
                    closestPos = fish->position;
                    minDistSq  = distSq;
                }
            }
        }
    }
    
    archer->targetPos = closestPos;
}

internal void
updateArcher( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, flo32 dt ) {
    PLAYER_STATE  * player = &appState->playerState;
    ARCHER_STATE  * state  = &appState->archerState;
    FISH_ATE_LIST * list   = &appState->fishAteList;
    
    // NOTE: can player move/stun archer with a tailwhip? there's really no reason to
    if( list->attemptBite ) {
        for( uint32 iFish = 0; iFish < ARCHER_MAX_COUNT; iFish++ ) {
            ARCHER * fish = state->archer + iFish;
            if( fish->isActive ) {
                circ bite = getArcherBiteCollision( fish );
                if( isInBound( player->position, bite ) ) {
                    fish->health = maxValue( fish->health - 1, 0 );
                    addBit( list, *fish );
                    
                    if( fish->health <= 0 ) {
                        fish->isActive = false;
                        state->nArcher--;
                        
                        addAte( list, *fish );
                    }
                }
            }
        }
    }
    
    if( player->doSlap ) {
        for( uint32 iFish = 0; iFish < ARCHER_MAX_COUNT; iFish++ ) {
            ARCHER * fish = state->archer + iFish;
            if( fish->isActive ) {
                vec2 playerPos = getSlapPos( player );
                vec2 V         = fish->position - playerPos;
                flo32 dist  = getLength( V );
                
                flo32 inner = 100.0f;
                flo32 outer = 186.0f;
                
                flo32 t     = 1.0f - clamp01( ( dist - inner ) / ( outer - inner ) );
                flo32 speed = lerp( 0.0f, t, 280.0f );
                vec2 dir = getNormal( fish->position - playerPos );
                fish->velocity += getNormal( V ) * speed;
            }
        }
    }
    
    for( uint32 iFish = 0; iFish < ARCHER_MAX_COUNT; iFish++ ) {
        ARCHER * fish = state->archer + iFish;
        if( fish->isActive ) {
            PARROTFISH_STATE * parrotfishState = &appState->parrotfishState;
            for( uint32 iParrotfish = 0; iParrotfish < PARROTFISH_MAX_COUNT; iParrotfish++ ) {
                PARROTFISH * parrotfish = parrotfishState->parrotfish + iParrotfish;
                if( ( parrotfish->isActive ) && ( parrotfish->state == fishState_stunned ) && ( parrotfish->timer < PARROTFISH_CAN_STUN_OTHER_FISH_TARGET_TIME ) ) {
                    vec2 V = fish->position - parrotfish->position;
                    
                    flo32 radius   = PARROTFISH_ARCHER_STUN_RADIUS;
                    flo32 radiusSq = radius * radius;
                    flo32 distSq   = getLengthSq( V );
                    if( distSq <= radiusSq ) {
                        fish->state = fishState_stunned;
                        fish->timer = 0.0f;
                        
                        vec2 dir = getNormal( V );
                        fish->velocity = dir * ARCHER_STUNNED_SPEED;
                        
                        vec2 particlePos = lerp( parrotfish->position, 1.4f, fish->position );
                        addSmExplosion( &appState->explosionList, particlePos );
                        
                        appState->audioManager.nSlap++;
                    }
                }
            }
            
            flo32 maxSpeed = ARCHER_MOVE_SPEED;
            flo32 friction = ARCHER_MOVE_FRICTION;
            
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
                    maxSpeed = ARCHER_EXIT_SPEED;
                } break;
                
                case fishState_dying:
                case fishState_stunned: {
                    maxSpeed = ARCHER_STUNNED_SPEED;
                    friction = ARCHER_STUNNED_FRICTION;
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
                    
                    flo32 range = ( PI / 6.0f );
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
        }
    }
}

internal void
drawArcher( RENDER_PASS * pass, APP_STATE * appState, ARCHER * fish, boo32 showDebug = false ) {
    Assert( fish->isActive );
    vec2 scale  = Vec2( ARCHER_BITMAP_SCALE_X,  ARCHER_BITMAP_SCALE_Y  );
    vec2 offset = Vec2( ARCHER_BITMAP_OFFSET_X, ARCHER_BITMAP_OFFSET_Y );
    
    boo32 facingLeft = fish->facingDir.isFacingLeft;
    
    vec4 modColor = Vec4( 1.0f, 0.55f, 0.55f, 1.0f );
    
    flo32 radians = fish->radians;
    switch( fish->state ) {
        case fishState_shooting: {
            flo32 t   = clamp01( fish->timer / fish->targetTime );
            scale.x  *= lerp( 1.0f, t, 0.7f );
            scale.y  *= lerp( 1.0f, t, 1.2f );
            offset.x *= lerp( 1.0f, t, 1.2f );
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
            
            modColor = Vec4( 0.55f, 1.0f, 0.55f, 1.0f );
        } break;
        
        case fishState_dying: {
            if( fish->timer >= ARCHER_DYING_FADE_DELAY_TARGET_TIME ) {
                flo32 numer = ARCHER_DYING_FADE_DELAY_TARGET_TIME - fish->timer;
                flo32 denom = ARCHER_DYING_FADE_DELAY_TARGET_TIME - ARCHER_DYING_TARGET_TIME;
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
    
    vec2 pos = fish->position + offset;
    drawQuad( pass, textureID_archer_idle, pos, xAxis, yAxis, modColor );
    
    vec2 health_dim  = Vec2( 34.0f, 4.0f );
    rect health_back = rectTCD( fish->position + Vec2( 0.0f, -10.0f ), health_dim );
    drawHealthBar( pass, health_back, fish->health, ARCHER_HEALTH );
    
    if( showDebug ) {
        circ c = getArcherBiteCollision( fish );
        drawCircleOutline( pass, c.center, c.radius, COLOR_RED );
        
        if( ( fish->state == fishState_idle ) || ( fish->state == fishState_shooting ) ) {
            drawLine( pass, fish->position, fish->targetPos, COLOR_GREEN );
        }
        
        //drawCircleOutline( pass, fish->position, 100.0f, COLOR_BLUE );
        //drawCircleOutline( pass, fish->position, 186.0f, COLOR_BLUE );
    }
}

internal void
drawArcher( RENDER_PASS * pass, APP_STATE * appState, boo32 showDebug = false ) {
    ARCHER_STATE * state  = &appState->archerState;
    
    for( uint32 iFish = 0; iFish < ARCHER_MAX_COUNT; iFish++ ) {
        ARCHER * fish = state->archer + iFish;
        if( fish->isActive ) {
            drawArcher( pass, appState, fish, showDebug );
        }
    }
}

internal void
finalizeArcher( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
    ARCHER_STATE * state = &appState->archerState;
    
    for( uint32 iFish = 0; iFish < ARCHER_MAX_COUNT; iFish++ ) {
        ARCHER * fish = state->archer + iFish;
        if( fish->isActive ) {
            switch( fish->state ) {
                case fishState_idle: {
                    if( fish->timer >= fish->targetTime ) {
                        fish->timer = 0.0f;
                        
                        rect bound = appState->cameraBound;
                        if( !isInBound( fish->position, bound ) ) {
                            vec2 P   = clampToBound( fish->position, bound );
                            vec2 dim = getDim( bound ) * 0.6667f;
                            
                            rect destBound = getIntersect( rectCD( P, dim ), bound );
                            vec2 dest = getRandomPoint( destBound );
                            
                            vec2 V = dest - fish->position;
                            fish->direction  = getNormal( V );
                            fish->targetTime = getLength( V ) / ARCHER_MOVE_SPEED;
                            
                            fish->state = fishState_moving;
                        } else {
                            boo32 toMoving = ( ( rand() % 3 ) == 0 );
                            if( toMoving ) {
                                fish->state = fishState_moving;
                                
                                flo32 radians   = RandomF32() * 2.0f * PI;
                                vec2  direction = Vec2( cosf( radians ), sinf( radians ) );
                                
                                flo32 distToBound = minDistToBound( bound, fish->position, &direction );
                                flo32 maxLength   = RandomF32InRange( ARCHER_MIN_DIST_TO_TRAVEL, ARCHER_MAX_DIST_TO_TRAVEL );
                                if( ( rand() % 100 ) == 0 ) {
                                    maxLength *= 4.0f;
                                }
                                flo32 length      = minValue( distToBound, maxLength );
                                fish->direction   = direction;
                                fish->targetTime  = length / ARCHER_MOVE_SPEED;
                            } else {
                                fish->state      = fishState_shooting;
                                fish->targetTime = 1.2f;
                            }
                        }
                    }
                } break;
                
                case fishState_shooting: {
                    if( fish->timer >= fish->targetTime ) {
                        BUBBLET_STATE * bubblet = &appState->bubbletState;
                        vec2 dir = getNormal( fish->targetPos - fish->position );
                        
                        shootBubblet( audioSystem, appState, fish->position, dir );
                        
                        fish->timer = 0.0f;
                        fish->state = fishState_idle;
                        fish->targetTime = RandomF32InRange( 0.6f, 1.4f );
                    }
                } break;
                
                case fishState_moving: {
                    if( fish->timer >= fish->targetTime ) {
                        fish->timer = 0.0f;
                        fish->state = fishState_idle;
                        fish->targetTime = RandomF32InRange( 0.6f, 1.4f );
                    }
                } break;
                
                case fishState_spawning: {
                    if( fish->timer >= fish->targetTime ) {
                        fish->state = fishState_idle;
                        fish->timer = 0.0f;
                        fish->targetTime = RandomF32InRange( 0.6f, 1.4f );
                    }
                } break;
                
                case fishState_exiting: {
                    rect exit_bound = addRadius( appState->cameraBound, SPAWN_AND_EXIT_MARGIN );
                    if( !isInBound( fish->position, exit_bound ) ) {
                        fish->isActive = false;
                        state->nArcher--;
                    }
                } break;
                
                case fishState_stunned: {
                    if( fish->timer >= ARCHER_STUNNED_TARGET_TIME ) {
                        fish->state = fishState_idle;
                        fish->timer = 0.0f;
                        fish->targetTime = RandomF32InRange( 0.6f, 1.4f );
                    }
                } break;
                
                case fishState_dying: {
                    if( fish->timer >= ARCHER_DYING_TARGET_TIME ) {
                        fish->isActive = false;
                        state->nArcher--;
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
        for( uint32 iFish = 0; iFish < ARCHER_MAX_COUNT; iFish++ ) {
            ARCHER * fish = state->archer + iFish;
            if( ( fish->isActive ) && ( fish->position.x < left ) ) {
                ARCHER archer = {};
                archer.isActive = true;
                archer.state    = fishState_idle;
                archer.health   = ARCHER_HEALTH;
                archer.position.x = right;
                *fish = archer;
            }
        }
    }
}

internal void
spawnArcher( APP_STATE * appState, uint32 nArcher = 8 ) {
    ARCHER_STATE * state = &appState->archerState;
    
    for( uint32 iFish = 0; iFish < nArcher; iFish++ ) {
        ARCHER fish = {};
        fish.isActive = true;
        fish.state    = fishState_spawning;
        fish.health   = ARCHER_HEALTH;
        
        SPAWN spawn = RandomSpawn( appState->cameraBound, ARCHER_MOVE_FRICTION );
        
        fish.position   = spawn.position;
        fish.direction  = spawn.direction;
        fish.velocity   = spawn.velocity;
        fish.spawnSpeed = spawn.speed;
        fish.targetTime = spawn.targetTime;
        fish.facingDir  = spawn.facingDir;
        
        addArcher( state, fish );
    }
}

internal void
spawnArcherAt( APP_STATE * appState, vec2 position ) {
    ARCHER_STATE * state = &appState->archerState;
    
    ARCHER fish = {};
    fish.isActive = true;
    fish.state    = fishState_idle;
    fish.health   = ARCHER_HEALTH;
    fish.position = position;
    
    addArcher( state, fish );
}

internal void
exitArcher( APP_STATE * appState ) {
    ARCHER_STATE * state = &appState->archerState;
    
    rect outerBound = addRadius( appState->cameraBound, SPAWN_AND_EXIT_MARGIN );
    vec2 outerDim   = getDim( outerBound );
    
    for( uint32 iFish = 0; iFish < ARCHER_MAX_COUNT; iFish++ ) {
        ARCHER * fish = state->archer + iFish;
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