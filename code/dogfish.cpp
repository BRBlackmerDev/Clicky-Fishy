
internal void
setIdle( DOGFISH * fish ) {
    fish->state = fishState_idle;
    fish->timer = 0.0f;
    
    flo32 radians = RandomF32() * PI * 2.0f;
    fish->direction  = Vec2( cosf( radians ), sinf( radians ) );
    fish->targetTime = RandomF32InRange( DOGFISH_IDLE_TARGET_TIME_LO, DOGFISH_IDLE_TARGET_TIME_HI );
    fish->speed      = RandomF32InRange( DOGFISH_IDLE_SPEED_LO, DOGFISH_IDLE_SPEED_HI );
    
    fish->chase_value = 0.0f;
}

internal void
removeThisTargetAndSetIdle( DOGFISH_STATE * state, FISH_TYPE type, uint32 index, int32 skipIndex ) {
    for( uint32 iter = 0; iter < DOGFISH_MAX_COUNT; iter++ ) {
        if( ( int32 )iter != skipIndex ) {
            DOGFISH * dogfish = state->dogfish + iter;
            if( dogfish->isActive ) {
                boo32 isValid = ( dogfish->chase_type == type ) && ( ( type == fishType_player ) || ( dogfish->chase_index == index ) );
                
                if( isValid ) {
                    dogfish->chase_prevType  = dogfish->chase_type;
                    dogfish->chase_prevIndex = dogfish->chase_index;
                    dogfish->chase_type      = fishType_null;
                    
                    if( ( dogfish->state == fishState_chasing ) ||( dogfish->state == fishState_lungePrep ) || ( dogfish->state == fishState_lunging ) ) {
                        setIdle( dogfish );
                    }
                }
            }
        }
    }
}

internal void
addDogfish( DOGFISH_STATE * state, DOGFISH dogfish ) {
    Assert( state->nDogfish < DOGFISH_MAX_COUNT );
    
    for( uint32 index = 0; index < DOGFISH_MAX_COUNT; index++ ) {
        DOGFISH fish = state->dogfish[ index ];
        if( !fish.isActive ) {
            state->dogfish[ index ] = dogfish;
            state->nDogfish++;
            break;
        }
    }
}

internal void
addDogfishToDebug( DEBUG_STATE * debug, DOGFISH_STATE * state ) {
    for( uint32 iFish = 0; iFish < DOGFISH_MAX_COUNT; iFish++ ) {
        DOGFISH dogfish = state->dogfish[ iFish ];
        if( dogfish.isActive ) {
            DEBUG__LAST_BITE_FRAME__FISH fish = {};
            fish.type       = fishType_dogfish;
            fish.pos        = dogfish.position;
            fish.facingLeft = dogfish.facingDir.isFacingLeft;
            addToDebug( debug->lastBite.frame + debug->lastBite.atIndex, fish );
        }
    }
}

