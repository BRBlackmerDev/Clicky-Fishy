
internal orect
getBiteBound( SWORDFISH * fish ) {
    orect result = {};
    switch( fish->state ) {
        case fishState_stabbing: {
            vec2 dim    = Vec2( SWORDFISH_DODGE_DIM_X, SWORDFISH_DODGE_DIM_Y );
            vec2 offset = Vec2( SWORDFISH_DODGE_OFFSET_X, SWORDFISH_DODGE_OFFSET_Y );
            
            flo32 framesA = 14.0f / 60.0f;
            flo32 framesB =  4.0f / 60.0f;
            
            flo32 loA = 0.0f;
            flo32 hiA = loA + framesA;
            
            flo32 loB = hiA;
            flo32 hiB = loB + framesB;
            
            if( ( fish->timer >= loA ) && ( fish->timer < hiA ) ) {
                offset.x = -100.0f;
            }
            if( ( fish->timer >= loB ) && ( fish->timer < hiB ) ) {
                dim.x = 285.0f;
                dim.y = 52.0f;
                offset.x = -88.0f;
                offset.y = 4.0f;
            }
            
            flo32 radians   = 0.0f;
            vec2  rotOffset = {};
            if( fish->nAttacks == 1 ) {
                radians     =  PI / 12.0f;
                rotOffset.y =  14.0f;
            }
            if( fish->nAttacks == 2 ) {
                radians     = -PI / 12.0f;
                rotOffset.y = -18.0f;
            }
            
            boo32 facingLeft = fish->facingDir.isFacingLeft;
            if( facingLeft ) {
                offset.x = -offset.x;
                radians = -radians;
            }
            
            vec2 xAxis = Vec2( cosf( radians ), sinf( radians ) );
            vec2 yAxis = getPerp( xAxis );
            
            result.center  = fish->position + offset + rotOffset;
            result.halfDim = dim * 0.5f;
            result.xAxis   = xAxis;
            result.yAxis   = yAxis;
        } break;
        
        default: {
            vec2 dim    = Vec2( SWORDFISH_DODGE_DIM_X, SWORDFISH_DODGE_DIM_Y );
            vec2 offset = Vec2( SWORDFISH_DODGE_OFFSET_X, SWORDFISH_DODGE_OFFSET_Y );
            
            boo32 facingLeft = fish->facingDir.isFacingLeft;
            if( facingLeft ) {
                offset.x = -offset.x;
            }
            
            result.center  = fish->position + offset;
            result.halfDim = dim * 0.5f;
            result.xAxis   = Vec2( 1.0f, 0.0f );
            result.yAxis   = Vec2( 0.0f, 1.0f );
        } break;
    }
    
    return result;
}

internal orect
getLungeBound( SWORDFISH * fish ) {
    vec2 dim     = Vec2( SWORDFISH_LUNGE_COLLISION_DIM_X, SWORDFISH_LUNGE_COLLISION_DIM_Y );
    vec2 offsetA = Vec2( SWORDFISH_BITMAP_OFFSET_X, SWORDFISH_BITMAP_OFFSET_Y );
    vec2 offsetB = Vec2( SWORDFISH_LUNGE_COLLISION_OFFSET_X, SWORDFISH_LUNGE_COLLISION_OFFSET_Y );
    
    vec2 xAxis = fish->headDir;
    if( fish->facingDir.isFacingLeft ) {
        xAxis = -xAxis;
        offsetA.x = -offsetA.x;
        offsetB.x = -offsetB.x;
    }
    vec2 yAxis = getPerp( xAxis );
    
    orect result = {};
    result.center  = fish->position + offsetA + xAxis * offsetB.x + yAxis * offsetB.y;
    result.halfDim = dim * 0.5f;
    result.xAxis   = xAxis;
    result.yAxis   = yAxis;
    
    return result;
}

internal orect
getStabBound( SWORDFISH * fish ) {
    vec2 box_offset = Vec2( SWORDFISH_STAB_COLLISION_OFFSET_X, SWORDFISH_STAB_COLLISION_OFFSET_Y );
    vec2 box_dim = Vec2( SWORDFISH_STAB_COLLISION_DIM_X, SWORDFISH_STAB_COLLISION_DIM_Y );
    
    flo32 radians   = 0.0f;
    vec2  rotOffset = {};
    if( fish->nAttacks == 1 ) {
        radians     =  PI / 12.0f;
        rotOffset.y =  100.0f;
    }
    if( fish->nAttacks == 2 ) {
        radians     = -PI / 12.0f;
        rotOffset.y = -100.0f;
    }
    if( fish->facingDir.isFacingLeft ) {
        box_offset.x = -box_offset.x;
        radians      = -radians;
    }
    
    orect result = orectCD( fish->position + box_offset, box_dim );
    result.xAxis = Vec2( cosf( radians ), sinf( radians ) );
    result.yAxis = getPerp( result.xAxis );
    result.center += rotOffset;
    return result;
}

internal vec2
getStabHeadDir( SWORDFISH * fish ) {
    flo32 radians = 0.0f;
    if( fish->nAttacks == 1 ) {
        radians =  PI / 12.0f;
    }
    if( fish->nAttacks == 2 ) {
        radians = -PI / 12.0f;
    }
    vec2 headDir = Vec2( cosf( radians ), sinf( radians ) );
    if( fish->facingDir.isFacingLeft ) {
        headDir.x = -headDir.x;
    }
    return headDir;
}

internal orect
getChargeBound( SWORDFISH * fish ) {
    vec2 dim    = Vec2( SWORDFISH_CHARGE_COLLISION_DIM_X, SWORDFISH_CHARGE_COLLISION_DIM_Y );
    vec2 offset = Vec2( SWORDFISH_CHARGE_COLLISION_OFFSET_X, SWORDFISH_CHARGE_COLLISION_OFFSET_Y );
    
    boo32 facingLeft = fish->facingDir.isFacingLeft;
    if( facingLeft ) {
        offset.x = -offset.x;
    }
    
    orect result = {};
    result.center  = fish->position + offset;
    result.halfDim = dim * 0.5f;
    result.xAxis   = Vec2( 1.0f, 0.0f );
    result.yAxis   = Vec2( 0.0f, 1.0f );
    return result;
}

struct SWORDFISH_COLLISION_ANCHOR {
    vec2 position;  // tip of nose
    vec2 direction; // may not be normalized, includes scale
};

struct SWORDFISH_COLLISION {
    orect primary;
    orect secondary;
};

internal SWORDFISH_COLLISION_ANCHOR
getCollisionAnchor( SWORDFISH * fish ) {
    SWORDFISH_COLLISION_ANCHOR result = {};
    boo32 facingLeft = fish->facingDir.isFacingLeft;
    
    switch( fish->state ) {
        case fishState_stabbing: {
            SWORDFISH_STAB_STATS;
            
            if( ( fish->timer >= loC ) && ( fish->timer < hiC ) ) {
                vec2 bitmap_offset = Vec2( SWORDFISH_STAB_BITMAP_OFFSET_X, SWORDFISH_STAB_BITMAP_OFFSET_Y );
                
                vec2 offset = Vec2( 280.0f, 2.0f );
                if( facingLeft ) {
                    bitmap_offset.x = -bitmap_offset.x;
                }
                
                flo32 radians   = 0.0f;
                vec2  rotOffset = {};
                if( fish->nAttacks == 1 ) {
                    radians     =  rad;
                    rotOffset.y =  rad_offYB;
                }
                if( fish->nAttacks == 2 ) {
                    radians     = -rad;
                    rotOffset.y = -rad_offYB;
                }
                
                vec2 xAxis = Vec2( cosf( radians ), sinf( radians ) );
                if( facingLeft ) {
                    xAxis.x = -xAxis.x;
                }
                
                vec2 yAxis = getPerp( xAxis );
                if( facingLeft ) {
                    yAxis = -yAxis;
                }
                
                result.direction = xAxis;
                result.position  = fish->position + bitmap_offset + rotOffset + xAxis * offset.x + yAxis * offset.y;
            }
        } break;
        
        case fishState_idle:
        case fishState_dodging:
        case fishState_lunging: {
            vec2 xAxis = fish->headDir;
            vec2 yAxis = getPerp( xAxis );
            vec2 bitmap_offset = Vec2( SWORDFISH_BITMAP_OFFSET_X, SWORDFISH_BITMAP_OFFSET_Y );
            
            result.direction = fish->headDir;
            
            vec2 offset = Vec2( 220.0f, 2.0f );
            if( facingLeft ) {
                bitmap_offset.x = -bitmap_offset.x;
                yAxis = -yAxis;
            }
            
            result.position = fish->position + bitmap_offset + xAxis * offset.x + yAxis * offset.y;
        } break;
        
        //case fishState_charging: {
        default: {
            result.direction = Vec2( 1.0f, 0.0f );
            
            vec2 offset = Vec2( 190.0f, 8.0f );
            if( facingLeft ) {
                offset.x = -offset.x;
                result.direction.x = -1.0f;
            }
            
            result.position = fish->position + offset;
        } break;
    }
    return result;
}

internal SWORDFISH_COLLISION
getCollisionBound( SWORDFISH * fish ) {
    SWORDFISH_COLLISION_ANCHOR anchor = getCollisionAnchor( fish );
    
    vec2 xAxis = getNormal( anchor.direction );
    vec2 yAxis = getPerp( xAxis );
    
    vec2 P = anchor.position + xAxis * SWORDFISH_PRIMARY_COLLISION_OFFSET;
    vec2 dimA = Vec2( SWORDFISH_PRIMARY_COLLISION_DIM_X, SWORDFISH_PRIMARY_COLLISION_DIM_Y );
    vec2 halfDimA = dimA * 0.5f;
    
    orect primary = {};
    primary.center  = P - xAxis * halfDimA.x;
    primary.halfDim = halfDimA;
    primary.xAxis   = xAxis;
    primary.yAxis   = yAxis;
    
    vec2 Q = anchor.position + xAxis * SWORDFISH_SECONDARY_COLLISION_OFFSET;
    vec2 dimB = Vec2( SWORDFISH_SECONDARY_COLLISION_DIM_X, SWORDFISH_SECONDARY_COLLISION_DIM_Y );
    vec2 halfDimB = dimB * 0.5f;
    
    orect secondary = {};
    secondary.center  = Q - xAxis * halfDimB.x;
    secondary.halfDim = halfDimB;
    secondary.xAxis   = xAxis;
    secondary.yAxis   = yAxis;
    
    SWORDFISH_COLLISION result = {};
    result.primary   = primary;
    result.secondary = secondary;
    
    return result;
}

