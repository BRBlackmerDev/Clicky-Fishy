
internal void
initShake( CAMERA_SHAKE * cam, flo32 upTime, flo32 holdTime, flo32 downTime, flo32 frequency, vec2 offsetInPixels ) {
    Assert( ( upTime > 0.0f ) || ( downTime > 0.0f ) );
    
    cam->timer     = 0.0f;
    cam->upTime    = upTime;
    cam->holdTime  = holdTime;
    cam->downTime  = downTime;
    cam->t         = 0.0f;
    cam->targetT   = 1.0f / frequency;
    cam->strength  = 0.0f;
    cam->offset    = offsetInPixels;
    
    cam->P.x = RandomF32InRange( -1.0f, 1.0f );
    cam->P.y = RandomF32InRange( -1.0f, 1.0f );
    cam->Q.x = RandomF32InRange( -1.0f, 1.0f );
    cam->Q.y = RandomF32InRange( -1.0f, 1.0f );
}

internal void
updateShake( CAMERA_SHAKE * cam, flo32 dt ) {
    flo32 targetTime = cam->upTime + cam->holdTime + cam->downTime;
    
    if( cam->timer < targetTime ) {
        cam->timer += dt;
        
        flo32 timeA = cam->upTime;
        flo32 timeB = timeA + cam->holdTime;
        
        if( cam->timer < timeA ) {
            cam->strength = clamp01( cam->timer / cam->upTime );
        } else if( ( cam->timer >= timeA ) && ( cam->timer < timeB ) ) {
            cam->strength = 1.0f;
        } else {
            cam->strength = 1.0f - clamp01( ( cam->timer - timeB ) / cam->downTime );
        }
        
        cam->t += dt;
        while( cam->t >= cam->targetT ) {
            cam->P = cam->Q;
            cam->Q.x = RandomF32InRange( -1.0f, 1.0f );
            cam->Q.y = RandomF32InRange( -1.0f, 1.0f );
            
            cam->t -= cam->targetT;
        }
    }
}

internal vec2
getOffset( CAMERA_SHAKE * cam ) {
    vec2 result = lerp( cam->P, cam->t, cam->Q ) * cam->strength * cam->offset;
    return result;
}

internal void
moveCamera( APP_STATE * appState, flo32 targetTime, vec2 pos ) {
    CAMERA_STATE * state = &appState->cameraState;
    
    state->pos        = state->dest;
    state->timer      = 0.0f;
    state->targetTime = targetTime;
    
    state->dest = pos + getHalfDim( appState->cameraBound );
}

internal flo32
cameraRel( APP_STATE * appState, flo32 value ) {
    rect bound = appState->cameraBound;
    
    flo32 result = bound.left + value * getHeight( bound );
    return result;
}

internal vec2
cameraRel( APP_STATE * appState, vec2 value ) {
    rect bound  = appState->cameraBound;
    
    vec2 result = getBL( bound ) + value * getHeight( bound );
    return result;
}

internal orect
cameraRel( APP_STATE * appState, orect R ) {
    rect bound = appState->cameraBound;
    
    orect result = R;
    result.center  = getBL( bound ) + result.center * getHeight( bound );
    result.halfDim = result.halfDim * getHeight( bound );
    return result;
}

internal LINE_SEG
cameraRel( APP_STATE * appState, LINE_SEG R ) {
    rect bound = appState->cameraBound;
    
    LINE_SEG result = R;
    result.P  = getBL( bound ) + result.P * getHeight( bound );
    result.Q  = getBL( bound ) + result.Q * getHeight( bound );
    return result;
}

internal LINE_SEG
baseRel( APP_STATE * appState, LINE_SEG R ) {
    flo32 camera_height = getHeight( appState->cameraBound );
    
    LINE_SEG result = R;
    result.P  = appState->world_basePos + result.P * camera_height;
    result.Q  = appState->world_basePos + result.Q * camera_height;
    return result;
}

internal vec2
baseRel( APP_STATE * appState, vec2 V ) {
    vec2 result = appState->world_basePos + V * getHeight( appState->cameraBound );
    return result;
}

internal rect
baseRel( APP_STATE * appState, rect R ) {
    vec2 P = baseRel( appState, getBL( R ) );
    vec2 Q = baseRel( appState, getTR( R ) );
    rect result = rectMM( P, Q );
    return result;
}

internal LINE_SEG
toWorldPos( GAME_CONTROL_STATE * state, LINE_SEG R ) {
    LINE_SEG result = R;
    result.P  = state->world_basePos + result.P * state->world_scale;
    result.Q  = state->world_basePos + result.Q * state->world_scale;
    return result;
}

internal vec2
toWorldPos( GAME_CONTROL_STATE * state, vec2 V ) {
    vec2 result = state->world_basePos + V * state->world_scale;
    return result;
}

internal rect
toWorldPos( GAME_CONTROL_STATE * state, rect R ) {
    vec2 P = toWorldPos( state, getBL( R ) );
    vec2 Q = toWorldPos( state, getTR( R ) );
    rect result = rectMM( P, Q );
    return result;
}

internal orect
toWorldPos( GAME_CONTROL_STATE * state, orect R ) {
    orect result = R;
    result.center   = toWorldPos( state, result.center );
    result.halfDim *= state->world_scale;
    return result;
}