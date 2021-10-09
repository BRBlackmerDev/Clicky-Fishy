
internal void
addTower( TOWER_STATE * state, TOWER tower ) {
    Assert( state->nTower < TOWER_MAX_COUNT );
    
    for( uint32 index = 0; index < TOWER_MAX_COUNT; index++ ) {
        TOWER fish = state->tower[ index ];
        if( !fish.isActive ) {
            state->tower[ index ] = tower;
            state->nTower++;
            break;
        }
    }
}

internal void
updateTower( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, flo32 dt ) {
    TOWER_STATE   * state  = &appState->towerState;
    PLAYER_STATE  * player = &appState->playerState;
    
    for( uint32 iFish = 0; iFish < TOWER_MAX_COUNT; iFish++ ) {
        TOWER * fish = state->tower + iFish;
        if( fish->isActive ) {
            if( fish->health <= 0 ) {
                fish->collision_isActive = false;
                fish->timer += dt;
            }
        }
    }
}

internal void
drawTower( RENDER_PASS * pass, APP_STATE * appState, TOWER * fish, boo32 showDebug = false ) {
    TEXTURE_ID textureID = textureID_tower_idle;
    
    vec2  offset  = {};
    flo32 radians = 0.0f;
    
    if( fish->health <= 0 ) {
        textureID = textureID_tower_crumble;
        
        flo32 dist = clamp01( fish->timer / 3.0f ) * 12.0f;
        offset +=  RandomDirection2D() * ( RandomF32() * dist );
        
        if( fish->timer >= 2.5f ) {
            flo32 t = clamp01( ( fish->timer - 2.5f ) / 4.0f );
            offset += Vec2( 0.0f, -1200.0f ) * t;
            
            radians = lerp( 0.0f, t, PI / 24.0f );
        }
    }
    
    vec2 pos     = getCenter ( fish->collision_bound );
    vec2 halfDim = getHalfDim( fish->collision_bound );
    
    orect bound = orectCHD( pos + offset, halfDim, radians );
    drawORect( pass, textureID, bound, COLOR_WHITE );
    
    if( fish->health > 0 ) {
        rect health_bound = rectBCD( getBC( fish->collision_bound ) + Vec2( -40.0f, 150.0f ), Vec2( 260.0f, 20.0f ) );
        drawHealthBar( pass, health_bound, fish->health, fish->maxHealth );
    }
}

internal void
drawTower( RENDER_PASS * pass, APP_STATE * appState, boo32 showDebug = false ) {
    TOWER_STATE * state = &appState->towerState;
    
    for( uint32 iFish = 0; iFish < TOWER_MAX_COUNT; iFish++ ) {
        TOWER * fish = state->tower + iFish;
        if( fish->isActive ) {
            drawTower( pass, appState, fish, showDebug );
        }
    }
}

internal void
finalizeTower( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
    TOWER_STATE * state = &appState->towerState;
    
    for( uint32 iFish = 0; iFish < TOWER_MAX_COUNT; iFish++ ) {
        TOWER * fish = state->tower + iFish;
        if( fish->isActive ) {
            if( fish->timer >= 6.5f ) {
                fish->isActive = false;
                state->nTower--;
            }
        }
    }
}

internal void
spawnTower( APP_STATE * appState, rect bound ) {
    TOWER_STATE * state = &appState->towerState;
    
    TOWER fish = {};
    fish.isActive           = true;
    fish.collision_isActive = true;
    fish.collision_bound = bound;
    
    fish.maxHealth = TOWER_HEALTH;
    fish.health    = TOWER_HEALTH;
    
    addTower( state, fish );
}