internal rect
getChargeExitBound( APP_STATE * appState ) {
    rect result = appState->cameraBound;
    result.left   -= 250.0f;
    result.right  += 250.0f;
    result.bottom +=  40.0f;
    result.top    -=  40.0f;
    return result;
}

internal void
parryLoSwordfish( SWORDFISH * fish, SWORDFISH_COLLISION coll, APP_STATE * appState, AUDIO_SYSTEM * audioSystem ) {
    PLAYER_STATE * playerState = &appState->playerState;
    
    vec2 xAxis = coll.secondary.xAxis;
    vec2 yAxis = coll.secondary.yAxis;
    
    vec2 V       = getNormal( playerState->position - coll.secondary.center );
    if( dot( V, yAxis ) < 0.0f ) { yAxis = -yAxis; }
    
    vec2  offset = Vec2( RandomF32() * 60.0f, 100.0f );
    vec2 bumpDir = getNormal( xAxis * offset.x + yAxis * offset.y );
    bumpDir = V;
    
    fish->velocity -= bumpDir * SWORDFISH_BUMP_SPEED;
    
    uint32 index = rand() % 3;
    AUDIO_ID audioID = ( AUDIO_ID )( audioID_swordfish_block01 + index );
    playAudio( audioSystem, appState->audio, audioID, 0.2f );
    
    addSmExplosion( &appState->explosionList, coll.secondary.center, playerState->position );
    
    playerState->timer = PLAYER_DEFEND_TARGET_TIME - PLAYER_PARRY_COOLDOWN_TIME;
}

internal void
updateCollisionWithSwordfish( SWORDFISH * fish, APP_STATE * appState, AUDIO_SYSTEM * audioSystem, vec2 swordfishDir, boo32 hiStun ) {
    PLAYER_STATE * playerState = &appState->playerState;
    
    if( fish->collision_isActive ) {
        SWORDFISH_COLLISION coll = getCollisionBound( fish );
        if( ( !playerState->isStunnedHi ) && ( !playerState->isStunnedLo ) ) {
            boo32 isInPrimary   = isInBound( playerState->position, coll.primary   );
            boo32 isInSecondary = isInBound( playerState->position, coll.secondary );
            
            if( isInPrimary ) {
                fish->collision_isActive = false;
                //if( playerState->parry_isActive ) {
                if( playerState->defend_isActive ) {
                    fish->initStun = true;
                    
                    vec2 xAxis = -swordfishDir;
                    vec2 yAxis = getPerp( xAxis );
                    
                    flo32 tY = RandomF32();
                    vec2  V = fish->position - playerState->position;
                    if( dot( V, yAxis ) < 0.0f ) {
                        tY = -tY;
                    }
                    vec2 dir = getNormal( xAxis + yAxis * tY );
                    
                    flo32 radians = toRadians( dir );
                    flo32 add     = RandomF32() * ( 4.0f * PI / 3.0f ) + ( PI / 3.0f );
                    fish->direction = toDirection2D( radians + add );
                    
                    if( hiStun ) {
                        fish->targetTime = SWORDFISH_STUNNED_HI_TARGET_TIME;
                        fish->velocity   = dir * SWORDFISH_STUNNED_HI_MAX_SPEED;
                        playAudio( audioSystem, appState->audio, audioID_swordfish_parry_hi, 0.5f );
                    } else {
                        fish->targetTime = SWORDFISH_STUNNED_LO_TARGET_TIME;
                        fish->velocity   = dir * SWORDFISH_STUNNED_LO_MAX_SPEED;
                        playAudio( audioSystem, appState->audio, audioID_swordfish_parry_lo, 0.5f );
                    }
                    
                    addExplosion( &appState->explosionList, fish->position, playerState->position );
                    
                    playerState->timer = PLAYER_DEFEND_TARGET_TIME - PLAYER_PARRY_COOLDOWN_TIME;
                } else {
                    vec2 headDir = getStabHeadDir( fish );
                    initPlayerStunHi( playerState, coll.primary, headDir );
                }
            } else if( isInSecondary ) {
                fish->collision_isActive = false;
                
                vec2 headDir = getStabHeadDir( fish );
                //if( playerState->parry_isActive ) {
                if( playerState->defend_isActive ) {
                    parryLoSwordfish( fish, coll, appState, audioSystem );
                } else {
                    playerState->initStunnedLo = true;
                    addSmExplosion( &appState->explosionList, playerState->position, coll.secondary.center );
                }
            }
        }
    }
}

internal void
getSlashCollision( SWORDFISH * fish, vec2 * vert_out ) {
    boo32 facingLeft = fish->facingDir.isFacingLeft;
    
    vec2 offsetA = Vec2( -110.0f, -460.0f );
    vec2 offsetB = Vec2(  200.0f,  730.0f );
    vec2 offsetC = Vec2(  300.0f,    0.0f );
    vec2 offsetD = Vec2(  140.0f,    0.0f );
    vec2 offsetE = Vec2(  130.0f,  -50.0f );
    vec2 offsetF = Vec2(   40.0f,    0.0f );
    vec2 offsetG = Vec2(  155.0f,   75.0f );
    vec2 offsetH = Vec2(   50.0f,   60.0f );
    if( facingLeft ) {
        offsetA.x = -offsetA.x;
        offsetB.x = -offsetB.x;
        offsetC.x = -offsetC.x;
        offsetD.x = -offsetD.x;
        offsetE.x = -offsetE.x;
        offsetF.x = -offsetF.x;
        offsetG.x = -offsetG.x;
        offsetH.x = -offsetH.x;
    }
    if( fish->slash_flip ) {
        offsetA.y = -offsetA.y;
        offsetB.y = -offsetB.y;
        offsetC.y = -offsetC.y;
        offsetD.y = -offsetD.y;
        offsetE.y = -offsetE.y;
        offsetF.y = -offsetF.y;
        offsetG.y = -offsetG.y;
        offsetH.y = -offsetH.y;
    }
    
    vec2 bPos = fish->slash_pos + offsetA;
    vec2 P = bPos;
    vec2 Q = bPos + offsetB;
    
    vec2 R = ( P + Q ) * 0.5f;
    vec2 S = R + offsetC;
    vec2 T = R + offsetD;
    
    vec2 U = ( P + T ) * 0.5f;
    vec2 V = U + offsetE;
    vec2 W = U + offsetF;
    
    vec2 X = ( T + Q ) * 0.5f;
    vec2 Y = X + offsetG;
    vec2 Z = X + offsetH;
    
    vert_out[ 0 ] = P;
    vert_out[ 1 ] = V;
    vert_out[ 2 ] = S;
    vert_out[ 3 ] = Y;
    vert_out[ 4 ] = Q;
    vert_out[ 5 ] = Z;
    vert_out[ 6 ] = T;
    vert_out[ 7 ] = W;
}

internal boo32
isInLineLoop( vec2 pos, vec2 * line, uint32 nLine ) {
    boo32 result = false;
    
    for( uint32 iLine = 0; iLine < nLine; iLine++ ) {
        uint32 indexA = iLine;
        uint32 indexB = ( iLine + 1 ) % nLine;
        
        vec2 U = line[ indexA ];
        vec2 V = line[ indexB ];
        flo32 denom = V.y - U.y;
        if( denom != 0.0f ) {
            flo32 t = ( pos.y - U.y ) / ( denom );
            // NOTE: not ( t <= 1.0f ), this would treat intersections with a vertex as two intersections
            if( ( t >= 0.0f ) && ( t < 1.0f ) ) {
                flo32 x = U.x + ( V.x - U.x ) * t;
                if( pos.x <= x ) {
                    result = !result;
                }
            }
        }
    }
    
    return result;
}

