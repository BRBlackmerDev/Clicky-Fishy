

internal vec3
screenCoordToWorld( vec2 pos, vec2 dim, mat4 camera_inverse ) {
    flo32 tX = ( pos.x / dim.x ) * 2.0f - 1.0f;
    flo32 tY = ( pos.y / dim.y ) * 2.0f - 1.0f;
    
    vec4 nearPoint = camera_inverse * Vec4( tX, tY, -1.0f, 1.0f );
    vec3 result = nearPoint.xyz / nearPoint.w;
    
    return result;
}

internal void
addShow( SECRET_STATE * secretState, SECRET_ID secretID ) {
    Assert( secretState->nShow < SECRET_STATE__SHOW_COUNT );
    secretState->show[ secretState->nShow++ ] = secretID;
}

internal void
initSecret( APP_STATE * appState, SECRET_ID secretID ) {
    SECRET_STATE * state = &appState->secretState;
    switch( secretID ) {
        case secretID__eat_all_fish_of_one_color_during_school_wave: {
            memset( state->school_nFishOfColor, 0, sizeof( uint32 ) * colorID_count );
            memset( state->school_nFishAte,     0, sizeof( uint32 ) * colorID_count );
            
            RAINBOW_STATE * rainbowState = &appState->rainbowState;
            for( uint32 iFish = 0; iFish < RAINBOW_MAX_COUNT; iFish++ ) {
                RAINBOW fish = rainbowState->rainbow[ iFish ];
                if( fish.isActive ) {
                    state->school_nFishOfColor[ fish.colorID ]++;
                }
            }
        } break;
    }
}

internal void
updateSecrets( APP_STATE * appState, FISH_ATE_LIST * list, flo32 dt ) {
    SECRET_STATE * state = &appState->secretState;
    SECRET_ID currID = {};
    
    currID = secretID__eat_3_or_more_fish_in_one_bite;
    if( ( !state->wasFound[ currID ] ) && ( list->nFishAte >= 3 ) ) {
        state->wasFound[ currID ] = true;
        addShow( state, currID );
    }
    
    currID = secretID__eat_1_of_each_color_fish_in_a_row;
    if( !state->wasFound[ currID ] ) {
        state->rainbowChain_nFishAte += list->nFishAte;
        if( state->rainbowChain_nFishAte >= colorID_count ) {
            for( uint32 iFish = 0; iFish < list->nFishAte; iFish++ ) {
                FISH_ATE fish = list->fishAte[ iFish ];
                if( fish.type == fishType_rainbow ) {
                    state->rainbowChain_color[ state->rainbowChain_atIndex ] = fish.colorID;
                    state->rainbowChain_atIndex = ( state->rainbowChain_atIndex + 1 ) % colorID_count;
                }
            }
            
            boo32 wasAte[ colorID_count ] = {};
            boo32 isCompleted = true;
            for( uint32 iFish = 0; ( isCompleted ) && ( iFish < colorID_count ); iFish++ ) {
                COLOR_ID colorID = state->rainbowChain_color[ iFish ];
                if( wasAte[ colorID ] ) {
                    isCompleted = false;
                } else {
                    wasAte[ colorID ] = true;
                }
            }
            if( isCompleted ) {
                state->wasFound[ currID ] = true;
                addShow( state, currID );
            }
        }
    }
    
    currID = secretID__eat_all_fish_of_one_color_during_school_wave;
    if( ( !state->wasFound[ currID ] ) && ( state->school_isActive ) ) {
        for( uint32 iFish = 0; iFish < list->nFishAte; iFish++ ) {
            FISH_ATE fish = list->fishAte[ iFish ];
            if( fish.type == fishType_rainbow ) {
                state->school_nFishAte[ fish.colorID ]++;
            }
        }
        
        boo32 isCompleted = false;
        for( uint32 iColor = 0; iColor < colorID_count; iColor++ ) {
            if( state->school_nFishOfColor[ iColor ] == state->school_nFishAte[ iColor ] ) {
                isCompleted = true;
            }
        }
        
        if( isCompleted ) {
            state->wasFound[ currID ] = true;
            addShow( state, currID );
        }
    }
    
    currID = secretID__eat_a_goldfish_immediately_after_it_spawns;
    if( !state->wasFound[ currID ] ) {
        boo32 isCompleted = false;
        for( uint32 iFish = 0; iFish < list->nFishAte; iFish++ ) {
            FISH_ATE fish = list->fishAte[ iFish ];
            
            if( ( fish.type == fishType_goldfish ) && ( fish.timer <= SECRET_GOLDFISH_SPAWN_TARGET_TIME ) ) {
                isCompleted = true;
            }
        }
        
        if( isCompleted ) {
            state->wasFound[ currID ] = true;
            addShow( state, currID );
        }
    }
    
    currID = secretID__eat_a_second_goldfish_immediately_after_eating_one;
    if( !state->wasFound[ currID ] ) {
        state->goldfish_secondTimer += dt;
        
        if( list->nAte[ fishType_goldfish ] > 0 ) {
            if( state->goldfish_secondTimer <= SECRET_SECOND_GOLDFISH_TARGET_TIME ) {
                state->wasFound[ currID ] = true;
                addShow( state, currID );
            }
            state->goldfish_secondTimer = 0.0f;
        }
    }
    
    currID = secretID__snipe_a_goldfish;
    if( !state->wasFound[ currID ] ) {
        state->goldfish_snipeTimer += dt;
        
        if( ( list->nAte[ fishType_goldfish ] > 0 ) && ( state->goldfish_snipeTimer >= SECRET_GOLDFISH_SNIPE_TARGET_TIME ) ) {
            state->wasFound[ currID ] = true;
            addShow( state, currID );
        }
        
        if( list->attemptBite ) {
            state->goldfish_snipeTimer = 0.0f;
        }
    }
    
    currID = secretID__bite_two_or_more_dogfish_in_one_bite;
    if( !state->wasFound[ currID ] ) {
        if( list->nBit[ fishType_dogfish ] >= 2 ) {
            state->wasFound[ currID ] = true;
            addShow( state, currID );
        }
    }
    
    currID = secretID__dodge_20_dogfish_in_20_seconds;
    if( !state->wasFound[ currID ] ) {
        PLAYER_STATE * playerState = &appState->playerState;
        // TODO: There's an issue with this! The dodge_timer only resets if the player bites or is bit. If the player swims around for a while without biting or being bit, then its like adding a handicap to the dodge timer. We should probably keep track of the last 20 dodges and the times. If the time between the first and last dodges is less than 20 seconds, then we award the secret.
        if( ( playerState->nDodges >= 20 ) && ( playerState->dodge_timer <= 20.0f ) ) {
            state->wasFound[ currID ] = true;
            addShow( state, currID );
        }
    }
}

