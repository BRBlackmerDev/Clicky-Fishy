
//----------
// SPAWN/BOUND
//----------

internal vec2
RandomSpawnPos( rect bound, flo32 margin ) {
    bound = addRadius( bound, margin );
    bound.bottom += margin;
    
    vec2 dim = getDim( bound );
    
    vec2 result = {};
    
    uint32 side = rand() % 3;
    switch( side ) {
        case 0: {
            result.y = RandomF32() * dim.y;
        } break;
        
        case 1: {
            result.x = RandomF32() * dim.x;
            result.y = dim.y;
        } break;
        
        case 2: {
            result.x = dim.x;
            result.y = RandomF32() * dim.y;
        } break;
    }
    
    result += getBL( bound );
    return result;
}

internal SPAWN
RandomSpawn( rect innerBound, flo32 friction ) {
    rect outerBound = addRadius( innerBound, SPAWN_AND_EXIT_MARGIN );
    outerBound.bottom += SPAWN_AND_EXIT_MARGIN;
    
    vec2 innerDim   = getDim( innerBound );
    vec2 outerDim   = getDim( outerBound );
    
    vec2 spawn = {};
    FACING_DIRECTION facingDir = {};
    
    uint32 side = rand() % 3;
    switch( side ) {
        case 0: {
            spawn.y = RandomF32() * outerDim.y;
        } break;
        
        case 1: {
            spawn.x = RandomF32() * outerDim.x;
            spawn.y = outerDim.y;
        } break;
        
        case 2: {
            spawn.x = outerDim.x;
            spawn.y = RandomF32() * outerDim.y;
            facingDir.isFacingLeft = true;
        } break;
    }
    
    spawn += getBL( outerBound );
    facingDir.transitionX = spawn.x;
    
    vec2 dest = getRandomPoint( innerBound );
    
    vec2 V = dest - spawn;
    vec2 N = getNormal( V );
    
    SPAWN result = {};
    result.position  = spawn;
    result.direction = N;
    result.facingDir = facingDir;
    
    flo32 length    = getLength( V );
    result.speed    = length / SPAWN_TARGET_TIME;
    result.velocity = N * result.speed;
    
    flo32 decelDist = result.speed / friction;
    length = maxValue( length - decelDist, 0.0f );
    result.targetTime = length / result.speed;
    
    return result;
}

internal flo32
minDistToBound( rect bound, vec2 pos, vec2 * outDir ) {
    vec2 dir = *outDir;
    Assert( ( dir.x != 0.0f ) || ( dir.y != 0.0f ) );
    
    rect innerBound = addRadius( bound, -PLAY_BOUND_SAFE_MARGIN );
    
    flo32 x = 0.0f;
    if( dir.x != 0.0f ) {
        if( dir.x < 0.0f ) {
            if( pos.x < innerBound.left ) {
                dir.x = -dir.x;
                x = ( innerBound.right - pos.x ) / dir.x;
            } else {
                x = ( innerBound.left  - pos.x ) / dir.x;
            }
        } else {
            if( pos.x > innerBound.right ) {
                dir.x = -dir.x;
                x = ( innerBound.left  - pos.x ) / dir.x;
            } else {
                x = ( innerBound.right - pos.x ) / dir.x;
            }
        }
    }
    
    flo32 y = 0.0f;
    if( dir.y != 0.0f ) {
        if( dir.y < 0.0f ) {
            if( pos.y < innerBound.bottom ) {
                dir.y = -dir.y;
                y = ( innerBound.top    - pos.y ) / dir.y;
            } else {
                y = ( innerBound.bottom - pos.y ) / dir.y;
            }
        } else {
            if( pos.y > innerBound.top ) {
                dir.y = -dir.y;
                y = ( innerBound.bottom - pos.y ) / dir.y;
            } else {
                y = ( innerBound.top    - pos.y ) / dir.y;
            }
        }
    }
    
    *outDir = dir;
    
    flo32 result = minValue( x, y );
    return result;
}

