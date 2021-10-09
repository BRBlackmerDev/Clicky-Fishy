
//----------
// PARROTFISH
//----------

internal void
addParrotfish( PARROTFISH_STATE * state, PARROTFISH Parrotfish ) {
    Assert( state->nParrotfish < PARROTFISH_MAX_COUNT );
    
    for( uint32 index = 0; index < PARROTFISH_MAX_COUNT; index++ ) {
        PARROTFISH fish = state->parrotfish[ index ];
        if( !fish.isActive ) {
            state->parrotfish[ index ] = Parrotfish;
            state->nParrotfish++;
            break;
        }
    }
}

internal circ
getParrotfishSlapCollision( PARROTFISH * fish ) {
    circ result = {};
    result.center = fish->position;
    result.radius = PARROTFISH_SLAP_COLLISION_RADIUS;
    return result;
}

internal void
updateParrotfish( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, flo32 dt ) {
    PLAYER_STATE     * player = &appState->playerState;
    PARROTFISH_STATE * state  = &appState->parrotfishState;
    AUDIO_MANAGER    * audioManager = &appState->audioManager;
    
    if( player->doSlap ) {
        for( uint32 iFish = 0; iFish < PARROTFISH_MAX_COUNT; iFish++ ) {
            PARROTFISH * fish = state->parrotfish + iFish;
            if( fish->isActive ) {
                circ slap = getParrotfishSlapCollision( fish );
                vec2 playerPos = getSlapPos( player );
                
                if( isInBound( playerPos, slap ) ) {
                    fish->state = fishState_stunned;
                    fish->timer = 0.0f;
                    
                    AUDIO_ID audioIDA = ( AUDIO_ID )( ( uint32 )audioID_slap01 + RandomU32InRange( 0, 1 ) );
                    playAudio( audioSystem, appState->audio, audioIDA, 1.0f );
                    
                    vec2 particlePos = lerp( playerPos, 0.8f, fish->position );
                    addMedExplosion( &appState->explosionList, particlePos );
                    
                    CAMERA_SHAKE * shake = &appState->cameraState.cameraShake;
                    initShake( shake, 0.0f, 0.0f, 0.5f, 60.0f, Vec2( 10.0f, 10.0f ) );
                    
                    vec2 dir = getNormal( fish->position - playerPos );
                    fish->velocity = dir * PARROTFISH_STUNNED_SPEED;
                    fish->ricochet_canStunPlayer = false;
                }
            }
        }
    }
    
    for( uint32 iFishA = 0; iFishA < PARROTFISH_MAX_COUNT; iFishA++ ) {
        PARROTFISH * fishA = state->parrotfish + iFishA;
        if( fishA->isActive ) {
            for( uint32 iFishB = iFishA + 1; iFishB < PARROTFISH_MAX_COUNT; iFishB++ ) {
                PARROTFISH * fishB = state->parrotfish + iFishB;
                if( fishB->isActive ) {
                    flo32 radius   = PARROTFISH_BOUNCE_RADIUS;
                    flo32 radiusSq = radius * radius;
                    
                    vec2 V = fishA->position - fishB->position;
                    flo32 distSq = getLengthSq( V );
                    if( distSq <= radiusSq ) {
                        if( dot( fishA->velocity, fishB->velocity ) < 0.0f ) {
                            _swap( vec2, fishA->velocity, fishB->velocity );
                        }
                        
                        vec2 bounce = getNormal( V ) * PARROTFISH_BOUNCE_SPEED;
                        fishA->velocity += bounce;
                        fishB->velocity -= bounce;
                        
                        audioManager->nParrotfishBounce++;
                        
                        // TODO: There might be gameplay issues with this creating an infinite stun chain, or being able to stun at too slow a speed. Keep an eye on it!
                        boo32 canStunA = ( fishA->state == fishState_stunned ) && ( getLength( fishA->velocity ) >= 320.0f );
                        boo32 canStunB = ( fishB->state == fishState_stunned ) && ( getLength( fishA->velocity ) >= 320.0f );
                        //boo32 canStunA = ( fishA->state == fishState_stunned ) && ( fishA->timer < PARROTFISH_CAN_STUN_OTHER_FISH_TARGET_TIME );
                        //boo32 canStunB = ( fishB->state == fishState_stunned ) && ( fishB->timer < PARROTFISH_CAN_STUN_OTHER_FISH_TARGET_TIME );
                        
                        if( canStunA ) {
                            fishB->state = fishState_stunned;
                            fishB->timer = 0.0f;
                        }
                        if( canStunB ) {
                            fishA->state = fishState_stunned;
                            fishA->timer = 0.0f;
                        }
                    }
                }
            }
        }
    }
    
    for( uint32 iFish = 0; iFish < PARROTFISH_MAX_COUNT; iFish++ ) {
        PARROTFISH * fish = state->parrotfish + iFish;
        if( fish->isActive ) {
            flo32 maxSpeed = 320.0f;
            flo32 friction = 2.0f;
            
            fish->timer += dt;
            
            vec2 dir = {};
            switch( fish->state ) {
                case fishState_idle_gameOver: {
                    
                } break;
                
                case fishState_idle: {
                    dir = getNormal( player->position - fish->position );
                } break;
                
                case fishState_stunned: {
                    friction = PARROTFISH_STUNNED_FRICTION;
                    
                    if( fish->timer < PARROTFISH_CAN_RICOCHET_TARGET_TIME ) {
                        CAGE_STATE * cageState = &appState->cageState;
                        for( uint32 iCage = 0; iCage < CAGE_MAX_COUNT; iCage++ ) {
                            CAGE * cage = cageState->cage + iCage;
                            if( ( cage->isActive ) && ( cage->health > 0 ) ) {
                                rect bound = rectCD( cage->position, PARROTFISH_CAGE_COLLISION_DIM );
                                if( isInBound( fish->position, bound ) ) {
                                    int32 damage = PARROTFISH_RICOCHET_DAMAGE;
                                    cage->health = maxValue( cage->health - damage, 0 );
                                    cage->timer  = 0.0f;
                                    
                                    RECT_PERIMETER A = getClosestPerimeterPoint( bound, fish->position );
                                    
                                    fish->position = A.position;
                                    if( dot( fish->velocity, A.normal ) < 0.0f ) {
                                        fish->velocity = reflect( fish->velocity, A.normal );
                                    }
                                    
                                    fish->velocity = getNormal( fish->velocity ) * PARROTFISH_RICOCHET_SPEED;
                                    fish->ricochet_canStunPlayer = true;
                                }
                            }
                        }
                        
                        NET_STATE * netState = &appState->netState;
                        for( uint32 iCage = 0; iCage < NET_MAX_COUNT; iCage++ ) {
                            NET * net = netState->net + iCage;
                            if( net->isActive ) {
                                NET_BLOCK * blockA = &net->blockA;
                                NET_BLOCK * blockB = &net->blockB;
                                
                                if( blockA->health > 0 ) {
                                    rect boundA = rectCD( blockA->position, PARROTFISH_NET_COLLISION_DIM );
                                    if( isInBound( fish->position, boundA ) ) {
                                        int32 damage = PARROTFISH_RICOCHET_DAMAGE;
                                        blockA->health = maxValue( blockA->health - damage, 0 );
                                        blockA->timer  = 0.0f;
                                        
                                        RECT_PERIMETER A = getClosestPerimeterPoint( boundA, fish->position );
                                        
                                        fish->position = A.position;
                                        if( dot( fish->velocity, A.normal ) < 0.0f ) {
                                            fish->velocity = reflect( fish->velocity, A.normal );
                                        }
                                        
                                        fish->velocity = getNormal( fish->velocity ) * PARROTFISH_RICOCHET_SPEED;
                                        fish->ricochet_canStunPlayer = true;
                                    }
                                }
                                
                                if( blockB->health > 0 ) {
                                    rect boundA = rectCD( blockB->position, PARROTFISH_NET_COLLISION_DIM );
                                    if( isInBound( fish->position, boundA ) ) {
                                        int32 damage = PARROTFISH_RICOCHET_DAMAGE;
                                        blockB->health = maxValue( blockB->health - damage, 0 );
                                        blockB->timer  = 0.0f;
                                        
                                        RECT_PERIMETER A = getClosestPerimeterPoint( boundA, fish->position );
                                        
                                        fish->position = A.position;
                                        if( dot( fish->velocity, A.normal ) < 0.0f ) {
                                            fish->velocity = reflect( fish->velocity, A.normal );
                                        }
                                        
                                        fish->velocity = getNormal( fish->velocity ) * PARROTFISH_RICOCHET_SPEED;
                                        fish->ricochet_canStunPlayer = true;
                                    }
                                }
                            }
                        }
                    }
                } break;
                
                case fishState_exiting: {
                    dir = fish->direction;
                    maxSpeed = PARROTFISH_EXIT_SPEED;
                } break;
                
                case fishState_headbutt: {
                } break;
            }
            
            vec2 accel = ( dir * maxSpeed - fish->velocity ) * friction;
            fish->velocity += accel * dt;
            
            vec2  P = fish->position;
            vec2  V = fish->velocity;
            vec2 dP = accel * ( dt * dt * 0.5f ) + fish->velocity * dt;
            vec2 prevN = {};
            boo32 doBounce    = false;
            int32 doBreakable = -1;
            
            // TODO: doBreakable only handles collision with a single breakable object for any single frame. This will need to change if we do multiple breakable objects that intersect to handle multiple breakable collisions in a single frame.
            
            OBSTACLE_STATE  * obstacleState  = &appState->obstacleState;
            BREAKABLE_STATE * breakableState = &appState->breakableState;
            for( uint32 iter = 0; iter < 4; iter++ ) {
                RAY2 ray = Ray2( P, dP );
                RAY2_INTERSECT bestIntersect = Ray2IntersectInit();
                
                for( uint32 iObstacle = 0; iObstacle < obstacleState->nObstacle; iObstacle++ ) {
                    OBSTACLE obstacle = obstacleState->obstacle[ iObstacle ];
                    CAPSULE2 cap = Capsule2( obstacle.bound.P, obstacle.bound.Q, 45.0f );
                    RAY2_INTERSECT intersect = doesIntersect( ray, cap );
                    if( ( intersect.isValid ) && ( intersect.t < bestIntersect.t ) ) {
                        bestIntersect = intersect;
                    }
                }
                
                for( uint32 iBreakable = 0; iBreakable < BREAKABLE_MAX_COUNT; iBreakable++ ) {
                    BREAKABLE breakable = breakableState->breakable[ iBreakable ];
                    if( ( breakable.isActive ) && ( breakable.collision_isActive ) ) {
                        RAY2_INTERSECT intersect = doesIntersect( ray, breakable.collision_bound, 45.0f );
                        if( ( intersect.isValid ) && ( intersect.t < bestIntersect.t ) && ( intersect.t <= 1.0f ) ) {
                            bestIntersect = intersect;
                            doBreakable   = iBreakable;
                        }
                    }
                }
                
                if( ( bestIntersect.isValid ) && ( bestIntersect.t <= 1.0f ) ) {
                    P   = bestIntersect.P;
                    dP -= ( 2.0f * dot( bestIntersect.N, dP ) ) * bestIntersect.N;
                    dP *= ( 1.0f - bestIntersect.t );
                    flo32 dotBPN = dot( bestIntersect.N, prevN );
                    if( dotBPN < 0.0f ) {
                        dP = {};
                        iter = 4;
                    }
                    prevN = bestIntersect.N;
                    
                    doBounce = true;
                    V -= ( 2.0f * dot( bestIntersect.N, V ) ) * bestIntersect.N;
                } else {
                    iter = 4;
                }
            }
            
            fish->position = P + dP;
            fish->velocity = V;
            if( doBounce ) {
                V *= 1.2f;
                fish->ricochet_canStunPlayer = true;
            }
            if( doBreakable > -1 ) {
                uint32 damage = 0;
                if( getLength( fish->velocity ) >= 320.0f ) {
                    damage = 2;
                }
                damageBreakable( audioSystem, appState, doBreakable, damage, -fish->velocity );
            }
            
            if( ( fish->state == fishState_idle_gameOver ) ) {
                updateFacingDir( &fish->facingDir, fish->position.x );
            } else if( fish->state != fishState_stunned ) {
                fish->facingDir.isFacingLeft = ( player->position.x < fish->position.x );
            }
        }
    }
}

