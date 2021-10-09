
internal void
uploadTextureFromFile( PLATFORM * platform, TEXTURE_ID textureID, const char * filename ) {
    TEXTURE_DATA texture = BMP_readFile( platform, "../../art", filename );
    uploadTexture( &platform->renderer, textureID, texture.width, texture.height, texture.texelData );
}

internal void
loadTextures( PLATFORM * platform ) {
    uploadTextureFromFile( platform, textureID_background_left,  "background_left.bmp"  );
    uploadTextureFromFile( platform, textureID_background_back,  "background_back.bmp"  );
    uploadTextureFromFile( platform, textureID_background_right, "background_right.bmp" );
    uploadTextureFromFile( platform, textureID_background_front, "background_front.bmp" );
    
    uploadTextureFromFile( platform, textureID_home_background,     "home_background.bmp" );
    uploadTextureFromFile( platform, textureID_home_storage,        "home_storage.bmp"    );
    uploadTextureFromFile( platform, textureID_home_storage_x,      "home_storage_x.bmp"    );
    uploadTextureFromFile( platform, textureID_home_storage_cursor, "home_storage_cursor.bmp"    );
    uploadTextureFromFile( platform, textureID_inventoryIcon_open,  "inventoryIcon_open.bmp"  );
    uploadTextureFromFile( platform, textureID_inventoryIcon_close, "inventoryIcon_close.bmp" );
    
    uploadTextureFromFile( platform, textureID_background_cave_exit, "background_cave_exit.bmp" );
    uploadTextureFromFile( platform, textureID_background_sandToCave, "background_sandToCave.bmp" );
    uploadTextureFromFile( platform, textureID_background_caveWall,   "background_caveWall.bmp"   );
    uploadTextureFromFile( platform, textureID_background_cave_rock,  "background_cave_rock.bmp"   );
    uploadTextureFromFile( platform, textureID_background_crate,      "background_crate.bmp"   );
    
    uploadTextureFromFile( platform, textureID_breakable_plank,  "breakable_plank.bmp"   );
    
    uploadTextureFromFile( platform, textureID_exitArrow, "exitArrow.bmp" );
    
    uploadTextureFromFile( platform, textureID_player_idle,      "player_idle.bmp" );
    uploadTextureFromFile( platform, textureID_player_bite_open, "player_bite_open.bmp" );
    uploadTextureFromFile( platform, textureID_player_bite_close, "player_bite_close.bmp" );
    uploadTextureFromFile( platform, textureID_player_parry, "player_parry.bmp" );
    uploadTextureFromFile( platform, textureID_player_stunned, "player_stunned.bmp" );
    uploadTextureFromFile( platform, textureID_player_bellyUp, "player_bellyUp.bmp" );
    uploadTextureFromFile( platform, textureID_healAttempts, "healAttempts.bmp" );
    
    uploadTextureFromFile( platform, textureID_peasant_idle, "peasant_idle.bmp" );
    
    uploadTextureFromFile( platform, textureID_turtle_idle,   "turtle_idle.bmp" );
    uploadTextureFromFile( platform, textureID_turtle_shadow, "turtle_shadow.bmp" );
    uploadTextureFromFile( platform, textureID_smTurtle_idle,   "smTurtle_idle.bmp" );
    
    uploadTextureFromFile( platform, textureID_fish,     "fish.bmp"     );
    uploadTextureFromFile( platform, textureID_fish_eye, "fish_eye.bmp" );
    
    uploadTextureFromFile( platform, textureID_goldfish, "goldfish.bmp" );
    uploadTextureFromFile( platform, textureID_cage_idle, "cage_idle.bmp" );
    uploadTextureFromFile( platform, textureID_cage_fish, "cage_fish.bmp" );
    
    uploadTextureFromFile( platform, textureID_dogfish_idle,     "dogfish_idle.bmp" );
    uploadTextureFromFile( platform, textureID_dogfish_dying,    "dogfish_dying.bmp" );
    uploadTextureFromFile( platform, textureID_dogfish_stunned,  "dogfish_stunned.bmp" );
    uploadTextureFromFile( platform, textureID_dogfish_biting,   "dogfish_biting.bmp" );
    uploadTextureFromFile( platform, textureID_dogfish_lunging,  "dogfish_lunging.bmp" );
    uploadTextureFromFile( platform, textureID_dogfish_injury01, "dogfish_injury01.bmp" );
    uploadTextureFromFile( platform, textureID_dogfish_injury02, "dogfish_injury02.bmp" );
    uploadTextureFromFile( platform, textureID_dogfish_injury03, "dogfish_injury03.bmp" );
    uploadTextureFromFile( platform, textureID_dogfish_lungePrep, "dogfish_lungePrep.bmp" );
    uploadTextureFromFile( platform, textureID_dogfish_shakingFree, "dogfish_shakingFree.bmp" );
    
    uploadTextureFromFile( platform, textureID_swordfish_idle, "swordfish_idle.bmp" );
    uploadTextureFromFile( platform, textureID_swordfish_head, "swordfish_head.bmp" );
    uploadTextureFromFile( platform, textureID_swordfish_tail, "swordfish_tail.bmp" );
    uploadTextureFromFile( platform, textureID_swordfish_stunnedByBite, "swordfish_stunnedByBite.bmp" );
    uploadTextureFromFile( platform, textureID_swordfish_stunned_hitFrame, "swordfish_stunned_hitFrame.bmp" );
    uploadTextureFromFile( platform, textureID_swordfish_stunned_lo, "swordfish_stunned_lo.bmp" );
    uploadTextureFromFile( platform, textureID_swordfish_stabbing, "swordfish_stabbing.bmp" );
    uploadTextureFromFile( platform, textureID_swordfish_stabbing_background, "swordfish_stabbing_background.bmp" );
    uploadTextureFromFile( platform, textureID_swordfish_angry, "swordfish_angry.bmp" );
    uploadTextureFromFile( platform, textureID_swordfish_blinkTransition, "swordfish_blinkTransition.bmp" );
    uploadTextureFromFile( platform, textureID_swordfish_dying, "swordfish_dying.bmp" );
    uploadTextureFromFile( platform, textureID_swordfish_slash, "swordfish_slash.bmp" );
    uploadTextureFromFile( platform, textureID_swordfish_slashing_up, "swordfish_slashing_up.bmp" );
    uploadTextureFromFile( platform, textureID_swordfish_slashing_down, "swordfish_slashing_down.bmp" );
    
    uploadTextureFromFile( platform, textureID_urchin_idle, "urchin_idle.bmp" );
    uploadTextureFromFile( platform, textureID_urchinSpine_idle, "urchinSpine_idle.bmp" );
    
    uploadTextureFromFile( platform, textureID_archer_idle, "archer_idle.bmp" );
    uploadTextureFromFile( platform, textureID_bubble_idle, "bubble_idle.bmp" );
    uploadTextureFromFile( platform, textureID_bubblet_idle, "bubblet_idle.bmp" );
    
    uploadTextureFromFile( platform, textureID_parrotfish_idle,     "parrotfish_idle.bmp"     );
    uploadTextureFromFile( platform, textureID_parrotfish_headbutt, "parrotfish_headbutt.bmp" );
    uploadTextureFromFile( platform, textureID_parrotfish_stunned,  "parrotfish_stunned.bmp"  );
    uploadTextureFromFile( platform, textureID_parrotfish_windup,   "parrotfish_windup.bmp"   );
    
    uploadTextureFromFile( platform, textureID_net_idle, "net_idle.bmp" );
    uploadTextureFromFile( platform, textureID_net_fish, "net_fish.bmp" );
    uploadTextureFromFile( platform, textureID_net_block, "net_block.bmp" );
    
    uploadTextureFromFile( platform, textureID_explosion01, "explosion01.bmp" );
    uploadTextureFromFile( platform, textureID_explosion02, "explosion02.bmp" );
    uploadTextureFromFile( platform, textureID_explosion03, "explosion03.bmp" );
    uploadTextureFromFile( platform, textureID_smExplosion01, "smExplosion01.bmp" );
}      

