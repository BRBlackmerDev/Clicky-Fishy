
internal circ
getBubbletCollision( PLAYER_STATE * player ) {
    vec2 offset = PLAYER__BUBBLET_HIT_COLLISION_OFFSET;
    
    if( player->facingDir.isFacingLeft ) {
        offset.x = -offset.x;
    }
    
    circ result = {};
    result.center = player->position + offset;
    result.radius = PLAYER__BUBBLET_HIT_COLLISION_RADIUS;
    return result;
}

internal circ
getBubblonCollision( PLAYER_STATE * player ) {
    vec2 offset = PLAYER__BUBBLON_HIT_COLLISION_OFFSET;
    
    if( player->facingDir.isFacingLeft ) {
        offset.x = -offset.x;
    }
    
    circ result = {};
    result.center = player->position + offset;
    result.radius = PLAYER__BUBBLON_HIT_COLLISION_RADIUS;
    return result;
}

internal vec2
getSlapPos( PLAYER_STATE * player ) {
    vec2 offset = PLAYER_BITMAP_OFFSET;
    if( player->facingDir.isFacingLeft ) {
        offset.x = -offset.x;
    }
    
    vec2 result = player->position + offset;
    return result;
}

internal vec2
getHealPos( PLAYER_STATE * player ) {
    vec2 offset = PLAYER_BITMAP_OFFSET;
    if( player->facingDir.isFacingLeft ) {
        offset.x = -offset.x;
    }
    
    vec2 result = player->position + offset;
    return result;
}

internal void
updatePlayerInput( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, MOUSE_STATE * mouse ) {
    PLAYER_STATE * player = &appState->playerState;
    
    player->doSlap = false;
    
    // TODO: NOTE: This expects camera_dim and play_dim to have the same scale 1:1. This will need to change when we introduce camera scaling.
    player->dPos = mouse->position - appState->mouse_base;
    
    if( wasPressed( mouse, mouseButton_left ) ) {
        // NOTE: bite is not bound to a timer, meaning that the player can potentially input damage every frame (if they are able to click the mouse every frame)
        boo32 isValid = ( !player->bitByDogfish ) && ( !player->defend_isActive ) && ( !player->isStunnedHi  ) && ( !player->isStunnedLo     ) && ( !player->useItem_isActive ) && ( !player->heldByBubble );
        if( isValid ) {
            appState->fishAteList.attemptBite = true;
            player->bite_isActive = true;
            player->timer = 0.0f;
            
            player->nDodges     = 0;
            player->dodge_timer = 0.0f;
            
            //DEBUG__LAST_BITE * debugState = &appState->debugState.lastBite;
            //debugState->atIndex = ( debugState->atIndex + 1 ) % DEBUG__LAST_BITE_FRAME_COUNT;
            
            //DEBUG__LAST_BITE_FRAME * frame = debugState->frame + debugState->atIndex;
            //frame->nFish = 0;
            //frame->player.pos        = player->position;
            //frame->player.facingLeft = player->facingDir.isFacingLeft;
        }
    } else if( wasPressed( mouse, mouseButton_right ) ) {
        boo32 isValid =
            ( !player->bite_isActive ) && ( !player->bitByDogfish ) && ( !player->defend_isActive ) && ( !player->isStunnedHi ) && ( !player->useItem_isActive ) && ( !player->heldByBubble );
        if( isValid ) {
            player->doSlap = true;
            
            //player->parry_isActive      = true;
            player->defend_isActive     = true;
            player->defendAnim_isActive = true;
            player->timer               = 0.0f;
            
            player->isStunnedLo = false;
            
            //AUDIO_ID audioIDA = ( AUDIO_ID )( ( uint32 )audioID_slap01 + RandomU32InRange( 0, 1 ) );
            //playAudio( audioSystem, appState->audio, audioIDA, 2.0f );
            
            AUDIO_ID audioIDB = ( AUDIO_ID )( audioID_player_parry01 + RandomU32InRange( 0, 2 ) );
            playAudio( audioSystem, appState->audio, audioIDB, 0.5f );
        }
    } else if( wasPressed( mouse, mouseButton_middle ) ) {
        boo32 isValid =
            ( !player->bite_isActive ) && ( !player->bitByDogfish ) && ( !player->defend_isActive ) && ( !player->isStunnedHi ) && ( !player->useItem_isActive ) && ( !player->heldByBubble );
        // TODO: isValid might change based on what item they are attempting to use
        if( isValid ) {
            initInventoryCharge( appState );
            player->useItem_isActive = true;
        }
    }
    
    if( wasReleased( mouse, mouseButton_middle ) ) {
        player->useItem_isActive   = false;
        
        ITEM_INVENTORY * inventory = &appState->item_inventory;
        inventory->charge_isActive = false;
    }
}

