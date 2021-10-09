
//----------
// HEAL BOUND
//----------

internal void
addHeal( HEAL_STATE * state, HEAL heal ) {
    Assert( state->nHeal < HEAL_MAX_COUNT );
    
    for( uint32 index = 0; index < HEAL_MAX_COUNT; index++ ) {
        HEAL fish = state->heal[ index ];
        if( !fish.isActive ) {
            state->heal[ index ] = heal;
            state->nHeal++;
            break;
        }
    }
}

internal void
drawHeal( RENDER_PASS * pass, APP_STATE * appState, HEAL * fish, boo32 showDebug = false ) {
    Assert( fish->isActive );
    
    showDebug = true;
    if( showDebug ) {
        drawRectOutline( pass, fish->bound, COLOR_RED );
    }
}

internal void
drawHeal( RENDER_PASS * pass, APP_STATE * appState, boo32 showDebug = false ) {
    HEAL_STATE * state = &appState->healState;
    
    for( uint32 iFish = 0; iFish < HEAL_MAX_COUNT; iFish++ ) {
        HEAL * fish = state->heal + iFish;
        if( fish->isActive ) {
            drawHeal( pass, appState, fish, showDebug );
        }
    }
}

internal void
finalizeHeal( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
    HEAL_STATE * state = &appState->healState;
    PLAYER_STATE * player = &appState->playerState;
    
    for( uint32 iFish = 0; iFish < HEAL_MAX_COUNT; iFish++ ) {
        HEAL * fish = state->heal + iFish;
        if( fish->isActive ) {
            vec2 playerPos = getSlapPos( player );
            if( ( player->attemptHeal ) && ( isInBound( playerPos, fish->bound ) ) ) {
                fish->isActive = false;
                state->nHeal--;
            }
        }
    }
}

internal void
spawnHeal( APP_STATE * appState, rect bound ) {
    HEAL_STATE * state = &appState->healState;
    
    HEAL fish = {};
    fish.isActive = true;
    fish.bound    = bound;
    
    addHeal( state, fish );
}