internal flo32
getDistToBound( rect bound, vec2 pos, vec2 dir ) {
    Assert( ( dir.x != 0.0f ) || ( dir.y != 0.0f ) );
    
    flo32 x = 0.0f;
    if( dir.x != 0.0f ) {
        if( dir.x < 0.0f ) {
            if( pos.x < bound.left ) {
                x = ( bound.right - pos.x ) / dir.x;
            } else {
                x = ( bound.left  - pos.x ) / dir.x;
            }
        } else {
            if( pos.x > bound.right ) {
                x = ( bound.left  - pos.x ) / dir.x;
            } else {
                x = ( bound.right - pos.x ) / dir.x;
            }
        }
    }
    
    flo32 y = 0.0f;
    if( dir.y != 0.0f ) {
        if( dir.y < 0.0f ) {
            if( pos.y < bound.bottom ) {
                y = ( bound.top    - pos.y ) / dir.y;
            } else {
                y = ( bound.bottom - pos.y ) / dir.y;
            }
        } else {
            if( pos.y > bound.top ) {
                y = ( bound.bottom - pos.y ) / dir.y;
            } else {
                y = ( bound.top    - pos.y ) / dir.y;
            }
        }
    }
    
    flo32 result = minValue( x, y );
    return result;
}

//----------
// ITEMS
//----------

internal void
addItem( ITEM_STATE * itemState, GAME_ITEM item ) {
    Assert( itemState->nItem < ITEM_STATE__GAME_ITEM_MAX_COUNT );
    itemState->item[ itemState->nItem++ ] = item;
}

internal void
addItem( ITEM_STATE * itemState, ITEM_ID itemID, vec2 position ) {
    GAME_ITEM item = {};
    item.itemID   = itemID;
    item.position = position;
    addItem( itemState, item );
}

//----------
// FISH ATE
//----------

internal void
addAte( FISH_ATE_LIST * list, FISH_ATE fishAte ) {
    Assert( list->nFishAte < FISH_ATE_LIST_COUNT );
    list->fishAte[ list->nFishAte++ ] = fishAte;
}

internal void
addBit( FISH_ATE_LIST * list, FISH_ATE fishBit ) {
    Assert( list->nFishBit < FISH_ATE_LIST_COUNT );
    list->fishBit[ list->nFishBit++ ] = fishBit;
    list->nBit[ fishBit.type ]++;
}

internal void
addRainbow( FISH_ATE_LIST * list, RAINBOW fish ) {
    list->nAte[ fishType_rainbow ]++;
    
    FISH_ATE fishAte = {};
    fishAte.type     = fishType_rainbow;
    fishAte.position = fish.position;
    fishAte.colorID  = fish.colorID;
    addAte( list, fishAte );
}

internal void
addGoldfish( FISH_ATE_LIST * list, GOLDFISH fish ) {
    list->nAte[ fishType_goldfish ]++;
    
    FISH_ATE fishAte = {};
    fishAte.type     = fishType_goldfish;
    fishAte.position = fish.position;
    fishAte.timer    = fish.lifeTimer;
    addAte( list, fishAte );
}

internal void
addBit( FISH_ATE_LIST * list, DOGFISH fish ) {
    FISH_ATE fishBit = {};
    fishBit.type = fishType_dogfish;
    fishBit.position = fish.position;
    fishBit.health   = fish.health;
    addBit( list, fishBit );
}

internal void
addAte( FISH_ATE_LIST * list, DOGFISH fish ) {
    FISH_ATE fishAte = {};
    fishAte.type     = fishType_dogfish;
    fishAte.position = fish.position;
    addAte( list, fishAte );
}

internal void
addBit( FISH_ATE_LIST * list, SWORDFISH fish ) {
    FISH_ATE fishBit = {};
    fishBit.type     = fishType_swordfish;
    fishBit.position = fish.position;
    fishBit.health   = fish.health;
    addBit( list, fishBit );
}

internal void
addAte( FISH_ATE_LIST * list, SWORDFISH fish ) {
    FISH_ATE fishAte = {};
    fishAte.type     = fishType_dogfish;
    fishAte.position = fish.position;
    addAte( list, fishAte );
}

internal void
addBit( FISH_ATE_LIST * list, ARCHER fish ) {
    FISH_ATE bit = {};
    bit.type     = fishType_archer;
    bit.position = fish.position;
    bit.health   = fish.health;
    addBit( list, bit );
}

internal void
addAte( FISH_ATE_LIST * list, ARCHER fish ) {
    FISH_ATE ate = {};
    ate.type     = fishType_archer;
    ate.position = fish.position;
    addAte( list, ate );
}

internal void
addBit( FISH_ATE_LIST * list, LGARCHER fish ) {
    FISH_ATE bit = {};
    bit.type     = fishType_lgArcher;
    bit.position = fish.position;
    bit.health   = fish.health;
    addBit( list, bit );
}

internal void
addAte( FISH_ATE_LIST * list, LGARCHER fish ) {
    FISH_ATE ate = {};
    ate.type     = fishType_lgArcher;
    ate.position = fish.position;
    addAte( list, ate );
}

