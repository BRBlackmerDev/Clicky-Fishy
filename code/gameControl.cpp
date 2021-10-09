
internal void
reset( GAME_CONTROL_STATE * state ) {
    state->prevEventIndex       = -1;
    state->chainID              = -1;
    state->tempValueForSubChain = -1;
    state->nEvent = 0;
    memset( state->event, 0, sizeof( GAME_CONTROL__EVENT ) * GAME_CONTROL__EVENT_MAX_COUNT );
    reset( &state->memory );
}

internal void
firstEvent( GAME_CONTROL_STATE * state, vec2 offset = Vec2( 0.825f, 0.5f ) ) {
    state->firstEvent_pos   = toWorldPos( state, offset );
    state->firstEvent_index = state->nEvent;
    state->firstEvent_springIsEnabled = state->camera_springIsEnabled;
    state->firstEvent_cameraBound     = state->camera_bound;
    state->firstEvent_basePos         = state->world_basePos;
}

internal GAME_CONTROL__EVENT *
newEvent( GAME_CONTROL_STATE * state ) {
    Assert( state->nEvent < GAME_CONTROL__EVENT_MAX_COUNT );
    int32 eventIndex = state->nEvent;
    
    GAME_CONTROL__EVENT * result = state->event + state->nEvent++;
    result->eventIndex     = eventIndex;
    result->dataOffset     = ( uint32 )state->memory.used;
    result->nextEventIndex = -1;
    result->subEventIndex  = -1;
    result->chainID        = state->chainID;
    
    return result;
}

internal GAME_CONTROL__EVENT *
nextEvent( GAME_CONTROL_STATE * state ) {
    GAME_CONTROL__EVENT * result = newEvent( state );
    
    if( state->prevEventIndex > -1 ) {
        GAME_CONTROL__EVENT * prev = state->event + state->prevEventIndex;
        prev->nextEventIndex = result->eventIndex;
    }
    state->prevEventIndex = result->eventIndex;
    
    return result;
}

internal GAME_CONTROL__EVENT *
newChain( GAME_CONTROL_STATE * state ) {
    Assert( state->tempValueForSubChain == -1 );
    state->tempValueForSubChain = state->prevEventIndex;
    
    GAME_CONTROL__EVENT * result = newEvent( state );
    result->chainID       = result->eventIndex;
    
    state->prevEventIndex = result->eventIndex;
    state->chainID        = result->eventIndex;
    return result;
}

internal void
endChain( GAME_CONTROL_STATE * state ) {
    state->prevEventIndex = state->tempValueForSubChain;
    state->tempValueForSubChain = -1;
    state->chainID = -1;
}

internal void
addCheckpoint( GAME_CONTROL_STATE * state ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_setCheckpoint;
    event->exit = exitCondition_instant;
    
    Assert( state->prevEventIndex > -1 );
    _writem( &state->memory, int32, state->prevEventIndex );
}

internal void
setBasePos( GAME_CONTROL_STATE * state, vec2 offset ) {
    state->world_basePos += ( offset * state->world_scale );
}

internal uint32
newGroupID( GAME_CONTROL_STATE * state ) {
    uint32 result = state->atGroupID++;
    return result;
}

internal void
spawnBackground( GAME_CONTROL_STATE * state, uint32 nBackground, boo32 newGroupID = true ) {
    if( newGroupID ) {
        state->atGroupID++;
    }
    
    _writem( &state->memory, GAME_CONTROL__SPAWN_TYPE, spawnType_background );
    _writem( &state->memory, uint32, state->atGroupID );
    _writem( &state->memory, uint32, nBackground );
}

#if 0
internal void
spawnBackground( GAME_CONTROL_STATE * state, BACKGROUND back ) {
    back.position = toWorldPos( state, back.position );
    back.scale   *= state->world_scale;
    _writem( &state->memory, BACKGROUND, back );
}

internal void
spawnBackgroundRect( GAME_CONTROL_STATE * state, TEXTURE_ID textureID, rect bound, vec4 modColor = COLOR_WHITE ) {
    bound = toWorldPos( state, bound );
    
    BACKGROUND back = {};
    back.modelID = modelID_rect;
    back.textureID = textureID;
    back.position  = getCenter ( bound );
    back.scale     = getHalfDim( bound );
    back.modColor  = modColor;
    _writem( &state->memory, BACKGROUND, back );
}

internal void
tileBackgroundRect( GAME_CONTROL_STATE * state, uint32 nRow, uint32 nCol, TEXTURE_ID textureID, rect bound, vec4 modColor = COLOR_WHITE ) {
    vec2 dim = getDim( bound );
    flo32 width  = ( dim.x / ( flo32 )nCol );
    flo32 height = ( dim.y / ( flo32 )nRow );
    vec2 bPos = getTL( bound );
    for( uint32 iRow = 0; iRow < nRow; iRow++ ) {
        vec2 atPos = bPos;
        for( uint32 iCol = 0; iCol < nCol; iCol++ ) {
            rect R = rectTLD( atPos, Vec2( width, height ) );
            spawnBackgroundRect( state, textureID, R, modColor );
            
            atPos.x += width;
        }
        bPos.y -= height;
    }
}
#endif