internal void
updateDogfish( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, flo32 dt ) {
    PLAYER_STATE * player = &appState->playerState;
    DOGFISH_STATE * state = &appState->dogfishState;
    FISH_ATE_LIST  * list  = &appState->fishAteList;
    
    if( list->attemptBite ) {
        int32 damage = 1;
        
#if 0
        ITEM_TOOLBAR * toolbar = &appState->itemToolbar;
        for( uint32 iter = 0; iter < toolbar->nItem; iter++ ) {
            ITEM item = toolbar->item[ iter ];
            
            if( item.itemID == itemID_strongBite ) {
                // TODO: Should you be able to stack multiple bite items?
                damage = 2;
            }
        }
#endif
        
        //addDogfishToDebug( &appState->debugState, state );
        
        for( uint32 iFish = 0; iFish < DOGFISH_MAX_COUNT; iFish++ ) {
            DOGFISH * fish = state->dogfish + iFish;
            if( ( fish->isActive ) && ( fish->health > 0 ) ) {
                vec2 dim    = Vec2( DOGFISH_BITE_DIM_X, DOGFISH_BITE_DIM_Y );
                vec2 offset = Vec2( -( DOGFISH_BITE_OFFSET_X + dim.x * 0.5f ), DOGFISH_BITE_OFFSET_Y );
                if( fish->facingDir.isFacingLeft ) {
                    offset.x = -offset.x;
                }
                rect bound = rectCD( fish->position + offset, dim );
                
                if( isInBound( player->position, bound ) ) {
                    fish->health = maxValue( fish->health - damage, 0 );
                    fish->lastBit_timer = DOGFISH_LAST_BITE_TARGET_TIME;
                    addBit( list, *fish );
                    if( fish->state != fishState_stunnedByBite ) {
                        fish->state  = fishState_stunnedByBite;
                        fish->timer  = 0.0f;
                        fish->chase_type    = fishType_player;
                        fish->chase_nLunges = 0;
                    }
                    if( fish->health == 0 ) {
                        fish->state = fishState_dying;
                        fish->timer = 0.0f;
                        state->nDogfish--;
                        
                        addAte( list, *fish );
                    }
                }
            }
        }
    }
    
    for( uint32 iFish = 0; iFish < DOGFISH_MAX_COUNT; iFish++ ) {
        DOGFISH * fish = state->dogfish + iFish;
        if( fish->isActive ) {
            boo32 changeState = false;
            
            flo32 maxSpeed = DOGFISH_MAX_SPEED;
            flo32 friction = DOGFISH_FRICTION;
            
            fish->timer += dt;
            fish->lastBit_timer -= dt;
            
            vec2 dir = {};
            switch( fish->state ) {
                case fishState_idle_gameOver: {
                    fish->dir_timer   += dt;
                    fish->speed_timer += dt;
                    
                    rect bound = addRadius( appState->cameraBound, -40.0f );
                    
                    if( fish->dir_timer >= fish->dir_targetTime ) {
                        fish->dir_timer      = 0.0f;
                        fish->dir_targetTime = RandomF32InRange( 0.8f, 1.4f );
                        
                        flo32 range = ( PI * 2.0f ) / 3.0f;
                        flo32 rad   = RandomF32() * range + RandomF32() * range - range;
                        
                        fish->targetRadians += rad;
                        if( fish->targetRadians < 0.0f ) { fish->targetRadians += TAU; }
                        
                        vec2 pos  = fish->position;
                        vec2 d    = toDirection2D( fish->targetRadians );
                        vec2 dest = fish->position + d * 200.0f;
                        if( isInBound( pos, bound ) ) {
                            if( ( dest.x < bound.left ) || ( dest.x > bound.right ) ) {
                                d.x = -d.x;
                            }
                            if( ( dest.y < bound.bottom ) || ( dest.y > bound.top ) ) {
                                d.y = -d.y;
                            }
                        } else {
                            if( ( ( pos.x < bound.left ) && ( d.x < 0.0f ) ) ||
                               ( ( pos.x > bound.right ) && ( d.x > 0.0f ) ) ) {
                                d.x = -d.x;
                            }
                            if( ( ( pos.y < bound.bottom ) && ( d.y < 0.0f ) ) ||
                               ( ( pos.y > bound.top ) && ( d.y > 0.0f ) ) ) {
                                d.y = -d.y;
                            }
                        }
                        
                        fish->targetRadians = atan2f( d.y, d.x );
                        if( fish->targetRadians < 0.0f ) { fish->targetRadians += TAU; }
                    }
                    
                    if( fish->speed_timer >= fish->speed_targetTime ) {
                        fish->speed_timer      = 0.0f;
                        fish->speed_targetTime = RandomF32InRange( 1.4f, 2.0f );
                        
                        fish->targetSpeed = RandomF32InRange( 40.0f, 140.0f );
                    }
                    
                    if( fish->speed > fish->targetSpeed ) {
                        flo32 decel = 40.0f * dt;
                        flo32 dist  = fish->speed - fish->targetSpeed;
                        if( dist < decel ) {
                            fish->speed = fish->targetSpeed;
                        } else {
                            fish->speed -= decel;
                        }
                        
                    }
                    if( fish->speed < fish->targetSpeed ) {
                        flo32 accel = 40.0f * dt;
                        flo32 dist  = fish->targetSpeed - fish->speed;
                        if( dist < accel ) {
                            fish->speed = fish->targetSpeed;
                        } else {
                            fish->speed += accel;
                        }
                    }
                    
                    flo32 turnRate = ( PI / 3.0f ) * dt;
                    if( fish->radians != fish->targetRadians ) {
                        flo32 dist = fabsf( fish->targetRadians - fish->radians );
                        if( dist > PI ) {
                            dist = TAU - dist;
                        }
                        
                        if( dist < turnRate ) {
                            fish->radians = fish->targetRadians;
                        } else {
                            vec2 dDir  = toDirection2D( fish->radians );
                            vec2 dDest = toDirection2D( fish->targetRadians );
                            if( dot( dDir, getPerp( dDest ) ) > 0.0f ) {
                                turnRate = -turnRate;
                            }
                            fish->radians += turnRate;
                            
                            if( fish->radians < 0.0f ) { fish->radians += TAU; }
                            if( fish->radians >= TAU ) { fish->radians -= TAU; }
                        }
                    }
                    
                    fish->direction = Vec2( cosf( fish->radians ), sinf( fish->radians ) );
                    dir      = fish->direction;
                    maxSpeed = fish->speed;
                } break;
                
                case fishState_idle: {
                    fish->dir_timer   += dt;
                    fish->speed_timer += dt;
                    
                    rect bound = addRadius( appState->cameraBound, -40.0f );
                    
                    if( fish->dir_timer >= fish->dir_targetTime ) {
                        fish->dir_timer      = 0.0f;
                        fish->dir_targetTime = RandomF32InRange( 0.8f, 1.4f );
                        
                        flo32 range = ( PI * 2.0f ) / 3.0f;
                        flo32 rad   = RandomF32() * range + RandomF32() * range - range;
                        
                        fish->targetRadians += rad;
                        if( fish->targetRadians < 0.0f ) { fish->targetRadians += TAU; }
                        
                        vec2 pos  = fish->position;
                        vec2 d    = toDirection2D( fish->targetRadians );
                        vec2 dest = fish->position + d * 200.0f;
                        if( isInBound( pos, bound ) ) {
                            if( ( dest.x < bound.left ) || ( dest.x > bound.right ) ) {
                                d.x = -d.x;
                            }
                            if( ( dest.y < bound.bottom ) || ( dest.y > bound.top ) ) {
                                d.y = -d.y;
                            }
                        } else {
                            if( ( ( pos.x < bound.left ) && ( d.x < 0.0f ) ) ||
                               ( ( pos.x > bound.right ) && ( d.x > 0.0f ) ) ) {
                                d.x = -d.x;
                            }
                            if( ( ( pos.y < bound.bottom ) && ( d.y < 0.0f ) ) ||
                               ( ( pos.y > bound.top ) && ( d.y > 0.0f ) ) ) {
                                d.y = -d.y;
                            }
                        }
                        
                        fish->targetRadians = atan2f( d.y, d.x );
                        if( fish->targetRadians < 0.0f ) { fish->targetRadians += TAU; }
                    }
                    
                    if( fish->speed_timer >= fish->speed_targetTime ) {
                        fish->speed_timer      = 0.0f;
                        fish->speed_targetTime = RandomF32InRange( 1.4f, 2.0f );
                        
                        fish->targetSpeed = RandomF32InRange( 40.0f, 140.0f );
                    }
                    
                    if( fish->speed > fish->targetSpeed ) {
                        flo32 decel = 40.0f * dt;
                        flo32 dist  = fish->speed - fish->targetSpeed;
                        if( dist < decel ) {
                            fish->speed = fish->targetSpeed;
                        } else {
                            fish->speed -= decel;
                        }
                        
                    }
                    if( fish->speed < fish->targetSpeed ) {
                        flo32 accel = 40.0f * dt;
                        flo32 dist  = fish->targetSpeed - fish->speed;
                        if( dist < accel ) {
                            fish->speed = fish->targetSpeed;
                        } else {
                            fish->speed += accel;
                        }
                    }
                    
                    flo32 turnRate = ( PI / 3.0f ) * dt;
                    if( fish->radians != fish->targetRadians ) {
                        flo32 dist = fabsf( fish->targetRadians - fish->radians );
                        if( dist > PI ) {
                            dist = TAU - dist;
                        }
                        
                        if( dist < turnRate ) {
                            fish->radians = fish->targetRadians;
                        } else {
                            vec2 dDir  = toDirection2D( fish->radians );
                            vec2 dDest = toDirection2D( fish->targetRadians );
                            if( dot( dDir, getPerp( dDest ) ) > 0.0f ) {
                                turnRate = -turnRate;
                            }
                            fish->radians += turnRate;
                            
                            if( fish->radians < 0.0f ) { fish->radians += TAU; }
                            if( fish->radians >= TAU ) { fish->radians -= TAU; }
                        }
                    }
                    
                    fish->direction = Vec2( cosf( fish->radians ), sinf( fish->radians ) );
                    dir      = fish->direction;
                    maxSpeed = fish->speed;
                    
                    if( fish->chase_type == fishType_null ) {
                        fish->chase_nLunges = 0;
                        fish->chase_value -= DOGFISH_SIGHT_EMPTY_RATE * dt;
                        fish->chase_value  = maxValue( fish->chase_value, 0.0f );
                    }
                    if( fish->chase_type != fishType_null ) {
                        flo32 rate = 0.0f;
                        
                        vec2  pos = getPosOfTarget( appState, fish->chase_type, fish->chase_index );
                        flo32 distSq = getLengthSq( pos - fish->position );
                        
                        boo32 seeTarget = false;
                        if( distSq < squareValue( DOGFISH_SIGHT_RADIUS_LO ) ) {
                            seeTarget = true;
                            rate = DOGFISH_SIGHT_FILL_RATE_HI;
                        } else if( distSq < squareValue( DOGFISH_SIGHT_RADIUS_MID ) ) {
                            seeTarget = true;
                            rate = DOGFISH_SIGHT_FILL_RATE_MID;
                        } else if( distSq < squareValue( DOGFISH_SIGHT_RADIUS_HI ) ) {
                            seeTarget = true;
                            
                            flo32 range = DOGFISH_SIGHT_RADIUS_HI - DOGFISH_SIGHT_RADIUS_MID;
                            flo32 dist  = getLength( pos - fish->position );
                            flo32 t     = clamp01( ( dist - DOGFISH_SIGHT_RADIUS_MID ) / range );
                            rate = lerp( DOGFISH_SIGHT_FILL_RATE_MID, t, DOGFISH_SIGHT_FILL_RATE_LO );
                        }
                        
                        if( seeTarget ) {
                            fish->chase_value = minValue( fish->chase_value + rate * dt, 1.0f );
                        } else {
                            fish->chase_prevType  = fish->chase_type;
                            fish->chase_prevIndex = fish->chase_index;
                            
                            fish->chase_type  = fishType_null;
                        }
                    } else {
                        vec2 playerPos = getPosOfTarget( appState, fishType_player, 0 );
                        
                        flo32 radiusSq = DOGFISH_SIGHT_RADIUS_HI * DOGFISH_SIGHT_RADIUS_HI;
                        flo32 player_distSq = getLengthSq( playerPos - fish->position );
                        if( ( !player->bitByDogfish ) && ( player_distSq < radiusSq ) ) {
                            fish->chase_type = fishType_player;
                            if( fish->chase_prevType != fishType_player ) {
                                fish->chase_value   = 0.0f;
                            }
                        } else {
                            int32 target    = -1;
                            flo32 maxDistSq = radiusSq;
                            
                            GOLDFISH_STATE * goldfishState = &appState->goldfishState;
                            for( uint32 iter = 0; iter < GOLDFISH_MAX_COUNT; iter++ ) {
                                GOLDFISH goldfish = goldfishState->goldfish[ iter ];
                                if( goldfish.isActive ) {
                                    flo32 distSq = getLengthSq( goldfish.position - fish->position );
                                    if( distSq < maxDistSq ) {
                                        target    = iter;
                                        maxDistSq = distSq;
                                    }
                                }
                            }
                            
                            if( target > -1 ) {
                                fish->chase_index = target;
                                fish->chase_type  = fishType_goldfish;
                                
                                if( ( fish->chase_prevType != fish->chase_type ) || ( fish->chase_prevIndex != fish->chase_index ) ) {
                                    fish->chase_value   = 0.0f;
                                }
                            }
                        }
                    }
                } break;
                
                case fishState_chasing: {
                    vec2 p = getPosOfTarget( appState, fish->chase_type, fish->chase_index );
                    vec2 v = p - fish->position;
                    dir = getNormal( v );
                } break;
                
                case fishState_lunging: {
                    friction = DOGFISH_LUNGE_FRICTION;
                    
                    if( ( fish->chase_type != fishType_null ) && ( fish->timer <= DOGFISH_LUNGE_BITE_TARGET_TIME ) ) {
                        vec2 P = getPosOfTarget( appState, fish->chase_type, fish->chase_index );
                        
                        flo32 radiusSq = DOGFISH_BITE_RADIUS * DOGFISH_BITE_RADIUS;
                        flo32 distSq   = getLengthSq( P - fish->position );
                        if( distSq <= radiusSq ) {
                            changeState = true;
                            switch( fish->chase_type ) {
                                case fishType_player: {
                                    fish->state = fishState_biting;
                                    fish->timer = 0.0f;
                                    
                                    player->bitByDogfish   = true;
                                    player->dogfish        = fish;
                                    player->dogfish_escape = DOGFISH_ESCAPE_VALUE * 0.5f;
                                    
                                    player->health = maxValue( player->health - DOGFISH_BITE_INITIAL_DAMAGE, 0.0f );
                                    player->dogfish_timer = 0.0f;
                                    
                                    removeThisTargetAndSetIdle( state, fish->chase_type, 0, iFish );
                                    
                                    AUDIO_ID audioID = ( AUDIO_ID )( ( rand() % 5 ) + audioID_dogfish_biting01 );
                                    playAudio( audioSystem, appState->audio, audioID, 0.6f );
                                } break;
                                
                                case fishType_goldfish: {
                                    GOLDFISH_STATE * goldfishState = &appState->goldfishState;
                                    goldfishState->nGoldfish--;
                                    
                                    GOLDFISH * goldfish = goldfishState->goldfish + fish->chase_index;
                                    goldfish->isActive = false;
                                    
                                    // add dogfish_ate_goldfish to fishAte list
                                    
                                    // TODO: cooldown timer after eating goldfish
                                    
                                    //fish->state = fishState_idle;
                                    //fish->timer = 0.0f;
                                    
                                    removeThisTargetAndSetIdle( state, fish->chase_type, fish->chase_index, iFish );
                                    
                                    fish->chase_ateTarget = true;
                                    fish->chase_index     = 0;
                                    fish->chase_type      = fishType_null;
                                    fish->chase_nLunges   = 0;
                                } break;
                            }
                        }
                    }
                } break;
                
                case fishState_biting: {
                    fish->escape    = player->dogfish_escape;
                    fish->escapeDir = player->dogfish_escapeDir;
                } break;
                
                case fishState_dying:
                case fishState_lungePrep:
                case fishState_stunnedByEscape:
                case fishState_stunnedByBite: {
                    // do nothing
                } break;
            }
            
            vec2 repel_dir = fish->repel;
            for( uint32 iter = iFish + 1; iter < DOGFISH_MAX_COUNT; iter++ ) {
                DOGFISH * fish0 = state->dogfish + iter;
                if( fish0->isActive ) {
                    vec2  V = getRepel( fish->position, fish0->position, DOGFISH_REPEL_MAX_STRENGTH, DOGFISH_REPEL_MAX_DISTANCE );
                    
                    repel_dir     -= V;
                    fish0->repel  += V;
                }
            }
            
            vec2 accel = ( dir * maxSpeed - fish->velocity + repel_dir ) * friction;
            fish->position += accel * ( dt * dt * 0.5f ) + fish->velocity * dt;
            fish->velocity += accel * dt;
            fish->repel = {};
            
            if( ( fish->state == fishState_biting ) || ( fish->state == fishState_idle ) ) {
                changeState = true;
            }
            if( ( appState->gameOver ) && ( changeState ) ) {
                fish->state = fishState_idle_gameOver;
                fish->timer = 0.0f;
            }
        }
    }
    
    for( uint32 iFish = 0; iFish < DOGFISH_MAX_COUNT; iFish++ ) {
        DOGFISH * fish = state->dogfish + iFish;
        if( fish->isActive ) {
            updateFacingDir( &fish->facingDir, fish->position.x );
        }
    }
}