internal void
addBit( FISH_ATE_LIST * list, CAGE fish ) {
    FISH_ATE bit = {};
    bit.type     = fishType_cage;
    bit.position = fish.position;
    bit.health   = fish.health;
    addBit( list, bit );
}

internal void
addBit( FISH_ATE_LIST * list, NET fish ) {
    FISH_ATE bit = {};
    bit.type     = fishType_net;
    bit.position = fish.position;
    addBit( list, bit );
}

internal void
reset( FISH_ATE_LIST * list ) {
    memset( list, 0, sizeof( FISH_ATE_LIST ) );
}

//----------
// UPDATE
//----------

internal vec2
getPosOfTarget( APP_STATE * appState, FISH_TYPE type, int32 index ) {
    Assert( type != fishType_null );
    
    vec2 result = {};
    switch( type ) {
        case fishType_player: {
            PLAYER_STATE * player = &appState->playerState;
            result = player->position;
        } break;
        
        case fishType_goldfish: {
            GOLDFISH_STATE * state = &appState->goldfishState;
            GOLDFISH         fish  = state->goldfish[ index ];
            result = fish.position;
        } break;
    }
    
    return result;
}

internal vec2
getRepel( vec2 posA, vec2 posB, flo32 maxStrength, flo32 maxDist ) {
    flo32 repel_strength = maxStrength * maxDist * maxDist;
    
    vec2  vector   = posB - posA;
    flo32 strength = minValue( repel_strength / getLengthSq( vector ), maxStrength );
    vec2  result   = getNormal( vector ) * strength;
    return result;
}

internal void
flip( FACING_DIRECTION * dir, flo32 xPos ) {
    flo32 threshold = 20.0f;
    dir->isFacingLeft = !dir->isFacingLeft;
    
    flo32 offset = ( dir->isFacingLeft ) ? threshold : -threshold;
    dir->transitionX = xPos + offset;
}

internal void
updateFacingDir( FACING_DIRECTION * dir, flo32 xPos ) {
    flo32 threshold = 20.0f;
    if( dir->isFacingLeft ) {
        dir->transitionX = minValue( dir->transitionX, xPos );
        if( ( xPos - dir->transitionX ) > threshold ) {
            dir->isFacingLeft = false;
            dir->transitionX  = xPos;
        }
    } else {
        dir->transitionX = maxValue( dir->transitionX, xPos );
        if( ( dir->transitionX - xPos ) > threshold ) {
            dir->isFacingLeft = true;
            dir->transitionX  = xPos;
        }
    }
}

internal void
updateColorChain( COLOR_CHAIN * colorChain, FISH_ATE_LIST * list ) {
    uint32  fishPoints[] = { 0, 10, 20, 30, 50, 100 };
    uint32 nFishPoints   = _arrayCount( fishPoints ) - 1;
    
    for( uint32 iFish = 0; iFish < list->nFishAte; iFish++ ) {
        FISH_ATE * fish = list->fishAte + iFish;
        
        if( fish->type == fishType_rainbow ) {
            if( fish->colorID == colorChain->color ) {
                colorChain->count = minValue( colorChain->count + 1, nFishPoints );
            } else {
                colorChain->color = fish->colorID;
                colorChain->count = 1;
            }
            
            fish->points = fishPoints[ colorChain->count ];
        }
    }
}

internal void
updateScoring( APP_STATE * appState, FISH_ATE_LIST * list ) {
    if( ( list->attemptBite ) && ( !appState->gameOver ) ) {
        appState->nBites++;
        appState->nThingsAte += list->nFishAte;
        appState->nThingsAte += list->nFishBit;
        
        updateColorChain( &appState->colorChain, list );
        
        for( uint32 iFish = 0; iFish < list->nFishAte; iFish++ ) {
            FISH_ATE * fish = list->fishAte + iFish;
            
            if( fish->type == fishType_goldfish ) {
                fish->points = GOLDFISH_POINTS;
            }
        }
        
        uint32 multiplier = 1;
#if 0
        ITEM_TOOLBAR * toolbar = &appState->itemToolbar;
        for( uint32 iItem = 0; iItem < toolbar->nItem; iItem++ ) {
            ITEM * item = toolbar->item + iItem;
            if( item->id == itemID_pointMultiplier ) {
                multiplier *= item->multiplier;
            }
        }
#endif
        
        for( uint32 iFish = 0; iFish < list->nFishAte; iFish++ ) {
            FISH_ATE * fish = list->fishAte + iFish;
            fish->points *= multiplier;
        }
        
        for( uint32 iFish = 0; iFish < list->nFishAte; iFish++ ) {
            FISH_ATE * fish = list->fishAte + iFish;
            appState->score += fish->points;
            addParticle( &appState->pointsList, fish->points, fish->position );
        }
    }
}