internal void
spawnBreakable( GAME_CONTROL_STATE * state, TEXTURE_ID textureID, orect bound, uint32 health ) {
    _writem( &state->memory, GAME_CONTROL__SPAWN_TYPE, spawnType_breakable );
    _writem( &state->memory, TEXTURE_ID, textureID );
    
    bound = toWorldPos( state, bound );
    _writem( &state->memory, orect, bound );
    _writem( &state->memory, uint32, health );
}

internal void
spawnBubble( GAME_CONTROL_STATE * state, vec2 position ) {
    _writem( &state->memory, GAME_CONTROL__SPAWN_TYPE, spawnType_bubble );
    
    position = toWorldPos( state, position );
    _writem( &state->memory, vec2, position );
}

internal void
spawnCage( GAME_CONTROL_STATE * state, vec2 position ) {
    _writem( &state->memory, GAME_CONTROL__SPAWN_TYPE, spawnType_cage );
    
    position = toWorldPos( state, position );
    _writem( &state->memory, vec2, position );
}

internal void
spawnItem( GAME_CONTROL_STATE * state, ITEM_ID itemID, vec2 position ) {
    _writem( &state->memory, GAME_CONTROL__SPAWN_TYPE, spawnType_item );
    _writem( &state->memory, uint32, itemID );
    
    position = toWorldPos( state, position );
    _writem( &state->memory, vec2, position );
}

internal void
spawnlgArcher( GAME_CONTROL_STATE * state, vec2 position ) {
    _writem( &state->memory, GAME_CONTROL__SPAWN_TYPE, spawnType_lgArcher );
    
    position = toWorldPos( state, position );
    _writem( &state->memory, vec2, position );
}

internal void
spawnNet( GAME_CONTROL_STATE * state, vec2 position ) {
    _writem( &state->memory, GAME_CONTROL__SPAWN_TYPE, spawnType_net );
    
    position = toWorldPos( state, position );
    _writem( &state->memory, vec2, position );
}

#if 0
internal void
spawnObstacle( GAME_CONTROL_STATE * state, uint32 nObstacle, boo32 newGroupID = true ) {
    if( newGroupID ) {
        state->atGroupID++;
    }
    
    _writem( &state->memory, GAME_CONTROL__SPAWN_TYPE, spawnType_obstacle );
    _writem( &state->memory, uint32, state->atGroupID );
    _writem( &state->memory, uint32, nObstacle );
}

internal void
spawnObstacle( GAME_CONTROL_STATE * state, OBSTACLE obs ) {
    obs.bound = toWorldPos( state, obs.bound );
    _writem( &state->memory, OBSTACLE, obs );
}
#endif

internal void
spawnParrotfish( GAME_CONTROL_STATE * state, vec2 position ) {
    position = toWorldPos( state, position );
    _writem( &state->memory, GAME_CONTROL__SPAWN_TYPE, spawnType_parrotfish );
    _writem( &state->memory, vec2, position );
}

internal void
spawnTrigger( GAME_CONTROL_STATE * state, uint32 eventID, rect bound ) {
    _writem( &state->memory, GAME_CONTROL__SPAWN_TYPE, spawnType_trigger );
    _writem( &state->memory, uint32, eventID );
    
    bound = toWorldPos( state, bound );
    _writem( &state->memory, rect, bound );
}

internal void
endSpawn( GAME_CONTROL_STATE * state ) {
    _writem( &state->memory, GAME_CONTROL__SPAWN_TYPE, spawnType_end );
}