internal void
updateSwordfish( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, flo32 dt ) {
    SWORDFISH_STATE * state = &appState->swordfishState;
    PLAYER_STATE * playerState = &appState->playerState;
    FISH_ATE_LIST * list = &appState->fishAteList;
    
    // TODO: maybe tailwhipping the swordfish knocks it a little away from the player, but otherwise does nothing
    
    if( list->attemptBite ) {
        for( uint32 iFish = 0; iFish < SWORDFISH_MAX_COUNT; iFish++ ) {
            SWORDFISH * fish = state->swordfish + iFish;
            
            if( fish->isActive ) {
                
                boo32 canBite =( fish->state == fishState_stabbing ) || ( fish->state == fishState_chasing  ) ||( fish->state == fishState_stunned  ) ||( fish->state == fishState_resting  ) || ( fish->state == fishState_stunnedByBite );
                if( canBite ) {
                    orect bound = getBiteBound( fish );
                    if( isInBound( playerState->position, bound ) ) {
                        // swordfish is bit
                        // TODO: damage swordfish
                        // TODO: add bite to list for sound and scoring
                        // TODO: swordfish dodge and parry
                        
                        fish->lastBit_timer = 0.0f;
                        if( fish->state == fishState_stunned ) {
                            fish->transitionToStunnedByBite = true;
                        } else if( fish->state == fishState_stunnedByBite ) {
                            // do nothing
                        } else {
                            // TODO: make sure that all other possible states are valid for this condition
                            fish->state = fishState_stunnedByBite;
                            fish->timer = 0.0f;
                        }
                        
                        fish->health = maxValue( fish->health - 1, 0 );
                        
                        // TODO: tune this based on swordfish max health points
                        int32 rangeA_lo = 1;
                        int32 rangeA_hi = SWORDFISH_CHARGE_HEALTH_02;
                        int32 rangeB_lo = SWORDFISH_CHARGE_HEALTH_02 + 1;
                        int32 rangeB_hi = SWORDFISH_CHARGE_HEALTH_01;
                        if( ( fish->health >= rangeA_lo ) && ( fish->health <= rangeA_hi ) && !fish->didCharge[ 0 ] ) {
                            fish->didCharge[ 0 ] = true;
                            fish->doCharge = true;
                        }
                        if( ( fish->health >= rangeB_lo ) && ( fish->health <= rangeB_hi ) && !fish->didCharge[ 1 ] ) {
                            fish->didCharge[ 1 ] = true;
                            fish->doCharge = true;
                        }
                        
                        addBit( list, *fish );
                        
                        if( fish->health == 0 ) {
                            fish->state = fishState_dying;
                            fish->timer = 0.0f;
                            
                            // TODO: cancel any potential attacks or state changes
                            
                            addAte( list, *fish );
                        }
                    }
                }
            }
        }
    }
    
    for( uint32 iFish = 0; iFish < SWORDFISH_MAX_COUNT; iFish++ ) {
        SWORDFISH * fish = state->swordfish + iFish;
        if( fish->isActive ) {
            flo32 maxSpeed = SWORDFISH_MAX_SPEED;
            flo32 friction = SWORDFISH_FRICTION;
            
            fish->lastBit_timer += dt;
            if( fish->state != fishState_stabbing ) {
                fish->timer += dt;
            }
            
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
                
                case fishState_introAnimation: {
                    flo32 targetA = SWORDFISH_INTRO_ANIMATION_WAIT;
                    flo32 targetB = SWORDFISH_INTRO_ANIMATION_BACKGROUND;
                    flo32 targetC = SWORDFISH_INTRO_ANIMATION_TARGET_TIME;
                    
                    friction = SWORDFISH_INTRO_ANIMATION_FRICTION;
                    if( ( fish->timer >= targetA ) && ( fish->timer < targetB ) ) {
                        maxSpeed = SWORDFISH_INTRO_ANIMATION_SPEED_01;
                        
                        dir = Vec2( -1.0f, 0.0f );
                        fish->velocity   = dir * maxSpeed;
                        fish->position.y = appState->cameraBound.top - 380.0f;
                    }
                    if( fish->timer >= targetB ) {
                        rect bound = getChargeExitBound( appState );
                        fish->position.x = maxValue( bound.left, fish->position.x );
                        maxSpeed = SWORDFISH_INTRO_ANIMATION_SPEED_02;
                        
                        dir = Vec2( 1.0f, 0.0f );
                        fish->velocity   = dir * maxSpeed;
                        fish->position.y = appState->cameraBound.top - 390.0f;
                        
                        fish->facingDir.isFacingLeft = false;
                    }
                } break;
                
                case fishState_spawning: {
                    flo32 targetA = SWORDFISH_SPAWN_END_MOVEMENT_TARGET_TIME;
                    flo32 targetB = SWORDFISH_SPAWN_BEGIN_ANIMATION_TARGET_TIME;
                    flo32 targetC = SWORDFISH_SPAWN_END_ANIMATION_TARGET_TIME;
                    flo32 targetD = SWORDFISH_SPAWN_TARGET_TIME;
                    
                    friction = SWORDFISH_SPAWN_FRICTION;
                    maxSpeed = SWORDFISH_SPAWN_SPEED;
                    if( ( fish->timer >= 0.0f ) && ( fish->timer < targetA ) ) {
                        dir = fish->direction;
                    }
                } break;
                
                case fishState_dying: {
                    // do nothing
                } break;
                
                case fishState_idle: {
                    //rect bound = getChargeExitBound( appState );
                    
                    //fish->state    = fishState_charging;
                    //fish->nAttacks = 0;
                    //fish->position = Vec2( bound.left + 1.0f, RandomF32InRange( bound.bottom, bound.top ) );
                    //fish->velocity  = {};
                    //fish->direction = Vec2( 1.0f, 0.0f );
                    
                    //fish->state = fishState_stabbing;
                    //fish->timer = 0.0f;
                    //fish->position = getCenter( bound );
                    //fish->velocity = {};
                    //fish->headDir = getNormal( playerState->position - fish->position );
                    
                    //fish->state = fishState_exiting;
                    //fish->timer = 0.0f;
                    //fish->position = getCenter( bound );
                    //fish->velocity = {};
                    
                    //vec2 dest = getTL( bound );
                    //fish->direction = getNormal( dest - fish->position );
                } break;
                
                case fishState_resting: {
                    friction = SWORDFISH_REST_FRICTION;
                    dir      = Vec2( 0.0f, -1.0f );
                    maxSpeed = SWORDFISH_REST_SPEED;
                } break;
                
                case fishState_chasing: {
                    vec2 offset = Vec2( SWORDFISH_STAB_INIT_OFFSET_X, SWORDFISH_STAB_INIT_OFFSET_Y );
                    
                    boo32 facingLeft = fish->facingDir.isFacingLeft;
                    if( facingLeft ) {
                        offset.x = -offset.x;
                    }
                    
                    vec2 b = fish->position + offset;
                    vec2 v = playerState->position - b;
                    
                    dir = getNormal( v );
                } break;
                
                case fishState_stabbing: {
                    friction = 12.0f;
                    
                    SWORDFISH_STAB_STATS;
                    
                    if( ( fish->timer >= loA ) && ( fish->timer < hiA ) ) {
                        fish->doStabAudio = true;
                        //scale.x  = SWORDFISH_STAB_WARMUP_BITMAP_SCALE_X;
                        //scale.y  = SWORDFISH_STAB_WARMUP_BITMAP_SCALE_Y;
                        //offset.x = SWORDFISH_STAB_WARMUP_BITMAP_OFFSET_X;
                        //offset.y = SWORDFISH_STAB_WARMUP_BITMAP_OFFSET_Y;
                    } else if( ( fish->timer >= loB ) && ( fish->timer < hiB ) ) {
                        flo32 target = lerp( loB, 0.3f, hiB );
                        if( fish->timer > target ) {
                            if( fish->doStabAudio ) {
                                fish->doStabAudio = false;
                                
                                AUDIO_ID audioID = audioID_swordfish_stab01;
                                if( fish->nAttacks == 1 ) {
                                    audioID = audioID_swordfish_stab02;
                                }
                                playAudio( audioSystem, appState->audio, audioID, 0.25f );
                            }
                        }
                    } else if( ( fish->timer >= loC ) && ( fish->timer < hiC ) ) {
                        if( fish->initCollision ) {
                            fish->initCollision      = false;
                            fish->collision_isActive = true;
                        }
                        
                        vec2 headDir = getStabHeadDir( fish );
                        updateCollisionWithSwordfish( fish, appState, audioSystem, headDir, false );
                    }
                } break;
                
                case fishState_exiting: {
                    dir = fish->direction;
                    maxSpeed = SWORDFISH_EXIT_SPEED;
                    friction = SWORDFISH_EXIT_FRICTION;
                } break;
                
                case fishState_charging: {
                    fish->toggleTimer += dt;
                    flo32 targetTime = 0.08f;
                    if( fish->toggleTimer >= targetTime ) {
                        fish->toggleTimer -= targetTime;
                        fish->toggleBackground = !fish->toggleBackground;
                    }
                    if( fish->timer >= SWORDFISH_CHARGE_INIT_DELAY ) {
                        dir = fish->direction;
                        maxSpeed = SWORDFISH_CHARGE_SPEED;
                        friction = SWORDFISH_CHARGE_FRICTION;
                        fish->velocity = fish->direction * SWORDFISH_CHARGE_SPEED;
                        
                        vec2 headDir = Vec2( 1.0f, 0.0f );
                        if( fish->facingDir.isFacingLeft ) {
                            headDir.x = -1.0f;
                        }
                        updateCollisionWithSwordfish( fish, appState, audioSystem, headDir, true );
                    }
                } break;
                
                case fishState_stunned: {
                    friction = SWORDFISH_STUNNED_FRICTION;
                    maxSpeed = SWORDFISH_STUNNED_DRIFT_SPEED;
                    dir      = fish->direction;
                } break;
                
                case fishState_stunnedByBite: {
                    friction = SWORDFISH_STUNNED_FRICTION;
                    // do nothing
                } break;
                
                case fishState_dodging: {
                    friction = SWORDFISH_DODGE_FRICTION;
                } break;
                
                case fishState_lunging: {
                    friction = SWORDFISH_LUNGE_FRICTION;
                    
                    updateCollisionWithSwordfish( fish, appState, audioSystem, fish->headDir, true );
                } break;
                
                case fishState_slashPrep: {
                    dir = getNormal( fish->destination - fish->position );
                    maxSpeed = SWORDFISH_SLASH_PREP_SPEED;
                    friction = SWORDFISH_SLASH_PREP_FRICTION;
                } break;
                
                case fishState_slashWarmup: {
                    friction = SWORDFISH_SLASH_WARMUP_FRICTION;
                } break;
                
                case fishState_slashing: {
                    friction = SWORDFISH_SLASH_FRICTION;
                    
                    if( ( fish->collision_isActive ) && ( !playerState->isStunnedHi ) && ( !playerState->isStunnedLo ) ) {
                        SWORDFISH_COLLISION coll = getCollisionBound( fish );
                        
                        vec2 vertex[ 8 ] = {};
                        getSlashCollision( fish, vertex );
                        boo32 isInCollision = isInLineLoop( playerState->position, vertex, 8 );
                        if( isInCollision ) {
                            fish->collision_isActive = false;
                            
                            if( playerState->defend_isActive ) {
                                parryLoSwordfish( fish, coll, appState, audioSystem );
                            } else {
                                playerState->initStunnedLo = true;
                                addSmExplosion( &appState->explosionList, playerState->position, coll.secondary.center );
                            }
                        } else if( isInBound( playerState->position, coll.secondary ) ) {
                            fish->collision_isActive = false;
                            
                            if( playerState->defend_isActive ) {
                                parryLoSwordfish( fish, coll, appState, audioSystem );
                            } else {
                                playerState->initStunnedLo = true;
                                addSmExplosion( &appState->explosionList, playerState->position, coll.secondary.center );
                            }
                        }
                    }
                } break;
            }
            
            vec2 accel = ( dir * maxSpeed - fish->velocity ) * friction;
            vec2 dPos  = accel * ( dt * dt * 0.5f ) + fish->velocity * dt;
            fish->position += dPos;
            fish->velocity += accel * dt;
        }
    }
    
    for( uint32 iFish = 0; iFish < SWORDFISH_MAX_COUNT; iFish++ ) {
        SWORDFISH * fish = state->swordfish + iFish;
        if( fish->isActive ) {
            // TODO: make sure that this covers all potential swordfish states. we don't want him flipping when not wanted
            
            FACING_DIRECTION * dir = &fish->facingDir;
            flo32 xPos = fish->position.x;
            if( dir->isFacingLeft ) {
                dir->transitionX = minValue( dir->transitionX, xPos );
            } else {
                dir->transitionX = maxValue( dir->transitionX, xPos );
            }
            
            flo32 xPlayer = playerState->position.x;
            switch( fish->state ) {
                case fishState_idle:
                case fishState_dodging: {
                    if( xPlayer > xPos ) {
                        dir->isFacingLeft = false;
                    }
                    if( xPlayer < xPos ) {
                        dir->isFacingLeft = true;
                    }
                    // TODO: lock direction for a temporary pause at the start of the dodge
                } break;
                
                case fishState_idle_gameOver:
                case fishState_slashPrep:
                case fishState_exiting: {
                    flo32 threshold = 20.0f;
                    if( dir->isFacingLeft ) {
                        if( ( xPos - dir->transitionX ) > threshold ) {
                            dir->isFacingLeft = false;
                            dir->transitionX  = xPos;
                        }
                    } else {
                        if( ( dir->transitionX - xPos ) > threshold ) {
                            dir->isFacingLeft = true;
                            dir->transitionX  = xPos;
                        }
                    }
                } break;
                
                case fishState_chasing: {
                    flo32 threshold = 20.0f;
                    if( dir->isFacingLeft ) {
                        boo32 canFlip = ( xPlayer - xPos ) > SWORDFISH_CHASE_OFFSET_FOR_FACING_DIR;
                        
                        if( ( canFlip ) && ( ( xPos - dir->transitionX ) > threshold ) ) {
                            dir->isFacingLeft = false;
                            dir->transitionX  = xPos;
                        }
                    } else {
                        boo32 canFlip = ( xPos - xPlayer ) > SWORDFISH_CHASE_OFFSET_FOR_FACING_DIR;
                        
                        if( ( canFlip ) && ( ( dir->transitionX - xPos ) > threshold ) ) {
                            dir->isFacingLeft = true;
                            dir->transitionX  = xPos;
                        }
                    }
                } break;
                
                default: {
                    // cannot flip, do nothing
                } break;
            }
        }
    }
    
    // set headDir
    for( uint32 iFish = 0; iFish < SWORDFISH_MAX_COUNT; iFish++ ) {
        SWORDFISH * fish = state->swordfish + iFish;
        if( fish->isActive ) {
            if( fish->state == fishState_dodging ) {
                fish->headDir = getNormal( playerState->position - fish->position );
            }
        }
    }
}