internal void
updateAudio( APP_STATE * appState, AUDIO_SYSTEM * audioSystem, AUDIO_MANAGER * manager, FISH_ATE_LIST * list, flo32 dt ) {
    if( list->attemptBite ) {
        if( ( list->nFishAte == 0 ) && ( list->nFishBit == 0 ) ) {
            playAudio( audioSystem, appState->audio, audioID_bite_fail, 0.5f, 1.0f );
        } else {
            if( list->nFishAte > 0 ) {
                boo32 ate[ fishType_count ] = {};
                
                for( uint32 iFish = 0; iFish < list->nFishAte; iFish++ ) {
                    FISH_ATE fish = list->fishAte[ iFish ];
                    
                    ate[ fish.type ] = true;
                }
                
                if( ate[ fishType_rainbow ] ) {
                    AUDIO_ID audioID = ( AUDIO_ID )( audioID_bite0 + appState->colorChain.count - 1 );
                    playAudio( audioSystem, appState->audio, audioID, 0.5f );
                }
                if( ate[ fishType_goldfish ] ) {
                    playAudio( audioSystem, appState->audio, audioID_bite4, 0.5f );
                }
                
            }
            
            if( list->nFishBit > 0 ) {
                boo32 playSound = 
                    ( list->nBit[ fishType_dogfish ] > 0 ) || ( list->nBit[ fishType_swordfish ] > 0 ) || ( list->nBit[ fishType_archer  ] > 0 ) || ( list->nBit[ fishType_lgArcher  ] > 0 ) || ( list->nBit[ fishType_cage    ] > 0 ) || ( list->nBit[ fishType_net ] > 0 );
                
                if( playSound ) {
                    playAudio( audioSystem, appState->audio, audioID_bite_dogfish, 0.5f );
                }
            }
        }
    }
    
    if( manager->nSlap > 0 ) {
        manager->nSlap = 0;
        AUDIO_ID audioIDA = ( AUDIO_ID )( ( uint32 )audioID_slap01 + RandomU32InRange( 0, 1 ) );
        playAudio( audioSystem, appState->audio, audioIDA, 1.0f );
    }
    
    manager->playerStun_timer += dt;
    if( ( manager->nPlayerStun > 0 ) && ( manager->playerStun_timer >= 0.1f ) ) {
        manager->playerStun_timer = 0.0f;
        
        flo32 volume = 0.7f;
        if( manager->playerStun_timer < 0.25f ) {
            volume = 0.35f;
        }
        
        manager->nPlayerStun = 0;
        playAudio( audioSystem, appState->audio, audioID_swordfish_hitPlayer, volume );
    }
    
    if( manager->nParrotfishBounce > 0 ) {
        manager->nParrotfishBounce = 0;
        
        uint32 index = RandomU32InRange( 0, 1 );
        AUDIO_ID audioID = ( AUDIO_ID )( audioID_bubble_spit01 + index );
        playAudio( audioSystem, appState->audio, audioID, 1.0f );
    }
    
    updateVolume( audioSystem, dt );
}

internal void
quietMusic( AUDIO_SYSTEM * audioSystem ) {
    for( uint32 iAudio = 0; iAudio < audioSystem->nAudio; iAudio++ ) {
        AUDIO_ENTRY * entry = audioSystem->audio + iAudio;
        if( ( entry->audioID >= audioID_music_start ) && ( entry->audioID <= audioID_music_end ) ) {
            setVolume( entry, 0.0f, 2.5f, 0.5f );
        }
    }
}

internal AUDIO_ID
getMusicPlaying( AUDIO_SYSTEM * audioSystem ) {
    boo32 multipleIDs = false;
    AUDIO_ID result   = audioID_music_start;
    
    for( uint32 iAudio = 0; iAudio < audioSystem->nAudio; iAudio++ ) {
        AUDIO_ENTRY audio = audioSystem->audio[ iAudio ];
        if( ( audio.audioID >= audioID_music_start ) && ( audio.audioID <= audioID_music_end ) ) {
            Assert( !multipleIDs );
            multipleIDs = true;
            result      = audio.audioID;
        }
    }
    
    Assert( result != audioID_music_start );
    return result;
}