internal void
updateCameraInput( EDITOR_STATE * editor, KEYBOARD_STATE * keyboard ) {
    if( wasPressed ( keyboard, keyCode_num4 ) ) { editor->camera_left  = true;  }
    if( wasReleased( keyboard, keyCode_num4 ) ) { editor->camera_left  = false; }
    if( wasPressed ( keyboard, keyCode_num6 ) ) { editor->camera_right = true;  }
    if( wasReleased( keyboard, keyCode_num6 ) ) { editor->camera_right = false; }
    if( wasPressed ( keyboard, keyCode_num9 ) ) { editor->camera_in    = true;  }
    if( wasReleased( keyboard, keyCode_num9 ) ) { editor->camera_in    = false; }
    if( wasPressed ( keyboard, keyCode_num7 ) ) { editor->camera_out   = true;  }
    if( wasReleased( keyboard, keyCode_num7 ) ) { editor->camera_out   = false; }
    if( wasPressed ( keyboard, keyCode_num8 ) ) { editor->camera_up    = true;  }
    if( wasReleased( keyboard, keyCode_num8 ) ) { editor->camera_up    = false; }
    if( wasPressed ( keyboard, keyCode_num5 ) ) { editor->camera_down  = true;  }
    if( wasReleased( keyboard, keyCode_num5 ) ) { editor->camera_down  = false; }
    
    if( editor->camera_left ) {
        editor->camera_pos.x -= editor->camera_dim.x * 0.01f;
    }
    if( editor->camera_right ) {
        editor->camera_pos.x += editor->camera_dim.x * 0.01f;
    }
    if( editor->camera_down ) {
        editor->camera_pos.y -= editor->camera_dim.y * 0.01f;
    }
    if( editor->camera_up ) {
        editor->camera_pos.y += editor->camera_dim.y * 0.01f;
    }
    if( editor->camera_in ) {
        editor->camera_dim *= 0.99f;
    }
    if( editor->camera_out ) {
        editor->camera_dim *= 1.01f;
    }
}