internal void
updatePlayerCollision( APP_STATE * appState, vec2 dP ) {
    PLAYER_STATE    * player         = &appState->playerState;
    TOWER_STATE     * towerState     = &appState->towerState;
    OBSTACLE_STATE  * obstacleState  = &appState->obstacleState;
    BREAKABLE_STATE * breakableState = &appState->breakableState;
    
    //player->position = getCenter( appState->cameraBound );
    //dP = Vec2( 800.0f, 200.0f );
    
    vec2  P = player->position;
    vec2  V = player->velocity;
    flo32 length    = getLength( dP );
    flo32 minLength = 0.0001f;
    if( length > minLength ) {
        vec2 prevN = {};
        for( uint32 iter = 0; iter < 4; iter++ ) {
            RAY2 ray = Ray2( P, dP );
            RAY2_INTERSECT bestIntersect = Ray2IntersectInit();
            
            { // camera bound
                rect bound = appState->cameraBound;
                vec2 pos[ 5 ] = {};
                pos[ 0 ] = getBL( bound );
                pos[ 1 ] = getBR( bound );
                pos[ 2 ] = getTR( bound );
                pos[ 3 ] = getTL( bound );
                pos[ 4 ] = pos[ 0 ];
                
                for( uint32 iPos = 0; iPos < 4; iPos++ ) {
                    RAY2_INTERSECT intersect = doesIntersect( ray, pos[ iPos ], pos[ iPos + 1 ] );
                    if( ( intersect.isValid ) && ( intersect.t < bestIntersect.t ) ) {
                        bestIntersect = intersect;
                    }
                }
            }
            
            for( uint32 iTower = 0; iTower < TOWER_MAX_COUNT; iTower++ ) {
                TOWER * tower = towerState->tower + iTower;
                if( ( tower->isActive ) && ( tower->collision_isActive ) ) {
                    RAY2_INTERSECT intersect = doesIntersect( ray, tower->collision_bound );
                    if( ( intersect.isValid ) && ( intersect.t < bestIntersect.t ) ) {
                        bestIntersect = intersect;
                    }
                }
            }
            
            for( uint32 iBreakable = 0; iBreakable < BREAKABLE_MAX_COUNT; iBreakable++ ) {
                BREAKABLE * breakable = breakableState->breakable + iBreakable;
                if( ( breakable->isActive ) && ( breakable->collision_isActive ) ) {
                    RAY2_INTERSECT intersect = doesIntersect( ray, breakable->collision_bound );
                    if( ( intersect.isValid ) && ( intersect.t < bestIntersect.t ) ) {
                        bestIntersect = intersect;
                    }
                }
            }
            
            for( uint32 iObstacle = 0; iObstacle < obstacleState->nObstacle; iObstacle++ ) {
                OBSTACLE obstacle = obstacleState->obstacle[ iObstacle ];
                RAY2_INTERSECT intersect = doesIntersect( ray, obstacle.bound.P, obstacle.bound.Q );
                if( ( intersect.isValid ) && ( intersect.t < bestIntersect.t ) ) {
                    bestIntersect = intersect;
                }
            }
            
            if( ( bestIntersect.isValid ) && ( bestIntersect.t <= 1.0f ) ) {
                P   = bestIntersect.P;
                dP -= dot( bestIntersect.N, dP ) * bestIntersect.N;
                dP *= ( 1.0f - bestIntersect.t );
                length = getLength( dP );
                flo32 dotBPN = dot( bestIntersect.N, prevN );
                if( ( length <= minLength ) || ( dotBPN < 0.0f ) ) {
                    dP = {};
                    iter = 4;
                }
                prevN = bestIntersect.N;
                
                V -= dot( bestIntersect.N, dP ) * bestIntersect.N;
            } else {
                iter = 4;
            }
        }
        
        player->position = P + dP;
        player->velocity = V;
    }
}