//----------
// DRAW
//----------

internal void
drawModel( RENDER_OBJECT_LIST * objectList, MODEL_ID modelID, TEXTURE_ID textureID, mat4 transform, vec4 modColor ) {
    RENDER_OBJECT object = {};
    object.modelID   = modelID;
    object.textureID = textureID;
    object.transform = transform;
    object.modColor  = modColor;
    
    Assert( objectList->nObjects < objectList->maxObjects );
    objectList->object[ objectList->nObjects++ ] = object;
}

internal void
drawModel( RENDER_PASS * pass, MODEL_ID modelID, TEXTURE_ID textureID, vec2 position, vec2 scale, flo32 radians, vec4 modColor ) {
    vec2 xAxis = Vec2( cosf( radians ), sinf( radians ) );
    vec2 yAxis = getPerp( xAxis );
    xAxis *= scale.x;
    yAxis *= scale.y;
    
    mat4 transform = mat4_translate( position, 0.0f ) * mat4_column( Vec3( xAxis, 0.0f ), Vec3( yAxis, 0.0f ), Vec3( 0.0f, 0.0f, 1.0f ) );
    drawModel( &pass->triObjectList, modelID, textureID, transform, modColor );
}

internal void
drawModel( RENDER_OBJECT_LIST * objectList, flo32 depth, MODEL_ID modelID, TEXTURE_ID textureID, mat4 transform, vec4 modColor ) {
    RENDER_OBJECT object = {};
    object.depth     = depth;
    object.modelID   = modelID;
    object.textureID = textureID;
    object.transform = transform;
    object.modColor  = modColor;
    
    Assert( objectList->nObjects < objectList->maxObjects );
    objectList->object[ objectList->nObjects++ ] = object;
}

internal void
drawModel( RENDER_PASS * pass, MODEL_ID modelID, TEXTURE_ID textureID, vec2 position, vec2 scale, flo32 radians, vec4 modColor, flo32 depth ) {
    vec2 xAxis = Vec2( cosf( radians ), sinf( radians ) );
    vec2 yAxis = getPerp( xAxis );
    xAxis *= scale.x;
    yAxis *= scale.y;
    
    mat4 transform = mat4_translate( position, 0.0f ) * mat4_column( Vec3( xAxis, 0.0f ), Vec3( yAxis, 0.0f ), Vec3( 0.0f, 0.0f, 1.0f ) );
    drawModel( &pass->triObjectList, depth, modelID, textureID, transform, modColor );
}

internal void
drawQuad( RENDER_PASS * pass, TEXTURE_ID textureID, vec2 position, vec2 xAxis, vec2 yAxis, vec4 modColor ) {
    mat4 transform = mat4_translate( position, 0.0f ) * mat4_column( Vec3( xAxis, 0.0f ), Vec3( yAxis, 0.0f ), Vec3( 0.0f, 0.0f, 1.0f ) );
    drawModel( &pass->triObjectList, modelID_rect, textureID, transform, modColor );
}

internal void
drawQuadOutline( RENDER_PASS * pass, vec2 position, vec2 xAxis, vec2 yAxis, vec4 modColor ) {
    mat4 transform = mat4_translate( position, 0.0f ) * mat4_column( Vec3( xAxis, 0.0f ), Vec3( yAxis, 0.0f ), Vec3( 0.0f, 0.0f, 1.0f ) );
    drawModel( &pass->lineObjectList, modelID_rectOutline, textureID_whiteTexture, transform, modColor );
}

internal void
drawQuad( RENDER_PASS * pass, TEXTURE_ID textureID, vec2 position, flo32 radians, vec2 scale, vec4 modColor ) {
    vec2 xAxis = Vec2( cosf( radians ), sinf( radians ) );
    vec2 yAxis = getPerp( xAxis );
    
    xAxis *= scale.x;
    yAxis *= scale.y;
    
    drawQuad( pass, textureID, position, xAxis, yAxis, modColor );
}

internal void
drawQuad( RENDER_PASS * pass, TEXTURE_ID textureID, vec2 position, vec2 scale, vec4 modColor ) {
    mat4 transform = mat4_translate( position, 0.0f ) * mat4_scale( scale, 1.0f );
    drawModel( &pass->triObjectList, modelID_rect, textureID, transform, modColor );
}

internal void
drawQuadOutline( RENDER_PASS * pass, vec2 position, vec2 scale, vec4 modColor ) {
    mat4 transform = mat4_translate( position, 0.0f ) * mat4_scale( scale, 1.0f );
    drawModel( &pass->lineObjectList, modelID_rectOutline, textureID_whiteTexture, transform, modColor );
}