internal void
drawSwordfish( RENDER_PASS * pass, APP_STATE * appState, SWORDFISH * fish, boo32 showDebug = false ) {
    boo32 facingLeft = fish->facingDir.isFacingLeft;
    
    vec2 bit_offset = {};
    if( fish->lastBit_timer <= SWORDFISH_LAST_BIT_TARGET_TIME ) {
        bit_offset = RandomDirection2D() * ( RandomF32() * SWORDFISH_DYING_BITMAP_OFFSET );
    }
    
    switch( fish->state ) {
        case fishState_stabbing: {
            vec2 warmup_scale = Vec2( SWORDFISH_STAB_WARMUP_BITMAP_SCALE_X, SWORDFISH_STAB_WARMUP_BITMAP_SCALE_Y );
            vec2 warmup_offset = Vec2( SWORDFISH_STAB_WARMUP_BITMAP_OFFSET_X, SWORDFISH_STAB_WARMUP_BITMAP_OFFSET_Y );
            vec2 stab_scale = Vec2( SWORDFISH_STAB_BITMAP_SCALE_X, SWORDFISH_STAB_BITMAP_SCALE_Y );
            vec2 stab_offset = Vec2( SWORDFISH_STAB_BITMAP_OFFSET_X, SWORDFISH_STAB_BITMAP_OFFSET_Y );
            
            SWORDFISH_STAB_STATS;
            
            vec2 scale  = {};
            vec2 offset = {};
            
            flo32 r = 1.0f;
            flo32 alphaA = 0.0f;
            flo32 alphaB = 0.0f;
            flo32 rad_offY = 0.0f;
            
            boo32 isStabbing = false;
            if( ( fish->timer >= 0.0f ) && ( fish->timer < hiA ) ) {
                flo32 t = clamp01( ( fish->timer - loA ) / ( hiA - loA ) );
                flo32 s = sinf( t * PI * 0.5f );
                r = lerp( 1.0f, t, 0.7f );
                
                rad_offY = rad_offYA;
                
                flo32 xScale  = lerp( 1.1f,  s, 1.0f );
                flo32 yScale  = lerp( 0.95f, s, 1.0f );
                flo32 xOffset = lerp( 0.83f, s, 1.0f );
                flo32 yOffset = lerp( 0.91f, s, 1.0f );
                scale  = warmup_scale  * Vec2( xScale,  yScale  );
                offset = warmup_offset * Vec2( xOffset, yOffset );
            } else if( ( fish->timer >= loB ) && ( fish->timer < hiB ) ) {
                r = 0.7f;
                
                flo32 t = 0.0f;
                flo32 target = lerp( loB, 0.5f, hiB );
                if( fish->timer > target ) {
                    t = clamp01( ( fish->timer - target ) / ( hiB - target ) );
                    //t = clamp01( ( fish->timer - loB ) / ( hiB - loB ) );
                    
                    alphaA = t;
                    alphaB = t;
                    
                    t = t * t * t * t;
                }
                scale  = lerp( warmup_scale,  t, stab_scale  );
                offset = lerp( warmup_offset, t, stab_offset );
                rad_offY = lerp( rad_offYA, t, rad_offYB );
                //scale  = lerp( warmup_scale,  t, warmup_scale  );
                //offset = lerp( warmup_offset, t, warmup_offset );
            } else if( ( fish->timer >= loC ) && ( fish->timer < hiC ) ) {
                alphaA = 1.0f;
                alphaB = 1.0f;
                
                rad_offY = rad_offYB;
                isStabbing = true;
                flo32 t = clamp01( ( fish->timer - loC ) / ( hiC - loC ) );
                flo32 s = sinf( t * ( 2.0f / 3.0f ) * PI );
                
                flo32 xScale  = lerp( 1.0f, s, 1.02f );
                flo32 yScale  = lerp( 1.0f, s, 1.0f );
                flo32 xOffset = lerp( 1.0f, s, 1.0f );
                flo32 yOffset = lerp( 1.0f, s, 1.0f );
                scale  = stab_scale  * Vec2( xScale,  yScale  );
                offset = stab_offset * Vec2( xOffset, yOffset );
            }
            
            if( facingLeft ) {
                scale.x  = -scale.x;
                offset.x = -offset.x;
            }
            
            flo32 radians   = 0.0f;
            vec2  rotOffset = {};
            if( fish->nAttacks == 1 ) {
                radians     =  rad;
                rotOffset.y =  rad_offY;
            }
            if( fish->nAttacks == 2 ) {
                radians     = -rad;
                rotOffset.y = -rad_offY;
            }
            if( facingLeft ) {
                radians = -radians;
            }
            vec2 xAxis = Vec2( cosf( radians ), sinf( radians ) );
            vec2 yAxis = getPerp( xAxis );
            
            xAxis *= scale.x;
            yAxis *= scale.y;
            
            vec2 pos = fish->position + offset + rotOffset;
            
            vec4 color = Vec4( r, 1.0f, 1.0f, 1.0f );
            //color = modAlpha( color, alphaA );
            
            drawQuad( pass, textureID_swordfish_stabbing_background, pos, xAxis, yAxis, Vec4( alphaB ) );
            drawQuad( pass, textureID_swordfish_idle, pos, xAxis, yAxis, modAlpha( color, 1.0f - alphaA ) );
            drawQuad( pass, textureID_swordfish_stabbing, pos, xAxis, yAxis, Vec4( alphaA ) );
            
            if( showDebug ) {
                if( ( fish->timer >= loB ) && ( fish->timer < hiB ) ) {
                    drawORectOutline( pass, getStabBound( fish ), COLOR_RED );
                }
            }
        } break;
        
        case fishState_resting: {
            vec2 scale  = Vec2( SWORDFISH_BITMAP_SCALE_X,  SWORDFISH_BITMAP_SCALE_Y  );
            vec2 offset = Vec2( SWORDFISH_BITMAP_OFFSET_X, SWORDFISH_BITMAP_OFFSET_Y );
            if( facingLeft ) {
                scale.x  = -scale.x;
                offset.x = -offset.x;
            }
            drawQuad( pass, textureID_swordfish_angry, fish->position + offset, scale, COLOR_WHITE );
        } break;
        
        case fishState_slashPrep: {
            vec2 scale  = Vec2( SWORDFISH_BITMAP_SCALE_X, SWORDFISH_BITMAP_SCALE_Y );
            vec2 offset = Vec2( SWORDFISH_BITMAP_OFFSET_X, SWORDFISH_BITMAP_OFFSET_Y );
            if( facingLeft ) {
                scale.x  = -scale.x;
                offset.x = -offset.x;
            }
            drawQuad( pass, textureID_swordfish_angry, fish->position + offset, scale, COLOR_WHITE );
            
            if( showDebug ) {
                rect bound = rectCD( fish->destination, Vec2( 8.0f, 8.0f ) );
                drawRect( pass, textureID_whiteTexture, bound, COLOR_ORANGE );
            }
        } break;
        
        case fishState_slashWarmup: {
            TEXTURE_ID textureID = textureID_swordfish_slashing_up;
            if( fish->slash_flip ) {
                textureID = textureID_swordfish_slashing_down;
            }
            
            vec2 scale  = Vec2( SWORDFISH_BITMAP_SCALE_X, SWORDFISH_BITMAP_SCALE_Y );
            vec2 offset = Vec2( SWORDFISH_BITMAP_OFFSET_X, SWORDFISH_BITMAP_OFFSET_Y );
            if( facingLeft ) {
                scale.x  = -scale.x;
                offset.x = -offset.x;
            }
            drawQuad( pass, textureID, fish->position + offset, scale, COLOR_WHITE );
            
            if( showDebug ) {
                rect bound = rectCD( fish->destination, Vec2( 8.0f, 8.0f ) );
                drawRect( pass, textureID_whiteTexture, bound, COLOR_ORANGE );
            }
        } break;
        
        case fishState_slashing: {
            vec2 slash_scaleA  = Vec2( 800.0f, 1600.0f ) * 0.5f;
            vec2 slash_scaleB  = Vec2( 830.0f, 1640.0f ) * 0.5f;
            vec2 slash_offsetA = Vec2( 0.0f,  -100.0f );
            vec2 slash_offsetB = Vec2( 20.0f, -100.0f );
            
            flo32 tA = clamp01( fish->timer / SWORDFISH_SLASH_TARGET_TIME );
            vec2 slash_scale  = lerp( slash_scaleA,  tA, slash_scaleB  );
            vec2 slash_offset = lerp( slash_offsetA, tA, slash_offsetB );
            flo32 alpha = 1.0f;
            
            flo32 targetA = 0.1f;
            flo32 targetB = 0.7f;
            if( tA < targetA ) {
                alpha = tA / targetA;
            }
            if( tA > targetB ) {
                alpha = 1.0f - ( ( tA - targetB ) / ( 1.0f - targetB ) );
            }
            
            flo32 targetC = 0.25f;
            TEXTURE_ID textureID = textureID_swordfish_stabbing;
            if( tA < targetC ) {
                textureID = textureID_swordfish_slashing_down;
                if( fish->slash_flip ) {
                    textureID = textureID_swordfish_slashing_up;
                }
            } else {
                textureID = textureID_swordfish_slashing_up;
                if( fish->slash_flip ) {
                    textureID = textureID_swordfish_slashing_down;
                }
            }
            
            if( facingLeft ) {
                slash_offset.x = -slash_offset.x;
                slash_scale.x  = -slash_scale.x;
            }
            if( fish->slash_flip ) {
                slash_offset.y = -slash_offset.y;
                slash_scale.y  = -slash_scale.y;
            }
            vec2 slash_pos = fish->slash_pos + slash_offset;
            drawQuad( pass, textureID_swordfish_slash, slash_pos, slash_scale, Vec4( alpha ) );
            
            flo32 tB = clamp01( -squareValue( 3.0f * tA - 1.0f ) + 1.0f );
            
            vec2 scaleA = Vec2( 280.0f, 140.0f );
            vec2 scaleB = Vec2( 460.0f, 120.0f );
            vec2 scale  = lerp( scaleA, tB, scaleB );
            
            vec2 offsetA = Vec2( -26.0f, 0.0f );
            vec2 offsetB = Vec2(  56.0f, 0.0f );
            
            vec2 offset = Vec2( SWORDFISH_BITMAP_OFFSET_X, SWORDFISH_BITMAP_OFFSET_Y ) + lerp( offsetA, tB, offsetB );
            if( facingLeft ) {
                scale.x  = -scale.x;
                offset.x = -offset.x;
            }
            
            drawQuad( pass, textureID, fish->position + offset, scale, COLOR_WHITE );
            //drawQuad( pass, textureID, getCenter( appState->cameraBound ) + offset, scale, COLOR_WHITE );
            
            if( showDebug ) {
                vec2 vertex[ 8 ] = {};
                getSlashCollision( fish, vertex );
                uint32 nVertex = 8;
                
                for( uint32 iVert = 0; iVert < nVertex; iVert++ ) {
                    uint32 indexA = iVert;
                    uint32 indexB = ( iVert + 1 ) % nVertex;
                    
                    drawLine( pass, vertex[ indexA ], vertex[ indexB ], COLOR_BLUE );
                }
            }
        } break;
        
        case fishState_dodging:
        case fishState_lunging: {
            vec2 scale  = Vec2( SWORDFISH_BITMAP_SCALE_X, SWORDFISH_BITMAP_SCALE_Y );
            vec2 offset = Vec2( SWORDFISH_BITMAP_OFFSET_X, SWORDFISH_BITMAP_OFFSET_Y );
            if( facingLeft ) {
                scale.x  = -scale.x;
                offset.x = -offset.x;
            }
            
            vec2 xAxis = fish->headDir;
            if( facingLeft ) {
                xAxis = -xAxis;
            }
            vec2 yAxis = getPerp( xAxis );
            xAxis *= scale.x;
            yAxis *= scale.y;
            
            if ( fish->state == fishState_lunging ) {
                flo32 a = 1.0f - clamp01( fish->timer / 0.4f );
                drawQuad( pass, textureID_swordfish_stabbing_background, fish->position + offset, xAxis, yAxis, Vec4( a ) );
            }
            drawQuad( pass, textureID_swordfish_tail, fish->position + offset, scale, COLOR_WHITE );
            drawQuad( pass, textureID_swordfish_head, fish->position + offset, xAxis, yAxis, COLOR_WHITE );
            
            if( showDebug ) {
                orect bound = getLungeBound( fish );
                drawORectOutline( pass, bound, COLOR_GREEN );
            }
            
            drawPoint( pass, fish->position + offset, COLOR_GREEN );
        } break;
        
        case fishState_dying: {
            vec2 scale  = Vec2( SWORDFISH_BITMAP_SCALE_X, SWORDFISH_BITMAP_SCALE_Y );
            vec2 offset = Vec2( SWORDFISH_BITMAP_OFFSET_X, SWORDFISH_BITMAP_OFFSET_Y );
            if( facingLeft ) {
                scale.x  = -scale.x;
                offset.x = -offset.x;
            }
            
            vec2  bitmap_offset = RandomDirection2D() * ( RandomF32() * SWORDFISH_DYING_BITMAP_OFFSET );
            
            SWORDFISH_DYING_STATS;
            
            flo32 alpha = 1.0f;
            if( fish->timer >= targetB ) {
                alpha = 1.0f - clamp01( ( fish->timer - targetB ) / ( targetC - targetB ) );
            }
            
            TEXTURE_ID textureID = textureID_swordfish_stunnedByBite;
            if( fish->timer >= targetA ) {
                textureID = textureID_swordfish_dying;
                scale *= Vec2( 1.05f, 1.1f );
            }
            vec4 color = Vec4( alpha, alpha, alpha, alpha );
            drawQuad( pass, textureID, fish->position + offset + bitmap_offset, scale, color );
        } break;
        
        case fishState_introAnimation: {
            flo32 targetA = SWORDFISH_INTRO_ANIMATION_WAIT;
            flo32 targetB = SWORDFISH_INTRO_ANIMATION_BACKGROUND;
            flo32 targetC = SWORDFISH_INTRO_ANIMATION_TARGET_TIME;
            
            vec2 scale  = Vec2( SWORDFISH_BITMAP_SCALE_X,  SWORDFISH_BITMAP_SCALE_Y  );
            vec2 offset = Vec2( SWORDFISH_BITMAP_OFFSET_X, SWORDFISH_BITMAP_OFFSET_Y );
            if( facingLeft ) {
                scale.x  = -scale.x;
                offset.x = -offset.x;
            }
            
            vec4 color = COLOR_BLACK;
            if( fish->timer < targetB ) {
                scale  *= 0.35f;
                offset *= 0.35f;
                color = COLOR_BLACK;
                // draw small shadow at position
            } else {
                scale  *= 0.80f;
                scale  *= 0.80f;
                color = COLOR_GRAY( 0.1f );
                // draw larger shadow at position
            }
            
            drawQuad( pass, textureID_swordfish_idle, fish->position + offset, scale, color );
        } break;
        
        case fishState_spawning: {
            flo32 targetA = SWORDFISH_SPAWN_END_MOVEMENT_TARGET_TIME;
            flo32 targetB = SWORDFISH_SPAWN_BEGIN_ANIMATION_TARGET_TIME;
            flo32 targetC = SWORDFISH_SPAWN_END_ANIMATION_TARGET_TIME;
            flo32 targetD = SWORDFISH_SPAWN_TARGET_TIME;
            
            TEXTURE_ID textureID = textureID_swordfish_idle;
            
            vec2 bitmap_offset = {};
            vec2 scale  = Vec2( SWORDFISH_BITMAP_SCALE_X, SWORDFISH_BITMAP_SCALE_Y );
            if( ( fish->timer >= targetA ) && ( fish->timer < targetB ) ) {
                flo32 tA = targetB - ( 48.0f / 60.0f );
                flo32 tB = targetB - ( 42.0f / 60.0f );
                if( ( fish->timer >= tA ) && ( fish->timer < tB ) ) {
                    textureID = textureID_swordfish_blinkTransition;
                }
                if( fish->timer >= tB ) {
                    textureID = textureID_swordfish_stunnedByBite;
                    flo32 t = clamp01( ( fish->timer - tB ) / ( targetB - tB ) );
                    bitmap_offset += Vec2( 7.0f, 2.0f ) * t * 0.5f;
                    bitmap_offset += Vec2( -4.0f, -6.0f );
                    
                    scale *= lerp( 1.0f, t, 1.01f );
                }
            }
            if( ( fish->timer >= targetB ) && ( fish->timer < targetC ) ) {
                flo32 t = clamp01( ( fish->timer - targetB ) / ( targetC - targetB ) );
                flo32 dist = lerp( SWORDFISH_STUNNED_BY_BITE_BITMAP_OFFSET_LO, t, SWORDFISH_STUNNED_BY_BITE_BITMAP_OFFSET_HI );
                bitmap_offset = RandomDirection2D() * ( RandomF32() * dist );
                textureID = textureID_swordfish_angry; 
            }
            
            vec2 offset = Vec2( SWORDFISH_BITMAP_OFFSET_X, SWORDFISH_BITMAP_OFFSET_Y );
            if( facingLeft ) {
                scale.x  = -scale.x;
                offset.x = -offset.x;
            }
            drawQuad( pass, textureID, fish->position + offset + bitmap_offset, scale, COLOR_WHITE );
        } break;
        
        case fishState_idle:
        case fishState_charging: {
            vec2 scale  = Vec2( SWORDFISH_BITMAP_SCALE_X, SWORDFISH_BITMAP_SCALE_Y );
            vec2 offset = Vec2( SWORDFISH_BITMAP_OFFSET_X, SWORDFISH_BITMAP_OFFSET_Y );
            if( facingLeft ) {
                scale.x  = -scale.x;
                offset.x = -offset.x;
            }
            
            vec2 pos = fish->position + offset;
            
            vec2 background_scale  = scale;
            vec2 background_offset = offset;
            if( fish->toggleBackground ) {
                background_scale.y  = -background_scale.y;
                //background_offset.y = -background_offset.y;
            }
            
            drawQuad( pass, textureID_swordfish_stabbing_background, fish->position + background_offset, background_scale, COLOR_WHITE );
            drawQuad( pass, textureID_swordfish_stabbing, fish->position + offset, scale, COLOR_WHITE );
            
            if( showDebug ) {
                orect bound = getChargeBound( fish );
                drawORectOutline( pass, bound, COLOR_RED );
            }
        } break;
        
        case fishState_stunnedByBite: {
            vec2 scale  = Vec2( SWORDFISH_BITMAP_SCALE_X, SWORDFISH_BITMAP_SCALE_Y );
            vec2 offset = Vec2( SWORDFISH_BITMAP_OFFSET_X, SWORDFISH_BITMAP_OFFSET_Y );
            if( facingLeft ) {
                scale.x  = -scale.x;
                offset.x = -offset.x;
            }
            drawQuad( pass, textureID_swordfish_stunnedByBite, fish->position + offset + bit_offset, scale, COLOR_WHITE );
        } break;
        
        case fishState_stunned: {
            vec2 exp_scale  = Vec2( 300.0f, 300.0f );
            vec2 exp_offset = Vec2( 200.0f, 0.0f );
            
            vec2 scale  = Vec2( SWORDFISH_BITMAP_SCALE_X, SWORDFISH_BITMAP_SCALE_Y );
            vec2 offset = Vec2( SWORDFISH_BITMAP_OFFSET_X, SWORDFISH_BITMAP_OFFSET_Y );
            if( facingLeft ) {
                scale.x  = -scale.x;
                offset.x = -offset.x;
                
                exp_offset.x = -exp_offset.x;
            }
            
            TEXTURE_ID textureID = textureID_swordfish_stunned_lo;
            if( fish->timer <= SWORDFISH_STUNNED_HIT_FRAME_TARGET_TIME ) {
                textureID = textureID_swordfish_stunned_hitFrame;
                
                //TEXTURE_ID explosionID = ( TEXTURE_ID )( textureID_explosion01 + fish->explosion_index );
                //drawQuad( pass, explosionID, fish->position + exp_offset, exp_scale, COLOR_WHITE );
            }
            
            vec2 bitmap_offset = {};
            flo32 tA = fish->targetTime - ( 4.0f / 60.0f );
            if( fish->timer >= tA ) {
                textureID = textureID_swordfish_blinkTransition;
                bitmap_offset += Vec2( 0.0f, 2.0f );
            }
            drawQuad( pass, textureID, fish->position + offset + bit_offset + bitmap_offset, scale, COLOR_WHITE );
        } break;
        
        case fishState_idle_gameOver:
        case fishState_exiting:
        case fishState_chasing: {
            vec2 scale  = Vec2( SWORDFISH_BITMAP_SCALE_X, SWORDFISH_BITMAP_SCALE_Y );
            vec2 offset = Vec2( SWORDFISH_BITMAP_OFFSET_X, SWORDFISH_BITMAP_OFFSET_Y );
            if( facingLeft ) {
                scale.x  = -scale.x;
                offset.x = -offset.x;
            }
            drawQuad( pass, textureID_swordfish_idle, fish->position + offset + bit_offset, scale, COLOR_WHITE );
        } break;
    }
    
    if( showDebug ) { // bite/dodge bound
        drawORectOutline( pass, getBiteBound( fish ), COLOR_MAGENTA );
        drawCircleOutline( pass, fish->position, SWORDFISH_LUNGE_RADIUS, COLOR_MAGENTA );
        
        if( fish->state == fishState_chasing ) {
            vec2 stab_offset = Vec2( SWORDFISH_STAB_INIT_OFFSET_X, SWORDFISH_STAB_INIT_OFFSET_Y );
            flo32 chase_offset = -SWORDFISH_CHASE_OFFSET_FOR_FACING_DIR;
            
            if( facingLeft ) {
                stab_offset.x = -stab_offset.x;
                chase_offset  = -chase_offset;
            }
            
            drawCircleOutline( pass, fish->position + stab_offset, SWORDFISH_STAB_INIT_RADIUS, COLOR_RED );
            
            vec2 P = fish->position + Vec2( chase_offset, -100.0f );
            vec2 Q = fish->position + Vec2( chase_offset,  100.0f );
            drawLine( pass, P, Q, COLOR_BLUE );
            
            rect R = rectCD( fish->position, Vec2( 4.0f, 4.0f ) );
            drawRect( pass, textureID_whiteTexture, R, COLOR_CYAN );
        }
        
        if( fish->collision_isActive ) {
            SWORDFISH_COLLISION coll = getCollisionBound( fish );
            drawORectOutline( pass, coll.primary, COLOR_GREEN );
            drawORectOutline( pass, coll.secondary, COLOR_RED );
            
            SWORDFISH_COLLISION_ANCHOR anchor = getCollisionAnchor( fish );
            rect R = rectCD( anchor.position, Vec2( 4.0f, 4.0f ) );
            drawRect( pass, textureID_whiteTexture, R, COLOR_RED );
        }
    }
    
    { // draw health bar
        // TODO: SWORDFISH_STATE is set up to handle multiple swordfish, but I only really expect a single swordfish as a boss fight, so need a way to turn off rendering the swordfish health bar if I ever consider multiple swordfish as regular enemies.
        
        rect playBound = appState->cameraBound;
        
        vec2 dim = Vec2( getWidth( playBound ) * 0.5f, 20.0f );
        vec2 pos = getBC( playBound ) + Vec2( 0.0f, 40.0f );
        rect bound_inner = rectBCD( pos, dim );
        rect bound_outer = addRadius( bound_inner, 3.0f );
        
        flo32 t = clamp01( ( flo32 )fish->health / ( flo32 )SWORDFISH_MAX_HEALTH );
        rect bound = rectTLD( getTL( bound_inner ), Vec2( dim.x * t, dim.y ) );
        
        drawRect( pass, textureID_whiteTexture, bound_outer, COLOR_BLACK );
        drawRect( pass, textureID_whiteTexture, bound, COLOR_RED );
        
        const char * str = "SWORDFISH";
        vec2 string_dim = getDim( appState->font, str );
        string_dim += Vec2( 8.0f, 4.0f );
        rect string_bound = rectBLD( getTL( bound_outer ), string_dim );
        drawRect( pass, textureID_whiteTexture, string_bound, COLOR_BLACK );
        drawString( pass, appState->font, "SWORDFISH", getCenter( string_bound ), textAlign_center, COLOR_WHITE );
    }
}

