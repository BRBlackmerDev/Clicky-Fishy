
internal void
addBubble( BUBBLE_STATE * state, BUBBLE bubble ) {
    Assert( state->nBubble < BUBBLE_MAX_COUNT );
    
    for( uint32 index = 0; index < BUBBLE_MAX_COUNT; index++ ) {
        BUBBLE fish = state->bubble[ index ];
        if( !fish.isActive ) {
            state->bubble[ index ] = bubble;
            state->nBubble++;
            break;
        }
    }
}

internal void
updateBubble( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, flo32 dt ) {
    BUBBLE_STATE * state  = &appState->bubbleState;
    PLAYER_STATE * player = &appState->playerState;
    OBSTACLE_STATE * obstacleState = &appState->obstacleState;
    
    for( uint32 iFish = 0; iFish < BUBBLE_MAX_COUNT; iFish++ ) {
        BUBBLE * fish = state->bubble + iFish;
        if( fish->isActive ) {
            flo32 maxSpeed = BUBBLE_MOVE_SPEED;
            flo32 friction = BUBBLE_MOVE_FRICTION;
            
            vec2 dir = Vec2( 0.0f, 1.0f );
            if( ( player->heldByBubble ) && ( player->bubble == fish ) ) {
                maxSpeed = BUBBLE_HOLD_MOVE_SPEED;
                fish->holdOffset = player->bubble_escapeDir * BUBBLE_ESCAPE_BITMAP_OFFSET;
            }
            
            vec2 repel_dir = fish->repel;
            for( uint32 iter = iFish + 1; iter < BUBBLE_MAX_COUNT; iter++ ) {
                BUBBLE * fish0 = state->bubble + iter;
                if( fish0->isActive ) {
                    flo32 radius   = BUBBLE_REPEL_MAX_DISTANCE * 2.0f;
                    flo32 radiusSq = radius * radius;
                    flo32 dist   = getLength( fish->position - fish0->position );
                    flo32 t = clamp01( dist / radius );
                    flo32 strength = -180.0f * ( powf( t, 4.0f ) - 1.0f );
                    vec2  V = getNormal( fish0->position - fish->position ) * strength;
                    
                    vec2 W = -V;
                    V.y = minValue( V.y, 0.0f );
                    W.y = minValue( W.y, 0.0f );
                    
                    repel_dir    += W;
                    fish0->repel += V;
                }
            }
            
            vec2 accel = ( dir * maxSpeed - fish->velocity + repel_dir ) * friction;
            vec2  P = fish->position;
            vec2 dP = accel * ( dt * dt * 0.5f ) + fish->velocity * dt;
            vec2 prevN = {};
            
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
                
                if( ( bestIntersect.isValid ) && ( bestIntersect.t <= 1.0f ) ) {
                    P   = bestIntersect.P;
                    dP -= dot( bestIntersect.N, dP ) * bestIntersect.N;
                    dP *= ( 1.0f - bestIntersect.t );
                    flo32 dotBPN = dot( bestIntersect.N, prevN );
                    if( dotBPN < 0.0f ) {
                        dP = {};
                        iter = 4;
                    }
                    prevN = bestIntersect.N;
                } else {
                    iter = 4;
                }
            }
            
            fish->position  = P + dP;
            fish->velocity += accel * dt;
            fish->repel = {};
        }
    }
}

internal void
drawBubble( RENDER_PASS * pass, APP_STATE * appState, BUBBLE * fish, boo32 showDebug = false ) {
    vec2 scale  = Vec2( BUBBLE_BITMAP_SCALE_X,  BUBBLE_BITMAP_SCALE_Y  );
    vec2 offset = Vec2( BUBBLE_BITMAP_OFFSET_X, BUBBLE_BITMAP_OFFSET_Y );
    
    vec2 pos = fish->position + offset + fish->holdOffset;
    drawQuad( pass, textureID_bubble_idle, pos, scale, COLOR_WHITE );
    
    if( showDebug ) {
        drawCircleOutline( pass, fish->position, BUBBLE_HOLD_RADIUS, COLOR_RED );
        drawCircleOutline( pass, fish->position, BUBBLE_REPEL_MAX_DISTANCE, COLOR_GREEN );
    }
}