internal void
drawRect( RENDER_PASS * pass, TEXTURE_ID textureID, rect bound, vec4 color ) {
    vec2 position = getCenter ( bound );
    vec2 scale    = getHalfDim( bound );
    drawQuad( pass, textureID, position, scale, color );
}

internal void
drawRect( RENDER_PASS * pass, rect bound, vec4 color ) {
    drawRect( pass, textureID_whiteTexture, bound, color );
}

internal void
drawORect( RENDER_PASS * pass, TEXTURE_ID textureID, orect bound, vec4 color ) {
    vec2 xAxis = bound.xAxis * bound.halfDim.x;
    vec2 yAxis = bound.yAxis * bound.halfDim.y;
    drawQuad( pass, textureID, bound.center, xAxis, yAxis, color );
}

internal void
drawORect( RENDER_PASS * pass, orect bound, vec4 color ) {
    drawORect( pass, textureID_whiteTexture, bound, color );
}

internal void
drawPoint( RENDER_PASS * pass, vec2 position, vec4 color ) {
    rect bound = rectCD( position, Vec2( 4.0f, 4.0f ) );
    drawRect( pass, textureID_whiteTexture, bound, color );
}

internal void
drawRectOutline( RENDER_PASS * pass, rect bound, vec4 color ) {
    vec2 position = getCenter ( bound );
    vec2 scale    = getHalfDim( bound );
    drawQuadOutline( pass, position, scale, color );
}

internal void
drawORectOutline( RENDER_PASS * pass, orect bound, vec4 color ) {
    vec2 xAxis = bound.xAxis * bound.halfDim.x;
    vec2 yAxis = bound.yAxis * bound.halfDim.y;
    drawQuadOutline( pass, bound.center, xAxis, yAxis, color );
}

internal void
drawLine( RENDER_PASS * pass, vec2 A, vec2 B, vec4 color ) {
    vec2 xAxis = B - A;
    vec2 yAxis = getPerp( xAxis );
    
    mat4 transform = mat4_translate( A, 0.0f ) *  mat4_column( Vec3( xAxis, 0.0f ), Vec3( yAxis, 0.0f ), Vec3( 0.0f, 0.0f, 1.0f ) );
    drawModel( &pass->lineObjectList, modelID_line, textureID_whiteTexture, transform, color );
}

internal void
drawCircle( RENDER_PASS * pass, vec2 center, flo32 radius, vec4 color ) {
    mat4 transform = mat4_translate( center, 0.0f ) * mat4_scale( radius, radius, 1.0f );
    drawModel( &pass->triObjectList, modelID_circle, textureID_whiteTexture, transform, color );
}

internal void
drawCircleOutline( RENDER_PASS * pass, vec2 position, flo32 radius, vec4 modColor ) {
    mat4 transform = mat4_translate( position, 0.0f ) * mat4_scale( radius, radius, 1.0f );
    drawModel( &pass->lineObjectList, modelID_circleOutline, textureID_whiteTexture, transform, modColor );
}

internal void
drawCircleOutline( RENDER_PASS * pass, circ C, vec4 modColor ) {
    mat4 transform = mat4_translate( C.center, 0.0f ) * mat4_scale( C.radius, C.radius, 1.0f );
    drawModel( &pass->lineObjectList, modelID_circleOutline, textureID_whiteTexture, transform, modColor );
}

internal void
drawCapsuleOutline( RENDER_PASS * pass, CAPSULE2 cap, vec4 modColor ) {
    vec2 xAxis = getNormal( cap.Q - cap.P );
    vec2 yAxis = getPerp( xAxis );
    yAxis *= cap.radius;
    
    drawLine( pass, cap.P + yAxis, cap.Q + yAxis, modColor );
    drawLine( pass, cap.P - yAxis, cap.Q - yAxis, modColor );
    drawCircleOutline( pass, cap.P, cap.radius, modColor );
    drawCircleOutline( pass, cap.Q, cap.radius, modColor );
}

//----------
// DEBUG
//----------