internal void
drawDogfish( RENDER_PASS * pass, APP_STATE * appState, DOGFISH * fish, boo32 showDebug = false ) {
    flo32 alpha = 1.0f;
    if( fish->state == fishState_dying ) {
        alpha = clamp01( 1.0f - ( fish->timer / DOGFISH_DYING_TARGET_TIME ) );
    }
    
    vec2 offset = {};
    if( fish->state == fishState_biting ) {
        flo32 t = clamp01( fish->escape / DOGFISH_ESCAPE_VALUE );
        offset  = fish->escapeDir * ( t * DOGFISH_ESCAPE_BITMAP_OFFSET );
        offset += RandomDirection2D() * ( RandomF32() * DOGFISH_STUNNED_BY_BITE_BITMAP_OFFSET );
    }
    if( fish->lastBit_timer > 0.0f ) {
        offset = RandomDirection2D() * ( RandomF32() * DOGFISH_STUNNED_BY_BITE_BITMAP_OFFSET );
    }
    
    TEXTURE_ID injuryID  = textureID_dogfish_injury01;
    if( fish->health <= 7 ) { injuryID = textureID_dogfish_injury02; }
    if( fish->health <= 4 ) { injuryID = textureID_dogfish_injury03; }
    
    TEXTURE_ID textureID = textureID_dogfish_idle;
    switch( fish->state ) {
        case fishState_dying: {
            textureID = textureID_dogfish_dying;
        } break;
        
        case fishState_stunnedByBite:
        case fishState_stunnedByEscape: {
            textureID = textureID_dogfish_stunned;
        } break;
        
        case fishState_biting: {
            textureID = textureID_dogfish_biting;
            if( fish->escape >= ( DOGFISH_ESCAPE_VALUE * 0.6667f ) ) {
                textureID = textureID_dogfish_shakingFree;
            }
            // TODO: biting offset
        } break;
        
        case fishState_lungePrep: {
            textureID = textureID_dogfish_lungePrep;
        } break;
        
        case fishState_lunging: {
            textureID = textureID_dogfish_lunging;
        } break;
    }
    
    vec2 bitmap_scale  = Vec2( DOGFISH_BITMAP_SCALE_X, DOGFISH_BITMAP_SCALE_Y );
    vec2 bitmap_offset = Vec2( DOGFISH_BITMAP_OFFSET_X, DOGFISH_BITMAP_OFFSET_Y );
    
    boo32 facingLeft = fish->facingDir.isFacingLeft;
    if( facingLeft ) {
        bitmap_scale.x  = -bitmap_scale.x;
        bitmap_offset.x = -bitmap_offset.x;
    }
    
    vec2 position = fish->position + offset;
    vec4 color = Vec4( alpha, alpha, alpha, alpha );
    drawQuad( pass, textureID, position + bitmap_offset, bitmap_scale, color );
    drawQuad( pass, injuryID,  position + bitmap_offset, bitmap_scale, color );
    
    if( showDebug ) {
        drawCircleOutline( pass, position, DOGFISH_BITE_RADIUS, COLOR_RED );
        drawCircleOutline( pass, position, DOGFISH_LUNGE_RADIUS, COLOR_CYAN );
        drawCircleOutline( pass, position, DOGFISH_SIGHT_RADIUS_HI,  COLOR_MAGENTA );
        drawCircleOutline( pass, position, DOGFISH_SIGHT_RADIUS_MID, COLOR_MAGENTA );
        drawCircleOutline( pass, position, DOGFISH_SIGHT_RADIUS_LO,  COLOR_MAGENTA );
        
        vec2 dim          = Vec2( DOGFISH_BITE_DIM_X, DOGFISH_BITE_DIM_Y );
        vec2 bite_offset = Vec2( -( DOGFISH_BITE_OFFSET_X + dim.x * 0.5f ), DOGFISH_BITE_OFFSET_Y );
        if( facingLeft ) { bite_offset.x = -bite_offset.x; }
        rect bound  = rectCD( position + bite_offset, dim );
        drawRectOutline( pass, bound, COLOR_RED );
    }
    
    if( fish->state == fishState_chasing ) {
        fish->chase_value = 1.0f;
    }
    if( ( ( fish->state == fishState_idle ) || ( fish->state == fishState_chasing ) ) && ( fish->chase_value > 0.0f ) ) {
        vec2 outerDim = Vec2( 100.0f, 6.0f );
        vec2 innerDim = outerDim;
        innerDim.x *= fish->chase_value;
        
        if( fish->chase_type == fishType_null ) {
            rect bound0 = rectCD( fish->position + Vec2( 0.0f, 14.0f ), outerDim );
            drawRect( pass, textureID_whiteTexture, bound0, COLOR_BLACK );
            
            rect bound1 = rectBRD( getBR( bound0 ), innerDim );
            drawRect( pass, textureID_whiteTexture, bound1, COLOR_YELLOW );
        } else {
            vec2 p = getPosOfTarget( appState, fish->chase_type, fish->chase_index );
            vec2 v = getNormal( p - fish->position );
            
            vec2 P0 = fish->position + v * ( outerDim.x * 0.5f );
            vec2 P1 = fish->position + v * ( innerDim.x * 0.5f );
            vec2 xAxis0 = v * ( outerDim.x * 0.5f );
            vec2 xAxis1 = v * ( innerDim.x * 0.5f );
            vec2 yAxis  = getPerp( v ) * ( outerDim.y * 0.5f );
            
            drawQuad( pass, textureID_whiteTexture, P0, xAxis0, yAxis, COLOR_BLACK );
            drawQuad( pass, textureID_whiteTexture, P1, xAxis1, yAxis, COLOR_YELLOW );
        }
    }
    
    if( showDebug ) {
        vec2 dest = fish->position + fish->direction * 200.0f;
        drawRect( pass, textureID_whiteTexture, rectCD( dest, Vec2( 4.0f, 4.0f ) ), COLOR_CYAN );
        
        char str[ 16 ] = {};
        sprintf( str, "%d", fish->health );
        drawString( pass, appState->font, str, fish->position, textAlign_center, Vec2( 2.0f, 2.0f ), COLOR_BLACK );
        
    }
}

