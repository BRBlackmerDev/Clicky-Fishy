
internal NET_COLLISION
getNetCollision( NET * fish ) {
    vec2 dim = Vec2( 80.0f, 80.0f );
    
    NET_COLLISION result = {};
    result.A = rectCD( fish->blockA.position, dim );
    result.B = rectCD( fish->blockB.position, dim );
    return result;
}

internal void
addNet( NET_STATE * state, NET net ) {
    Assert( state->nNet < NET_MAX_COUNT );
    
    for( uint32 index = 0; index < NET_MAX_COUNT; index++ ) {
        NET fish = state->net[ index ];
        if( !fish.isActive ) {
            state->net[ index ] = net;
            state->nNet++;
            break;
        }
    }
}

internal void
updateNet( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, flo32 dt ) {
    NET_STATE    * state   = &appState->netState;
    PLAYER_STATE  * player = &appState->playerState;
    FISH_ATE_LIST * list   = &appState->fishAteList;
    
    if( list->attemptBite ) {
        for( uint32 iFish = 0; iFish < NET_MAX_COUNT; iFish++ ) {
            NET * fish = state->net + iFish;
            if( fish->isActive ) {
                NET_COLLISION coll = getNetCollision( fish );
                NET_BLOCK * blockA = &fish->blockA;
                NET_BLOCK * blockB = &fish->blockB;
                if( ( blockA->health > 0 ) && ( isInBound( player->position, coll.A ) ) ) {
                    blockA->timer  = 0.0f;
                    blockA->health = maxValue( blockA->health - 1, 0 );
                    addBit( list, *fish );
                }
                if( ( blockB->health > 0 ) && ( isInBound( player->position, coll.B ) ) ) {
                    blockB->timer  = 0.0f;
                    blockB->health = maxValue( blockB->health - 1, 0 );
                    addBit( list, *fish );
                }
            }
        }
    }
    
    for( uint32 iFish = 0; iFish < NET_MAX_COUNT; iFish++ ) {
        NET * fish = state->net + iFish;
        if( fish->isActive ) {
            fish->blockA.timer += dt;
            fish->blockB.timer += dt;
            
            if( ( fish->blockA.health <= 0 ) && ( fish->blockB.health <= 0 ) ) {
                fish->timer += dt;
                fish->isFree = true;
                
                if( fish->timer >= NET_FISH_ESCAPE_DELAY_TARGET_TIME ) {
                    flo32 maxSpeed = NET_FISH_MOVE_SPEED;
                    flo32 friction = NET_FISH_MOVE_FRICTION;
                    
                    vec2 dest = getBL( appState->cameraBound ) + Vec2( -200.0f, 200.0f );
                    vec2 dir  = getNormal( dest - fish->position );
                    
                    vec2 accel = ( dir * maxSpeed - fish->velocity ) * friction;
                    fish->position += accel * ( dt * dt * 0.5f ) + fish->velocity * dt;
                    fish->velocity += accel * dt;
                }
            }
        }
    }
}