internal void
drawParrotfish( RENDER_PASS * pass, APP_STATE * appState, PARROTFISH * fish, boo32 showDebug = false ) {
    TEXTURE_ID textureID = textureID_parrotfish_idle;
    
    if( fish->state == fishState_stunned ) {
        textureID = textureID_parrotfish_stunned;
    } else if( fish->state == fishState_headbutt ) {
        textureID = textureID_parrotfish_headbutt;
    } else {
        PLAYER_STATE * player = &appState->playerState;
        vec2  playerPos = getSlapPos( player );
        flo32 radiusSq = squareValue( PARROTFISH_WINDUP_BITMAP_RADIUS );
        flo32 distSq   = getLengthSq( playerPos - fish->position );
        if( distSq < radiusSq ) {
            textureID = textureID_parrotfish_windup;
        }
    }
    
    vec2 scale  = Vec2( 120.0f, 120.0f );
    vec2 offset = Vec2( 0.0f, 0.0f );
    
    boo32 facingLeft = fish->facingDir.isFacingLeft;
    if( facingLeft ) {
        offset.x = -offset.x;
        scale.x  = -scale.x;
    }
    
    vec2 pos = fish->position + offset;
    drawQuad( pass, textureID, pos, scale, COLOR_WHITE );
    
    //showDebug = true;
    if( showDebug ) {
        //circ c = getParrotfishSlapCollision( fish );
        //drawCircleOutline( pass, c.center, c.radius, COLOR_BLUE );
        //drawCircleOutline( pass, fish->position, PARROTFISH_HEADBUTT_RADIUS, COLOR_CYAN );
        //drawCircleOutline( pass, fish->position, PARROTFISH_BUBBLE_COLLISION_RADIUS, COLOR_MAGENTA );
        //drawCircleOutline( pass, fish->position, PARROTFISH_WINDUP_BITMAP_RADIUS, COLOR_GREEN );
        //drawCircleOutline( pass, fish->position, PARROTFISH_ARCHER_STUN_RADIUS, COLOR_GREEN );
        
        drawCircleOutline( pass, fish->position, PARROTFISH_BOUNCE_RADIUS, COLOR_RED );
        drawPoint( pass, fish->position, COLOR_CYAN );
        
        flo32 speed = getLength( fish->velocity );
        DISPLAY_VALUE( flo32, speed );
        DISPLAY_VALUE( vec2, fish->position );
    }
}