internal void
drawDogfish( RENDER_PASS * pass, APP_STATE * appState, boo32 showDebug = false ) {
    DOGFISH_STATE * state = &appState->dogfishState;
    
    for( uint32 iFish = 0; iFish < DOGFISH_MAX_COUNT; iFish++ ) {
        DOGFISH * fish = state->dogfish + iFish;
        if( fish->isActive ) {
            drawDogfish( pass, appState, fish, showDebug );
        }
    }
}

internal void
finalizeDogfish( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
    DOGFISH_STATE * state = &appState->dogfishState;
    PLAYER_STATE * playerState = &appState->playerState;
    
    for( uint32 iFish = 0; iFish < DOGFISH_MAX_COUNT; iFish++ ) {
        DOGFISH * fish = state->dogfish + iFish;
        if( fish->isActive ) {
            boo32 changeState = false;
            
            switch( fish->state ) {
                case fishState_idle_gameOver: {
                    // do nothing
                } break;
                
                case fishState_idle: {
                    if( fish->chase_value >= 1.0f ) {
                        changeState = true;
                        fish->state       = fishState_chasing;
                        fish->chase_value = 0.0f;
                    }
                } break;
                
                case fishState_chasing: {
                    vec2 p = getPosOfTarget( appState, fish->chase_type, fish->chase_index );
                    vec2 v = p - fish->position;
                    
                    flo32 radiusSq = DOGFISH_LUNGE_RADIUS * DOGFISH_LUNGE_RADIUS;
                    flo32 distSq   = getLengthSq( v );
                    if( distSq <= radiusSq ) {
                        fish->chase_nLunges++;
                        changeState = true;
                        fish->state      = fishState_lungePrep;
                        fish->direction  = getNormal( v );
                        fish->timer      = 0.0f;
                        fish->facingDir.transitionX  = fish->position.x;
                        fish->facingDir.isFacingLeft = ( v.x < 0.0f );
                        
                        AUDIO_ID audioID = ( AUDIO_ID )( ( rand() % 11 ) + audioID_dogfish_lunge01 );
                        playAudio( audioSystem, appState->audio, audioID, 0.6f );
                    }
                } break;
                
                case fishState_lungePrep: {
                    if( fish->timer >= DOGFISH_LUNGE_WARMUP_TARGET_TIME ) {
                        changeState = true;
                        fish->state      = fishState_lunging;
                        fish->velocity   = fish->direction * DOGFISH_LUNGE_SPEED;
                        fish->timer      = 0.0f;
                    }
                } break;
                
                case fishState_lunging: {
                    if( fish->timer >= DOGFISH_LUNGE_TARGET_TIME ) {
                        if( fish->chase_type == fishType_player ) {
                            playerState->nDodges++;
                        }
                        changeState = true;
                        if( ( fish->chase_ateTarget ) || ( fish->chase_nLunges >= 3 ) ) {
                            // TODO: there is a potential time gap between when the dogfish eats the goldfish and when it finishes its lunge, during which the player (or others) can interfere and change its state. remember to watch for any of these cases
                            fish->chase_ateTarget = false;
                            fish->chase_nLunges   = 0;
                            
                            setIdle( fish );
                        } else {
                            fish->state = fishState_chasing;
                        }
                    }
                } break;
                
                case fishState_biting: {
                    if( fish->timer >= DOGFISH_BITE_TARGET_TIME ) {
                        changeState = true;
                        fish->state = fishState_idle;
                        fish->timer = 0.0f;
                        
                        flo32 radians = RandomF32() * PI * 2.0f;
                        fish->direction  = Vec2( cosf( radians ), sinf( radians ) );
                        fish->targetTime = RandomF32InRange( DOGFISH_IDLE_TARGET_TIME_LO, DOGFISH_IDLE_TARGET_TIME_HI );
                        fish->speed      = RandomF32InRange( DOGFISH_IDLE_SPEED_LO, DOGFISH_IDLE_SPEED_HI );
                        
                        playerState->bitByDogfish = false;
                        playerState->dogfish      = 0;
                    }
                } break;
                
                case fishState_stunnedByEscape: {
                    if( fish->timer >= DOGFISH_ESCAPE_STUN_TARGET_TIME ) {
                        changeState = true;
                        if( !playerState->bitByDogfish ) {
                            fish->state = fishState_chasing;
                            fish->chase_nLunges = 0;
                            fish->chase_type    = fishType_player;
                        } else {
                            setIdle( fish );
                        }
                    }
                } break;
                
                case fishState_stunnedByBite: {
                    if( fish->timer >= DOGFISH_STUNNED_BY_BITE_TARGET_TIME ) {
                        changeState = true;
                        if( !playerState->bitByDogfish ) {
                            flip( &fish->facingDir, fish->position.x );
                            fish->state = fishState_chasing;
                            fish->chase_nLunges = 0;
                            fish->chase_type    = fishType_player;
                        } else {
                            setIdle( fish );
                        }
                    }
                } break;
                
                case fishState_dying: {
                    if( fish->timer >= DOGFISH_DYING_TARGET_TIME ) {
                        fish->isActive = false;
                    }
                } break;
            }
            
            if( ( appState->gameOver ) && ( changeState ) ) {
                fish->state = fishState_idle_gameOver;
                fish->timer = 0.0f;
            }
        }
    }
}