internal void
drawSwordfish( RENDER_PASS * pass, APP_STATE * appState, boo32 showDebug = false ) {
    SWORDFISH_STATE * state = &appState->swordfishState;
    PLAYER_STATE * playerState = &appState->playerState;
    
    for( uint32 iFish = 0; iFish < SWORDFISH_MAX_COUNT; iFish++ ) {
        SWORDFISH * fish = state->swordfish + iFish;
        if( fish->isActive ) {
            drawSwordfish( pass, appState, fish, showDebug );
        }
    }
}

internal void
initSlash( SWORDFISH * fish, APP_STATE * appState ) {
    fish->state       = fishState_slashPrep;
    fish->timer       = 0.0f;
    
    PLAYER_STATE * playerState = &appState->playerState;
    vec2 playerPos = playerState->position;
    
    vec2 offset = Vec2( 400.0f, 300.0f );
    vec2 P[ 4 ] = {
        playerPos + Vec2( -offset.x, -offset.y ),
        playerPos + Vec2(  offset.x, -offset.y ),
        playerPos + Vec2( -offset.x,  offset.y ),
        playerPos + Vec2(  offset.x,  offset.y ),
    };
    vec2 Pend[ 4 ] = { P[ 3 ], P[ 2 ], P[ 1 ], P[ 0 ] };
    
    rect bound = appState->cameraBound;
    //bound = addRadius( bound, -100.0f );
    
    vec2    Q   [ 4 ] = {};
    vec2    Qend[ 4 ] = {};
    uint32 nQ         = 0;
    for( uint32 iter = 0; iter < 4; iter++ ) {
        if( isInBound( P[ iter ], bound ) ) {
            Q   [ nQ ] = P   [ iter ];
            Qend[ nQ ] = Pend[ iter ];
            nQ++;
        }
    }
    Assert( nQ > 0 );
    
    uint32 index = 0;
    if( nQ > 1 ) {
        int32 i = -1;
        flo32 maxDistSq = 1000000000.0f;
        flo32 minDistSq = SWORDFISH_SLASH_PREP_MIN_DIST * SWORDFISH_SLASH_PREP_MIN_DIST;
        for( uint32 iter = 0; iter < nQ; iter++ ) {
            flo32 distSq = getLengthSq( fish->position - Q[ iter ] );
            if( ( distSq > minDistSq ) && ( distSq < maxDistSq ) ) {
                i         = iter;
                maxDistSq = distSq;
            }
        }
        Assert( i != -1 );
        index = i;
    }
    
    vec2 dest = Q   [ index ];
    vec2 end  = Qend[ index ];
    
    if( end.x < bound.left   ) { dest.x += RandomF32InRange( 150.0f, 250.0f ); }
    if( end.x > bound.right  ) { dest.x -= RandomF32InRange( 150.0f, 250.0f ); }
    if( end.y < bound.bottom ) { dest.y += RandomF32InRange( 150.0f, 250.0f ); }
    if( end.y > bound.top    ) { dest.y -= RandomF32InRange( 150.0f, 250.0f ); }
    fish->destination = dest;
    
    // should choose ideal position, then move, then face player
    // this causes a problem if the player moves while the swordfish is moving
    // slash should not exit the screen
    // should move to the closest ideal slash position
    // if already at slash position, then should move to a different position
}