internal void
drawNet( RENDER_PASS * pass, APP_STATE * appState, NET * fish, boo32 showDebug = false ) {
    { // draw fish
        drawQuad( pass, textureID_net_fish, fish->position, fish->halfDim, COLOR_WHITE );
    }
    
    { // draw net
        flo32 targetTime = 1.0f;
        if( fish->timer < targetTime ) {
            vec2 scale = fish->halfDim * Vec2( 1.25f, 1.15f );
            flo32 t = clamp01( fish->timer / targetTime );
            
            vec2 dest = fish->position + Vec2( 0.0f, -50.0f );
            vec2 pos  = lerp( fish->position, t, dest );
            flo32 rad = lerp( 0.0f, t, PI / 12.0f );
            flo32 sca = lerp( 1.0f, t, 0.95f );
            scale *= sca;
            flo32 alp = 1.0f - t;
            vec4  modColor = Vec4( alp );
            
            drawQuad( pass, textureID_net_idle, pos, rad, scale, modColor );
        }
    }
    
    { // draw blocks
        NET_BLOCK * blocks[ 2 ] = { &fish->blockA, &fish->blockB };
        
        for( uint32 iter = 0; iter < 2; iter++ ) {
            NET_BLOCK * block = blocks[ iter ];
            
            vec2 scale = Vec2( 40.0f, 40.0f );
            if( block->health > 0 ) {
                vec2 offset = {};
                if( block->timer < 0.35f ) {
                    offset += RandomDirection2D() * ( RandomF32() * 5.0f );
                }
                drawQuad( pass, textureID_net_block, block->position + offset, scale, COLOR_WHITE );
                
                rect bound = rectBCD( block->position + Vec2( 0.0f, -20.0f ), Vec2( 32.0f, 12.0f ) );
                drawHealthBar( pass, bound, block->health, NET_BLOCK_HEALTH );
            } else {
                flo32 targetTime = 0.85f;
                if( block->timer < targetTime ) {
                    flo32 t   = clamp01( block->timer / targetTime );
                    
                    vec2 dest = block->position + Vec2( 0.0f, -60.0f );
                    vec2 pos  = lerp( block->position, t, dest );
                    flo32 rad = lerp( 0.0f, t, PI / 6.0f );
                    flo32 sca = lerp( 1.0f, t, 0.9f );
                    scale *= sca;
                    flo32 alp = 1.0f - t;
                    vec4  modColor = Vec4( alp );
                    
                    drawQuad( pass, textureID_net_block, pos, rad, scale, modColor );
                }
            }
        }
    }
    
    if( showDebug ) {
        NET_COLLISION coll = getNetCollision( fish );
        
        drawRectOutline( pass, coll.A, COLOR_RED );
        drawRectOutline( pass, coll.B, COLOR_RED );
        
        rect boundA = rectCD( fish->blockA.position, PARROTFISH_NET_COLLISION_DIM );
        rect boundB = rectCD( fish->blockB.position, PARROTFISH_NET_COLLISION_DIM );
        
        drawRectOutline( pass, boundA, COLOR_CYAN );
        drawRectOutline( pass, boundB, COLOR_CYAN );
    }
}

internal void
drawNet( RENDER_PASS * pass, APP_STATE * appState, boo32 showDebug = false ) {
    NET_STATE * state = &appState->netState;
    
    for( uint32 iFish = 0; iFish < NET_MAX_COUNT; iFish++ ) {
        NET * fish = state->net + iFish;
        if( fish->isActive ) {
            drawNet( pass, appState, fish, showDebug );
        }
    }
}

internal void
finalizeNet( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
    NET_STATE * state = &appState->netState;
    
    // TODO: how do i handle nets (and other entities) that scroll in from off screen
    // TODO: how do i handle nets (and other entities) that can scroll off screen after a certain amount of time (from camera movement), like a timed bonus?
    
    if( !appState->gameOver ) {
        rect bound = addRadius( appState->cameraBound, 100.0f );
        for( uint32 iFish = 0; iFish < NET_MAX_COUNT; iFish++ ) {
            NET * fish = state->net + iFish;
            if( fish->isActive ) {
                if( ( fish->isFree ) && ( !isInBound( fish->position, bound ) ) ) {
                    fish->isActive = false;
                    state->nNet--;
                }
            }
        }
    }
}

internal void
spawnNet( APP_STATE * appState, vec2 position ) {
    NET_STATE * state = &appState->netState;
    
    vec2 halfDim = Vec2( 250.0f, 175.0f );
    
    NET fish = {};
    fish.isActive  = true;
    fish.net.position = position;
    fish.net.halfDim  = halfDim;
    
    flo32 offsetX = halfDim.x * 0.5f;
    vec2  offsetA = Vec2( -offsetX + 10.0f, -40.0f );
    vec2  offsetB = Vec2(  offsetX + 10.0f, -40.0f );
    fish.blockA.position = position + offsetA;
    fish.blockA.health   = NET_BLOCK_HEALTH;
    fish.blockA.timer    = 1.0f;
    fish.blockB.position = position + offsetB;
    fish.blockB.health   = NET_BLOCK_HEALTH;
    fish.blockB.timer    = 1.0f;
    fish.position = position;
    fish.halfDim  = halfDim;
    
    addNet( state, fish );
}
