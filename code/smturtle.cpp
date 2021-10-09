
//----------
// SM TURTLE
//----------

internal void
addsmTurtle( SMTURTLE_STATE * state, SMTURTLE smTurtle ) {
    Assert( state->nsmTurtle < SMTURTLE_MAX_COUNT );
    
    for( uint32 index = 0; index < SMTURTLE_MAX_COUNT; index++ ) {
        SMTURTLE fish = state->smTurtle[ index ];
        if( !fish.isActive ) {
            state->smTurtle[ index ] = smTurtle;
            state->nsmTurtle++;
            break;
        }
    }
}

internal void
findClosestEnemyPos( APP_STATE * appState, SMTURTLE * turtle ) {
    boo32 isValid    = false;
    vec2  closestPos = {};
    flo32 minDistSq = FLT_MAX;
    
    ARCHER_STATE * archerState = &appState->archerState;
    for( uint32 iFish = 0; iFish < ARCHER_MAX_COUNT; iFish++ ) {
        ARCHER * fish = archerState->archer + iFish;
        if( fish->isActive ) {
            rect bound = appState->cameraBound;
            // TODO: maybe add a margin
            
            boo32 canChase = ( fish->state != fishState_dying ) && ( isInBound( fish->position, bound ) );
            if( canChase ) {
                flo32 distSq = getLengthSq( turtle->position - fish->position );
                if( distSq < minDistSq ) {
                    isValid    = true;
                    closestPos = fish->position;
                    minDistSq = distSq;
                }
            }
        }
    }
    
    LGARCHER_STATE * lgArcherState = &appState->lgArcherState;
    for( uint32 iFish = 0; iFish < LGARCHER_MAX_COUNT; iFish++ ) {
        LGARCHER * fish = lgArcherState->lgArcher + iFish;
        if( fish->isActive ) {
            rect bound = appState->cameraBound;
            // TODO: maybe add a margin
            
            boo32 canChase = ( fish->state != fishState_dying ) && ( isInBound( fish->position, bound ) );
            if( canChase ) {
                flo32 distSq = getLengthSq( turtle->position - fish->position );
                if( distSq < minDistSq ) {
                    isValid    = true;
                    closestPos = fish->position;
                    minDistSq = distSq;
                }
            }
        }
    }
    
    turtle->chase_isValid  = isValid;
    turtle->chase_position = closestPos;
}