internal void
initSlashArt( SWORDFISH * fish ) {
    flo32 dist = distToStop( fish->velocity, SWORDFISH_SLASH_FRICTION, 60.0f );
    vec2  dest = destAtStop( fish->position, fish->velocity, SWORDFISH_SLASH_FRICTION, 60.0f );
    vec2 offset = dest - fish->position;
    
    fish->slash_flip = ( fish->velocity.y < 0.0f );
    fish->slash_pos  = ( fish->position + dest ) * 0.5f;
}

internal void
finalizeSwordfish( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, flo32 dt ) {
    SWORDFISH_STATE * state = &appState->swordfishState;
    PLAYER_STATE * playerState = &appState->playerState;
    
    for( uint32 iFish = 0; iFish < SWORDFISH_MAX_COUNT; iFish++ ) {
        SWORDFISH * fish = state->swordfish + iFish;
        
        if( fish->isActive ) {
            boo32 changeState = false;
            
            if( fish->initStun ) {
                changeState = true;
                fish->initStun = false;
                fish->state    = fishState_stunned;
                fish->timer    = 0.0f;
                fish->nAttacks = 0;
            } else {
                switch( fish->state ) {
                    case fishState_introAnimation: {
                        if( fish->timer >= SWORDFISH_INTRO_ANIMATION_TARGET_TIME ) {
                            rect bound = getChargeExitBound( appState );
                            
                            changeState = true;
                            fish->state = fishState_spawning;
                            fish->timer = 0.0f;
                            
                            fish->position  = getTR( bound ) + Vec2( 0.0f, -300.0f );
                            fish->direction = getNormal( getCenter( bound ) - fish->position );
                            fish->velocity  = fish->direction * SWORDFISH_SPAWN_SPEED;
                            
                            fish->facingDir.isFacingLeft = true;
                            fish->facingDir.transitionX  = fish->position.x;
                        }
                    } break;
                    
                    case fishState_spawning: {
                        if( fish->timer >= SWORDFISH_SPAWN_TARGET_TIME ) {
                            changeState = true;
                            fish->state = fishState_chasing;
                            fish->timer = 0.0f;
                        }
                    } break;
                    
                    case fishState_dying: {
                        SWORDFISH_DYING_STATS;
                        if( fish->timer >= targetD ) {
                            fish->isActive = false;
                        }
                    } break;
                    
                    case fishState_exiting: {
                        rect bound = getChargeExitBound( appState );
                        rect exitBound = bound;
                        exitBound.top  = appState->cameraBound.top + 250.0f;
                        if( !isInBound( fish->position, exitBound ) ) {
                            changeState = true;
                            fish->state = fishState_charging;
                            fish->timer = 0.0f;
                            fish->nAttacks = 0;
                            fish->collision_isActive = true;
                            
                            int32 rangeA_lo = 1;
                            int32 rangeA_hi = SWORDFISH_CHARGE_HEALTH_02;
                            int32 rangeB_lo = SWORDFISH_CHARGE_HEALTH_02 + 1;
                            int32 rangeB_hi = SWORDFISH_CHARGE_HEALTH_01;
                            if( ( fish->health >= rangeA_lo ) && ( fish->health <= rangeA_hi ) && !fish->didCharge[ 0 ] ) {
                                fish->didCharge[ 0 ] = true;
                            }
                            if( ( fish->health >= rangeB_lo ) && ( fish->health <= rangeB_hi ) && !fish->didCharge[ 1 ] ) {
                                fish->didCharge[ 1 ] = true;
                            }
                            
                            boo32 enterRight = false;
                            if( ( fish->position.x > bound.left ) && ( fish->position.x < bound.right ) ) {
                                enterRight = ( rand() % 2 ) == 1;
                            } else if( fish->position.x >= bound.right ) {
                                enterRight = true;
                            }
                            
                            if( enterRight ) {
                                fish->direction = Vec2( -1.0f, 0.0f );
                                fish->position  = Vec2( bound.right - 1.0f, RandomF32InRange( bound.bottom, bound.top ) );
                                fish->facingDir.isFacingLeft = true;
                                fish->facingDir.transitionX  = fish->position.x + 20.0f;
                            } else {
                                fish->direction = Vec2( 1.0f, 0.0f );
                                fish->position  = Vec2( bound.left + 1.0f, RandomF32InRange( bound.bottom, bound.top ) );
                                fish->facingDir.isFacingLeft = false;
                                fish->facingDir.transitionX  = fish->position.x - 20.0f;
                            }
                            
                            boo32 targetPlayer = ( ( rand() % 2 ) == 1 );
                            fish->charge_targetPlayer[ 0 ] =  targetPlayer;
                            fish->charge_targetPlayer[ 1 ] = !targetPlayer;
                            
                            // TODO: allow swordfish to exit left, right or top
                            // TODO: exiting top randomly chooses a side to charge from
                        }
                    } break;
                    
                    case fishState_charging: {
                        rect bound = getChargeExitBound( appState );
                        uint32 maxCharge = 6;
                        boo32 isOutsideBound_left  = ( fish->direction.x < 0.0f ) && ( fish->position.x < bound.left );
                        boo32 isOutsideBound_right = ( fish->direction.x > 0.0f ) && ( fish->position.x > bound.right );
                        
                        if( ( isOutsideBound_left ) || ( isOutsideBound_right ) ) {
                            fish->nAttacks++;
                            if( fish->nAttacks < maxCharge ) {
                                fish->collision_isActive = true;
                                if( ( fish->nAttacks % 2 ) == 0 ) {
                                    boo32 targetPlayer = ( ( rand() % 2 ) == 1 );
                                    fish->charge_targetPlayer[ 0 ] =  targetPlayer;
                                    fish->charge_targetPlayer[ 1 ] = !targetPlayer;
                                }
                                
                                flo32 yPos = playerState->position.y;
                                flo32 offset = ( RandomF32() * 2.0f - 1.0f ) * SWORDFISH_CHARGE_POSITION_OFFSET_Y;
                                
                                boo32 targetPlayer = fish->charge_targetPlayer[ fish->nAttacks % 2 ];
                                if( !targetPlayer ) {
                                    yPos = RandomF32InRange( bound.bottom, bound.top );
                                    
                                    flo32 destY = yPos + offset;
                                    if( ( destY <= bound.bottom ) || ( destY >= bound.top ) ) {
                                        offset = -offset;
                                    }
                                    yPos += offset;
                                }
                                
                                if( isOutsideBound_left ) {
                                    fish->direction = Vec2( 1.0f, 0.0f );
                                    fish->position  = Vec2( bound.left + 1.0f, yPos );
                                    fish->facingDir.isFacingLeft = false;
                                }
                                if( isOutsideBound_right ) {
                                    fish->direction = Vec2( -1.0f, 0.0f );
                                    fish->position  = Vec2( bound.right - 1.0f, yPos );
                                    fish->facingDir.isFacingLeft = true;
                                }
                            } else {
                                changeState = true;
                                fish->state    = fishState_chasing;
                                fish->velocity = {};
                                
                                fish->initSlash_nAttacks       = 0;
                                fish->initSlash_nTargetAttacks = RandomU32InRange( 2, 4 );
                            }
                        }
                    } break;
                    
                    case fishState_stabbing: {
                        fish->timer += dt;
                        
                        SWORDFISH_STAB_STATS;
                        if( fish->timer >= max ) {
                            fish->timer -= max;
                            fish->nAttacks++;
                            fish->initCollision = true;
                            
                            if( fish->nAttacks >= 3 ) {
                                changeState = true;
                                fish->state = fishState_resting;
                                fish->timer = 0.0f;
                            }
                        }
                    } break;
                    
                    case fishState_resting: {
                        if( fish->timer >= SWORDFISH_REST_TARGET_TIME ) {
                            changeState = true;
                            if( fish->initSlash_nAttacks >= fish->initSlash_nTargetAttacks ) {
                                initSlash( fish, appState );
                            } else {
                                fish->state = fishState_chasing;
                                fish->timer = 0.0f;
                            }
                        }
                    } break;
                    
                    case fishState_stunnedByBite: {
                        if( fish->timer >= SWORDFISH_STUNNED_BY_BITE_TARGET_TIME ) {
                            if( fish->doCharge ) {
                                fish->doCharge = false;
                                changeState = true;
                                fish->state = fishState_exiting;
                                fish->timer = 0.0f;
                                
                                rect playBound = appState->cameraBound;
                                rect exitBound = getChargeExitBound( appState );
                                // 0: exit top
                                // 1: exit left
                                // 2: exit right
                                uint32 iSide = 0;
                                flo32  dist  = playBound.top - fish->position.y;
                                if( ( fish->position.x - playBound.left ) < dist ) {
                                    iSide = 1;
                                    dist  = fish->position.x - playBound.left;
                                }
                                if( ( playBound.right - fish->position.x ) < dist ) {
                                    iSide = 2;
                                    dist  = playBound.right - fish->position.x;
                                }
                                
                                vec2  dest   = {};
                                flo32 offset = ( RandomF32() * 2.0f - 1.0f ) * SWORDFISH_EXIT_OFFSET;
                                switch( iSide ) {
                                    case 0: {
                                        dest = Vec2( fish->position.x, exitBound.top );
                                        if( ( ( dest.x + offset ) <= exitBound.left ) || ( ( dest.x + offset ) >= exitBound.right ) ) {
                                            offset = -offset;
                                        }
                                        dest.x += offset;
                                        
                                    } break;
                                    
                                    case 1:
                                    case 2: {
                                        dest.x = ( iSide == 1 ) ? exitBound.left : exitBound.right;
                                        dest.y = fish->position.y;
                                        
                                        if( ( ( dest.y + offset ) <= exitBound.bottom ) ||
                                           ( ( dest.y + offset ) >= exitBound.top ) ) {
                                            offset = -offset;
                                        }
                                        dest.y += offset;
                                    } break;
                                }
                                fish->direction = getNormal( dest - fish->position );
                            } else {
                                changeState = true;
                                if( fish->initSlash_nAttacks >= fish->initSlash_nTargetAttacks ) {
                                    initSlash( fish, appState );
                                } else {
                                    flo32 radiusSq = SWORDFISH_DODGE_RADIUS * SWORDFISH_DODGE_RADIUS;
                                    flo32 distSq   = getLengthSq( playerState->position - fish->position );
                                    if( distSq <= radiusSq ) {
                                        // TODO: only dodge if player is still within a certain range
                                        rect bound = appState->cameraBound;
                                        
                                        vec2 dir  = getNormal( fish->position - playerState->position );
                                        vec2 dest = fish->position + dir * SWORDFISH_DODGE_DISTANCE;
                                        
                                        if( ( dest.x <= bound.left ) || ( dest.x >= bound.right ) ) {
                                            dir.x = -dir.x;
                                        }
                                        if( ( dest.y <= bound.bottom ) || ( dest.y >= bound.top ) ) {
                                            dir.y = -dir.y;
                                        }
                                        
                                        fish->state = fishState_dodging;
                                        fish->timer      = 0.0f;
                                        fish->direction  = dir;
                                        fish->velocity   = dir * SWORDFISH_DODGE_MAX_SPEED;
                                    } else {
                                        fish->state = fishState_chasing;
                                    }
                                }
                            }
                        }
                    } break;
                    
                    case fishState_stunned: {
                        if( fish->timer >= fish->targetTime ) {
                            changeState = true;
                            if( fish->transitionToStunnedByBite ) {
                                fish->transitionToStunnedByBite = false;
                                fish->state = fishState_stunnedByBite;
                                fish->timer = 0.0f;
                            } else {
                                if( fish->initSlash_nAttacks >= fish->initSlash_nTargetAttacks ) {
                                    initSlash( fish, appState );
                                } else {
                                    fish->state = fishState_chasing;
                                }
                            }
                        }
                    } break;
                    
                    case fishState_dodging: {
                        if( fish->timer >= SWORDFISH_DODGE_TARGET_TIME ) {
                            // TODO: set swordfish to be facing the player
                            fish->timer = 0.0f;
                            
                            vec2 V   = playerState->position - fish->position;
                            vec2 dir = getNormal( V );
                            
                            flo32 rangeSq = SWORDFISH_LUNGE_RADIUS * SWORDFISH_LUNGE_RADIUS;
                            flo32 distSq  = getLengthSq( V );
                            if( distSq <= rangeSq ) {
                                changeState = true;
                                fish->state = fishState_lunging;
                                fish->direction = dir;
                                fish->velocity  = dir * SWORDFISH_LUNGE_SPEED;
                                fish->headDir   = dir;
                                fish->collision_isActive = true;
                                if( !fish->initSlash_dontCountAttack ) {
                                    fish->initSlash_nAttacks++;
                                } else {
                                    // Attack was already counted during the _stabbing state. Don't count twice!
                                }
                                fish->initSlash_dontCountAttack = false;
                            } else {
                                changeState = true;
                                fish->state = fishState_chasing;
                            }
                        }
                    } break;
                    
                    case fishState_slashPrep: {
                        flo32 distSq   = getLengthSq( fish->position - fish->destination );
                        flo32 radiusSq = SWORDFISH_SLASH_RADIUS * SWORDFISH_SLASH_RADIUS;
                        if( distSq <= radiusSq ) {
                            changeState = true;
                            fish->state = fishState_slashWarmup;
                            fish->timer = 0.0f;
                            
                            vec2 center = playerState->position;
                            fish->facingDir.isFacingLeft = false;
                            fish->slash_flip = true;
                            
                            vec2 dir = getNormal( Vec2( 0.35f, 1.0f ) );
                            if( fish->position.x > center.x ) {
                                dir.x = -dir.x;
                                fish->facingDir.isFacingLeft = true;
                            }
                            if( fish->position.y > center.y ) {
                                dir.y = -dir.y;
                                fish->slash_flip = false;
                            }
                            
                            fish->direction = dir;
                        }
                    } break;
                    
                    case fishState_slashWarmup: {
                        if( fish->timer >= SWORDFISH_SLASH_WARMUP_TARGET_TIME ) {
                            changeState = true;
                            fish->state = fishState_slashing;
                            fish->timer = 0.0f;
                            fish->nAttacks = 0;
                            
                            fish->velocity = fish->direction * SWORDFISH_SLASH_SPEED;
                            fish->collision_isActive = true;
                            
                            playAudio( audioSystem, appState->audio, audioID_swordfish_slash01, 0.25f );
                            
                            initSlashArt( fish );
                        }
                    } break;
                    
                    case fishState_slashing: {
                        if( fish->timer >= SWORDFISH_SLASH_TARGET_TIME ) {
                            fish->timer = 0.0f;
                            
                            uint32 maxAttacks = 3;
                            fish->nAttacks++;
                            if( fish->nAttacks < maxAttacks ) {
                                fish->direction.y = -fish->direction.y;
                                fish->velocity = fish->direction * SWORDFISH_SLASH_SPEED;
                                fish->collision_isActive = true;
                                
                                AUDIO_ID audioID = audioID_swordfish_slash01;
                                if( fish->nAttacks == 1 ) {
                                    audioID = audioID_swordfish_slash02;
                                }
                                playAudio( audioSystem, appState->audio, audioID, 0.25f );
                                
                                initSlashArt( fish );
                            } else {
                                changeState = true;
                                fish->state = fishState_chasing;
                                fish->timer = 0.0f;
                                
                                fish->initSlash_nAttacks       = 0;
                                fish->initSlash_nTargetAttacks = RandomU32InRange( 2, 4 );
                            }
                        }
                    } break;
                    
                    case fishState_lunging: {
                        if( fish->timer >= SWORDFISH_LUNGE_TARGET_TIME ) {
                            changeState = true;
                            fish->state = fishState_resting;
                            fish->timer = 0.0f;
                        }
                    } break;
                    
                    case fishState_chasing: {
                        vec2 offset = Vec2( SWORDFISH_STAB_INIT_OFFSET_X, SWORDFISH_STAB_INIT_OFFSET_Y );
                        
                        boo32 facingLeft = fish->facingDir.isFacingLeft;
                        if( facingLeft ) {
                            offset.x = -offset.x;
                        }
                        
                        vec2 b = fish->position + offset;
                        vec2 v = playerState->position - b;
                        
                        flo32 radiusSq = SWORDFISH_STAB_INIT_RADIUS * SWORDFISH_STAB_INIT_RADIUS;
                        flo32 distSq   = getLengthSq( v );
                        if( distSq <= radiusSq ) {
                            changeState = true;
                            fish->state = fishState_stabbing;
                            fish->timer    = 0.0f;
                            fish->nAttacks = 0;
                            fish->initCollision = true;
                            fish->initSlash_nAttacks++;
                            fish->initSlash_dontCountAttack = true;
                            // This flag prevents the attack chain: _stabbing -> _dodging -> _lunging from being counted as two attacks. This should only be counted as a single attack.
                        } else if( fish->timer >= SWORDFISH_INIT_SLASH_TARGET_TIME ) {
                            changeState = true;
                            initSlash( fish, appState );
                        }
                    } break;
                }
            }
            
            if( ( appState->gameOver ) && ( changeState ) ) {
                fish->state = fishState_idle_gameOver;
                fish->timer = 0.0f;
            }
        }
    }
}