internal void
drawBubble( RENDER_PASS * pass, APP_STATE * appState, boo32 showDebug = false ) {
    BUBBLE_STATE * state  = &appState->bubbleState;
    
    for( uint32 iFish = 0; iFish < BUBBLE_MAX_COUNT; iFish++ ) {
        BUBBLE * fish = state->bubble + iFish;
        if( fish->isActive ) {
            drawBubble( pass, appState, fish, showDebug );
        }
    }
}

internal void
popBubble( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, BUBBLE * bubble ) {
    BUBBLE_STATE * state  = &appState->bubbleState;
    PLAYER_STATE * player = &appState->playerState;
    
    bubble->isActive = false;
    state->nBubble--;
    
    addPop( &appState->explosionList, bubble->position, bubble->position );
    
    uint32 index = rand() % 2;
    AUDIO_ID audioID = ( AUDIO_ID )( audioID_pop01 + index );
    playAudio( audioSystem, appState->audio, audioID, 0.85f );
    
    boo32 heldByBubble = ( player->heldByBubble ) && ( player->bubble == bubble );
    if( heldByBubble ) {
        player->heldByBubble = false;
        player->bubble = 0;
        player->position = clampToBound( player->position, appState->cameraBound );
    }
}

internal void
finalizeBubble( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
    BUBBLE_STATE * state = &appState->bubbleState;
    
    if( !appState->gameOver ) {
        PLAYER_STATE * player = &appState->playerState;
        for( uint32 iFish = 0; iFish < BUBBLE_MAX_COUNT; iFish++ ) {
            BUBBLE * fish = state->bubble + iFish;
            if( fish->isActive ) {
                PARROTFISH_STATE * parrotfishState = &appState->parrotfishState;
                for( uint32 iParrotfish = 0; iParrotfish < PARROTFISH_MAX_COUNT; iParrotfish++ ) {
                    PARROTFISH * parrotfish = parrotfishState->parrotfish + iParrotfish;
                    if( parrotfish->isActive ) {
                        flo32 radius   = PARROTFISH_BUBBLE_COLLISION_RADIUS;
                        flo32 radiusSq = radius * radius;
                        flo32 distSq = getLengthSq( fish->position - parrotfish->position );
                        if( distSq <= radiusSq ) {
                            popBubble( audioSystem, appState, fish );
                        }
                    }
                }
            }
            if( fish->isActive ) {
                boo32 heldByBubble = ( player->heldByBubble ) && ( player->bubble == fish );
                
                rect bound = addRadius( appState->cameraBound, 100.0f );
                if( heldByBubble ) {
                    bound.top -= 80.0f;
                }
                if( !isInBound( fish->position, bound ) ) {
                    //popBubble( audioSystem, appState, fish );
                } else if( !player->heldByBubble ) { // PLAYER COLLISION
                    circ collision = {};
                    collision.center = fish->position;
                    collision.radius = BUBBLE_HOLD_RADIUS;
                    
                    if( isInBound( player->position, collision ) ) {
                        player->heldByBubble  = true;
                        player->bubble        = fish;
                        player->bubble_escape = 0.0f;
                        
                        player->bite_isActive       = false;
                        player->defend_isActive     = false;
                        player->defendAnim_isActive = false;
                        player->useItem_isActive    = false;
                        
                        player->initStunnedHi = false;
                        player->isStunnedHi   = false;
                        
                        player->initStunnedLo = false;
                        player->isStunnedLo   = false;
                        
                        playAudio( audioSystem, appState->audio, audioID_bubble_capture, 0.35f );
                    }
                }
            }
        }
    }
}

internal void
spawnBubble( BUBBLE_STATE * state, vec2 position, vec2 direction ) {
    BUBBLE fish = {};
    fish.isActive  = true;
    fish.position  = position;
    fish.velocity  = direction * BUBBLE_SPAWN_SPEED;
    
    addBubble( state, fish );
}

internal void
spawnBubble( BUBBLE_STATE * state, vec2 position ) {
    BUBBLE fish = {};
    fish.isActive  = true;
    fish.position  = position;
    
    addBubble( state, fish );
}