
internal orect
getCageCollision( CAGE * fish ) {
    orect result = {};
    result.center  = fish->position;
    result.halfDim = fish->halfDim;
    result.xAxis   = Vec2( 1.0f, 0.0f );
    result.yAxis   = Vec2( 0.0f, 1.0f );
    return result;
}

internal void
addCage( CAGE_STATE * state, CAGE cage ) {
    Assert( state->nCage < CAGE_MAX_COUNT );
    
    for( uint32 index = 0; index < CAGE_MAX_COUNT; index++ ) {
        CAGE fish = state->cage[ index ];
        if( !fish.isActive ) {
            state->cage[ index ] = cage;
            state->nCage++;
            break;
        }
    }
}

internal void
updateCage( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, flo32 dt ) {
    CAGE_STATE    * state = &appState->cageState;
    PLAYER_STATE  * player = &appState->playerState;
    FISH_ATE_LIST * list  = &appState->fishAteList;
    
    if( list->attemptBite ) {
        for( uint32 iFish = 0; iFish < CAGE_MAX_COUNT; iFish++ ) {
            CAGE * fish = state->cage + iFish;
            
            orect bound = orectCHD( fish->position, fish->halfDim );
            if( ( fish->isActive ) && ( fish->health > 0 ) && ( isInBound( player->position, bound ) ) ) {
                fish->health = maxValue( fish->health - 1, 0 );
                fish->timer  = 0.0f;
                
                addBit( list, *fish );
                
            }
        }
    }
    
    for( uint32 iFish = 0; iFish < CAGE_MAX_COUNT; iFish++ ) {
        CAGE * fish = state->cage + iFish;
        if( fish->isActive ) {
            fish->timer += dt;
            
            if( ( fish->health <= 0 ) && ( fish->timer >= CAGE_FISH_ESCAPE_DELAY_TARGET_TIME ) ) {
                flo32 maxSpeed = CAGE_FISH_MOVE_SPEED;
                flo32 friction = CAGE_FISH_MOVE_FRICTION;
                
                vec2 dest = getBL( appState->cameraBound ) + Vec2( -200.0f, 200.0f );
                vec2 dir  = getNormal( dest - fish->position );
                
                vec2 accel = ( dir * maxSpeed - fish->velocity ) * friction;
                fish->position += accel * ( dt * dt * 0.5f ) + fish->velocity * dt;
                fish->velocity += accel * dt;
            }
        }
    }
}

internal void
drawCage( RENDER_PASS * pass, APP_STATE * appState, CAGE * fish, boo32 showDebug = false ) {
    vec2 scale  = fish->halfDim * Vec2( 1.9f, 2.4f );
    vec2 offset = Vec2( -10.0f, 10.0f );
    
    drawQuad( pass, textureID_cage_fish, fish->position + offset, scale, COLOR_WHITE );
    
    if( fish->health > 0 ) {
        if( fish->timer < CAGE_SHAKE_TARGET_TIME ) {
            offset +=  RandomDirection2D() * ( RandomF32() * CAGE_SHAKE_BITMAP_OFFSET );
        }
        drawQuad( pass, textureID_cage_idle, fish->position + offset, scale, COLOR_WHITE );
    }
    
    if( fish->health > 0 ) {
        rect health_bound = rectBCD( fish->position + Vec2( 0.0f, -fish->halfDim.y + 10.0f ), Vec2( fish->halfDim.x * 1.5f, 20.0f ) );
        drawHealthBar( pass, health_bound, fish->health, CAGE_HEALTH );
    }
    
    if( showDebug ) {
        orect bound = getCageCollision( fish );
        drawORectOutline( pass, bound, COLOR_RED );
        
        rect boundA = rectCD( fish->position, PARROTFISH_CAGE_COLLISION_DIM );
        drawRectOutline( pass, boundA, COLOR_GREEN );
    }
}

internal void
drawCage( RENDER_PASS * pass, APP_STATE * appState, boo32 showDebug = false ) {
    CAGE_STATE * state  = &appState->cageState;
    PLAYER_STATE  * player = &appState->playerState;
    
    for( uint32 iFish = 0; iFish < CAGE_MAX_COUNT; iFish++ ) {
        CAGE * fish = state->cage + iFish;
        if( fish->isActive ) {
            drawCage( pass, appState, fish, showDebug );
        }
    }
}

internal void
finalizeCage( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
    CAGE_STATE * state = &appState->cageState;
    
    if( !appState->gameOver ) {
        rect bound = addRadius( appState->cameraBound, 100.0f );
        for( uint32 iFish = 0; iFish < CAGE_MAX_COUNT; iFish++ ) {
            CAGE * fish = state->cage + iFish;
            if( fish->isActive ) {
                if( ( fish->health <= 0 ) && ( !isInBound( fish->position, bound ) ) ) {
                    fish->isActive = false;
                    state->nCage--;
                }
            }
        }
    }
}

internal void
spawnCage( APP_STATE * appState, vec2 position ) {
    CAGE_STATE * state = &appState->cageState;
    
    CAGE fish = {};
    fish.isActive  = true;
    fish.position  = position;
    fish.halfDim   = Vec2( 150.0f, 100.0f );
    fish.health    = CAGE_HEALTH;
    fish.timer     = CAGE_SHAKE_TARGET_TIME;
    
    addCage( state, fish );
}