internal void
addSwordfish( SWORDFISH_STATE * state, SWORDFISH swordfish ) {
    Assert( state->nSwordfish < SWORDFISH_MAX_COUNT );
    
    for( uint32 index = 0; index < SWORDFISH_MAX_COUNT; index++ ) {
        SWORDFISH fish = state->swordfish[ index ];
        if( !fish.isActive ) {
            state->swordfish[ index ] = swordfish;
            state->nSwordfish++;
            break;
        }
    }
}

internal void
spawnSwordfish( APP_STATE * appState, uint32 nSwordfish ) {
    SWORDFISH_STATE * state = &appState->swordfishState;
    
    rect bound = getChargeExitBound( appState );
    for( uint32 iter = 0; iter < nSwordfish; iter++ ) {
        SWORDFISH fish = {};
        fish.isActive = true;
        fish.health   = SWORDFISH_MAX_HEALTH;
        fish.initSlash_nTargetAttacks = 3;
        fish.initSlash_nTargetAttacks = 1;
        
        //fish.state = fishState_spawning;
        //fish.position  = getTR( bound );
        //fish.direction = getNormal( getCenter( bound ) - fish.position );
        //fish.velocity  = fish.direction * SWORDFISH_SPAWN_SPEED;
        
        //fish.facingDir.isFacingLeft = true;
        //fish.facingDir.transitionX  = fish.position.x;
        
        //fish.state = fishState_chasing;
#if 0
        fish.state = fishState_idle;
        fish.position = getCenter( bound );
#endif
        
#if 0
        fish.state = fishState_stabbing;
        fish.position = getCenter( bound );
        fish.initCollision = true;
#endif
        
#if 0
        vec2 dir = getNormal( Vec2( 0.35f, 1.0f ) );
        
        fish.state = fishState_slashing;
        fish.timer = 0.0f;
        fish.nAttacks = 0;
        
        fish.position  = Vec2( 200.0f, 200.0f );
        fish.direction = dir;
        fish.velocity  = fish.direction * SWORDFISH_SLASH_SPEED;
        fish.collision_isActive = true;
        
        initSlashArt( &fish );
#endif
        
#if 0
        fish.state = fishState_spawning;
        fish.position  = getTR( bound ) + Vec2( 0.0f, -300.0f );
        fish.direction = getNormal( getCenter( bound ) - fish.position );
        fish.velocity  = fish.direction * SWORDFISH_SPAWN_SPEED;
        fish.facingDir.isFacingLeft = true;
        fish.facingDir.transitionX  = fish.position.x;
#endif
        
#if 1
        fish.state = fishState_introAnimation;
        fish.position.x = bound.right;
        fish.facingDir.isFacingLeft = true;
        fish.facingDir.transitionX  = fish.position.x;
#endif
        
        addSwordfish( state, fish );
    }
}