internal void
updatesmTurtle( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, flo32 dt ) {
    PLAYER_STATE   * player = &appState->playerState;
    SMTURTLE_STATE * state  = &appState->smTurtleState;
    AUDIO_MANAGER  * audioManager = &appState->audioManager;
    
    if( player->attemptHeal ) {
        for( uint32 iFish = 0; iFish < SMTURTLE_MAX_COUNT; iFish++ ) {
            SMTURTLE * fish = state->smTurtle + iFish;
            if( fish->isActive ) {
                circ heal = Circ( fish->position, PLAYER_SMTURTLE_HEAL_RADIUS );
                vec2 playerPos = getHealPos( player );
                
                if( isInBound( playerPos, heal ) ) {
                    fish->health = SMTURTLE_HEALTH;
                    if( fish->state == fishState_stunned ) {
                        fish->state = fishState_idle;
                    }
                }
            }
        }
    }
    
    for( uint32 iFish = 0; iFish < SMTURTLE_MAX_COUNT; iFish++ ) {
        SMTURTLE * fish = state->smTurtle + iFish;
        if( fish->isActive ) {
            flo32 maxSpeed = 280.0f;
            flo32 friction = 2.0f;
            
            fish->timer += dt;
            
            vec2 dir = {};
            switch( fish->state ) {
                case fishState_idle_gameOver: {
                    
                } break;
                
                case fishState_idle: {
                    findClosestEnemyPos( appState, fish );
                    if( fish->chase_isValid ) {
                        dir = getNormal( fish->chase_position - fish->position );
                    } else {
                        dir = getNormal( player->position - fish->position );
                    }
                    
                    boo32 isValid = false;
                    
                    ARCHER_STATE * archerState = &appState->archerState;
                    for( uint32 iArcher = 0; iArcher < ARCHER_MAX_COUNT; iArcher++ ) {
                        ARCHER * archer = archerState->archer + iArcher;
                        if( ( archer->isActive ) && ( archer->state != fishState_dying ) ) {
                            flo32 radius   = SMTURTLE_ATTACK_ARCHER_RADIUS;
                            flo32 radiusSq = radius * radius;
                            
                            vec2 V = archer->position - fish->position;
                            flo32 distSq = getLengthSq( V );
                            if( distSq <= radiusSq ) {
                                isValid = true;
                                
                                archer->health = maxValue( archer->health - 1, 0 );
                                if( archer->health > 0 ) {
                                    archer->state = fishState_stunned;
                                } else {
                                    archer->state = fishState_dying;
                                }
                                archer->timer = 0.0f;
                                
                                archer->velocity = getNormal( V ) * ARCHER_STUNNED_SPEED;
                                
                                vec2 particlePos = lerp( fish->position, 1.0f, archer->position );
                                addSmExplosion( &appState->explosionList, particlePos );
                                
                                appState->audioManager.nSlap++;
                            }
                        }
                    }
                    
                    LGARCHER_STATE * lgArcherState = &appState->lgArcherState;
                    for( uint32 iArcher = 0; iArcher < LGARCHER_MAX_COUNT; iArcher++ ) {
                        LGARCHER * lgArcher = lgArcherState->lgArcher + iArcher;
                        if( ( lgArcher->isActive ) && ( lgArcher->state != fishState_dying ) ) {
                            flo32 radius   = SMTURTLE_ATTACK_LGARCHER_RADIUS;
                            flo32 radiusSq = radius * radius;
                            
                            vec2 V = lgArcher->position - fish->position;
                            flo32 distSq = getLengthSq( V );
                            if( distSq <= radiusSq ) {
                                isValid = true;
                                
                                lgArcher->health = maxValue( lgArcher->health - 1, 0 );
                                if( lgArcher->health > 0 ) {
                                    lgArcher->state = fishState_stunned;
                                } else {
                                    lgArcher->state = fishState_dying;
                                }
                                lgArcher->timer = 0.0f;
                                
                                lgArcher->velocity = getNormal( V ) * LGARCHER_STUNNED_SPEED;
                                
                                vec2 particlePos = lerp( fish->position, 1.0f, lgArcher->position );
                                addSmExplosion( &appState->explosionList, particlePos );
                                
                                appState->audioManager.nSlap++;
                            }
                        }
                    }
                    
                    if( isValid ) {
                        fish->state = fishState_headbutt;
                        fish->timer = 0.0f;
                    }
                } break;
                
                case fishState_headbutt: {
                    
                } break;
                
#if 0
                case fishState_stunned: {
                    friction = SMTURTLE_STUNNED_FRICTION;
                    
                    if( fish->timer < SMTURTLE_CAN_RICOCHET_TARGET_TIME ) {
                        CAGE_STATE * cageState = &appState->cageState;
                        for( uint32 iCage = 0; iCage < CAGE_MAX_COUNT; iCage++ ) {
                            CAGE * cage = cageState->cage + iCage;
                            if( ( cage->isActive ) && ( cage->health > 0 ) ) {
                                rect bound = rectCD( cage->position, SMTURTLE_CAGE_COLLISION_DIM );
                                if( isInBound( fish->position, bound ) ) {
                                    int32 damage = SMTURTLE_RICOCHET_DAMAGE;
                                    cage->health = maxValue( cage->health - damage, 0 );
                                    cage->timer  = 0.0f;
                                    
                                    RECT_PERIMETER A = getClosestPerimeterPoint( bound, fish->position );
                                    
                                    fish->position = A.position;
                                    if( dot( fish->velocity, A.normal ) < 0.0f ) {
                                        fish->velocity = reflect( fish->velocity, A.normal );
                                    }
                                    
                                    fish->velocity = getNormal( fish->velocity ) * SMTURTLE_RICOCHET_SPEED;
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
                                    rect boundA = rectCD( blockA->position, SMTURTLE_NET_COLLISION_DIM );
                                    if( isInBound( fish->position, boundA ) ) {
                                        int32 damage = SMTURTLE_RICOCHET_DAMAGE;
                                        blockA->health = maxValue( blockA->health - damage, 0 );
                                        blockA->timer  = 0.0f;
                                        
                                        RECT_PERIMETER A = getClosestPerimeterPoint( boundA, fish->position );
                                        
                                        fish->position = A.position;
                                        if( dot( fish->velocity, A.normal ) < 0.0f ) {
                                            fish->velocity = reflect( fish->velocity, A.normal );
                                        }
                                        
                                        fish->velocity = getNormal( fish->velocity ) * SMTURTLE_RICOCHET_SPEED;
                                        fish->ricochet_canStunPlayer = true;
                                    }
                                }
                                
                                if( blockB->health > 0 ) {
                                    rect boundA = rectCD( blockB->position, SMTURTLE_NET_COLLISION_DIM );
                                    if( isInBound( fish->position, boundA ) ) {
                                        int32 damage = SMTURTLE_RICOCHET_DAMAGE;
                                        blockB->health = maxValue( blockB->health - damage, 0 );
                                        blockB->timer  = 0.0f;
                                        
                                        RECT_PERIMETER A = getClosestPerimeterPoint( boundA, fish->position );
                                        
                                        fish->position = A.position;
                                        if( dot( fish->velocity, A.normal ) < 0.0f ) {
                                            fish->velocity = reflect( fish->velocity, A.normal );
                                        }
                                        
                                        fish->velocity = getNormal( fish->velocity ) * SMTURTLE_RICOCHET_SPEED;
                                        fish->ricochet_canStunPlayer = true;
                                    }
                                }
                            }
                        }
                    }
                } break;
#endif
            }
            
            vec2 accel = ( dir * maxSpeed - fish->velocity ) * friction;
            fish->position += accel * ( dt * dt * 0.5f ) + fish->velocity * dt;
            fish->velocity += accel * dt;
            
            if( fish->chase_isValid ) {
                fish->facingDir.transitionX  = fish->position.x;
                fish->facingDir.isFacingLeft = ( fish->chase_position.x < fish->position.x );
            } else {
                updateFacingDir( &fish->facingDir, fish->position.x );
            }
        }
    }
}