internal void
updatePlayer( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, flo32 dt ) {
    PLAYER_STATE * player = &appState->playerState;
    
    if( appState->gameOver ) {
        player->timer += dt;
        
        flo32 maxSpeed   = 1600.0f;
        flo32 revsPerSec = 0.125f;
        flo32 bob_height = 40.0f;
        
        flo32 t = 0.5f / revsPerSec;
        flo32 acc = bob_height / ( 0.5f * t * t * maxSpeed );
        
        flo32 bob_radians = player->timer * 2.0f * PI * revsPerSec;
        
        vec2 dir = {};
        dir.y = sinf( bob_radians ) * acc;
        
        vec2 accel = ( dir * maxSpeed - player->velocity ) * PLAYER_STUNNED_FRICTION;
        player->position += accel * ( dt * dt * 0.5f ) + player->velocity * dt;
        player->velocity += accel * dt;
        
        player->position = clampToBound( player->position, appState->cameraBound );
        
        // TODO: maybe move the player a little bit with the mouse, like stunned movement
        // TODO: make the player bob in place
    } else {
        if( ( player->useItem_isActive ) || ( player->defend_isActive ) || ( player->isStunnedHi ) ) {
            vec2  dPos = player->dPos;
            flo32 dist = minValue( getLength( dPos ), PLAYER_MAX_MOVEMENT_DIST_FOR_DEFEND_AND_STUN );
            dPos = getNormal( dPos ) * dist;
            
            if( player->isStunnedHi ) {
                vec2 accel = player->velocity * -PLAYER_STUNNED_FRICTION;
                dPos += accel * ( dt * dt * 0.5f ) + player->velocity * dt;
                player->velocity += accel * dt;
            }
            
            updatePlayerCollision( appState, dPos );
            // TODO: i don't think i need to clamp this to cameraBound, but make sure
        } else if( player->bitByDogfish ) {
            player->position = player->dogfish->position;
        } else if( player->heldByBubble ) {
            player->position = player->bubble->position;
        } else {
            updatePlayerCollision( appState, player->dPos );
        }
        //player->position = clampToBound( player->position, appState->cameraBound );
        
        if( player->bitByDogfish ) {
            vec2 dim = getDim( appState->cameraBound );
            vec2 dPos = player->dPos / dim;
            player->dogfish_escape += getLength( dPos );
            player->dogfish_escape -= ( DOGFISH_ESCAPE_EMPTY_RATE * dt );
            player->dogfish_escape = maxValue( player->dogfish_escape, 0.0f );
            
            player->dogfish_escapeDir = getNormal( player->dPos );
            
            player->dogfish_timer += dt;
            if( player->dogfish_timer >= DOGFISH_BITE_DAMAGE_WARMUP_TARGET_TIME ) {
                player->health = maxValue( player->health - DOGFISH_BITE_DAMAGE_RATE * dt, 0.0f );
            }
            
            if( player->dogfish_escape >= DOGFISH_ESCAPE_VALUE ) {
                player->dogfish->state = fishState_stunnedByEscape;
                player->dogfish->timer = 0.0f;
                player->dogfish        = 0;
                
                player->bitByDogfish = false;
                
                for( uint32 iAudio = 0; iAudio < 5; iAudio++ ) {
                    AUDIO_ID audioID = ( AUDIO_ID )( iAudio + audioID_dogfish_biting01 );
                    stopAudio( audioSystem, audioID );
                }
            }
            
            player->dodge_timer = 0.0f;
            player->nDodges     = 0;
        }
        
        if( player->heldByBubble ) {
            vec2 dim = getDim( appState->cameraBound );
            vec2 dPos = player->dPos / dim;
            player->bubble_escape += getLength( dPos );
            
            player->bubble_escapeDir = getNormal( player->dPos );
            
            if( player->bubble_escape >= BUBBLE_ESCAPE_VALUE ) {
                player->bubble->isActive = false;
                player->bubble           = 0;
                
                player->heldByBubble = false;
                
                BUBBLE_STATE * bubbleState = &appState->bubbleState;
                bubbleState->nBubble--;
                
                addPop( &appState->explosionList, player->position, player->position );
                
                uint32 index = rand() % 2;
                AUDIO_ID audioID = ( AUDIO_ID )( audioID_pop01 + index );
                playAudio( audioSystem, appState->audio, audioID, 0.85f );
            }
            
            player->dodge_timer = 0.0f;
            player->nDodges     = 0;
        }
        
        player->dodge_timer += dt;
        
        player->timer += dt;
        
        { // low health timer
            if( ( player->health_prev > PLAYER_FLASH_LOW_HEALTH_THRESHOLD ) && ( player->health <= PLAYER_FLASH_LOW_HEALTH_THRESHOLD ) ) {
                player->health_timer = 0.0f;
            }
            player->health_prev = player->health;
            player->health_timer += dt;
            
            flo32 targetTime = PLAYER_FLASH_LOW_HEALTH_TARGET_TIME * 2.0f;
            if( player->health_timer >= targetTime ) {
                player->health_timer = 0.0f;
            }
        }
        
        { // was damaged timer
            player->wasDamaged_timer -= dt;
        }
    }
}

