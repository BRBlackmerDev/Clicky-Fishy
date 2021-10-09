
internal OBSTACLE
Obstacle( LINE_SEG bound, TEXTURE_ID textureID ) {
    OBSTACLE result = {};
    result.bound         = bound;
    result.textureID     = textureID;
    return result;
}

internal void
addObstacle( APP_STATE * appState, OBSTACLE obstacle ) {
    OBSTACLE_STATE * state = &appState->obstacleState;
    
    Assert( state->nObstacle < OBSTACLE_MAX_COUNT );
    state->obstacle[ state->nObstacle++ ] = obstacle;
}

#if 0
internal void
unloadObstacleGroup( APP_STATE * appState, uint32 obsID ) {
    OBSTACLE_STATE * state = &appState->obstacleState;
    Assert( state->nGroup > 0 );
    
    int32 index = -1;
    for( uint32 iGroup = 0; iGroup < state->nGroup; iGroup++ ) {
        OBSTACLE_GROUP group = state->group[ iGroup ];
        if( group.obsID == obsID ) {
            index = iGroup;
        }
    }
    
    if( index > -1 ) {
        OBSTACLE_GROUP remove = state->group[ index ];
        
        if( ( uint32 )index < ( state->nGroup - 1 ) ) {
            for( uint32 iObs = remove.bObs + remove.nObs; iObs < state->nObstacle; iObs++ ) {
                state->obstacle[ iObs - remove.nObs ] = state->obstacle[ iObs ];
            }
        }
        state->nObstacle -= remove.nObs;
        
        for( uint32 iGroup = index + 1; iGroup < state->nGroup; iGroup++ ) {
            OBSTACLE_GROUP * group = state->group + iGroup;
            group->bObs -= remove.nObs;
        }
        
        state->nGroup--;
        for( uint32 iGroup = index; iGroup < state->nGroup; iGroup++ ) {
            state->group[ iGroup ] = state->group[ iGroup + 1 ];
        }
    }
}
#endif

internal void
drawObstacle( RENDER_PASS * pass, APP_STATE * appState, OBSTACLE * obs, boo32 showDebug = false ) {
    orect bound = orectTHD( obs->bound.Q, obs->bound.P, 5.0f );
    drawORect( pass, obs->textureID, bound, COLOR_GRAY( 0.3f ) );
    
    if( showDebug ) {
        CAPSULE2 cap = Capsule2( obs->bound.P, obs->bound.Q, 45.0f );
        drawCapsuleOutline( pass, cap, COLOR_RED );
        
        //char str[ 4 ] = {};
        //sprintf( str, "%u", obsID );
        //drawString( pass, appState->font, str, bound.center, Vec2( 2.0f, 2.0f ), COLOR_WHITE );
    }
}

internal void
drawObstacle( RENDER_PASS * pass, APP_STATE * appState, OBSTACLE obs, uint32 obsID, boo32 showDebug = false ) {
    orect bound = orectTHD( obs.bound.Q, obs.bound.P, 5.0f );
    drawORect( pass, obs.textureID, bound, COLOR_BLACK );
    
    if( showDebug ) {
        char str[ 4 ] = {};
        sprintf( str, "%u", obsID );
        drawString( pass, appState->font, str, bound.center, Vec2( 2.0f, 2.0f ), COLOR_WHITE );
        
        CAPSULE2 cap = Capsule2( obs.bound.P, obs.bound.Q, 45.0f );
        drawCapsuleOutline( pass, cap, COLOR_RED );
    }
}

internal void
drawObstacle( RENDER_PASS * pass, APP_STATE * appState, boo32 showDebug = false ) {
    OBSTACLE_STATE * state = &appState->obstacleState;
    
    for( uint32 iObs = 0; iObs < state->nObstacle; iObs++ ) {
        OBSTACLE obs = state->obstacle[ iObs ];
        drawObstacle( pass, appState, obs, iObs, showDebug );
    }
}