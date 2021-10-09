
internal void
addBreakable( BREAKABLE_STATE * state, BREAKABLE breakable ) {
    Assert( state->nBreakable < BREAKABLE_MAX_COUNT );
    
    for( uint32 index = 0; index < BREAKABLE_MAX_COUNT; index++ ) {
        BREAKABLE fish = state->breakable[ index ];
        if( !fish.isActive ) {
            state->breakable[ index ] = breakable;
            state->nBreakable++;
            break;
        }
    }
}

internal void
damageBreakable( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, uint32 index, uint32 damage, vec2 vel ) {
    BREAKABLE_STATE * state = &appState->breakableState;
    
    BREAKABLE * breakable = state->breakable + index;
    Assert( breakable->isActive );
    
    breakable->timer = 0.0f;
    
    uint32 dmg = minValue( damage, breakable->health );
    breakable->health -= dmg;
    
    if( breakable->health <= 0 ) {
        breakable->collision_isActive = false;
        breakable->velocity = vel * 0.5f;
    }
    
    // TODO: play sound
    // TODO: weak hit sound  vs  strong hit sound
}

internal void
updateBreakable( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, flo32 dt ) {
    BREAKABLE_STATE * state  = &appState->breakableState;
    PLAYER_STATE    * player = &appState->playerState;
    
    if( player->doSlap ) {
        vec2 slapPos = getSlapPos( player );
        for( uint32 iFish = 0; iFish < BREAKABLE_MAX_COUNT; iFish++ ) {
            BREAKABLE * fish = state->breakable + iFish;
            if( ( fish->isActive ) && ( fish->collision_isActive ) ) {
                orect R = fish->collision_bound;
                R.halfDim += Vec2( 20.0f, 20.0f );
                if( isInBound( slapPos, R ) ) {
                    damageBreakable( audioSystem, appState, iFish, 0, Vec2( 0.0f, 0.0f ) );
                }
            }
        }
    }
    
    for( uint32 iFish = 0; iFish < BREAKABLE_MAX_COUNT; iFish++ ) {
        BREAKABLE * fish = state->breakable + iFish;
        if( fish->isActive ) {
            fish->timer += dt;
            if( fish->health <= 0 ) {
                fish->collision_isActive = false;
                
                vec2 accel = -fish->velocity * BREAKABLE_FADE_FRICTION;
                fish->collision_bound.center += accel * ( dt * dt * 0.5f ) + fish->velocity * dt;
                fish->velocity               += accel * dt;
            }
        }
    }
}

internal void
drawBreakable( RENDER_PASS * pass, APP_STATE * appState, BREAKABLE * fish, boo32 showDebug = false ) {
    vec4 modColor = COLOR_WHITE;
    
    vec2 offset = {};
    if( fish->health <= 0 ) {
        flo32 t = clamp01( fish->timer / BREAKABLE_FADE_TARGET_TIME );
        flo32 a = lerp( 0.75f, t, 0.0f );
        modColor = Vec4( a, a, a, a );
    } else {
        if( fish->timer < BREAKABLE_SHAKE_TARGET_TIME ) {
            offset += RandomDirection2D() * ( RandomF32() * BREAKABLE_SHAKE_BITMAP_OFFSET );
        }
    }
    
    orect R = fish->collision_bound;
    R.center += offset;
    drawORect( pass, fish->textureID, R, modColor );
    
    if( fish->health > 0 ) {
        rect health_bound = rectBCD( getBC( fish->collision_bound ) + Vec2( -40.0f, 150.0f ), Vec2( 260.0f, 20.0f ) );
        drawHealthBar( pass, health_bound, fish->health, fish->maxHealth );
    }
}

internal void
drawBreakable( RENDER_PASS * pass, APP_STATE * appState, boo32 showDebug = false ) {
    BREAKABLE_STATE * state = &appState->breakableState;
    
    for( uint32 iFish = 0; iFish < BREAKABLE_MAX_COUNT; iFish++ ) {
        BREAKABLE * fish = state->breakable + iFish;
        if( fish->isActive ) {
            drawBreakable( pass, appState, fish, showDebug );
        }
    }
}

internal void
finalizeBreakable( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
    BREAKABLE_STATE * state = &appState->breakableState;
    
    for( uint32 iFish = 0; iFish < BREAKABLE_MAX_COUNT; iFish++ ) {
        BREAKABLE * fish = state->breakable + iFish;
        if( fish->isActive ) {
            if( ( !fish->collision_isActive ) && ( fish->timer >= BREAKABLE_FADE_TARGET_TIME ) ) {
                fish->isActive = false;
                state->nBreakable--;
            }
        }
    }
}

internal void
spawnBreakable( APP_STATE * appState, TEXTURE_ID textureID, orect bound, uint32 health ) {
    BREAKABLE_STATE * state = &appState->breakableState;
    
    BREAKABLE fish = {};
    fish.isActive           = true;
    fish.collision_isActive = true;
    fish.collision_bound = bound;
    
    fish.textureID = textureID;
    
    fish.maxHealth = health;
    fish.health    = health;
    
    addBreakable( state, fish );
}