internal void
drawPlayer( RENDER_PASS * pass, TEXTURE_ID textureID, vec2 position, boo32 facingLeft, flo32 radians, flo32 adjScale, vec4 modColor, boo32 showDebug = false ) {
    vec2 scale  = Vec2( PLAYER_BITMAP_SCALE_X, PLAYER_BITMAP_SCALE_Y ) * adjScale;
    vec2 offset = Vec2( PLAYER_BITMAP_OFFSET_X, PLAYER_BITMAP_OFFSET_Y );
    if( facingLeft ) {
        scale.x  = -scale.x;
        offset.x = -offset.x;
    }
    drawQuad( pass, textureID, position + offset, radians, scale, modColor );
    
    //rect bound = rectCHD( position, Vec2( 300.0f, 300.0f ) );
    //drawRectOutline( pass, bound, COLOR_RED );
    //drawCircleOutline( pass, position, URCHIN_SPINE_COLLISION__PLAYER_RADIUS, COLOR_RED );
}

internal void
drawPlayer( RENDER_PASS * pass, APP_STATE * appState, PLAYER_STATE * player, boo32 showDebug = false ) {
    if( appState->gameOver ) {
        TEXTURE_ID textureID = textureID_player_bellyUp;
        if( player->timer <= 1.0f ) {
            textureID = textureID_player_stunned;
        }
        
        drawPlayer( pass, textureID, player->position, player->facingDir.isFacingLeft, 0.0f, 1.0f, COLOR_WHITE );
    } else {
        TEXTURE_ID textureID = textureID_player_idle;
        if( player->bite_isActive ) {
            if( player->timer > PLAYER_ANIM_BITE_OPEN_TARGET_TIME ) {
                textureID = textureID_player_bite_close;
            } else {
                textureID = textureID_player_bite_open;
            }
        }
        
        flo32 radians = 0.0f;
        flo32 scale   = 1.0f;
        if( player->defendAnim_isActive ) {
            if( player->timer < PLAYER_DEFEND_TARGET_TIME ) {
                textureID = textureID_player_parry;
            }
            
            flo32 target = 10.0f / 60.0f;
            flo32 rad    = PI / 9.0f;
            flo32 sca    = 1.1f;
            
            if( ( player->timer >= 0.0f ) && ( player->timer < target ) ) {
                flo32 t = clamp01( player->timer / target );
                radians = t * rad;
                scale   = lerp( 1.0f, t, sca );
            }
            if( player->timer >= target ) {
                flo32 t = clamp01( ( player->timer - target ) / ( PLAYER_DEFEND_ANIMATION_TIME - target ) );
                radians = ( 1.0f - t ) * rad;
                scale   = lerp( sca, t, 1.0f );
            }
            
            // TODO: add more frames to defend animation
        }
        if( player->isStunnedHi ) {
            // TODO: add animation so the player revolves around some offset center point
            flo32 t = clamp01( player->timer / PLAYER_STUNNED_HI_TARGET_TIME );
            radians = t * PI * 2.0f;
            textureID = textureID_player_stunned;
        }
        if( player->isStunnedLo ) {
            // TODO: change animation to lo stun, player rotates back a little then rotates forward back to starting position
            flo32 rad = PI / 3.0f;
            flo32 target  = 10.0f / 60.0f;
            if( player->timer < target ) {
                flo32 t = clamp01( player->timer / target );
                radians = t * rad;
            } else {
                flo32 t = 1.0f - clamp01( ( player->timer - target ) / ( PLAYER_STUNNED_LO_TARGET_TIME - target ) );
                radians = t * rad;
            }
            
            textureID = textureID_player_stunned;
        }
        boo32 facingLeft = player->facingDir.isFacingLeft;
        if( facingLeft ) {
            radians = -radians;
        }
        
        vec4 modColor = COLOR_WHITE;
        if( player->wasDamaged_timer > 0.0f ) {
            textureID = textureID_player_stunned;
            modColor  = Vec4( 1.0f, 0.5f, 0.5f, 1.0f );
        }
        if( ( player->health <= 0.25f ) && ( player->health_timer < PLAYER_FLASH_LOW_HEALTH_TARGET_TIME ) ) {
            modColor = Vec4( 1.0f, 0.5f, 0.5f, 1.0f );
        }
        
        vec2 offset = {};
        if( player->heldByBubble ) {
            textureID = textureID_player_stunned;
            vec2 bitmap_offset = Vec2( 10.0f, -10.0f );
            if( facingLeft ) {
                bitmap_offset.x = -bitmap_offset.x;
            }
            offset += player->bubble_escapeDir * BUBBLE_ESCAPE_BITMAP_OFFSET;
            offset += bitmap_offset;
        }
        if( !player->bitByDogfish ) {
            drawPlayer( pass, textureID, player->position + offset, facingLeft, radians, scale, modColor );
        }
        
        if( player->useItem_isActive ) {
            ITEM_INVENTORY * inventory = &appState->item_inventory;
            rect outer = rectTCD( player->position, Vec2( 52.0f, 52.0f ) );
            
            INVENTORY_ITEM item = inventory->item[ inventory->activeSlot ];
            flo32 t = clamp01( inventory->charge_timer / inventory->charge_targetTime );
            drawInventoryItem( pass, appState->font, item, outer, true, t );
        }
        
        if( showDebug ) {
            rect R = rectCD( player->position, Vec2( 4.0f, 4.0f ) );
            drawRect( pass, textureID_whiteTexture, R, COLOR_CYAN );
            
            rect S = rectCD( getSlapPos( player ), Vec2( 8.0f, 8.0f ) );
            drawRect( pass, textureID_whiteTexture, S, COLOR_BLUE );
            
            circ collision = getBubbletCollision( player );
            drawCircleOutline( pass, collision.center, collision.radius, COLOR_RED );
        }
    }
}