internal void
addToDebug( DEBUG__LAST_BITE_FRAME * debug, DEBUG__LAST_BITE_FRAME__FISH fish ) {
    boo32 findSlot = false;
    if( debug->nFish < DEBUG__LAST_BITE_FRAME__FISH_COUNT ) {
        debug->nFish++;
        findSlot = true;
    } else if( fish.distToPlayerSq < debug->fish[ debug->nFish - 1 ].distToPlayerSq ) {
        findSlot = true;
    }
    
    if( findSlot ) {
        uint32 atIndex = debug->nFish - 1;
        while( ( atIndex > 0 ) && ( fish.distToPlayerSq < debug->fish[ atIndex - 1 ].distToPlayerSq ) ) {
            debug->fish[ atIndex ] = debug->fish[ atIndex - 1 ];
        }
        debug->fish[ atIndex ] = fish;
    }
}

//----------
// EXPLOSION PARTICLES
//----------

#define EXPLOSION_PARTICLE_TARGET_TIME     ( 0.1f )
#define EXPLOSION_PARTICLE_BITMAP_SCALE_X  ( 640.0f )
#define EXPLOSION_PARTICLE_BITMAP_SCALE_Y  ( 640.0f )

internal void
addParticle( EXPLOSION_PARTICLE_LIST * particleList, TEXTURE_ID textureID, vec2 position, vec2 scale, vec2 drift ) {
    EXPLOSION_PARTICLE particle = {};
    
    particle.textureID = textureID;
    particle.position  = position;
    particle.drift     = drift;
    particle.scale     = scale * 0.5f;
    particle.radians   = RandomF32() * ( PI * 2.0f );
    
    Assert( particleList->nParticles < EXPLOSION_PARTICLE_LIST__PARTICLE_COUNT );
    particleList->particle[ particleList->nParticles++ ] = particle;
}

internal void
addExplosion( EXPLOSION_PARTICLE_LIST * list, vec2 swordfishPos, vec2 playerPos ) {
    vec2 expPos = lerp( playerPos, 0.8f, swordfishPos );
    expPos.y += 60.0f;
    
    uint32 index = rand() % 3;
    TEXTURE_ID textureID = ( TEXTURE_ID )( textureID_explosion01 + index );
    
    vec2 scale = 
        Vec2( EXPLOSION_PARTICLE_BITMAP_SCALE_X, EXPLOSION_PARTICLE_BITMAP_SCALE_Y );
    vec2 drift = getNormal( expPos - playerPos );
    addParticle( list, textureID, expPos, scale, drift );
}

internal void
addMedExplosion( EXPLOSION_PARTICLE_LIST * list, vec2 position ) {
    vec2 scale  = Vec2( 200.0f, 200.0f );
    addParticle( list, textureID_smExplosion01, position, scale, Vec2( 0.0f, 0.0f ) );
}

internal void
addSmExplosion( EXPLOSION_PARTICLE_LIST * list, vec2 swordfishPos, vec2 playerPos ) {
    vec2 expPos = lerp( playerPos, 0.6f, swordfishPos );
    vec2 scale  = Vec2( 120.0f, 120.0f );
    addParticle( list, textureID_smExplosion01, expPos, scale, Vec2( 0.0f, 0.0f ) );
}

internal void
addSmExplosion( EXPLOSION_PARTICLE_LIST * list, vec2 pos ) {
    vec2 scale  = Vec2( 120.0f, 120.0f );
    addParticle( list, textureID_smExplosion01, pos, scale, Vec2( 0.0f, 0.0f ) );
}

internal void
addPop( EXPLOSION_PARTICLE_LIST * list, vec2 swordfishPos, vec2 playerPos ) {
    vec2 expPos = lerp( playerPos, 0.6f, swordfishPos );
    vec2 scale  = Vec2( 20.0f, 20.0f );
    addParticle( list, textureID_smExplosion01, expPos, scale, Vec2( 0.0f, 0.0f ) );
}

internal void
addPop( EXPLOSION_PARTICLE_LIST * list, vec2 position ) {
    vec2 scale  = Vec2( 20.0f, 20.0f );
    addParticle( list, textureID_smExplosion01, position, scale, Vec2( 0.0f, 0.0f ) );
}

internal void
updateExplosionParticles( EXPLOSION_PARTICLE_LIST * list, flo32 dt ) {
    for( uint32 iPar = 0; iPar < list->nParticles; ) {
        EXPLOSION_PARTICLE * par = list->particle + iPar;
        par->timer += dt;
        
        vec2  dPos   = par->drift * ( 80.0f * dt );
        flo32 dScale = 200.0f * dt;
        flo32 dRadians = ( ( ( PI * 2.0f ) / 10.0f ) * dt );
        
        par->position += dPos;
        par->scale    += Vec2( dScale, dScale );
        par->radians  += dRadians;
        
        if( par->timer > EXPLOSION_PARTICLE_TARGET_TIME ) {
            list->particle[ iPar ] = list->particle[ --list->nParticles ];
        } else {
            iPar++;
        }
    }
}