internal void
drawDebugDogfish( RENDER_PASS * pass, vec2 position, boo32 facingLeft ) {
    vec2 bitmap_scale  = Vec2( DOGFISH_BITMAP_SCALE_X, DOGFISH_BITMAP_SCALE_Y );
    vec2 bitmap_offset = Vec2( DOGFISH_BITMAP_OFFSET_X, DOGFISH_BITMAP_OFFSET_Y );
    
    if( facingLeft ) {
        bitmap_scale.x  = -bitmap_scale.x;
        bitmap_offset.x = -bitmap_offset.x;
    }
    
    drawQuad( pass, textureID_dogfish_idle, position + bitmap_offset, bitmap_scale, COLOR_WHITE );
    
    drawCircleOutline( pass, position, DOGFISH_BITE_RADIUS, COLOR_RED );
    drawCircleOutline( pass, position, DOGFISH_LUNGE_RADIUS, COLOR_CYAN );
    drawCircleOutline( pass, position, DOGFISH_SIGHT_RADIUS_HI,  COLOR_MAGENTA );
    drawCircleOutline( pass, position, DOGFISH_SIGHT_RADIUS_MID, COLOR_MAGENTA );
    drawCircleOutline( pass, position, DOGFISH_SIGHT_RADIUS_LO,  COLOR_MAGENTA );
    
    vec2 dim          = Vec2( DOGFISH_BITE_DIM_X, DOGFISH_BITE_DIM_Y );
    vec2 bite_offset = Vec2( -( DOGFISH_BITE_OFFSET_X + dim.x * 0.5f ), DOGFISH_BITE_OFFSET_Y );
    if( facingLeft ) { bite_offset.x = -bite_offset.x; }
    rect bound  = rectCD( position + bite_offset, dim );
    drawRectOutline( pass, bound, COLOR_RED );
}

internal void
spawnDogfish( APP_STATE * appState, uint32 nDogfish = 8 ) {
    DOGFISH_STATE * state = &appState->dogfishState;
    
    for( uint32 iFish = 0; iFish < nDogfish; iFish++ ) {
        vec2 spawn = RandomSpawnPos( appState->cameraBound, DOGFISH_SPAWN_MARGIN );
        
        DOGFISH fish = {};
        fish.isActive  = true;
        fish.state     = fishState_idle;
        fish.position  = spawn;
        fish.direction = getNormal( getCenter( appState->cameraBound ) - spawn );
        fish.speed     = DOGFISH_IDLE_SPEED_HI;
        
        fish.dir_targetTime = 1.0f;
        fish.speed_targetTime = 1.0f;
        
        fish.radians     = atan2f( fish.direction.y, fish.direction.x );
        if( fish.radians < 0.0f ) { fish.radians += ( PI * 2.0f ); }
        fish.targetRadians = fish.radians;
        
        fish.health = DOGFISH_HEALTH;
        
        addDogfish( state, fish );
    }
}