internal void
drawParrotfish( RENDER_PASS * pass, APP_STATE * appState, boo32 showDebug = false ) {
    PARROTFISH_STATE * state = &appState->parrotfishState;
    
    for( uint32 iFish = 0; iFish < PARROTFISH_MAX_COUNT; iFish++ ) {
        PARROTFISH * fish = state->parrotfish + iFish;
        if( fish->isActive ) {
            drawParrotfish( pass, appState, fish, showDebug );
        }
    }
}

internal void
finalizeParrotfish( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
    PARROTFISH_STATE * state  = &appState->parrotfishState;
    PLAYER_STATE     * player = &appState->playerState;
    
    for( uint32 iFish = 0; iFish < PARROTFISH_MAX_COUNT; iFish++ ) {
        PARROTFISH * fish = state->parrotfish + iFish;
        if( fish->isActive ) {
            switch( fish->state ) {
                case fishState_stunned: {
                    if( fish->timer >= PARROTFISH_STUNNED_TARGET_TIME ) {
                        fish->state = fishState_idle;
                        fish->ricochet_canStunPlayer = false;
                    }
                    
                    //if( ( fish->timer < PARROTFISH_CAN_STUN_OTHER_FISH_TARGET_TIME ) && ( fish->ricochet_canStunPlayer ) ) {
                    if( ( getLength( fish->velocity ) >= 320.0f ) && ( fish->ricochet_canStunPlayer ) ) {
                        flo32 radius = PARROTFISH_HEADBUTT_RADIUS;
                        flo32 radiusSq = radius * radius;
                        
                        vec2 playerPos = getSlapPos( player );
                        flo32 distSq = getLengthSq( playerPos - fish->position );
                        if( distSq <= radiusSq ) {
                            fish->ricochet_canStunPlayer = false;
                            initPlayerStun( player, fish );
                        }
                    }
                } break;
                
                case fishState_headbutt: {
                    if( fish->timer >= 1.0f ) {
                        fish->state = fishState_idle;
                    }
                } break;
                
                case fishState_exiting: {
                    rect exit_bound = addRadius( appState->cameraBound, SPAWN_AND_EXIT_MARGIN );
                    if( !isInBound( fish->position, exit_bound ) ) {
                        fish->isActive = false;
                        state->nParrotfish--;
                    }
                } break;
                
                case fishState_idle: {
                    flo32 radius = PARROTFISH_HEADBUTT_RADIUS;
                    flo32 radiusSq = radius * radius;
                    
                    vec2 playerPos = getSlapPos( player );
                    flo32 distSq = getLengthSq( playerPos - fish->position );
                    if( distSq <= radiusSq ) {
                        initPlayerStun( player, fish );
                        
                        fish->state = fishState_headbutt;
                        fish->timer = 0.0f;
                    }
                } break;
            }
            
            if( ( appState->gameOver ) && ( fish->state == fishState_idle ) ) {
                fish->state = fishState_idle_gameOver;
                fish->timer = 0.0f;
            }
        }
    }
}