internal void
playerDeath( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
    if( !appState->gameOver ) {
        PLAYER_STATE * player = &appState->playerState;
        if( player->health <= 0.0f ) {
            appState->gameOver = true;
            player->timer = 0.0f;
            player->bite_isActive   = false;
            player->defend_isActive = false;
            player->defendAnim_isActive = false;
            
            // TODO: instead of just canceling everyone's current state, see if you can transition everyone to gameOver state after they complete their current attack/state
            player->bitByDogfish      = false;
            player->dogfish           = 0;
            player->dogfish_escape    = 0;
            player->dogfish_escapeDir = {};
            
            player->health = 0.0f;
            
            player->dodge_timer = 0.0f;
            player->nDodges     = 0;
            
            player->initStunnedHi = false;
            player->isStunnedHi   = false;
            player->initStunnedLo = false;
            player->isStunnedLo   = false;
            
            appState->musicPlaying = getMusicPlaying( audioSystem );
            
            quietMusic( audioSystem );
            playAudio( audioSystem, appState->audio, audioID_music_underwaterAmbience, 0.0f, 1.0f, true );
            setVolume( audioSystem, audioID_music_underwaterAmbience, 0.3f, 5.0f, 2.5f );
            
            playAudio( audioSystem, appState->audio, audioID_player_death, 0.35f, 1.0f, false );
        }
    }
    
}