#if 0
internal void
captureSwordfishDebug( APP_STATE * appState ) {
    DEBUG__SWORDFISH * debug = &appState->debugState.swordfish;
    
    if( !debug->show ) {
        SWORDFISH_STATE * swordfishState = &appState->swordfishState;
        SWORDFISH       * swordfish      = swordfishState->swordfish;
        debug->swordfish[ debug->atIndex ] = *swordfish;
        
        PLAYER_STATE * playerState = &appState->playerState;
        debug->player[ debug->atIndex ] = *playerState;
        
        debug->atIndex = ( debug->atIndex + 1 ) % DEBUG__SWORDFISH__FRAME_COUNT;
    }
}

internal void
drawSwordfishDebug( RENDER_PASS * pass, APP_STATE * appState, MOUSE_STATE * mouse, KEYBOARD_STATE * keyboard ) {
    DEBUG__SWORDFISH * debug = &appState->debugState.swordfish;
    
    DISPLAY_VALUE( uint32, debug->atIndex );
    
    if( debug->show ) {
        drawRect( pass, textureID_whiteTexture, appState->cameraBound, COLOR_WHITE );
        
        if( wasPressed( mouse, mouseButton_right ) ) {
            debug->pause = !debug->pause;
        }
        
        if( !debug->pause ) {
            flo32 t = clamp01( mouse->position.x / appState->app_dim.x );
            debug->show_index = ( ( uint32 )( t * ( flo32 )DEBUG__SWORDFISH__FRAME_COUNT ) + debug->atIndex ) % DEBUG__SWORDFISH__FRAME_COUNT;
            DISPLAY_VALUE( uint32, debug->show_index );
        }
        
        uint32 index = debug->show_index;
        
        SWORDFISH fish = debug->swordfish[ index ];
        DISPLAY_VALUE( vec2, fish.position );
        
        drawSwordfish( pass, appState, debug->swordfish[ index ], true, mouse );
        drawPlayer( pass, appState, debug->player + index, true );
        
        rect bound = rectCD( mouse->position, Vec2( 4.0f, 4.0f ) );
        drawRect( pass, textureID_whiteTexture, bound, COLOR_GREEN );
    }
}
#endif