internal void
spawnParrotfish( APP_STATE * appState, uint32 nParrotfish = 8 ) {
    PARROTFISH_STATE * state = &appState->parrotfishState;
    
    for( uint32 iFish = 0; iFish < nParrotfish; iFish++ ) {
        PARROTFISH fish = {};
        fish.isActive = true;
        fish.state    = fishState_idle;
        fish.position = RandomSpawnPos( appState->cameraBound, 100.0f );
        
        addParrotfish( state, fish );
    }
}

internal void
spawnParrotfish( APP_STATE * appState, vec2 position ) {
    PARROTFISH_STATE * state = &appState->parrotfishState;
    
    PARROTFISH fish = {};
    fish.isActive = true;
    fish.state    = fishState_idle;
    fish.position = position;
    
    addParrotfish( state, fish );
}


internal void
exitParrotfish( APP_STATE * appState ) {
    PARROTFISH_STATE * state = &appState->parrotfishState;
    
    rect outerBound = addRadius( appState->cameraBound, SPAWN_AND_EXIT_MARGIN );
    vec2 outerDim   = getDim( outerBound );
    
    for( uint32 iFish = 0; iFish < PARROTFISH_MAX_COUNT; iFish++ ) {
        PARROTFISH * fish = state->parrotfish + iFish;
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