internal void
drawsmTurtle( RENDER_PASS * pass, APP_STATE * appState, SMTURTLE * fish, boo32 showDebug = false ) {
    TEXTURE_ID textureID = textureID_smTurtle_idle;
    
    vec2 scale  = Vec2( 132.0f, 132.0f );
    vec2 offset = Vec2( 0.0f, 0.0f );
    
    boo32 facingLeft = fish->facingDir.isFacingLeft;
    if( facingLeft ) {
        offset.x = -offset.x;
        scale.x  = -scale.x;
    }
    
    vec2 pos = fish->position + offset;
    drawQuad( pass, textureID, pos, scale, COLOR_WHITE );
    
    if( showDebug ) {
        drawCircleOutline( pass, fish->position, PLAYER_SMTURTLE_HEAL_RADIUS, COLOR_RED );
        
        if( fish->chase_isValid ) {
            drawLine( pass, fish->position, fish->chase_position, COLOR_CYAN );
        }
    }
}

internal void
drawsmTurtle( RENDER_PASS * pass, APP_STATE * appState, boo32 showDebug = false ) {
    SMTURTLE_STATE * state = &appState->smTurtleState;
    
    for( uint32 iFish = 0; iFish < SMTURTLE_MAX_COUNT; iFish++ ) {
        SMTURTLE * fish = state->smTurtle + iFish;
        if( fish->isActive ) {
            drawsmTurtle( pass, appState, fish, showDebug );
            
            vec2 health_dim   = Vec2( 34.0f, 4.0f ) * 2.5f;
            rect health_bound = rectTCD( fish->position + Vec2( 0.0f, -10.0f ), health_dim );
            drawHealthBar( pass, health_bound, fish->health, SMTURTLE_HEALTH );
        }
    }
}