internal void
drawExplosionParticles( RENDER_PASS * pass, EXPLOSION_PARTICLE_LIST * list ) {
    for( uint32 iPar = 0; iPar < list->nParticles; iPar++ ) {
        EXPLOSION_PARTICLE par = list->particle[ iPar ];
        
        vec2 xAxis = Vec2( cosf( par.radians ), sinf( par.radians ) );
        vec2 yAxis = getPerp( xAxis );
        
        xAxis *= par.scale.x;
        yAxis *= par.scale.y;
        
        drawQuad( pass, par.textureID, par.position, xAxis, yAxis, COLOR_WHITE );
    }
}

//----------
// HEALTH
//----------

internal void
drawHealthBar( RENDER_PASS * pass, rect outer, int32 health, int32 maxHealth ) {
    flo32 t = 1.0f - clamp01( ( flo32 )health / ( flo32 )maxHealth );
    rect  inner = outer;
    inner.right = lerp( outer.right, t, outer.left );
    
    drawRect( pass, textureID_whiteTexture, outer, COLOR_BLACK );
    drawRect( pass, textureID_whiteTexture, inner, COLOR_RED );
}

//----------
// movement functions
//----------

internal flo32
distToStop( vec2 velocity, flo32 friction, flo32 fps ) {
    vec2 start = {};
    vec2 atPos = start;
    
    vec2  vel = velocity;
    flo32 dt  = 1.0f / fps;
    
    flo32 epsilon   = 0.0001f;
    flo32 epsilonSq = epsilon * epsilon;
    while( getLengthSq( vel ) > epsilonSq ) {
        vec2 accel = -vel * friction;
        vec2 dPos  = accel * ( dt * dt * 0.5f ) + vel * dt;
        atPos += dPos;
        vel   += accel * dt;
    }
    
    flo32 result = getLength( atPos - start );
    return result;
}

internal vec2
destAtStop( vec2 start, vec2 velocity, flo32 friction, flo32 fps ) {
    vec2 atPos = start;
    
    vec2  vel = velocity;
    flo32 dt  = 1.0f / fps;
    
    flo32 epsilon   = 0.0001f;
    flo32 epsilonSq = epsilon * epsilon;
    while( getLengthSq( vel ) > epsilonSq ) {
        vec2 accel = -vel * friction;
        vec2 dPos  = accel * ( dt * dt * 0.5f ) + vel * dt;
        atPos += dPos;
        vel   += accel * dt;
    }
    
    return atPos;
}

internal vec2
adjustDirToBound( vec2 pos, vec2 dir, flo32 length, rect bound ) {
    vec2 dest = pos + dir * length;
    if( ( dest.x < bound.left   ) || ( dest.x > bound.right ) ) { dir.x = -dir.x; }
    if( ( dest.y < bound.bottom ) || ( dest.y > bound.top   ) ) { dir.y = -dir.y; }
    vec2 result = dir;
    return result;
}

//----------
// SPRING
//----------

struct SPRING_COEF {
    // P0 = a * P + b * V + dest
    // V0 = c * P + d * V;
    flo32 a;
    flo32 b;
    flo32 c;
    flo32 d;
};

internal SPRING_COEF
getSpringCoef( flo32 w, flo32 t ) {
    flo32 e = expf( -w * t );
    flo32 f = e * t;
    flo32 g = w * f;
    
    SPRING_COEF result = {};
    result.a = g + e;
    result.b = f;
    result.c = -w * g;
    result.d = -g + e;
    return result;
}

#if 0
{
    SPRING_COEF h = getSpringCoef( 180.0f, dt );
    flo32 r = appState->camera_hRadians - appState->camera_hRadians_dest;
    flo32 v = appState->camera_hRadians_vel;
    
    appState->camera_hRadians     = h.a * r + h.b * v + appState->camera_hRadians_dest;
    appState->camera_hRadians_vel = h.c * r + h.d * v;
}
#endif

//----------
// output text file
//----------

internal void
writeSegment( MEMORY * memory, char * str ) {
    uint32 length = ( uint32 )strlen( str );
    _writeb( memory, str, length );
}

internal void
writeF32( MEMORY * memory, flo32 value ) {
    char str[ 32 ] = {};
    sprintf( str, "%f", value );
    writeSegment( memory, str );
    writeSegment( memory, "f" );
}