internal void
stunPlayer( APP_STATE * appState, AUDIO_SYSTEM * audioSystem, flo32 dt ) {
    PLAYER_STATE * player = &appState->playerState;
    
    if( player->initStunnedHi ) {
        player->initStunnedHi = false;
        
        player->isStunnedHi = true;
        player->timer       = 0.0f;
        
        player->position  = player->stun_position;
        player->velocity  = player->stun_direction * PLAYER_STUNNED_HI_SPEED;
        
        appState->audioManager.nPlayerStun++;
        //playAudio( audioSystem, appState->audio, audioID_swordfish_hitPlayer, 0.8f );
        addMedExplosion( &appState->explosionList, player->stun_particlePos );
        
        CAMERA_SHAKE * shake = &appState->cameraState.cameraShake;
        initShake( shake, 0.0f, 0.0f, 0.5f, 60.0f, Vec2( 10.0f, 10.0f ) );
        
        player->health = maxValue( player->health - player->stun_damage, 0.0f );
    } else if( player->initStunnedLo ) {
        player->initStunnedLo = false;
        
        player->isStunnedLo = true;
        player->timer       = 0.0f;
        
        playAudio( audioSystem, appState->audio, audioID_swordfish_hitPlayer, 0.8f );
        
        player->health = maxValue( player->health - ( flo32 )SWORDFISH_STAB_DAMAGE_LO, 0.0f );
    }
}

internal void
finalizePlayer( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, flo32 dt ) {
    PLAYER_STATE * player = &appState->playerState;
    player->attemptHeal = false;
    
    if( !appState->gameOver ) {
        if( player->bite_isActive ) {
            flo32 targetTime = PLAYER_ANIM_BITE_OPEN_TARGET_TIME + PLAYER_ANIM_BITE_CLOSE_TARGET_TIME;
            if( player->timer >= targetTime ) {
                player->bite_isActive = false;
            }
        }
        //if( ( player->parry_isActive ) && ( player->timer >= PLAYER_PARRY_TARGET_TIME ) ) {
        //player->parry_isActive = false;
        //}
        if( ( player->defend_isActive ) && ( player->timer >= PLAYER_DEFEND_TARGET_TIME ) ) {
            //player->parry_isActive  = false;
            player->defend_isActive = false;
        }
        if( ( player->defendAnim_isActive ) && ( player->timer >= PLAYER_DEFEND_ANIMATION_TIME ) ) {
            player->defendAnim_isActive = false;
        }
        if( ( player->isStunnedHi ) && ( player->timer >= PLAYER_STUNNED_HI_TARGET_TIME ) ) {
            player->isStunnedHi = false;
        }
        if( ( player->isStunnedLo ) && ( player->timer >= PLAYER_STUNNED_LO_TARGET_TIME ) ) {
            player->isStunnedLo = false;
        }
        
        stunPlayer( appState, audioSystem, dt );
    }
}

internal void
initPlayerStunHi( PLAYER_STATE * player, orect bound, vec2 dir ) {
    // TODO: maybe separate into stun frames and damage frames. it doesn't really make sense for the swordfish to stab, then the player moves into the stab collision, then is stunned. probably better is small stun or just small damage. then again, maybe the stab frames are too short to care
    
    // TODO: maybe adjust stunned direction based on where player hits the swordfish nose collision
    // TODO: maybe set player position to some point along border of nose collision if he lies too much into the interior to prevent continuous stuns, gives the player a better chance of being knocked clear
    
    vec2 V    = getNormal( player->position - bound.center );
    vec2 axis = getPerp( dir );
    vec2 stunDir = dir;
    if( dot( V, axis ) >= 0.0f ) {
        stunDir += axis;
    } else {
        stunDir -= axis;
    }
    stunDir = getNormal( stunDir );
    
    player->initStunnedHi  = true;
    player->stun_position  = player->position;
    player->stun_direction = stunDir;
    player->stun_damage    = SWORDFISH_STAB_DAMAGE_HI;
    player->stun_particlePos = lerp( player->position, 0.6f, bound.center );
}

internal void
initPlayerStun( PLAYER_STATE * player, PARROTFISH * fish ) {
    player->initStunnedHi  = true;
    player->stun_position  = player->position;
    
    vec2 playerPos = getSlapPos( player );
    player->stun_direction = getNormal( playerPos - fish->position );
    player->stun_damage    = 0.0f;
    
    player->stun_particlePos = player->position;
}