internal void
finalizesmTurtle( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
    SMTURTLE_STATE * state  = &appState->smTurtleState;
    PLAYER_STATE   * player = &appState->playerState;
    
    for( uint32 iFish = 0; iFish < SMTURTLE_MAX_COUNT; iFish++ ) {
        SMTURTLE * fish = state->smTurtle + iFish;
        if( fish->isActive ) {
            BUBBLET_STATE * bubbletState = &appState->bubbletState;
            for( uint32 iBubblet = 0; iBubblet < BUBBLET_MAX_COUNT; iBubblet++ ) {
                BUBBLET * bubblet = bubbletState->bubblet + iBubblet;
                if( bubblet->isActive ) {
                    flo32 radius = SMTURTLE_BUBBLET_COLLISION_RADIUS;
                    flo32 radiusSq = radius * radius;
                    
                    flo32 distSq = getLengthSq( fish->position - bubblet->position );
                    if( distSq <= radiusSq ) {
                        fish->health = maxValue( fish->health - SMTURTLE_BUBBLET_DAMAGE, 0 );
                        if( fish->health == 0 ) {
                            fish->state = fishState_stunned;
                        }
                        
                        popBubblet( audioSystem, appState, bubblet );
                    }
                }
            }
            
            BUBBLE_STATE * bubbleState = &appState->bubbleState;
            for( uint32 iBubble = 0; iBubble < BUBBLE_MAX_COUNT; iBubble++ ) {
                BUBBLE * bubble = bubbleState->bubble + iBubble;
                if( bubble->isActive ) {
                    flo32 radius = SMTURTLE_BUBBLE_COLLISION_RADIUS;
                    flo32 radiusSq = radius * radius;
                    
                    flo32 distSq = getLengthSq( fish->position - bubble->position );
                    if( distSq <= radiusSq ) {
                        fish->health = maxValue( fish->health - SMTURTLE_BUBBLE_DAMAGE, 0 );
                        if( fish->health == 0 ) {
                            fish->state = fishState_stunned;
                        }
                        
                        popBubble( audioSystem, appState, bubble );
                    }
                }
            }
            
            switch( fish->state ) {
                case fishState_idle: {
                } break;
                
                case fishState_headbutt: {
                    if( fish->timer >= SMTURTLE_ATTACK_COOLDOWN_TARGET_TIME ) {
                        fish->timer = 0.0f;
                        fish->state = fishState_idle;
                    }
                } break;
            }
            
            // TODO: what should sm turtle do during game over?
        }
    }
}

internal void
spawnsmTurtle( APP_STATE * appState, uint32 nsmTurtle = 8 ) {
    SMTURTLE_STATE * state = &appState->smTurtleState;
    
    for( uint32 iFish = 0; iFish < nsmTurtle; iFish++ ) {
        SMTURTLE fish = {};
        fish.isActive = true;
        fish.state    = fishState_idle;
        fish.position = getRandomPoint( appState->cameraBound );
        fish.health   = SMTURTLE_HEALTH;
        
        addsmTurtle( state, fish );
    }
}


internal void
exitsmTurtle( APP_STATE * appState ) {
    SMTURTLE_STATE * state = &appState->smTurtleState;
    
    rect outerBound = addRadius( appState->cameraBound, SPAWN_AND_EXIT_MARGIN );
    vec2 outerDim   = getDim( outerBound );
    
    for( uint32 iFish = 0; iFish < SMTURTLE_MAX_COUNT; iFish++ ) {
        SMTURTLE * fish = state->smTurtle + iFish;
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