#if 0
internal void
addItem( ITEM_TOOLBAR * toolbar, ITEM item ) {
    Assert( toolbar->nItem < ITEM_TOOLBAR_ITEM_COUNT );
    
    item.timer = 0.0f;
    toolbar->item[ toolbar->nItem++ ] = item;
}
#endif

internal flo32
getTargetTime( ITEM_ID id ) {
    flo32 targetTime[ itemID_count ] = {};
    targetTime[ itemID_pointMultiplier ] = 6.0f;
    targetTime[ itemID_slowDownTime    ] = 6.0f;
    targetTime[ itemID_strongBite      ] = 12.0f;
    
    flo32 result = targetTime[ id ];
    return result;
}

#if 0
internal void
updateDebug( APP_STATE * appState, vec2 player_pos, MEMORY * tempMemory ) {
    if( appState->nFish > 0 ) {
        DEBUG_LAST_BITE_DATA data = {};
        data.player_pos        = player_pos;
        data.player_facingLeft = appState->player_facingDir.isFacingLeft;
        
        FISH * fish = _pushArray( tempMemory, FISH, appState->nFish );
        memcpy( fish, appState->fish, sizeof( FISH ) * appState->nFish );
        
        boo32 isSorted = false;
        while( !isSorted ) {
            isSorted = true;
            
            for( uint32 iFish = 0; iFish < ( appState->nFish - 1 ); iFish++ ) {
                uint32 indexA = iFish;
                uint32 indexB = iFish + 1;
                
                FISH fishA = fish[ indexA ];
                FISH fishB = fish[ indexB ];
                
                flo32 distSqA = getLengthSq( fishA.position - player_pos );
                flo32 distSqB = getLengthSq( fishB.position - player_pos );
                
                if( distSqB < distSqA ) {
                    _swap( FISH, fish[ indexA ], fish[ indexB ] );
                    isSorted = false;
                }
            }
        }
        
        uint32 count = minValue( appState->nFish, ( uint32 )DEBUG_LAST_BITE_DATA_FISH_COUNT );
        for( uint32 iFish = 0; iFish < count; iFish++ ) {
            FISH fish0 = fish[ iFish ];
            
            data.fish_pos       [ iFish ] = fish0.position;
            data.fish_facingLeft[ iFish ] = fish0.facingDir.isFacingLeft;
            data.fish_colorID   [ iFish ] = fish0.colorID;
        }
        data.nFish = count;
        
        appState->debug_lastBite.data[ appState->debug_lastBite.index ] = data;
        appState->debug_lastBite.index = ( appState->debug_lastBite.index + 1 ) % DEBUG_LAST_BITE_DATA_COUNT;
        
        _popArray( tempMemory, FISH, appState->nFish );
    }
}
#endif