internal void
addCaveRock( EDITOR_STATE * editor, vec2 position, vec2 scale, flo32 radians ) {
    BACKGROUND back = {};
    back.position  = position;
    back.scale     = scale;
    back.radians   = radians;
    back.modelID   = modelID_rect;
    back.textureID = textureID_background_cave_rock;
    back.modColor  = COLOR_WHITE;
    
    Assert( editor->nBack < EDITOR_STATE__BACKGROUND_MAX_COUNT );
    editor->back[ editor->nBack++ ] = back;
}

internal void
addCaveWall( EDITOR_STATE * editor, vec2 position, vec2 scale, flo32 radians ) {
    BACKGROUND back = {};
    back.position  = position;
    back.scale     = scale;
    back.radians   = radians;
    back.modelID   = modelID_rect;
    back.textureID = textureID_background_caveWall;
    back.modColor  = COLOR_WHITE;
    
    Assert( editor->nBack < EDITOR_STATE__BACKGROUND_MAX_COUNT );
    editor->back[ editor->nBack++ ] = back;
}

internal void
EDITOR_updateAndRender( APP_STATE * appState, PLATFORM * platform ) {
    EDITOR_STATE * editor = &appState->editorState;
    
    RENDERER * renderer = &platform->renderer;
    RENDER_PASS * pass = &renderer->pass_game;
    
	MOUSE_STATE    * mouse    = &platform->mouse;
	KEYBOARD_STATE * keyboard = &platform->keyboard;
	
	MEMORY * permMemory = &platform->permMemory;
	MEMORY * tempMemory = &platform->tempMemory;
    
    if( !editor->isInitialized ) {
        editor->isInitialized = true;
        editor->mode = editorMode_default;
        
        appState->resetMousePos = true;
        loadTextures( platform );
        
        vec2 center = getCenter( appState->app_bound );
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
        
        { // cave
            uint32 nObs = 0;
            
            LINE_SEG * obs = editor->obs;
            
            obs[ nObs++ ] = LineSegQP( Vec2( 0.0f, 0.0f ), 0.6f,    PI * 17.0f / 32.0f );
            obs[ nObs++ ] = LineSegQP( obs[  0 ].P,        0.35f,   PI * 3.0f  / 8.0f );
            obs[ nObs++ ] = LineSegQP( obs[  1 ].P,        0.35f,   PI * 1.0f  / 64.0f );
            obs[ nObs++ ] = LineSegQP( obs[  2 ].P,        0.75f,  -PI * 1.0f  / 8.0f );
            obs[ nObs++ ] = LineSegQP( obs[  3 ].P,        0.6f,   PI * 7.0f  / 32.0f );
            obs[ nObs++ ] = LineSegQP( obs[  4 ].P,        0.1f,   0.0f );
            obs[ nObs++ ] = LineSegQP( obs[  5 ].P,        0.1f,  -PI * 15.0f / 32.0f );
            obs[ nObs++ ] = LineSegQP( obs[  6 ].P,        0.7f,  -PI *  1.0f / 32.0f );
            
            obs[ nObs++ ] = LineSeg( obs[  0 ].Q + Vec2( 0.5f, -PI * 1.0f / 32.0f ), 0.2f, PI / 4.0f );
            obs[ nObs++ ] = LineSeg( obs[  8 ].Q,         0.23f,   PI * 1.0f / 2.0f );
            obs[ nObs++ ] = LineSeg( obs[  9 ].Q,         0.6f,  PI * 1.0f / 8.0f );
            obs[ nObs++ ] = LineSeg( obs[ 10 ].Q,         0.5f,   PI * 7.0f / 32.0f );
            obs[ nObs++ ] = LineSeg( obs[ 11 ].Q,         0.1f,   PI * 13.0f / 32.0f );
            obs[ nObs++ ] = LineSeg( obs[ 12 ].Q,         0.1f,  -PI *  5.0f / 16.0f );
            obs[ nObs++ ] = LineSeg( obs[ 13 ].Q,         0.15f, -PI *  1.0f / 32.0f );
            obs[ nObs++ ] = LineSeg( obs[ 14 ].Q,         0.2f,  -PI *  1.0f / 4.0f );
            obs[ nObs++ ] = LineSeg( obs[ 15 ].Q, obs[ 7 ].P + toVec2( 0.35f, -PI * 17.0f / 32.0f ) );
            editor->nObs = nObs;
        }
        
#if 0
        { // auto-generate initial background for cave
            for( uint32 iObs = 0; iObs < editor->nObs; iObs++ ) {
                LINE_SEG obs = editor->obs[ iObs ];
                
                orect R = orectTHD( obs.Q, obs.P, 0.35f );
                
                BACKGROUND back = {};
                back.position  = R.center;
                back.scale     = R.halfDim;
                back.radians   = atan2f( R.xAxis.y, R.xAxis.x );
                back.modelID   = modelID_rect;
                back.textureID = textureID_background_cave_rock;
                back.modColor  = COLOR_WHITE;
                
                Assert( editor->nBack < EDITOR_STATE__BACKGROUND_MAX_COUNT );
                editor->back[ editor->nBack++ ] = back;
            }
        }
#else
        { // load previously generated background objects
            addCaveWall( editor, Vec2( 0.376951f, 0.470054f ), Vec2( 0.565104f, 0.753243f ), 1.495921f );
            addCaveWall( editor, Vec2( 1.673114f, 0.746459f ), Vec2( 0.486244f, 0.753243f ), 1.495921f );
            addCaveRock( editor, Vec2( -0.387077f, 0.359258f ), Vec2( 0.395469f, 0.350000f ), -1.472621f );
            addCaveRock( editor, Vec2( -0.261426f, 1.022548f ), Vec2( 0.476109f, 0.350000f ), -1.963495f );
            addCaveRock( editor, Vec2( 0.110671f, 1.272637f ), Vec2( 0.490341f, 0.350000f ), -3.092505f );
            addCaveRock( editor, Vec2( 0.843938f, 1.142828f ), Vec2( 0.441202f, 0.350000f ), 2.748893f );
            addCaveRock( editor, Vec2( 1.127482f, 1.111501f ), Vec2( 0.300000f, 0.350000f ), -2.454369f );
            addCaveRock( editor, Vec2( 1.562679f, 1.381266f ), Vec2( 0.432723f, 0.350000f ), -3.141593f );
            addCaveRock( editor, Vec2( 2.011876f, 1.246927f ), Vec2( 0.282232f, 0.350000f ), 1.668971f );
            addCaveRock( editor, Vec2( 2.073846f, 1.245756f ), Vec2( 0.350000f, 0.350000f ), 3.043418f );
            addCaveRock( editor, Vec2( 0.722680f, -0.081997f ), Vec2( 0.168898f, 0.146019f ), 0.785398f );
            addCaveRock( editor, Vec2( 0.991421f, 0.106949f ), Vec2( 0.166298f, 0.350000f ), 1.570796f );
            addCaveRock( editor, Vec2( 1.060219f, 0.130590f ), Vec2( 0.332326f, 0.292064f ), 0.392699f );
            addCaveRock( editor, Vec2( 1.534401f, 0.419058f ), Vec2( 0.345684f, 0.279615f ), 0.687223f );
            addCaveRock( editor, Vec2( 1.610648f, 0.863690f ), Vec2( 0.050000f, 0.014504f ), 1.276273f );
            addCaveRock( editor, Vec2( 1.627445f, 0.866412f ), Vec2( 0.050000f, 0.013971f ), -0.981748f );
            addCaveRock( editor, Vec2( 1.707106f, 0.712737f ), Vec2( 0.098178f, 0.115339f ), -0.098175f );
            addCaveRock( editor, Vec2( 1.767375f, 0.525417f ), Vec2( 0.172349f, 0.241282f ), -0.785398f );
            addCaveRock( editor, Vec2( 2.017194f, 0.274086f ), Vec2( 0.345024f, 0.350000f ), -0.387565f );
        }
#endif
        
        { // init camera
            rect R = RectMin();
            for( uint32 iObs = 0; iObs < editor->nObs; iObs++ ) {
                LINE_SEG obs = editor->obs[ iObs ];
                
                vec2 P = obs.P;
                vec2 Q = obs.Q;
                
                R.left   = minValue( R.left,   P.x );
                R.bottom = minValue( R.bottom, P.y );
                R.right  = maxValue( R.right,  P.x );
                R.top    = maxValue( R.top,    P.y );
                R.left   = minValue( R.left,   Q.x );
                R.bottom = minValue( R.bottom, Q.y );
                R.right  = maxValue( R.right,  Q.x );
                R.top    = maxValue( R.top,    Q.y );
            }
            R = addRadius( R, 0.1f );
            vec2 dim = getDim( R );
            
            flo32 aspA = 1920.0f / 1080.0f;
            flo32 aspB = dim.x / dim.y;
            
            if( aspB < aspA ) {
                dim.x = dim.y * aspA;
            } else {
                dim.y = dim.x / aspA;
            }
            
            editor->camera_pos = getCenter( R );
            editor->camera_dim = dim;
        }
        
        BACKGROUND back = {};
        back.position  = {};
        back.scale     = Vec2( 0.25f, 0.25f );
        back.radians   = 0.0f;
        back.modelID   = modelID_rect;
        back.textureID = textureID_background_cave_rock;
        back.modColor  = COLOR_WHITE;
        editor->lastCreated = back;
    }
    
    // input
    int32 iHover = -1;
    { // cursor
        rect camera_bound = rectCD( editor->camera_pos, editor->camera_dim );
        vec2 P = getT( camera_bound, clamp01( mouse->position      / appState->app_dim ) );
        vec2 Q = getT( camera_bound, clamp01( appState->mouse_base / appState->app_dim ) );
        editor->cursor_dPos = P - Q;
        DISPLAY_VALUE( vec2, editor->cursor_dPos );
        
        boo32 updateCursor = ( editor->mode == editorMode_default ) || ( ( editor->mode == editorMode_scale ) && ( !editor->scale_isActive ) ) || ( editor->mode == editorMode_order ) || ( editor->mode == editorMode_texture );
        
        if( updateCursor ) {
            editor->cursor_pos += editor->cursor_dPos;
            editor->cursor_pos  = clampToBound( editor->cursor_pos, camera_bound );
        }
    }
    
    switch( editor->mode ) {
        case editorMode_default: {
            updateCameraInput( editor, keyboard );
            for( uint32 iBack = 0; iBack < editor->nBack; iBack++ ) {
                BACKGROUND back = editor->back[ iBack ];
                orect bound = orectCHD( back.position, back.scale, back.radians );
                if( isInBound( editor->cursor_pos, bound ) ) {
                    iHover = iBack;
                }
            }
            
            if( wasPressed( mouse, mouseButton_left ) ) {
                if( iHover > -1 ) {
                    editor->back_isSelected[ iHover ] = !editor->back_isSelected[ iHover ];
                    if( editor->back_isSelected[ iHover ] ) {
                        editor->nSelected++;
                    } else {
                        editor->nSelected--;
                    }
                } else {
                    editor->nSelected = 0;
                    memset( editor->back_isSelected, 0, sizeof( boo32 ) * editor->nBack );
                }
            }
            
            if( wasPressed( keyboard, keyCode_n ) ) {
                BACKGROUND back = editor->lastCreated;
                back.position  = editor->cursor_pos;
                
                Assert( editor->nBack < EDITOR_STATE__BACKGROUND_MAX_COUNT );
                editor->back[ editor->nBack++ ] = back;
                
                editor->hasUnsavedChanges = true;
            }
            
#if 0        
            if( ( iHover > -1 ) && ( mouse->wheelClicks != 0 ) ) {
                uint32 nTextureID = 6;
                
                BACKGROUND * back = editor->back + iHover;
                
                int32 n = back->textureID - textureID_background_cave_rock01 + nTextureID;
                Assert( n > mouse->wheelClicks );
                n += mouse->wheelClicks;
                n %= nTextureID;
                
                back->textureID = ( TEXTURE_ID )( textureID_background_cave_rock01 + n );
            }
#endif
            
            
            if( wasPressed( keyboard, keyCode_s ) ) {
                editor->mode = editorMode_scale;
            }
            if( wasPressed( keyboard, keyCode_o ) ) {
                editor->mode = editorMode_order;
            }
            if( wasPressed( keyboard, keyCode_t ) ) {
                editor->mode = editorMode_texture;
            }
            if( editor->nSelected > 0 ) {
                if( wasPressed( keyboard, keyCode_g ) ) {
                    editor->mode = editorMode_translate;
                    memcpy( editor->back_temp, editor->back, sizeof( BACKGROUND ) * editor->nBack );
                }
                if( wasPressed( keyboard, keyCode_r ) ) {
                    editor->mode = editorMode_rotate;
                    editor->modify_pos      = editor->cursor_pos;
                    editor->modify_basePos  = editor->cursor_pos;
                    memcpy( editor->back_temp, editor->back, sizeof( BACKGROUND ) * editor->nBack );
                }
                if( wasPressed( keyboard, keyCode_delete ) ) {
                    uint32 newCount = 0;
                    for( uint32 iBack = 0; iBack < editor->nBack; iBack++ ) {
                        if( !editor->back_isSelected[ iBack ] ) {
                            editor->back[ newCount++ ] = editor->back[ iBack ];
                        } else {
                            editor->back_isSelected[ iBack ] = false;
                        }
                    }
                    editor->nBack = newCount;
                    editor->nSelected = 0;
                }
            }
            
            if( wasPressed( keyboard, keyCode_w ) ) {
                MEMORY  _output = subArena( tempMemory, _MB( 1 ) );
                MEMORY * output = &_output;
                
                writeSegment( output, "\n" );
                for( uint32 iBack = 0; iBack < editor->nBack; iBack++ ) {
                    BACKGROUND back = editor->back[ iBack ];
                    
                    switch( back.textureID ) {
                        case textureID_background_cave_rock: {
                            writeSegment( output, "addCaveRock" );
                        } break;
                        
                        case textureID_background_caveWall: {
                            writeSegment( output, "addCaveWall" );
                        } break;
                        
                        default: {
                            writeSegment( output, "noFunctionForTextureID" );
                        } break;
                    }
                    
                    writeSegment( output, "( controlState, world, " );
                    writeSegment( output, "Vec2( " );
                    writeF32    ( output, back.position.x );
                    writeSegment( output, ", " );
                    writeF32    ( output, back.position.y );
                    writeSegment( output, " ), Vec2( " );
                    writeF32    ( output, back.scale.x );
                    writeSegment( output, ", " );
                    writeF32    ( output, back.scale.y );
                    writeSegment( output, " ), " );
                    writeF32    ( output, back.radians );
                    writeSegment( output, " );\n" );
                    
                }
                writeSegment( output, "\n" );
                platform->writeFile( 0, "../../code/temp_output.cpp", output->base, ( uint32 )output->used );
                
                editor->hasUnsavedChanges = false;
            }
            
        } break;
        
        case editorMode_translate: {
            for( uint32 iBack = 0; iBack < editor->nBack; iBack++ ) {
                if( editor->back_isSelected[ iBack ] ) {
                    BACKGROUND * back = editor->back + iBack;
                    back->position += editor->cursor_dPos;
                }
            }
            
            if( wasPressed( keyboard, keyCode_escape ) ) {
                editor->mode = editorMode_default;
                memcpy( editor->back, editor->back_temp, sizeof( BACKGROUND ) * editor->nBack );
            }
            
            boo32 confirmModify = ( wasPressed( keyboard, keyCode_enter ) ) || ( wasPressed( mouse, mouseButton_left ) );
            if( confirmModify ) {
                editor->mode = editorMode_default;
                editor->hasUnsavedChanges = true;
            }
        } break;
        
        case editorMode_scale: {
            if( editor->scale_isActive ) {
                editor->modify_pos += editor->cursor_dPos;
                
                BACKGROUND * back = editor->back + editor->scale_sBack;
                
                vec2 origin = editor->scale_bPos;
                vec2 axis   = editor->scale_bAxis;
                
                vec2  V = editor->modify_pos - origin;
                vec2  N = getNormal( axis );  
                
                flo32 t = maxValue( dot( V, axis ) / getLengthSq( axis ), 0.0f );
                flo32 d = getLength( axis ) * t * 0.5f;
                
                back->position = origin + N * d;
                if( ( editor->scale_sSide % 2 ) == 0 ) {
                    back->scale.x = d;
                } else {
                    back->scale.y = d;
                }
                
                orect R = orectCHD( back->position, back->scale, back->radians );
                drawORectOutline( pass, R, COLOR_YELLOW );
                
                if( wasReleased( mouse, mouseButton_left ) ) {
                    editor->scale_isActive = false;
                    editor->scale_sBack    = -1;
                    editor->scale_sSide    = -1;
                    editor->hasUnsavedChanges = true;
                    
                    editor->lastCreated.scale = back->scale;
                }
                if( wasPressed( keyboard, keyCode_escape ) ) {
                    editor->scale_isActive = false;
                    editor->scale_sBack    = -1;
                    editor->scale_sSide    = -1;
                    memcpy( editor->back, editor->back_temp, sizeof( BACKGROUND ) * editor->nBack );
                }
            } else {
                updateCameraInput( editor, keyboard );
                
                int32 sBack = -1;
                int32 sSide = -1;
                orect sBound = {};
                vec2  bPos   = {};
                vec2  bAxis  = {};
                
                for( uint32 iBack = 0; iBack < editor->nBack; iBack++ ) {
                    BACKGROUND back = editor->back[ iBack ];
                    
                    switch( back.modelID ) {
                        case modelID_rect: {
                            orect R = orectCHD( back.position, back.scale, back.radians );
                            
                            orect side[ 4 ] = {};
                            for( uint32 iter = 0; iter < 4; iter++ ) { side[ iter ] = R; }
                            
                            side[ 0 ].center    = R.center - R.xAxis * R.halfDim.x;
                            side[ 0 ].halfDim.x = 0.01f;
                            side[ 1 ].center    = R.center - R.yAxis * R.halfDim.y;
                            side[ 1 ].halfDim.y = 0.01f;
                            side[ 2 ].center    = R.center + R.xAxis * R.halfDim.x;
                            side[ 2 ].halfDim.x = 0.01f;
                            side[ 3 ].center    = R.center + R.yAxis * R.halfDim.y;
                            side[ 3 ].halfDim.y = 0.01f;
                            
                            for( uint32 iter = 0; iter < 4; iter++ ) {
                                if( isInBound( editor->cursor_pos, side[ iter ] ) ) {
                                    sBack  = iBack;
                                    sSide  = iter;
                                    sBound = side[ iter ];
                                    
                                    uint32 opposite = ( iter + 2 ) % 4;
                                    bPos   = side[ opposite ].center;
                                    bAxis  = side[ iter ].center - bPos;
                                }
                            }
                        } break;
                        
                        default: {
                            InvalidCodePath;
                        } break;
                    }
                }
                if( wasPressed( keyboard, keyCode_escape ) ) {
                    editor->mode = editorMode_default;
                }
                
                if( sBack > -1 ) {
                    Assert( sSide > -1 );
                    drawORectOutline( pass, sBound, COLOR_YELLOW );
                    
                    if( wasPressed( mouse, mouseButton_left ) ) {
                        editor->scale_isActive = true;
                        editor->scale_sBack    = sBack;
                        editor->scale_sSide    = sSide;
                        editor->scale_bPos     = bPos;
                        editor->scale_bAxis    = bAxis;
                        
                        editor->modify_pos     = editor->cursor_pos;
                        
                        memcpy( editor->back_temp, editor->back, sizeof( BACKGROUND ) * editor->nBack );
                    }
                }
            }
        } break;
        
        case editorMode_rotate: {
            editor->modify_pos += editor->cursor_dPos;
            
            for( uint32 iBack = 0; iBack < editor->nBack; iBack++ ) {
                if( editor->back_isSelected[ iBack ] ) {
                    BACKGROUND * back      = editor->back      + iBack;
                    BACKGROUND * back_temp = editor->back_temp + iBack;
                    
                    vec2 U = getNormal( editor->modify_pos     - back->position );
                    vec2 V = getNormal( editor->modify_basePos - back->position );
                    vec2 N = getPerp( V );
                    
                    flo32 radians = acosf( dot( U, V ) );
                    if( dot( U, N ) < 0.0f ) {
                        radians = -radians;
                    }
                    back->radians = back_temp->radians + radians;
                    
                    drawLine( pass, back->position, editor->modify_pos,     COLOR_GREEN );
                    drawLine( pass, back->position, editor->modify_basePos, COLOR_BLUE  );
                    
                    editor->lastCreated.radians = back->radians;
                }
            }
            
            if( wasPressed( keyboard, keyCode_escape ) ) {
                editor->mode = editorMode_default;
                memcpy( editor->back, editor->back_temp, sizeof( BACKGROUND ) * editor->nBack );
            }
            
            boo32 confirmModify = ( wasPressed( keyboard, keyCode_enter ) ) || ( wasPressed( mouse, mouseButton_left ) );
            if( confirmModify ) {
                editor->mode = editorMode_default;
                editor->hasUnsavedChanges = true;
            }
        } break;
        
        case editorMode_order: {
            updateCameraInput( editor, keyboard );
            for( uint32 iBack = 0; iBack < editor->nBack; iBack++ ) {
                BACKGROUND back = editor->back[ iBack ];
                orect bound = orectCHD( back.position, back.scale, back.radians );
                if( isInBound( editor->cursor_pos, bound ) ) {
                    iHover = iBack;
                }
            }
            
            if( ( iHover > -1 ) && ( mouse->wheelClicks != 0 ) ) {
                editor->hasUnsavedChanges = true;
                
                if( mouse->wheelClicks < 0 ) {
                    int32 end = maxValue( iHover + mouse->wheelClicks, 0 );
                    for( int32 iter = iHover; iter > end; iter-- ) {
                        _swap( BACKGROUND, editor->back[ iter ], editor->back[ iter - 1 ] );
                    }
                } else {
                    int32 end = minValue( iHover + mouse->wheelClicks, ( int32 )editor->nBack - 1 );
                    for( int32 iter = iHover; iter < end; iter++ ) {
                        _swap( BACKGROUND, editor->back[ iter ], editor->back[ iter + 1 ] );
                    }
                }
            }
            
            for( uint32 iBack = 0; iBack < editor->nBack; iBack++ ) {
                BACKGROUND back = editor->back[ iBack ];
                
                flo32 t = 0.002f * editor->camera_dim.y;
                
                char str[ 8 ] = {};
                sprintf( str, "%u", iBack );
                drawString( pass, appState->font, str, back.position, textAlign_center, Vec2( t, t ), COLOR_WHITE );
            }
            
            if( wasPressed( keyboard, keyCode_escape ) ) {
                editor->mode = editorMode_default;
            }
        } break;
        
        case editorMode_texture: {
            TEXTURE_ID textureID[] = {
                textureID_background_cave_rock,
                textureID_background_caveWall,
            };
            uint32 nTexID = _arrayCount( textureID );
            
            updateCameraInput( editor, keyboard );
            for( uint32 iBack = 0; iBack < editor->nBack; iBack++ ) {
                BACKGROUND back = editor->back[ iBack ];
                orect bound = orectCHD( back.position, back.scale, back.radians );
                if( isInBound( editor->cursor_pos, bound ) ) {
                    iHover = iBack;
                }
            }
            
            if( ( iHover > -1 ) && ( mouse->wheelClicks != 0 ) ) {
                editor->hasUnsavedChanges = true;
                
                BACKGROUND * back = editor->back + iHover;
                
                int32 texID = -1;
                for( uint32 iter = 0; iter < nTexID; iter++ ) {
                    if( back->textureID == textureID[ iter ] ) {
                        texID = iter;
                    }
                }
                Assert( texID > -1 );
                
                texID = ( texID + 1 ) % nTexID;
                back->textureID = textureID[ texID ];
                
                editor->lastCreated.textureID = textureID[ texID ];
            }
            
            if( wasPressed( keyboard, keyCode_escape ) ) {
                editor->mode = editorMode_default;
            }
        } break;
    }
    
    
#if 0    
    if( editor->scale_isActive ) {
        editor->modify_pos += editor->cursor_dPos;
        
        if( wasPressed( keyboard, keyCode_x ) ) {
            editor->scale_mode = scaleMode_xAxis;
            memcpy( editor->back, editor->back_temp, sizeof( BACKGROUND ) * editor->nBack );
        }
        if( wasPressed( keyboard, keyCode_y ) ) {
            editor->scale_mode = scaleMode_yAxis;
            memcpy( editor->back, editor->back_temp, sizeof( BACKGROUND ) * editor->nBack );
        }
        
        for( uint32 iBack = 0; iBack < editor->nBack; iBack++ ) {
            if( editor->back_isSelected[ iBack ] ) {
                BACKGROUND * back = editor->back + iBack;
                
                flo32 lengthA = getLength( editor->modify_basePos - back->position );
                flo32 lengthB = getLength( editor->modify_pos     - back->position );
                flo32 t = lengthB / lengthA;
                
                switch( editor->scale_mode ) {
                    case scaleMode_bothAxes: {
                        back->scale.x = t;
                        back->scale.y = t;
                    } break;
                    
                    case scaleMode_xAxis: {
                        back->scale.x = t;
                    } break;
                    
                    case scaleMode_yAxis: {
                        back->scale.y = t;
                    } break;
                }
                
                back->scale.x = maxValue( back->scale.x, 0.05f );
                back->scale.y = maxValue( back->scale.y, 0.05f );
                
                editor->lastCreated.scale = back->scale;
            }
        }
    }
#endif
    
    DISPLAY_VALUE( uint32, editor->nSelected );
    
    // draw
    if( iHover > -1 ) {
        BACKGROUND back = editor->back[ iHover ];
        
        orect bound = orectCHD( back.position, back.scale, back.radians );
        drawORectOutline( pass, bound, COLOR_GRAY( 0.25f ) );
    }
    
    for( uint32 iBack = 0; iBack < editor->nBack; iBack++ ) {
        BACKGROUND back = editor->back[ iBack ];
        
        drawModel( pass, back.modelID, back.textureID, back.position, back.scale, back.radians, back.modColor );
        
        if( editor->back_isSelected[ iBack ] ) {
            orect bound = orectCHD( back.position, back.scale, back.radians );
            drawORectOutline( pass, bound, COLOR_YELLOW );
        }
    }
    
    for( uint32 iObs = 0; iObs < editor->nObs; iObs++ ) {
        LINE_SEG obs = editor->obs[ iObs ];
        
        drawLine( pass, obs.P, obs.Q, COLOR_GREEN );
    }
    
    { // draw cursor
        flo32 t = editor->camera_dim.y;
        rect R = rectCD( editor->cursor_pos, Vec2( 0.01f, 0.01f ) * Vec2( t, t ) );
        rect S = addRadius( R, -0.0025f * t);
        
        drawRect( pass, R, COLOR_BLACK );
        drawRect( pass, S, COLOR_WHITE );
    }
    
    switch( editor->mode ) {
        case editorMode_default:   { DISPLAY_STRING( "MODE: DEFAULT"   ); } break;
        case editorMode_translate: { DISPLAY_STRING( "MODE: TRANSLATE" ); } break;
        case editorMode_scale:     { DISPLAY_STRING( "MODE: SCALE"     ); } break;
        case editorMode_rotate:    { DISPLAY_STRING( "MODE: ROTATE"    ); } break;
        case editorMode_order:     { DISPLAY_STRING( "MODE: ORDER"     ); } break;
        case editorMode_texture:   { DISPLAY_STRING( "MODE: TEXTURE"   ); } break;
    }
    if( editor->hasUnsavedChanges ) {
        DISPLAY_STRING( "UNSAVED CHANGES!!!" );
    }
}