internal void
spawnEvent( APP_STATE * appState, GAME_CONTROL__EVENT * event ) {
    GAME_CONTROL_STATE * state = &appState->gameControlState;
    uint8 * data = ( uint8 * )state->memory.base + event->dataOffset;
    
    switch( event->type ) {
        case eventType_setCameraBound: {
            rect bound = _read( data, rect );
            appState->world_cameraBound = bound;
        } break;
        
        case eventType_spawn_relativeToBasePos: {
            boo32 endSpawn = false;
            while( !endSpawn ) {
                GAME_CONTROL__SPAWN_TYPE type = _read( data, GAME_CONTROL__SPAWN_TYPE );
                Assert( type <= spawnType_end );
                
                switch( type ) {
                    case spawnType_background: {
                        uint32 backID = _read( data, uint32 );
                        uint32 nBack  = _read( data, uint32 );
                        
                        //addBackgroundGroup( appState, backID, nBack );
                        for( uint32 iBack = 0; iBack < nBack; iBack++ ) {
                            BACKGROUND background = _read( data, BACKGROUND );
                            //addBackground( appState, background );
                        }
                    } break;
                    
                    case spawnType_breakable: {
                        TEXTURE_ID textureID = _read( data, TEXTURE_ID );
                        orect      bound     = _read( data, orect      );
                        uint32     health    = _read( data, uint32     );
                        spawnBreakable( appState, textureID, bound, health );
                    } break;
                    
                    case spawnType_bubble: {
                        vec2 P = _read( data, vec2 );
                        spawnBubble( &appState->bubbleState, P );
                    } break;
                    
                    case spawnType_cage: {
                        vec2 P = _read( data, vec2 );
                        spawnCage( appState, P );
                    } break;
                    
                    case spawnType_item: {
                        GAME_ITEM item = {};
                        item.itemID   = _read( data, ITEM_ID );
                        item.position = _read( data, vec2 );
                        addItem( &appState->itemState, item );
                    } break;
                    
                    case spawnType_lgArcher: {
                        vec2 P = _read( data, vec2 );
                        spawnlgArcherAt( appState, P );
                    } break;
                    
                    case spawnType_net: {
                        vec2 P = _read( data, vec2 );
                        spawnNet( appState, P );
                    } break;
                    
                    case spawnType_obstacle: {
                        uint32 obsID = _read( data, uint32 );
                        uint32 nObs  = _read( data, uint32 );
                        
                        //addObstacleGroup( appState, obsID, nObs );
                        for( uint32 iObs = 0; iObs < nObs; iObs++ ) {
                            OBSTACLE obstacle = _read( data, OBSTACLE );
                            //addObstacle( appState, obstacle );
                        }
                    } break;
                    
                    case spawnType_parrotfish: {
                        vec2 P = _read( data, vec2 );
                        spawnParrotfish( appState, P );
                    } break;
                    
                    case spawnType_trigger: {
                        uint32 eventID = _read( data, uint32 );
                        rect   bound   = _read( data, rect );
                        addEventTrigger( appState, EventTrigger( eventID, bound ) );
                    } break;
                    
                    case spawnType_end: {
                        endSpawn = true;
                    } break;
                    
                    default: {
                        InvalidCodePath;
                    } break;
                }
            }
        } break;
        
        case eventType_spawnEnemies: {
            GAME_CONTROL__SPAWN * spawn = _addr( data, GAME_CONTROL__SPAWN );
            
            spawnRainbow   ( appState, spawn->nRainbow    );
            spawnGoldfish  ( appState, spawn->nGoldfish   );
            spawnDogfish   ( appState, spawn->nDogfish    );
            spawnSwordfish ( appState, spawn->nSwordfish  );
            spawnUrchin    ( appState, spawn->nUrchin     );
            spawnArcher    ( appState, spawn->nArcher     );
            spawnlgArcher  ( appState, spawn->nlgArcher   );
            spawnParrotfish( appState, spawn->nParrotfish );
            
            vec2  camera_base   = getBL( appState->cameraBound );
            flo32 camera_height = getHeight( appState->cameraBound );
            
            for( uint32 iFish = 0; iFish < spawn->nCage; iFish++ ) {
                vec2 pos = camera_base + spawn->cage_position[ iFish ] * camera_height;
                spawnCage( appState, pos );
            }
            
            for( uint32 iFish = 0; iFish < spawn->nNet; iFish++ ) {
                vec2 pos = camera_base + spawn->net_position[ iFish ] * camera_height;
                spawnNet( appState, pos );
            }
            
            for( uint32 iFish = 0; iFish < spawn->nTower; iFish++ ) {
                rect boundA = spawn->tower_bound[ iFish ];
                vec2 posA = camera_base + getBL( boundA ) * camera_height;
                vec2 posB = camera_base + getTR( boundA ) * camera_height;
                
                rect boundB = rectMM( posA, posB );
                spawnTower( appState, boundB );
            }
            
            for( uint32 iFish = 0; iFish < spawn->nHeal; iFish++ ) {
                rect boundA = spawn->heal_bound[ iFish ];
                vec2 posA = camera_base + getBL( boundA ) * camera_height;
                vec2 posB = camera_base + getTR( boundA ) * camera_height;
                
                rect boundB = rectMM( posA, posB );
                spawnHeal( appState, boundB );
            }
            
            spawnsmTurtle( appState, spawn->nsmTurtle );
            
            if( spawn->enableBubblonGen ) {
                BUBBLON_GEN * gen = &appState->bubblonGen;
                gen->isActive = true;
                gen->timer    = 0.0f;
            }
            if( spawn->turtle.enable ) {
                spawnTurtle( appState, spawn->turtle, camera_base, camera_height );
            }
            if( spawn->defendZone.isActive ) {
                appState->defendZoneState = spawn->defendZone;
            }
            
            for( uint32 iObstacle = 0; iObstacle < spawn->nObstacle; iObstacle++ ) {
                OBSTACLE obstacle = spawn->obstacle[ iObstacle ];
                obstacle.bound = cameraRel( appState, obstacle.bound );
                addObstacle( appState, obstacle );
            }
            
#if 0
            if( spawn->hasSchool ) {
                state->wave_timer = 0.0f;
                initSecret( appState,secretID__eat_all_fish_of_one_color_during_school_wave );
            }
#endif
        } break;
        
        case eventType_spawnIfNeeded: {
            // TODO: add other fish types if needed
            GAME_CONTROL__SPAWN * spawn = _addr( data, GAME_CONTROL__SPAWN );
            
            if( spawn->nParrotfish > 0 ) {
                PARROTFISH_STATE * parrotfishState = &appState->parrotfishState;
                
                int32 nSpawn = maxValue( ( int32 )spawn->nParrotfish - ( int32 )parrotfishState->nParrotfish, 0 );
                spawnParrotfish( appState, nSpawn );
            }
            
            if( spawn->nsmTurtle > 0 ) {
                SMTURTLE_STATE * smTurtleState = &appState->smTurtleState;
                
                int32 nSpawn = maxValue( ( int32 )spawn->nsmTurtle - ( int32 )smTurtleState->nsmTurtle, 0 );
                spawnsmTurtle( appState, nSpawn );
            }
        } break;
        
        case eventType_animation: {
            ANIMATION_SYSTEM * animationSystem = &appState->animationSystem;
            uint32 index = _read( data, uint32 );
            playAnimation( animationSystem, animationSystem->animation[ index ] );
        } break;
        
        case eventType_winGame: {
            InvalidCodePath;
        } break;
        
        case eventType_holdUntil: {
            // do nothing
        } break;
        
        case eventType_setCheckpoint: {
            int32 eventIndex = _read( data, int32 );
            Assert( eventIndex > -1 );
            
            appState->checkpoint_eventIndex    = eventIndex;
            appState->checkpoint_cameraBound   = appState->cameraBound;
            appState->checkpoint_backgroundPos = appState->backgroundPos;
        } break;
        
        case eventType_exitRainbow: {
            exitRainbow( appState );
        } break;
        
        case eventType_moveCamera: {
            vec2 pos = _read( data, vec2 );
            moveCamera( appState, event->targetTime, pos );
            
            boo32 enableEntityUpdateDuringCameraMovement = _read( data, boo32 );
            appState->entityUpdate_isEnabled = enableEntityUpdateDuringCameraMovement;
        } break;
        
        case eventType_scrollingSpawn: {
            event->timer = 0.0f;
            appState->respawnEnemyForScrolling = true;
        } break;
        
        case eventType_respawnEnemies: {
            event->timer = 0.0f;
        } break;
        
        case eventType_exitEnemies: {
            exitArcher( appState );
            exitlgArcher( appState );
            exitParrotfish( appState );
            // TODO: add any other enemies that will need to exit
        } break;
        
        case eventType_endAnimation: {
            ANIMATION_SYSTEM * animationSystem = &appState->animationSystem;
            animationSystem->nAnim = 0;
            memset( animationSystem->anim, 0, sizeof( ACTIVE_ANIMATION ) * ANIMATION_SYSTEM__ACTIVE_ANIMATION_MAX_COUNT );
        } break;
        
        case eventType_killChain: {
            Assert( event->chainIDToKill > -1 );
            
            for( uint32 iChain = 0; iChain < state->nChain; iChain++ ) {
                GAME_CONTROL__EVENT * chain = state->chain + iChain;
                if( chain->chainID == event->chainIDToKill ) {
                    chain->shouldKill = true;
                }
            }
        } break;
        
        case eventType_killBubbles: {
            BUBBLE_STATE * bubbleState = &appState->bubbleState;
            for( uint32 iFish = 0; iFish < BUBBLE_MAX_COUNT; iFish++ ) {
                BUBBLE * fish = bubbleState->bubble + iFish;
                if( fish->isActive ) {
                    fish->isActive = false;
                    bubbleState->nBubble--;
                }
            }
            
            BUBBLET_STATE * bubbletState = &appState->bubbletState;
            for( uint32 iFish = 0; iFish < BUBBLET_MAX_COUNT; iFish++ ) {
                BUBBLET * fish = bubbletState->bubblet + iFish;
                if( fish->isActive ) {
                    fish->isActive = false;
                    bubbletState->nBubblet--;
                }
            }
        } break;
        
        case eventType_killItems: {
            ITEM_STATE * itemState = &appState->itemState;
            itemState->nItem = 0;
        } break;
        
        case eventType_enableCameraSpring: {
            CAMERA_STATE * camera = &appState->cameraState;
            camera->springIsEnabled = true;
        } break;
        
        case eventType_disableCameraSpring: {
            CAMERA_STATE * camera = &appState->cameraState;
            camera->springIsEnabled = false;
            camera->pos   = camera->at;
            camera->dest  = camera->at;
            camera->timer = 0.0f;
        } break;
        
        case eventType_unloadObstacle: {
            uint32 obsID = _read( data, uint32 );
            //unloadObstacleGroup( appState, obsID );
        } break;
        
        case eventType_enableMovementBound: {
            rect bound = _read( data, rect );
            appState->movementBound = bound;
        } break;
        
        case eventType_beginGame: {
            // do nothing
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
}

#define GAME_CONTROL__DEBUG_ENABLED  ( 0 )
#if GAME_CONTROL__DEBUG_ENABLED
#define GAME_CONTROL__DISPLAY_STRING( string )      DISPLAY_STRING( string )
#define GAME_CONTROL__DISPLAY_VALUE( type, value )  DISPLAY_VALUE( type, value )
#else
#define GAME_CONTROL__DISPLAY_STRING( string )
#define GAME_CONTROL__DISPLAY_VALUE( type, value )
#endif

internal void
updateGameControl( APP_STATE * appState, flo32 dt ) {
    GAME_CONTROL_STATE * state = &appState->gameControlState;
    Assert( state->nChain > 0 );
    
    GAME_CONTROL__DISPLAY_VALUE( uint32, state->nChain );
    for( uint32 iChain = 0; iChain < state->nChain; iChain++ ) {
        GAME_CONTROL__EVENT * curr = state->chain + iChain;
        GAME_CONTROL__DISPLAY_VALUE( int32, curr->chainID );
        
        if( curr->type == eventType_respawnEnemies ) {
            curr->timer += dt;
            // TODO: make targetTime a random value
            if( curr->timer >= 1.0f ) {
                curr->timer = 0.0f;
                
                uint8 * data = ( uint8 * )state->memory.base + curr->dataOffset;
                GAME_CONTROL__SPAWN * spawn = _addr( data, GAME_CONTROL__SPAWN );
                
                // TODO: put in controls to only spawn in 1 fish of 1 type for any given spawn
                if( appState->rainbowState.nRainbow < spawn->nRainbow ) {
                    spawnRainbow( appState, 1 );
                }
                if( appState->goldfishState.nGoldfish < spawn->nGoldfish ) {
                    spawnGoldfish( appState, 1 );
                }
                if( appState->dogfishState.nDogfish < spawn->nDogfish ) {
                    spawnDogfish( appState, 1 );
                }
                if( appState->urchinState.nUrchin < spawn->nUrchin ) {
                    spawnUrchin( appState, 1 );
                }
                if( appState->archerState.nArcher < spawn->nArcher ) {
                    spawnArcher( appState, 1 );
                }
                if( appState->lgArcherState.nlgArcher < spawn->nlgArcher ) {
                    spawnlgArcher( appState, 1 );
                }
            }
        }
        if( curr->type == eventType_scrollingSpawn ) {
            curr->timer += dt;
            if( curr->timer >= 1.0f ) {
                curr->timer = 0.0f;
                
                vec2  camera_base   = getBL( appState->cameraBound );
                flo32 camera_height = getHeight( appState->cameraBound );
                rect bound = addOffset( Rect( 1.8f, -0.1f, 1.8f, 1.1f ) * camera_height, camera_base );
                
                uint8 * data = ( uint8 * )state->memory.base + curr->dataOffset;
                GAME_CONTROL__SPAWN * spawn = _addr( data, GAME_CONTROL__SPAWN );
                
                // TODO: put in controls to only spawn in 1 fish of 1 type for any given spawn
                if( appState->archerState.nArcher < spawn->nArcher ) {
                    spawnArcherAt( appState, getRandomPoint( bound ) );
                }
                if( appState->lgArcherState.nlgArcher < spawn->nlgArcher ) {
                    spawnlgArcherAt( appState, getRandomPoint( bound ) );
                }
            }
        }
        
        boo32 nextEvent = false;
        switch( curr->exit ) {
            case exitCondition_noEnemies: {
                uint32 nFishRemain = 0;
                nFishRemain += appState->rainbowState.nRainbow;
                nFishRemain += appState->goldfishState.nGoldfish;
                nFishRemain += appState->dogfishState.nDogfish;
                nFishRemain += appState->swordfishState.nSwordfish;
                nFishRemain += appState->urchinState.nUrchin;
                nFishRemain += appState->archerState.nArcher;
                nFishRemain += appState->lgArcherState.nlgArcher;
                nFishRemain += appState->cageState.nCage;
                nFishRemain += appState->netState.nNet;
                //nFishRemain += appState->parrotfishState.nParrotfish;
                
#if GAME_CONTROL__DEBUG_ENABLED
                if( appState->rainbowState.nRainbow > 0 ) {
                    DISPLAY_VALUE( uint32, appState->rainbowState.nRainbow );
                }
                if( appState->goldfishState.nGoldfish > 0 ) {
                    DISPLAY_VALUE( uint32, appState->goldfishState.nGoldfish );
                }
                if( appState->dogfishState.nDogfish > 0 ) {
                    DISPLAY_VALUE( uint32, appState->dogfishState.nDogfish );
                }
                if( appState->swordfishState.nSwordfish > 0 ) {
                    DISPLAY_VALUE( uint32, appState->swordfishState.nSwordfish );
                }
                if( appState->urchinState.nUrchin > 0 ) {
                    DISPLAY_VALUE( uint32, appState->urchinState.nUrchin );
                }
                if( appState->archerState.nArcher > 0 ) {
                    DISPLAY_VALUE( uint32, appState->archerState.nArcher );
                }
                if( appState->lgArcherState.nlgArcher > 0 ) {
                    DISPLAY_VALUE( uint32, appState->lgArcherState.nlgArcher );
                }
                if( appState->cageState.nCage > 0 ) {
                    DISPLAY_VALUE( uint32, appState->cageState.nCage );
                }
                if( appState->netState.nNet > 0 ) {
                    DISPLAY_VALUE( uint32, appState->netState.nNet );
                }
                DISPLAY_VALUE( uint32, nFishRemain );
                DISPLAY_STRING( "NO ENEMIES" );
#endif
                
                nextEvent = ( nFishRemain == 0 );
            } break;
            
            case exitCondition_noAnimation: {
                ANIMATION_SYSTEM * animationSystem = &appState->animationSystem;
                nextEvent = ( animationSystem->nAnim == 0 );
                
                GAME_CONTROL__DISPLAY_VALUE( uint32, animationSystem->nAnim );
                GAME_CONTROL__DISPLAY_STRING( "NO ANIMATION" );
            } break;
            
            case exitCondition_exitBound: {
                PLAYER_STATE * player = &appState->playerState;
                flo32 camera_height = getHeight( appState->cameraBound );
                vec2  camera_base   = getBL    ( appState->cameraBound );
                
                vec2 exit_dim = EXIT_BOUND_DIM * camera_height;
                vec2 exit_pos = EXIT_BOUND_POS * camera_height + camera_base;
                rect exit_bound = rectCD( exit_pos, exit_dim );
                
                if( isInBound( player->position, exit_bound ) ) {
                    flo32 targetTime = EXIT_BOUND_TARGET_TIME;
                    
                    curr->timer += dt;
                    nextEvent = ( curr->timer >= targetTime );
                } else {
                    curr->timer = 0.0f;
                }
                
                GAME_CONTROL__DISPLAY_STRING( "EXIT BOUND" );
            } break;
            
            case exitCondition_targetTime: {
                curr->timer += dt;
                nextEvent = ( curr->timer >= curr->targetTime );
                if( nextEvent ) {
                    appState->entityUpdate_isEnabled = true;
                }
                
                GAME_CONTROL__DISPLAY_VALUE( flo32, curr->timer );
                GAME_CONTROL__DISPLAY_VALUE( flo32, curr->targetTime );
                GAME_CONTROL__DISPLAY_STRING( "TARGET TIME" );
            } break;
            
            case exitCondition_loadTrigger: {
                PLAYER_STATE * player = &appState->playerState;
                
                uint8 * data = ( uint8 * )state->memory.base + curr->dataOffset;
                rect bound = _read( data, rect );
                nextEvent = isInBound( player->position, bound );
                
                GAME_CONTROL__DISPLAY_STRING( "LOAD TRIGGER" );
            } break;
            
            case exitCondition_none: {
                GAME_CONTROL__DISPLAY_STRING( "EXIT CONDITION: NONE" );
                // do nothing
            } break;
            
            case exitCondition_instant: {
                nextEvent = true;
            } break;
            
            case exitCondition_noNets: {
                NET_STATE * netState = &appState->netState;
                nextEvent = ( netState->nNet == 0 );
                
                GAME_CONTROL__DISPLAY_VALUE( uint32, netState->nNet );
                GAME_CONTROL__DISPLAY_STRING( "NO NETS" );
            } break;
            
            case exitCondition_noCages: {
                CAGE_STATE * cageState = &appState->cageState;
                nextEvent = ( cageState->nCage == 0 );
                
                GAME_CONTROL__DISPLAY_VALUE( uint32, cageState->nCage );
                GAME_CONTROL__DISPLAY_STRING( "NO CAGES" );
            } break;
            
            case exitCondition_noTowers: {
                TOWER_STATE * towerState = &appState->towerState;
                nextEvent = ( towerState->nTower == 0 );
                
                GAME_CONTROL__DISPLAY_VALUE( uint32, towerState->nTower );
                GAME_CONTROL__DISPLAY_STRING( "NO TOWERS" );
            } break;
            
            case exitCondition_noHeal: {
                HEAL_STATE * healState = &appState->healState;
                nextEvent = ( healState->nHeal == 0 );
                
                GAME_CONTROL__DISPLAY_VALUE( uint32, healState->nHeal );
                GAME_CONTROL__DISPLAY_STRING( "NO HEALS" );
            } break;
            
            case exitCondition_towersAtHPOrLower: {
                TOWER_STATE * towerState = &appState->towerState;
                boo32 isValid = true;
                for( uint32 iFish = 0; iFish < TOWER_MAX_COUNT; iFish++ ) {
                    TOWER * fish = towerState->tower + iFish;
                    if( fish->isActive ) {
                        flo32 hp = ( flo32 )( fish->health );
                        flo32 percent = ( flo32 )fish->health / ( flo32 )fish->maxHealth;
                        if( percent > curr->atHP ) {
                            isValid = false;
                        }
                    }
                }
                nextEvent = isValid;
                if( nextEvent ) {
                    uint32 breakHere = 10;
                }
                
                GAME_CONTROL__DISPLAY_STRING( "TOWERS AT HP OR LOWER" );
            } break;
            
            case exitCondition_netsAtHPOrLower: {
                NET_STATE * netState = &appState->netState;
                flo32 maxHP = ( flo32 )( NET_BLOCK_HEALTH * 2 );
                
                boo32 isValid = true;
                for( uint32 iFish = 0; iFish < NET_MAX_COUNT; iFish++ ) {
                    NET * fish = netState->net + iFish;
                    if( fish->isActive ) {
                        flo32 hp = ( flo32 )( fish->blockA.health + fish->blockB.health );
                        flo32 percent = hp / maxHP;
                        if( percent > curr->atHP ) {
                            isValid = false;
                        }
                    }
                }
                nextEvent = isValid;
                
                GAME_CONTROL__DISPLAY_STRING( "NETS AT HP OR LOWER" );
            } break;
            
            default: {
                InvalidCodePath;
            } break;
        }
        
        while( nextEvent ) {
            if( curr->nextEventIndex > -1 ) {
                *curr = state->event[ curr->nextEventIndex ];
                spawnEvent( appState, curr );
                
                if( curr->subEventIndex > -1 ) {
                    Assert( state->nChain < GAME_CONTROL__CHAIN_MAX_COUNT  );
                    
                    GAME_CONTROL__EVENT * event = state->chain + state->nChain++;
                    *event = state->event[ curr->subEventIndex ];
                    spawnEvent( appState, event );
                }
                
                nextEvent = ( curr->exit == exitCondition_instant );
            } else {
                state->nChain--;
                for( uint32 iter = iChain; iter < state->nChain; iter++ ) {
                    state->chain[ iter ] = state->chain[ iter + 1 ];
                }
                nextEvent = false;
            }
        }
    }
    
    uint32 newCount = 0;
    for( uint32 iChain = 0; iChain < state->nChain; iChain++ ) {
        GAME_CONTROL__EVENT chain = state->chain[ iChain ];
        if( !chain.shouldKill ) {
            state->chain[ newCount++ ] = chain;
        }
    }
    state->nChain = newCount;
    
    Assert( state->nChain > 0 );
}

internal void
drawGameControl( RENDER_PASS * pass, APP_STATE * appState ) {
    GAME_CONTROL_STATE * state = &appState->gameControlState;
    Assert( state->nChain > 0 );
    
    for( uint32 iChain = 0; iChain < state->nChain; iChain++ ) {
        GAME_CONTROL__EVENT * curr = state->chain + iChain;
        
        if( curr->exit == exitCondition_exitBound ) {
            flo32 camera_height = getHeight( appState->cameraBound );
            vec2  camera_base   = getBL    ( appState->cameraBound );
            
            vec2 exit_dim = EXIT_BOUND_DIM * camera_height;
            vec2 exit_pos = EXIT_BOUND_POS * camera_height + camera_base;
            rect exit_bound = rectCD( exit_pos, exit_dim );
            
            drawRectOutline( pass, exit_bound, COLOR_RED );
            
            flo32 targetTime = EXIT_BOUND_TARGET_TIME;
            flo32 t = clamp01( curr->timer / targetTime );
            rect bound = exit_bound;
            bound.top = lerp( bound.bottom, t, bound.top );
            drawRect       ( pass, textureID_whiteTexture, bound, COLOR_YELLOW );
            drawRect       ( pass, textureID_exitArrow, exit_bound, COLOR_WHITE );
        }
        if( curr->exit == exitCondition_loadTrigger ) {
            uint8 * data = ( uint8 * )state->memory.base + curr->dataOffset;
            rect bound = _read( data, rect );
            //drawRectOutline( pass, bound, COLOR_GREEN );
        }
    }
}

internal GAME_CONTROL__EVENT *
nextEvent_spawn( GAME_CONTROL_STATE * state, GAME_CONTROL__SPAWN ** spawn_out ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_spawnEnemies;
    event->exit = exitCondition_instant;
    
    *spawn_out = _pushType( &state->memory, GAME_CONTROL__SPAWN );
    return event;
}

internal GAME_CONTROL__EVENT *
newChain_spawn( GAME_CONTROL_STATE * state, GAME_CONTROL__SPAWN ** spawn_out ) {
    GAME_CONTROL__EVENT * event = newChain( state );
    event->type = eventType_spawnEnemies;
    event->exit = exitCondition_instant;
    
    *spawn_out = _pushType( &state->memory, GAME_CONTROL__SPAWN );
    return event;
}

internal GAME_CONTROL__EVENT *
newChain_spawnw( GAME_CONTROL_STATE * state ) {
    GAME_CONTROL__EVENT * event = newChain( state );
    event->type = eventType_spawn_relativeToBasePos;
    event->exit = exitCondition_instant;
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_spawnh( GAME_CONTROL_STATE * state, GAME_CONTROL__SPAWN ** spawn_out ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_spawnEnemies;
    event->exit = exitCondition_noEnemies;
    
    *spawn_out = _pushType( &state->memory, GAME_CONTROL__SPAWN );
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_spawnw( GAME_CONTROL_STATE * state ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_spawn_relativeToBasePos;
    event->exit = exitCondition_instant;
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_spawnIf( GAME_CONTROL_STATE * state, GAME_CONTROL__SPAWN ** spawn_out ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_spawnIfNeeded;
    event->exit = exitCondition_instant;
    
    *spawn_out = _pushType( &state->memory, GAME_CONTROL__SPAWN );
    return event;
}

internal GAME_CONTROL__EVENT *
newChain_spawnIf( GAME_CONTROL_STATE * state, GAME_CONTROL__SPAWN ** spawn_out ) {
    GAME_CONTROL__EVENT * event = newChain( state );
    event->type = eventType_spawnIfNeeded;
    event->exit = exitCondition_instant;
    
    *spawn_out = _pushType( &state->memory, GAME_CONTROL__SPAWN );
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_hold( GAME_CONTROL_STATE * state, flo32 targetTime ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_holdUntil;
    event->exit = exitCondition_targetTime;
    event->targetTime = targetTime;
    
    return event;
}

internal GAME_CONTROL__EVENT *
newChain_hold( GAME_CONTROL_STATE * state, flo32 targetTime ) {
    GAME_CONTROL__EVENT * event = newChain( state );
    event->type = eventType_holdUntil;
    event->exit = exitCondition_targetTime;
    event->targetTime = targetTime;
    
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_spawnArcher( GAME_CONTROL_STATE * state, uint32 nArcher, uint32 nlgArcher ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_spawnEnemies;
    event->exit = exitCondition_instant;
    GAME_CONTROL__SPAWN * spawn = _pushType( &state->memory, GAME_CONTROL__SPAWN );
    spawn->nArcher   = nArcher;
    spawn->nlgArcher = nlgArcher;
    
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_spawnArcherh( GAME_CONTROL_STATE * state, uint32 nArcher, uint32 nlgArcher ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_spawnEnemies;
    event->exit = exitCondition_noEnemies;
    GAME_CONTROL__SPAWN * spawn = _pushType( &state->memory, GAME_CONTROL__SPAWN );
    spawn->nArcher   = nArcher;
    spawn->nlgArcher = nlgArcher;
    
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_moveCamera( GAME_CONTROL_STATE * state, flo32 targetTime, vec2 pos, boo32 updateBasePos = true, boo32 moveEntitiesWithCamera = true ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_moveCamera;
    event->exit = exitCondition_instant;
    event->targetTime = targetTime;
    
    pos = toWorldPos( state, pos );
    if( updateBasePos ) {
        state->world_basePos = pos;
    }
    
    _writem( &state->memory, vec2, pos );
    _writem( &state->memory, boo32, moveEntitiesWithCamera );
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_moveCamerah( GAME_CONTROL_STATE * state, flo32 targetTime, vec2 pos, boo32 updateBasePos = true, boo32 enableEntityUpdateDuringCameraMovement = true ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_moveCamera;
    event->exit = exitCondition_targetTime;
    event->targetTime = targetTime;
    
    pos = toWorldPos( state, pos );
    if( updateBasePos ) {
        state->world_basePos = pos;
    }
    
    _writem( &state->memory, vec2, pos );
    _writem( &state->memory, boo32, enableEntityUpdateDuringCameraMovement );
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_noEnemies( GAME_CONTROL_STATE * state ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_holdUntil;
    event->exit = exitCondition_noEnemies;
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_noCages( GAME_CONTROL_STATE * state ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_holdUntil;
    event->exit = exitCondition_noCages;
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_noNets( GAME_CONTROL_STATE * state ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_holdUntil;
    event->exit = exitCondition_noNets;
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_newAnim( GAME_CONTROL_STATE * state  ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_animation;
    event->exit = exitCondition_instant;
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_newAnimh( GAME_CONTROL_STATE * state ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_animation;
    event->exit = exitCondition_noAnimation;
    return event;
}

internal GAME_CONTROL__EVENT *
newChain_newAnimh( GAME_CONTROL_STATE * state ) {
    GAME_CONTROL__EVENT * event = newChain( state );
    event->type = eventType_animation;
    event->exit = exitCondition_noAnimation;
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_exit( GAME_CONTROL_STATE * state ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_holdUntil;
    event->exit = exitCondition_exitBound;
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_exitEnemies( GAME_CONTROL_STATE * state, flo32 targetTime ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_exitEnemies;
    event->exit = exitCondition_targetTime;
    event->targetTime = targetTime;
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_killChain( GAME_CONTROL_STATE * state, uint32 chainIDToKill ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_killChain;
    event->exit = exitCondition_instant;
    event->chainIDToKill = chainIDToKill;
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_endAnimation( GAME_CONTROL_STATE * state ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_endAnimation;
    event->exit = exitCondition_instant;
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_enableCameraSpring( GAME_CONTROL_STATE * state ) {
    state->camera_springIsEnabled = true;
    
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_enableCameraSpring;
    event->exit = exitCondition_instant;
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_disableCameraSpring( GAME_CONTROL_STATE * state ) {
    state->camera_springIsEnabled = false;
    
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_disableCameraSpring;
    event->exit = exitCondition_instant;
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_setCameraBound( GAME_CONTROL_STATE * state, rect bound ) {
    bound = toWorldPos( state, bound );
    state->camera_bound = bound;
    
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_setCameraBound;
    event->exit = exitCondition_instant;
    
    _writem( &state->memory, rect, bound );
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_loadTrigger( GAME_CONTROL_STATE * state, rect bound ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_holdUntil;
    event->exit = exitCondition_loadTrigger;
    
    bound = toWorldPos( state, bound );
    _writem( &state->memory, rect, bound );
    
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_enableMovementBound( GAME_CONTROL_STATE * state, rect bound ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_enableMovementBound;
    event->exit = exitCondition_instant;
    
    bound = toWorldPos( state, bound );
    _writem( &state->memory, rect, bound );
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_killItems( GAME_CONTROL_STATE * state ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_killItems;
    event->exit = exitCondition_instant;
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_killBubbles( GAME_CONTROL_STATE * state ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_killBubbles;
    event->exit = exitCondition_instant;
    return event;
}

internal GAME_CONTROL__EVENT *
nextEvent_none( GAME_CONTROL_STATE * state ) {
    GAME_CONTROL__EVENT * event = nextEvent( state );
    event->type = eventType_holdUntil;
    event->exit = exitCondition_none;
    return event;
}

internal void
dummyChain( GAME_CONTROL_STATE * state, uint32 eventID ) {
    Assert( state->nChain < GAME_CONTROL__CHAIN_MAX_COUNT  );
    
    GAME_CONTROL__EVENT * event = state->chain + state->nChain++;
    *event = {};
    event->type = eventType_dummy;
    event->exit = exitCondition_instant;
    event->nextEventIndex = eventID;
}

internal void
updateEventTrigger( APP_STATE * appState ) {
    EVENT_TRIGGER_STATE * state  = &appState->eventTriggerState;
    PLAYER_STATE        * player = &appState->playerState;
    
    uint32 newCount = 0;
    for( uint32 iEventTrigger = 0; iEventTrigger < state->nEventTrigger; iEventTrigger++ ) {
        EVENT_TRIGGER eventTrigger = state->eventTrigger[ iEventTrigger ];
        if( isInBound( player->position, eventTrigger.bound ) ) {
            dummyChain( &appState->gameControlState, eventTrigger.eventID );
        } else {
            state->eventTrigger[ newCount++ ] = eventTrigger;
        }
    }
    state->nEventTrigger = newCount;
}