internal void
APP_updateAndRender( APP_STATE * appState, PLATFORM * platform ) {
    RENDERER * renderer = &platform->renderer;
    RENDER_PASS * pass_game = &renderer->pass_game;
    RENDER_PASS * pass_ui   = &renderer->pass_ui;
    
	MOUSE_STATE    * mouse    = &platform->mouse;
	KEYBOARD_STATE * keyboard = &platform->keyboard;
	
	MEMORY * permMemory = &platform->permMemory;
	MEMORY * tempMemory = &platform->tempMemory;
	
    AUDIO_SYSTEM * audioSystem = &platform->audioSystem;
	//ASSET_SYSTEM * assetSystem = &platform->assetSystem;
    
    flo32 dt        = platform->targetSec;
    flo32 dt_global = platform->targetSec;
    
    if( ( appState->gameOver ) && ( appState->gameOver_timer < GAME_OVER_SLOW_TARGET_TIME ) ) {
        dt *= 0.5f;
        // TODO: temporarily slow down time, resume normal time after death animation
    }
    //dt *= 0.1f;
    //dt *= 0.25f;
    if( wasPressed( keyboard, keyCode_o ) ) {
        appState->isPaused = !appState->isPaused;
    }
    if( appState->isPaused ) { dt = 0.0f; }
    
    //DISPLAY_VALUE( uint64, tempMemory->used );
	
	if( !appState->isInitialized ) {
		appState->isInitialized = true;
        
        loadTextures( platform );
        
        appState->audio[ audioID_music ] = WAV_readFile( platform, "../../art/music01.wav" );
        appState->audio[ audioID_music_battle01 ] = WAV_readFile( platform, "../../art/music_battle01.wav" );
        appState->audio[ audioID_music_underwaterAmbience ] = WAV_readFile( platform, "../../art/music_underwaterAmbience.wav" );
        appState->audio[ audioID_music_village_level01 ] = WAV_readFile( platform, "../../art/music_village_level01.wav" );
        
        appState->audio[ audioID_bite_fail ] = WAV_readFile( platform, "../../art/sound_bite_fail.wav" );
        appState->audio[ audioID_bite0 ]     = WAV_readFile( platform, "../../art/sound_bite01.wav" );
        appState->audio[ audioID_bite1 ]     = WAV_readFile( platform, "../../art/sound_bite02.wav" );
        appState->audio[ audioID_bite2 ]     = WAV_readFile( platform, "../../art/sound_bite03.wav" );
        appState->audio[ audioID_bite3 ]     = WAV_readFile( platform, "../../art/sound_bite04.wav" );
        appState->audio[ audioID_bite4 ]     = WAV_readFile( platform, "../../art/sound_maxColorBonus.wav" );
        appState->audio[ audioID_scoreMultiplier ] = WAV_readFile( platform, "../../art/sound_multiplier.wav" );
        appState->audio[ audioID_bite_dogfish ] = WAV_readFile( platform, "../../art/sound_bite_dogfish.wav" );
        
        appState->audio[ audioID_player_parry01 ] = WAV_readFile( platform, "../../art/sound_player_parry01.wav" );
        appState->audio[ audioID_player_parry02 ] = WAV_readFile( platform, "../../art/sound_player_parry02.wav" );
        appState->audio[ audioID_player_parry03 ] = WAV_readFile( platform, "../../art/sound_player_parry03.wav" );
        appState->audio[ audioID_player_death ] = WAV_readFile( platform, "../../art/sound_player_death.wav" );
        
        appState->audio[ audioID_dogfish_biting01 ] = WAV_readFile( platform, "../../art/sound_dogfish_biting01.wav" );
        appState->audio[ audioID_dogfish_biting02 ] = WAV_readFile( platform, "../../art/sound_dogfish_biting02.wav" );
        appState->audio[ audioID_dogfish_biting03 ] = WAV_readFile( platform, "../../art/sound_dogfish_biting03.wav" );
        appState->audio[ audioID_dogfish_biting04 ] = WAV_readFile( platform, "../../art/sound_dogfish_biting04.wav" );
        appState->audio[ audioID_dogfish_biting05 ] = WAV_readFile( platform, "../../art/sound_dogfish_biting05.wav" );
        
        appState->audio[ audioID_dogfish_lunge01 ] = WAV_readFile( platform, "../../art/sound_dogfish_lunge01.wav" );
        appState->audio[ audioID_dogfish_lunge02 ] = WAV_readFile( platform, "../../art/sound_dogfish_lunge02.wav" );
        appState->audio[ audioID_dogfish_lunge03 ] = WAV_readFile( platform, "../../art/sound_dogfish_lunge03.wav" );
        appState->audio[ audioID_dogfish_lunge04 ] = WAV_readFile( platform, "../../art/sound_dogfish_lunge04.wav" );
        appState->audio[ audioID_dogfish_lunge05 ] = WAV_readFile( platform, "../../art/sound_dogfish_lunge05.wav" );
        appState->audio[ audioID_dogfish_lunge06 ] = WAV_readFile( platform, "../../art/sound_dogfish_lunge06.wav" );
        appState->audio[ audioID_dogfish_lunge07 ] = WAV_readFile( platform, "../../art/sound_dogfish_lunge07.wav" );
        appState->audio[ audioID_dogfish_lunge08 ] = WAV_readFile( platform, "../../art/sound_dogfish_lunge08.wav" );
        appState->audio[ audioID_dogfish_lunge09 ] = WAV_readFile( platform, "../../art/sound_dogfish_lunge09.wav" );
        appState->audio[ audioID_dogfish_lunge10 ] = WAV_readFile( platform, "../../art/sound_dogfish_lunge10.wav" );
        appState->audio[ audioID_dogfish_lunge11 ] = WAV_readFile( platform, "../../art/sound_dogfish_lunge11.wav" );
        
        appState->audio[ audioID_swordfish_parry_lo ] = WAV_readFile( platform, "../../art/sound_swordfish_parry_lo.wav" );
        appState->audio[ audioID_swordfish_parry_hi ] = WAV_readFile( platform, "../../art/sound_swordfish_parry_hi.wav" );
        appState->audio[ audioID_swordfish_stab01 ] = WAV_readFile( platform, "../../art/sound_swordfish_stab01.wav" );
        appState->audio[ audioID_swordfish_stab02 ] = WAV_readFile( platform, "../../art/sound_swordfish_stab02.wav" );
        appState->audio[ audioID_swordfish_slash01 ] = WAV_readFile( platform, "../../art/sound_swordfish_slash01.wav" );
        appState->audio[ audioID_swordfish_slash02 ] = WAV_readFile( platform, "../../art/sound_swordfish_slash02.wav" );
        appState->audio[ audioID_swordfish_hitPlayer ] = WAV_readFile( platform, "../../art/sound_swordfish_hitPlayer.wav" );
        appState->audio[ audioID_swordfish_block01 ] = WAV_readFile( platform, "../../art/sound_swordfish_block01.wav" );
        appState->audio[ audioID_swordfish_block02 ] = WAV_readFile( platform, "../../art/sound_swordfish_block02.wav" );
        appState->audio[ audioID_swordfish_block03 ] = WAV_readFile( platform, "../../art/sound_swordfish_block03.wav" );
        appState->audio[ audioID_pop01 ] = WAV_readFile( platform, "../../art/sound_pop01.wav" );
        appState->audio[ audioID_pop02 ] = WAV_readFile( platform, "../../art/sound_pop02.wav" );
        appState->audio[ audioID_spit01 ] = WAV_readFile( platform, "../../art/sound_spit01.wav" );
        appState->audio[ audioID_spit02 ] = WAV_readFile( platform, "../../art/sound_spit02.wav" );
        appState->audio[ audioID_spit03 ] = WAV_readFile( platform, "../../art/sound_spit03.wav" );
        appState->audio[ audioID_bubble_spit01 ] = WAV_readFile( platform, "../../art/sound_bubble_spit01.wav" );
        appState->audio[ audioID_bubble_spit02 ] = WAV_readFile( platform, "../../art/sound_bubble_spit02.wav" );
        appState->audio[ audioID_bubble_capture ] = WAV_readFile( platform, "../../art/sound_bubble_capture.wav" );
        appState->audio[ audioID_slap01 ] = WAV_readFile( platform, "../../art/sound_slap01.wav" );
        appState->audio[ audioID_slap02 ] = WAV_readFile( platform, "../../art/sound_slap02.wav" );
        
        AUDIO_ID musicID = audioID_music;
        musicID = audioID_music_village_level01;
        
#if BOSS_FIGHT
        musicID = audioID_music_battle01;
#endif
        
        //musicID = audioID_music_underwaterAmbience;
        //playMusic( musicSystem, appState->audio[ musicID ], 0.30f, 1.0f );
        //playAudio( audioSystem, appState->audio, musicID, 0.30f, 1.0f, true );
        audioSystem->mainVolume = 1.0f;
        appState->musicPlaying  = musicID;
        
        rect bound = appState->app_bound;
        bound.right = 138.0f;
        appState->panel_bound = bound;
        
        rect play_bound = appState->app_bound;
        play_bound.left = bound.right;
        appState->init_cameraDim = getDim( play_bound );
        
        appState->play_bound = play_bound;
        appState->play_dim   = getDim( play_bound );
        
        GAME_CONTROL_STATE * controlState = &appState->gameControlState;
        controlState->memory = subArena( permMemory, _MB( 1 ) );
        
        ANIMATION_SYSTEM * animationSystem = &appState->animationSystem;
        animationSystem->stringBuffer = StringBuffer( permMemory, 256, _MB( 1 ) );
        
        DEBUG_FRAME_SYSTEM * debug = &appState->debugFrameSystem;
        debug->memory = subArena( permMemory );
        
        vec2 center = getCenter( appState->play_bound );
        appState->mouse_baseX = ( int32 )center.x;
        appState->mouse_baseY = ( int32 )center.y;
        appState->mouse_base = Vec2( ( flo32 )appState->mouse_baseX, ( flo32 )appState->mouse_baseY );
        {
            int32 X = appState->mouse_baseX;
            int32 Y = 1080 - 1 - appState->mouse_baseY;
            SetCursorPos( X, Y );
            
            mouse->position     = center;
            mouse->prevPosition = center;
        }
        
        //startGame( tempMemory, appState, audioSystem, false );
        initHome( audioSystem, appState );
    }
    
    switch( appState->gameMode ) {
        case gameMode_home: {
            updateHome  ( audioSystem, appState, keyboard, mouse, dt );
            drawHome    ( renderer,    appState, mouse );
            finalizeHome( tempMemory, audioSystem, appState );
            // update player
            // update storage
            // update exit
            // draw background
            // draw player
            // draw storage
            // draw exit
            
            appState->resetMousePos = true;
        } break;
        
        case gameMode_game: {
            reset( &appState->fishAteList );
            
            //DEBUG__SWORDFISH * debug = &appState->debugState.swordfish;
            DEBUG_FRAME_SYSTEM * debug = &appState->debugFrameSystem;
            boo32 UpdateGame = !appState->isPaused;
            boo32 UpdateAndDrawGame = ( !appState->isPaused ) && ( !debug->show );
            
            if( UpdateAndDrawGame ) {
                if( UpdateGame ) {
                    { // update rate
                        flo32 rate = 1.0f;
                        
#if 0
                        ITEM_TOOLBAR * toolbar = &appState->itemToolbar;
                        for( uint32 iItem = 0; iItem < toolbar->nItem; iItem++ ) {
                            ITEM item = toolbar->item[ iItem ];
                            
                            if( item.itemID == itemID_slowDownTime ) {
                                rate *= 0.8f;
                            }
                        }
#endif
                        
                        //setMusicRate( musicSystem, rate );
                        dt *= rate;
                    }
                    
                    updateEventTrigger( appState );
                    updateGameControl ( appState, dt );
                    
                    { // update play_bound
                        CAMERA_STATE * camera = &appState->cameraState;
                        PLAYER_STATE * player = &appState->playerState;
                        
                        appState->camera_isMoving = false;
                        //DISPLAY_VALUE( boo32, camera->springIsEnabled );
                        if( camera->springIsEnabled ) {
                            SPRING_COEF coef = getSpringCoef( 1.25f, dt );
                            
                            rect bound = addDim( appState->cameraBound, Vec2( -440.0f, -360.0f ) );
                            //drawRectOutline( pass_game, bound, COLOR_RED );
                            
                            vec2 offset = {};
                            if( !isInBound( player->position, bound ) ) {
                                if( player->position.x < bound.left ) {
                                    offset.x = player->position.x - bound.left;
                                }
                                if( player->position.x > bound.right ) {
                                    offset.x = player->position.x - bound.right;
                                }
                                if( player->position.y < bound.bottom ) {
                                    offset.y = player->position.y - bound.bottom;
                                }
                                if( player->position.y > bound.top ) {
                                    offset.y = player->position.y - bound.top;
                                }
                                camera->dest = camera->at + offset * 2.0f;
                                camera->dest = clampToBound( camera->dest, appState->world_cameraBound );
                                
                            }
                            //drawRectOutline( pass_game, appState->world_cameraBound, COLOR_GREEN );
                            
                            //rect R = addDim( appState->world_cameraBound, Vec2( 880.0f, 720.0f ) );
                            //drawRectOutline( pass_game, R, COLOR_RED );
                            
                            vec2 pos  = camera->at;
                            vec2 dest = camera->dest;
                            
                            vec2 r = pos - dest;
                            vec2 v = camera->vel;
                            
                            camera->at  = coef.a * r + coef.b * v + dest;
                            camera->vel = coef.c * r + coef.d * v;
                            
                            //DISPLAY_VALUE( vec2, camera->at );
                            //DISPLAY_VALUE( vec2, camera->vel );
                            //DISPLAY_VALUE( vec2, camera->dest );
                            
                            //DISPLAY_VALUE( vec2, player->position );
                            vec2 V = ( player->position - appState->world_basePos ) / getHeight( appState->cameraBound );
                            //DISPLAY_VALUE( vec2, V );
                            
                            appState->cameraBound = rectCD( camera->at, camera->dim );
                        } else {
                            camera->timer += dt;
                            
                            vec2 cameraPrev = camera->at;
                            
                            flo32 t = clamp01( camera->timer / camera->targetTime );
                            camera->at = lerp( camera->pos, t, camera->dest );
                            
                            appState->cameraBound = rectCD( camera->at, camera->dim );
                            
                            if( appState->entityUpdate_isEnabled ) {
                                vec2 camera_dPos = camera->at - cameraPrev;
                                if( camera->pos != camera->dest ) {
                                    appState->camera_isMoving = true;
                                    appState->camera_dPos     = camera_dPos;
                                } else {
                                    appState->camera_isMoving = false;
                                }
                                
                                if( ( camera_dPos.x != 0.0f ) || ( camera_dPos.y != 0.0f ) ) {
                                    updatePlayerCollision( appState, camera_dPos );
                                }
                            }
                            
                            //DISPLAY_VALUE( vec2, camera->at );
                            //DISPLAY_VALUE( rect, appState->cameraBound );
                        }
                    }
                    
                    updateWorldSystem ( appState, dt );
                    if( appState->entityUpdate_isEnabled ) {
                        // TODO: should there be parts of player update that are enabled during camera movement?
                        updatePlayerInput( audioSystem, appState, mouse );
                        deflectUrchinSpine( audioSystem, appState );
                        updatePlayer   ( audioSystem, appState, dt );
                        updateItems    ( audioSystem, appState );
                        updateInventory( audioSystem, appState, mouse, dt );
                        
                        { // update score particles
                            SCORE_PARTICLE_LIST * particleList = &appState->pointsList;
                            for( uint32 iParticle = 0; iParticle < particleList->nParticles; ) {
                                SCORE_PARTICLE * particle = particleList->particle + iParticle;
                                particle->timer += dt;
                                if( particle->timer > SCORE_PARTICLE_TARGET_TIME ) {
                                    particleList->particle[ iParticle ] = particleList->particle[ --particleList->nParticles ];
                                } else {
                                    iParticle++;
                                }
                            }
                        } // END update score particles
                        
                        updateExplosionParticles( &appState->explosionList, dt );
                        
#if 0
                        { // update items toolbar
                            ITEM_TOOLBAR * toolbar = &appState->itemToolbar;
                            
                            for( uint32 iItem = 0; iItem < toolbar->nItem; ) {
                                ITEM * item = toolbar->item + iItem;
                                
                                item->timer += dt;
                                if( item->timer >= getTargetTime( item->id ) ) {
                                    toolbar->nItem--;
                                    for( uint32 iter = iItem; iter < toolbar->nItem; iter++ ) {
                                        toolbar->item[ iter ] = toolbar->item[ iter + 1 ];
                                    }
                                } else {
                                    iItem++;
                                }
                            }
                        }
#endif
                        
                        updateAnimation( audioSystem, appState, dt );
                        
                        updateRainbow ( appState, dt );
                        updateGoldfish( appState, dt );
                        updateDogfish    ( audioSystem, appState, dt );
                        updateSwordfish  ( audioSystem, appState, dt );
                        updateUrchin     ( audioSystem, appState, dt );
                        updateUrchinSpine( appState, dt );
                        updateArcher     ( audioSystem, appState, dt );
                        updateBubblet    ( audioSystem, appState, dt );
                        updatelgArcher   ( audioSystem, appState, dt );
                        updateBubble     ( audioSystem, appState, dt );
                        updateBubblonGen ( audioSystem, appState, dt );
                        updateBubblon    ( audioSystem, appState, dt );
                        updateParrotfish ( audioSystem, appState, dt );
                        updateTurtle     ( audioSystem, appState, dt );
                        updatesmTurtle   ( audioSystem, appState, dt );
                        updateCage       ( audioSystem, appState, dt );
                        updateNet        ( audioSystem, appState, dt );
                        updateDefendZone ( audioSystem, appState, dt );
                        updateTower      ( audioSystem, appState, dt );
                        updateBreakable  ( audioSystem, appState, dt );
                        
                        
                        updateFacingDir( &appState->playerState.facingDir, appState->playerState.position.x );
                        updateScoring( appState, &appState->fishAteList );
                        updateSecrets( appState, &appState->fishAteList, dt );
                        updateAudio( appState, audioSystem, &appState->audioManager, &appState->fishAteList, dt );
                        
                        updateShake( &appState->cameraState.cameraShake, dt );
                    }
                    
#if 0
                    {
                        // TODO: This clamps the mouse pointer to play_bound, meaning that the mouse cannot move over the item panel. This will need to change later when we add camera zooming or if we want to activate things in the item panel with the mouse.
                        PLAYER_STATE * player = &appState->playerState;
                        CAMERA_STATE * camera = &appState->cameraState;
                        
                        appState->setMousePos = true;
                        appState->newMousePos = getCenter( appState->play_bound ) + player->position - camera->at;
                    }
#endif
                } // END UpdateGame
                
                //captureSwordfishDebug( appState );
                captureDebugFrame( appState );
                
                { // draw entities
                    flo32 modt     = 1.0f;
                    vec4  modColor = COLOR_WHITE;
                    if( appState->gameOver ) {
                        appState->gameOver_timer += dt_global;
                        flo32 t   = clamp01( ( appState->gameOver_timer - GAME_OVER_BACKGROUND_COLOR_DELAY_TARGET_TIME ) / ( GAME_OVER_BACKGROUND_COLOR_TARGET_TIME ) );
                        flo32 rgb = lerp( 1.0f, t, 0.4f );
                        
                        modt     = 1.0f - t;
                        modColor = COLOR_GRAY( rgb );
                    }
                    
#if 0
                    { // draw background
                        CAMERA_STATE * camera = &appState->cameraState;
                        
                        vec2 halfDim = Vec2( 1024.0f, 1024.0f );
                        vec2 offset  = Vec2( halfDim.x * 2.0f, 0.0f );
                        
                        flo32 xAdvance = appState->backgroundPos.x + offset.x * 5.0f;
                        if( camera->at.x > xAdvance ) {
                            appState->backgroundPos.x += ( offset.x * 4.0f );
                        }
                        
                        // TODO: Temporary! Background will be drawn differently when we start creating the actual large background.
                        vec2 pos = appState->backgroundPos;
                        for( uint32 iter = 0; iter < 8; iter++ ) {
                            TEXTURE_ID textureID = ( TEXTURE_ID )( ( uint32 )textureID_background_left + ( iter % 4 ) );
                            drawQuad( pass_game, textureID, pos, halfDim, COLOR_WHITE * modColor );
                            
                            pos += offset;
                        }
                    }
#endif
                    
                    drawBackground ( pass_game, appState );
                    drawGameControl( pass_game, appState );
                    drawAnimation  ( pass_game, appState );
                    drawWorldSystem( pass_game, appState );
                    
                    drawDefendZone ( pass_game, appState, &appState->defendZoneState );
                    drawTower      ( pass_game, appState );
                    drawObstacle   ( pass_game, appState );
                    drawBreakable  ( pass_game, appState );
                    drawEventTrigger( pass_game, appState );
                    drawHeal       ( pass_game, appState );
                    drawTurtle     ( pass_game, appState, &appState->turtleState );
                    drawExplosionParticles( pass_game, &appState->explosionList );
                    drawCage       ( pass_game, appState );
                    drawNet        ( pass_game, appState );
                    drawPlayer     ( pass_game, appState, &appState->playerState   );
                    drawRainbow    ( pass_game, appState );
                    drawGoldfish   ( pass_game, appState );
                    drawDogfish    ( pass_game, appState );
                    drawSwordfish  ( pass_game, appState );
                    drawUrchin     ( pass_game, appState );
                    drawUrchinSpine( pass_game, appState );
                    drawArcher     ( pass_game, appState );
                    drawBubblet    ( pass_game, appState );
                    drawlgArcher   ( pass_game, appState );
                    drawBubble     ( pass_game, appState );
                    drawBubblon    ( pass_game, appState );
                    drawParrotfish ( pass_game, appState );
                    drawsmTurtle   ( pass_game, appState );
                    
                    if( appState->gameOver ) {
                        RENDER_OBJECT_LIST * objectList = &pass_game->triObjectList;
                        for( uint32 iObject = 0; iObject < objectList->nObjects; iObject++ ) {
                            RENDER_OBJECT * object = objectList->object + iObject;
                            if( ( object->textureID >= textureID_entity_start ) && ( object->textureID <= textureID_entity_end ) ) {
                                object->modColor *= COLOR_GRAY( modt );
                            }
                        }
                        
                        drawPlayer( pass_game, appState, &appState->playerState );
                    }
                    
                    if( appState->cameraState.cameraShake.strength > 0.0f ) {
                        vec2 offset    = getOffset( &appState->cameraState.cameraShake );
                        mat4 transform = mat4_translate( offset );
                        
                        RENDER_OBJECT_LIST * objectList = &pass_game->triObjectList;
                        for( uint32 iObject = 0; iObject < objectList->nObjects; iObject++ ) {
                            RENDER_OBJECT * object = objectList->object + iObject;
                            
                            object->transform = transform * object->transform;
                        }
                    }
                } // END draw entities
                
                { // draw score particles
                    FONT * font = appState->font;
                    
                    SCORE_PARTICLE_LIST * particleList = &appState->pointsList;
                    for( uint32 iParticle = 0; iParticle < particleList->nParticles; iParticle++ ) {
                        SCORE_PARTICLE particle = particleList->particle[ iParticle ];
                        
                        char str[ 24 ] = {};
                        sprintf( str, "%u", particle.value );
                        drawString( pass_game, font, str, particle.position, textAlign_center, COLOR_WHITE );
                    }
                } // END draw score particles
                
                drawItems( pass_game, appState );
                
                { // draw score
                    FONT * font = appState->font;
                    
                    vec2 basePos = getTR( appState->play_bound ) + Vec2( -20.0f, -20.0f );
                    drawString( pass_ui, appState->font, "SCORE:", basePos, textAlign_topRight, COLOR_WHITE );
                    
                    char str[ 128 ] = {};
                    sprintf( str, "%u", appState->score );
                    drawString( pass_ui, font, str, basePos + newLine( font ), textAlign_topRight, COLOR_WHITE );
                } // END draw score
                
                drawInventory( pass_ui, appState );
                
                { // draw health bar
                    PLAYER_STATE * player = &appState->playerState;
                    
                    vec2 atPos = getTR( appState->panel_bound ) + Vec2( 12.0f, -4.0f );
                    drawString( pass_ui, appState->font, "HEALTH", atPos, textAlign_topLeft, COLOR_WHITE );
                    
                    vec2 outerDim = Vec2( 200.0f, 12.0f );
                    vec2 innerDim = outerDim;
                    innerDim.x = player->health * outerDim.x;
                    
                    atPos += Vec2( -6.0f, -18.0f );
                    drawRect( pass_ui, textureID_whiteTexture, rectTLD( atPos, outerDim ), COLOR_BLACK );
                    drawRect( pass_ui, textureID_whiteTexture, rectTLD( atPos, innerDim ), COLOR_RED );
                } // END draw health bar
                
#if 0
                { // draw item toolbar
                    ITEM_TOOLBAR * toolbar = &appState->itemToolbar;
                    //DISPLAY_VALUE( uint32, toolbar->nItem );
                    
                    const char * label[ itemID_count ] = {
                        "2x",
                        "SLOW",
                        "BITE",
                    };
                    
                    vec2 dim = Vec2( ITEM_TOOLBAR_ITEM_DIM, ITEM_TOOLBAR_ITEM_DIM );
                    
                    vec2 atPos = getTR( appState->itemPanel.bound ) + Vec2( 0.0f, -64.0f );
                    for( uint32 iItem = 0; iItem < toolbar->nItem; iItem++ ) {
                        ITEM item = toolbar->item[ iItem ];
                        
                        rect bound = addRadius( rectTLD( atPos, dim ), -2.0f );
                        drawRect( pass_ui, textureID_whiteTexture, bound, COLOR_BLACK );
                        
                        flo32 t = 1.0f - clamp01( item.timer / getTargetTime( item.itemID ) );
                        
                        rect timeLeft_bound = bound;
                        timeLeft_bound.top = timeLeft_bound.bottom + getHeight( bound ) * t;
                        drawRect( pass_ui, textureID_whiteTexture, timeLeft_bound, COLOR_GREEN );
                        
                        switch( item.itemID ) {
                            case itemID_pointMultiplier: {
                                char str[ 4 ] = {};
                                sprintf( str, "%ux", item.multiplier );
                                drawString( pass_ui, appState->font, str, getCenter( bound ), textAlign_center, COLOR_BLACK );
                            } break;
                            
                            default: {
                                drawString( pass_ui, appState->font, label[ item.itemID ], getCenter( bound ), textAlign_center, COLOR_BLACK );
                            } break;
                        }
                        
                        atPos.x += dim.x;
                    }
                } // END draw item toolbar
#endif
                
                uint32 gameOver_renderObject_start = 0;
                uint32 gameOver_renderText_start   = 0;
                { // draw secrets
                    SECRET_STATE * state = &appState->secretState;
                    
                    const char * label[ secretID_count ] = {
                        "CHUBBY BUNNY! Eat 3 or more fish in one bite!",
                        "RAINBOW SAMPLER! Eat 1 fish of each color in a row!",
                        "FINICKY PALATE! Eat all fish of one color during Rainbow School!",
                        
                        "NOT EVEN WAITIN' FOR IT TO COOL OFF! Eat a goldfish immediately after it spawns!",
                        "GOIN' FOR SECONDS! Eat a second goldfish immediately after eating one!",
                        "REFINEMENT AND PRECISION! Snipe a goldfish! Wait for at least 5 seconds, then eat a goldfish in one bite!",
                        
                        "FEARLESS! Bite two or more dogfish on the tail in one bite!",
                        "PLAYFUL CHASE! Dodge the dogfish 20 times in 20 seconds without biting or being bit!",
                    };
                    
                    if( state->nShow > 0 ) {
                        SECRET_ID showID = state->show[ 0 ];
                        
                        state->show_timer += dt;
                        if( state->show_timer >= SECRET_SHOW_TARGET_TIME ) {
                            state->show_timer = 0.0f;
                            
                            state->nShow--;
                            for( uint32 iShow = 0; iShow < state->nShow; iShow++ ) {
                                state->show[ iShow ] = state->show[ iShow + 1 ];
                            }
                        }
                        
                        FONT * font = appState->font;
                        
                        vec2 dim   = appState->play_dim * Vec2( 0.4f, 0.1f );
                        rect bound = rectBCD( getBC( appState->play_bound ) + Vec2( 0.0f, 20.0f ), dim );
                        drawRect( pass_ui, textureID_whiteTexture, bound, COLOR_WHITE );
                        drawString( pass_ui, font, "NEW SECRET!", getTC( bound ) + Vec2( 0.0f, -10.0f ), textAlign_topCenter, COLOR_BLACK );
                        drawString( pass_ui, font, label[ showID ], getCenter( bound ), textAlign_center, COLOR_BLACK );
                    }
                    
                    if( appState->gameOver ) {
                        gameOver_renderObject_start = pass_ui->triObjectList.nObjects;
                        gameOver_renderText_start   = pass_ui->textList.nObjects;
                        
                        uint32 nFound = 0;
                        for( uint32 iSecret = 0; iSecret < secretID_count; iSecret++ ) {
                            if( state->wasFound[ iSecret ] ) {
                                nFound++;
                            }
                        }
                        
                        vec2 atPos = getTR( appState->play_bound ) + Vec2( -100.0f, -100.0f );
                        
                        vec2 dimA = Vec2( 350.0f, 100.0f );
                        vec2 dimB = Vec2( 440.0f, 60.0f );
                        vec2 offset = Vec2( ( dimB.x - dimA.x ) * 0.5f, 30.0f );
                        
                        rect panelA = rectTRD( atPos, dimA );
                        
                        drawRect( pass_ui, textureID_whiteTexture, panelA, COLOR_WHITE );
                        
                        char str[ 128 ] = {};
                        sprintf( str, "Found: %u/%u Secrets!", nFound, secretID_count );
                        drawString( pass_ui, appState->font, str, getCenter( panelA ), textAlign_center, COLOR_BLACK );
                        
                        atPos.x += offset.x;
                        atPos.y -= ( dimA.y + offset.y );
                        for( uint32 iSecret = 0; iSecret < secretID_count; iSecret++ ) {
                            rect bound = rectTRD( atPos, dimB );
                            drawRect( pass_ui, textureID_whiteTexture, bound, COLOR_WHITE );
                            
                            if( state->wasFound[ iSecret ] ) {
                                drawString( pass_ui, appState->font, label[ iSecret ], getCenter( bound ), textAlign_center, COLOR_BLACK );
                            } else {
                                drawString( pass_ui, appState->font, "??????", getCenter( bound ), textAlign_center, COLOR_BLACK );
                            }
                            
                            atPos.y -= ( dimB.y + offset.y );
                        }
                        
                    }
                } // END draw secrets
                
                if( appState->entityUpdate_isEnabled ) {
                    finalizeAnimation( audioSystem, appState );
                    finalizeHeal       ( audioSystem, appState );
                    finalizePlayer     ( audioSystem, appState, dt );
                    finalizeRainbow    ( appState );
                    finalizeGoldfish   ( appState );
                    finalizeDogfish    ( audioSystem, appState );
                    finalizeSwordfish  ( audioSystem, appState, dt );
                    finalizeUrchin     ( audioSystem, appState );
                    finalizeUrchinSpine( appState );
                    finalizeArcher     ( audioSystem, appState );
                    finalizeBubblet    ( audioSystem, appState );
                    finalizelgArcher   ( audioSystem, appState );
                    finalizeBubble     ( audioSystem, appState );
                    finalizeBubblon    ( audioSystem, appState );
                    finalizeParrotfish ( audioSystem, appState );
                    finalizeTurtle     ( audioSystem, appState );
                    finalizesmTurtle   ( audioSystem, appState );
                    finalizeCage       ( audioSystem, appState );
                    finalizeNet        ( audioSystem, appState );
                    finalizeDefendZone ( audioSystem, appState );
                    finalizeTower      ( audioSystem, appState );
                    finalizeBreakable  ( audioSystem, appState );
                    finalizeItems   ( audioSystem, appState );
                    playerDeath( audioSystem, appState );
                }
                
                { // draw game over
                    if( appState->gameOver ) {
                        // TODO: white flash on player death
                        if( wasPressed( keyboard, keyCode_r ) ) {
                            appState->restartGame_isActive = true;
                        }
                        if( wasReleased( keyboard, keyCode_r ) ) {
                            appState->restartGame_isActive = false;
                            appState->restartGame_timer = 0.0f;
                        }
                        if( wasPressed( keyboard, keyCode_c ) ) {
                            appState->restartCheckpoint_isActive = true;
                        }
                        if( wasReleased( keyboard, keyCode_c ) ) {
                            appState->restartCheckpoint_isActive = false;
                            appState->restartCheckpoint_timer = 0.0f;
                        }
                        
                        if( appState->gameOver_timer >= GAME_OVER_DELAY_STATS_TARGET_TIME ) {
                            if( appState->restartCheckpoint_isActive ) {
                                appState->restartCheckpoint_timer = clamp01( appState->restartCheckpoint_timer + dt_global );
                            } else if( appState->restartGame_isActive ) {
                                appState->restartGame_timer = clamp01( appState->restartGame_timer + dt_global );
                            }
                        }
                        
                        vec2 dim   = Vec2( 700.0f, 300.0f );
                        rect bound = rectCD( getCenter( appState->play_bound ) + Vec2( 0.0f, -50.0f ), dim );
                        drawRect( pass_ui, textureID_whiteTexture, bound, COLOR_WHITE );
                        
                        vec2 P = getCenter( bound );
                        
                        char str[ 128 ] = {};
                        sprintf( str, "SCORE: %u", appState->score );
                        drawString( pass_ui, appState->font, str, P, textAlign_center, Vec2( 3.0f, 3.0f ), COLOR_BLACK );
                        drawString( pass_ui, appState->font, "GAME OVER!", P + Vec2( 0.0f, -60.0f ), textAlign_center, Vec2( 2.0f, 2.0f ), COLOR_BLACK );
                        drawString( pass_ui, appState->font, "Hold down 'C' to go back to the last checkpoint.", P + Vec2( 0.0f, -100.0f ), textAlign_center, Vec2( 1.5f, 1.5f ), COLOR_BLACK );
                        drawString( pass_ui, appState->font, "Hold down 'R' to restart the game.", P + Vec2( 0.0f, -140.0f ), textAlign_center, Vec2( 1.5f, 1.5f ), COLOR_BLACK );
                        
                        flo32 accuracy = 0.0f;
                        if( appState->nBites > 0 ) {
                            accuracy = ( flo32 )appState->nThingsAte / ( flo32 )appState->nBites;
                        }
                        
                        sprintf( str, "Accuracy: %u Things Ate : %u Bites  %u%%", appState->nThingsAte, appState->nBites, ( uint32 )( accuracy * 100.0f ) );
                        drawString( pass_ui, appState->font, str, getBL( bound ) + Vec2( 10.0f, 10.0f ), textAlign_bottomLeft, COLOR_BLACK );
                        
                        if( appState->restartCheckpoint_isActive ) {
                            vec2 dim0   = Vec2( 80.0f, 40.0f );
                            rect bound0 = rectTCD( getBC( bound ) + Vec2( 0.0f, -10.0f ), dim0 );
                            
                            vec2 dim1   = dim0;
                            dim1.x     *= appState->restartCheckpoint_timer;
                            rect bound1 = rectTLD( getTL( bound0 ), dim1 );
                            
                            drawRect( pass_ui, textureID_whiteTexture, bound0, COLOR_WHITE );
                            drawRect( pass_ui, textureID_whiteTexture, bound1, COLOR_YELLOW );
                            drawString( pass_ui, appState->font, "Checkpoint", getCenter( bound0 ), textAlign_center, COLOR_BLACK );
                        } else if( appState->restartGame_isActive ) {
                            vec2 dim0   = Vec2( 80.0f, 40.0f );
                            rect bound0 = rectTCD( getBC( bound ) + Vec2( 0.0f, -10.0f ), dim0 );
                            
                            vec2 dim1   = dim0;
                            dim1.x     *= appState->restartGame_timer;
                            rect bound1 = rectTLD( getTL( bound0 ), dim1 );
                            
                            drawRect( pass_ui, textureID_whiteTexture, bound0, COLOR_WHITE );
                            drawRect( pass_ui, textureID_whiteTexture, bound1, COLOR_YELLOW );
                            drawString( pass_ui, appState->font, "Restart", getCenter( bound0 ), textAlign_center, COLOR_BLACK );
                        }
                        
                        uint32 gameOver_renderObject_end = pass_ui->triObjectList.nObjects;
                        uint32 gameOver_renderText_end   = pass_ui->textList.nObjects;
                        
                        { // fade in game over stats
                            flo32 t = clamp01( ( appState->gameOver_timer - GAME_OVER_DELAY_STATS_TARGET_TIME ) / ( GAME_OVER_FADE_IN_STATS_TARGET_TIME ) );
                            vec4 modColor = Vec4( t );
                            
                            uint32 startA = gameOver_renderObject_start;
                            uint32 endA   = gameOver_renderObject_end;
                            uint32 startB = gameOver_renderText_start;
                            uint32 endB   = gameOver_renderText_end;
                            
                            RENDER_OBJECT_LIST * objectList = &pass_ui->triObjectList;
                            for( uint32 iObject = startA; iObject < endA; iObject++ ) {
                                RENDER_OBJECT * object = objectList->object + iObject;
                                object->modColor *= modColor;
                            }
                            
                            TEXT_RENDER_OBJECT_LIST * textList = &pass_ui->textList;
                            for( uint32 iText = startB; iText < endB; iText++ ) {
                                TEXT_RENDER_OBJECT * text = textList->object + iText;
                                text->modColor *= modColor;
                            }
                        }
                        
                        if( appState->restartGame_timer >= 1.0f ) {
                            startGame( tempMemory, appState, audioSystem, false );
                        }
                        if( appState->restartCheckpoint_timer >= 1.0f ) {
                            startGame( tempMemory, appState, audioSystem, true );
                        }
                    }
                } // END draw game over
                
                if( wasPressed( keyboard, keyCode_r, ( KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) ) {
                    startGame( tempMemory, appState, audioSystem, false );
                }
            } // END UpdateAndDrawGame
            
            drawDebugFrame( renderer, appState, mouse, keyboard );
            
            if( wasPressed( keyboard, keyCode_d, ( KEYBOARD_FLAGS__CONTROL | KEYBOARD_FLAGS__SHIFT ) ) ) {
                //DEBUG__SWORDFISH * debug = &appState->debugState.swordfish;
                //debug->show  = !debug->show;
                //debug->pause = false;
                
                debug->show  = !debug->show;
                debug->pause = false;
            }
            
            appState->resetMousePos = !debug->show;
            
            if( wasPressed( keyboard, keyCode_escape ) ) {
                appState->exitToHome_timer    = 0.0f;
                appState->exitToHome_isActive = true;
            }
            if( wasReleased( keyboard, keyCode_escape ) ) {
                appState->exitToHome_isActive = false;
            }
            if( appState->exitToHome_isActive ) {
                appState->exitToHome_timer += dt_global;
                if( appState->exitToHome_timer >= EXIT_TO_HOME__TARGET_TIME ) {
                    appState->exitToHome_isActive = false;
                    appState->gameMode = gameMode_home;
                    moveStorageToInventory( appState );
                    initHome( audioSystem, appState );
                }
                
                vec2 pos   = getBC( appState->play_bound ) + Vec2( 0.0f, 120.0f );
                vec2 dim   = Vec2( 600.0f, 120.0f );
                rect bound = rectBCD( pos, dim );
                drawRect( pass_ui, bound, COLOR_WHITE );
                
                drawString( pass_ui, appState->font, "Returning home...", getTC( bound ) + Vec2( 0.0f, -8.0f ), textAlign_topCenter, Vec2( 2.0f, 2.0f ), COLOR_BLACK );
                
                rect R = rectBCD( getBC( bound ) + Vec2( 0.0f, 20.0f ), Vec2( 400.0f, 32.0f ) );
                drawRect( pass_ui, R, COLOR_BLACK );
                
                rect S = R;
                flo32 t = clamp01( appState->exitToHome_timer / EXIT_TO_HOME__TARGET_TIME );
                S.right = lerp( R.left, t, R.right );
                drawRect( pass_ui, S, COLOR_YELLOW );
            }
        } break;
    }
}