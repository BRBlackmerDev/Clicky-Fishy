
internal BACKGROUND
Background( MODEL_ID modelID, TEXTURE_ID textureID, vec2 position, vec2 scale, flo32 radians, vec4 modColor, flo32 depth = 0.0f ) {
    BACKGROUND result = {};
    result.modelID   = modelID;
    result.textureID = textureID;
    result.position  = position;
    result.scale     = scale;
    result.radians   = radians;
    result.modColor  = modColor;
    result.depth     = depth;
    return result;
}

internal void
addBackground( APP_STATE * appState, BACKGROUND background ) {
    BACKGROUND_STATE * state = &appState->backgroundState;
    
    Assert( state->nBackground < BACKGROUND_MAX_COUNT );
    state->background[ state->nBackground++ ] = background;
}


#if 0
internal void
unloadBackgroundGroup( APP_STATE * appState, uint32 backID ) {
    BACKGROUND_STATE * state = &appState->backgroundState;
    Assert( state->nGroup > 0 );
    
    int32 index = -1;
    for( uint32 iGroup = 0; iGroup < state->nGroup; iGroup++ ) {
        BACKGROUND_GROUP group = state->group[ iGroup ];
        if( group.backID == backID ) {
            index = iGroup;
        }
    }
    
    if( index > -1 ) {
        BACKGROUND_GROUP remove = state->group[ index ];
        
        if( ( uint32 )index < ( state->nGroup - 1 ) ) {
            for( uint32 iBack = remove.bBack + remove.nBack; iBack < state->nBackground; iBack++ ) {
                state->background[ iBack - remove.nBack ] = state->background[ iBack ];
            }
        }
        state->nBackground -= remove.nBack;
        
        for( uint32 iGroup = index + 1; iGroup < state->nGroup; iGroup++ ) {
            BACKGROUND_GROUP * group = state->group + iGroup;
            group->bBack -= remove.nBack;
        }
        
        state->nGroup--;
        for( uint32 iGroup = index; iGroup < state->nGroup; iGroup++ ) {
            state->group[ iGroup ] = state->group[ iGroup + 1 ];
        }
    }
}
#endif

internal void
drawBackground( RENDER_PASS * pass, APP_STATE * appState, BACKGROUND back, boo32 showDebug = false ) {
    drawModel( pass, back.modelID, back.textureID, back.position, back.scale, back.radians, back.modColor, back.depth );
    
    if( showDebug ) {
    }
}

internal void
drawBackground( RENDER_PASS * pass, APP_STATE * appState, boo32 showDebug = false ) {
    BACKGROUND_STATE * state = &appState->backgroundState;
    
    for( uint32 iBack = 0; iBack < state->nBackground; iBack++ ) {
        BACKGROUND back = state->background[ iBack ];
        drawBackground( pass, appState, back, showDebug );
    }
}