
internal void
initHome( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
    PLAYER_STATE * player = &appState->playerState;
    player->position = {};
    
    CAMERA_STATE * camera = &appState->cameraState;
    camera->at  = {};
    camera->dim = appState->init_cameraDim;
    appState->cameraBound = rectCD( camera->at, camera->dim );
    
    ITEM_STORAGE   * storage   = &appState->item_storage;
    ITEM_INVENTORY * inventory = &appState->item_inventory;
    appState->item_inventory.nSlots = 1;
    appState->item_inventory.item[ 0 ] = InventoryItem( itemID_heal, 3 );
    
    //storage->nItem[ itemID_heal ]            = 4;
    //storage->nItem[ itemID_pointMultiplier ] = 4;
    //storage->nItem[ itemID_slowDownTime    ] = 4;
    //storage->nItem[ itemID_strongBite      ] = 4;
    
    //inventory->item[ inventory->nSlots++ ] = InventoryItem( itemID_heal,            3 );
    //inventory->item[ inventory->nSlots++ ] = InventoryItem( itemID_pointMultiplier, 3 );
    //inventory->item[ inventory->nSlots++ ] = InventoryItem( itemID_slowDownTime,    3 );
}

internal void
updateHome( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, KEYBOARD_STATE * keyboard, MOUSE_STATE * mouse, flo32 dt ) {
    HOME_STATE   * state  = &appState->homeState;
    PLAYER_STATE * player = &appState->playerState;
    
    vec2 dPos = ( mouse->position - appState->mouse_base );
    if( state->storage_isActive ) {
        ITEM_INVENTORY * inventory = &appState->item_inventory;
        ITEM_STORAGE   * storage   = &appState->item_storage;
        
        { // update cursor
            state->cursor_pos += dPos;
            state->cursor_pos  = clampToBound( state->cursor_pos, appState->app_bound );
        }
        
        { // update inventory
            
            flo32 marginX = 8.0f;
            flo32 slot_width = getWidth( appState->panel_bound ) - marginX * 2.0f;
            vec2  slot_dim   = Vec2( slot_width, slot_width );
            flo32 advanceY = slot_dim.y + 6.0f;
            
            inventory->activeSlot = ITEM_INVENTORY__SLOT_MAX_COUNT;
            
            if( ( state->isHolding_inventory ) || ( state->isHolding_storage ) ) {
                vec2 atPos = getTL( appState->panel_bound ) + Vec2( marginX, -4.0f );
                for( uint32 iItem = 0; iItem < inventory->nSlots; iItem++ ) {
                    INVENTORY_ITEM item = inventory->item[ iItem ];
                    
                    rect R = rectTLD( atPos, slot_dim );
                    
                    boo32 isValid = ( isInBound( state->cursor_pos, R ) ) && ( ( item.itemID != state->isHolding_item.itemID ) || ( item.nItem < item.maxItem ) );
                    if( isValid ) {
                        inventory->activeSlot = iItem;
                    }
                    
                    atPos.y -= advanceY;
                }
                
                if( wasReleased( mouse, mouseButton_left ) ) {
                    if( inventory->activeSlot < ITEM_INVENTORY__SLOT_MAX_COUNT ) {
                        if( state->isHolding_inventory ) {
                            uint32 indexA = inventory->activeSlot;
                            uint32 indexB = state->isHolding_index;
                            _swap( INVENTORY_ITEM, inventory->item[ indexA ], inventory->item[ indexB ] );
                        }
                        if( state->isHolding_storage ) {
                            INVENTORY_ITEM * itemA = &state->isHolding_item;
                            INVENTORY_ITEM * itemB = inventory->item + inventory->activeSlot;
                            
                            if( itemA->itemID == itemB->itemID ) {
                                uint32 nItemA = itemB->nItem;
                                uint32 nItemB = itemB->maxItem - itemB->nItem;
                                uint32 nItem = minValue( nItemA, nItemB );
                                
                                itemA->nItem                             += nItem;
                                storage->nItem[ state->isHolding_index ] -= nItem;
                            } else {
                                storage->nItem[ itemB->itemID ] += itemB->nItem;
                                
                                *itemB = *itemA;
                                storage->nItem[ itemB->itemID ] -= itemB->nItem;
                                
                                uint32 breakHere = 10;
                            }
                        }
                        
                        // TODO: check if combining two less than full slots of the same item
                    }
                }
            } else {
                vec2 atPos = getTL( appState->panel_bound ) + Vec2( marginX, -4.0f );
                for( uint32 iItem = 0; iItem < inventory->nSlots; iItem++ ) {
                    INVENTORY_ITEM item = inventory->item[ iItem ];
                    
                    rect R = rectTLD( atPos, slot_dim );
                    
                    if( ( isInBound( state->cursor_pos, R ) ) && ( item.nItem > 0 ) ) {
                        inventory->activeSlot = iItem;
                    }
                    
                    atPos.y -= advanceY;
                }
                
                if( ( wasPressed( mouse, mouseButton_left ) ) && ( inventory->activeSlot < ITEM_INVENTORY__SLOT_MAX_COUNT ) ) {
                    state->isHolding_inventory = true;
                    state->isHolding_index     = inventory->activeSlot;
                    state->isHolding_item      = inventory->item[ inventory->activeSlot ];
                }
            }
        }
        
        { // update storage
            
            vec2 posA   = getCenter( appState->play_bound );
            vec2 dimA   = Vec2( 1200.0f, 720.0f );
            rect boundA = rectCD( posA, dimA );
            
            { // update items
                vec2 margin = Vec2( 20.0f, 20.0f );
                vec2 offset = Vec2( 20.0f, 20.0f );
                vec2 dim   = {};
                dim.x = ( dimA.x - ( margin.x * 2.0f ) - ( offset.x * 6.0f ) ) / 7.0f;
                dim.y = ( dimA.y - ( margin.y * 2.0f ) - ( offset.y * 3.0f ) ) / 4.0f;
                
                ITEM_ID activeID = itemID_count;
                
                vec2 basePos = getTL( boundA ) + Vec2( margin.x, -margin.y );
                vec2 atPos   = basePos;
                for( uint32 iItem = 0; iItem < itemID_count; iItem++ ) {
                    if( storage->nItem[ iItem ] > 0 ) {
                        rect R = rectTLD( atPos, dim );
                        
                        if( isInBound( state->cursor_pos, R ) ) {
                            activeID = ( ITEM_ID )iItem;
                        }
                        
                        atPos.x += ( dim.x + offset.x );
                        if( ( ( iItem + 1 ) % 7 ) == 0 ) {
                            atPos.x  = basePos.x;
                            atPos.y -= ( dim.y + offset.y );
                        }
                    }
                }
                
                if( ( wasPressed( mouse, mouseButton_left ) ) && ( activeID < itemID_count ) ) {
                    state->isHolding_storage = true;
                    state->isHolding_index = activeID;
                    state->isHolding_item  = InventoryItem( activeID, storage->nItem[ activeID ] );
                }
                if( ( state->isHolding_inventory ) && ( wasReleased( mouse, mouseButton_left ) ) && ( isInBound( state->cursor_pos, appState->play_bound ) ) ) {
                    INVENTORY_ITEM * item = inventory->item + state->isHolding_index;
                    
                    storage->nItem[ item->itemID ] += item->nItem;
                    *item = InventoryItem( itemID_empty, 0 );
                }
            }
            
            rect close_bound = rectCD( getTR( boundA ) + Vec2( -16.0f, -16.0f ), Vec2( 64.0f, 64.0f ) );
            boo32 closeStorage = ( wasPressed( keyboard, keyCode_escape ) ) || ( ( wasPressed( mouse, mouseButton_left ) ) && ( ( !isInBound( state->cursor_pos, boundA ) && isInBound( state->cursor_pos, appState->play_bound ) ) ||
                                                                                                                               isInBound( state->cursor_pos, close_bound ) ) );
            if( closeStorage ) {
                state->isHolding_inventory = false;
                state->isHolding_storage   = false;
                state->storage_isActive    = false;
            }
        }
        
        if( wasReleased( mouse, mouseButton_left ) ) {
            state->isHolding_inventory = false;
            state->isHolding_storage   = false;
        }
        
        DISPLAY_VALUE( boo32, state->isHolding_inventory );
        DISPLAY_VALUE( boo32, state->isHolding_storage   );
        DISPLAY_VALUE( uint32, state->isHolding_index );
    } else {
        { // update player
            player->position += dPos;
            player->position  = clampToBound( player->position, appState->cameraBound );
            
            updateFacingDir( &player->facingDir, player->position.x );
            
            DISPLAY_VALUE( vec2, player->position );
            DISPLAY_VALUE( rect, appState->cameraBound );
        }
        
        { // update storage
            vec2 pos = cameraRel( appState, Vec2( 1.2f, 0.2f ) );
            vec2 dim = Vec2( 512.0f, 512.0f ) * 0.8f;
            rect bound = rectCD( pos, dim * Vec2( 0.9f, 0.65f ) );
            
            if( isInBound( player->position, bound ) ) {
                state->storage_timer += dt;
            } else {
                state->storage_timer = 0.0f;
            }
        }
        
        { // update exit
            vec2 pos     = cameraRel( appState, Vec2( 1.3f, 0.74f ) );
            vec2 halfDim = Vec2( 100.0f, 100.0f );
            rect bound = rectCHD( pos, halfDim );
            
            if( isInBound( player->position, bound ) ) {
                state->exit_timer += dt;
            } else {
                state->exit_timer = 0.0f;
            }
        }
        
        if( wasPressed( keyboard, keyCode_escape ) ) {
            // open menu
            //  - exit game
            //  - load save
        }
    }
}

internal void
drawHome( RENDERER * renderer, APP_STATE * appState, MOUSE_STATE * mouse ) {
    RENDER_PASS * pass_game = &renderer->pass_game;
    RENDER_PASS * pass_ui   = &renderer->pass_ui;
    
    HOME_STATE * state = &appState->homeState;
    
    { // draw background
        //vec2 background_dim   = Vec2( 1024.0f, 1024.0f );
        vec2 background_dim   = Vec2( 1280.0f, 720.0f ) * 1.5f;
        rect background_bound = rectCD( Vec2( 0.0f, 0.0f ), background_dim );
        drawRect( pass_game, textureID_home_background, background_bound, COLOR_GRAY( 0.5f ) );
    }
    
    { // draw storage
        vec2 pos = cameraRel( appState, Vec2( 1.2f, 0.2f ) );
        //vec2 dim = Vec2( 400.0f, 300.0f );
        vec2 dim = Vec2( 512.0f, 512.0f ) * 0.8f;
        rect bound = rectCD( pos, dim );
        
        drawRect( pass_game, textureID_home_storage, bound, COLOR_WHITE );
        
        //rect boundA = scaleRect( bound, Vec2( 0.9f, 0.65f ) );
        //drawRectOutline( pass_game, boundA, COLOR_RED );
        
        rect R = rectCD( getBL( bound ) + Vec2( 48.0f, 120.0f ), Vec2( 160.0f, 68.0f ) );
        
        flo32 t = clamp01( state->storage_timer / HOME__OPEN_STORAGE_TARGET_TIME );
        
        rect status = R;
        status.right = lerp( status.left, t, status.right );
        drawRect       ( pass_game, R, COLOR_WHITE );
        drawRect       ( pass_game, status, COLOR_YELLOW );
        drawRectOutline( pass_game, R, COLOR_BLACK );
        drawString( pass_game, appState->font, "STORAGE", getCenter( R ), textAlign_center, Vec2( 2.0f, 2.0f ), COLOR_BLACK );
    }
    
    { // draw exit
        vec2 pos     = cameraRel( appState, Vec2( 1.3f, 0.74f ) );
        vec2 halfDim = Vec2( 100.0f, 100.0f );
        rect bound = rectCHD( pos, halfDim );
        
        drawRectOutline( pass_game, bound, COLOR_RED );
        
        vec2 xAxis = Vec2(  0.0f, 1.0f ) * halfDim;
        vec2 yAxis = Vec2( -1.0f, 0.0f ) * halfDim;
        
        flo32 t = clamp01( state->exit_timer / HOME__EXIT_BOUND_TARGET_TIME );
        
        rect status = bound;
        status.top = lerp( bound.bottom, t, bound.top );
        drawRect( pass_game, textureID_whiteTexture, status, COLOR_YELLOW );
        drawQuad( pass_game, textureID_exitArrow, pos, xAxis, yAxis, COLOR_WHITE );
    }
    
    { // draw player
        PLAYER_STATE * player = &appState->playerState;
        
        vec2 scale  = Vec2( PLAYER_BITMAP_SCALE_X,  PLAYER_BITMAP_SCALE_Y  );
        vec2 offset = Vec2( PLAYER_BITMAP_OFFSET_X, PLAYER_BITMAP_OFFSET_Y );
        if( player->facingDir.isFacingLeft ) {
            scale.x  = -scale.x;
            offset.x = -offset.x;
        }
        drawQuad( pass_game, textureID_player_idle, player->position + offset, scale, COLOR_WHITE );
    }
    
    if( state->storage_isActive ) {
        ITEM_STORAGE * storage = &appState->item_storage;
        //drawRect( pass_game, appState->cameraBound, Vec4( 0.0f, 0.0f, 0.0f, 0.8f ) );
        
        { // draw storage
            vec2 posA   = getCenter( appState->play_bound );
            vec2 dimA   = Vec2( 1200.0f, 720.0f );
            rect boundA = rectCD( posA, dimA );
            
            { // draw background
                rect boundB = addRadius( boundA, -6.0f );
                
                vec4 background_color = COLOR_GRAY( 0.2f );
                if( ( state->isHolding_inventory ) && ( isInBound( state->cursor_pos, appState->play_bound ) ) ) {
                    background_color = COLOR_YELLOW;
                }
                drawRect( pass_ui, boundA, background_color );
                drawRect( pass_ui, boundB, COLOR_BLACK );
            }
            
            { // draw items
                vec2 margin = Vec2( 20.0f, 20.0f );
                vec2 offset = Vec2( 20.0f, 20.0f );
                vec2 dim   = {};
                dim.x = ( dimA.x - ( margin.x * 2.0f ) - ( offset.x * 6.0f ) ) / 7.0f;
                dim.y = ( dimA.y - ( margin.y * 2.0f ) - ( offset.y * 3.0f ) ) / 4.0f;
                
                vec2 basePos = getTL( boundA ) + Vec2( margin.x, -margin.y );
                vec2 atPos   = basePos;
                for( uint32 iItem = 0; iItem < itemID_count; iItem++ ) {
                    if( storage->nItem[ iItem ] > 0 ) {
                        rect R = rectTLD( atPos, dim );
                        
                        boo32 isActiveSlot = ( !state->isHolding_inventory ) && ( !state->isHolding_storage ) && ( isInBound( state->cursor_pos, R ) );
                        flo32 t = 0.0f;
                        
                        INVENTORY_ITEM item = {};
                        item.itemID = ( ITEM_ID )iItem;
                        item.nItem  = storage->nItem[ iItem ];
                        drawInventoryItem( pass_ui, appState->font, item, R, isActiveSlot, t );
                        
                        atPos.x += ( dim.x + offset.x );
                        if( ( ( iItem + 1 ) % 7 ) == 0 ) {
                            atPos.x  = basePos.x;
                            atPos.y -= ( dim.y + offset.y );
                        }
                    }
                }
            }
            
            { // draw close button
                rect close_boundA = rectCD( getTR( boundA ) + Vec2( -16.0f, -16.0f ), Vec2( 64.0f, 64.0f ) );
                rect close_boundB = addRadius( close_boundA, -4.0f );
                
                vec4 background_color = COLOR_GRAY( 0.2f );
                if( ( !state->isHolding_inventory ) && ( !state->isHolding_storage ) && isInBound( state->cursor_pos, close_boundA ) ) {
                    background_color = COLOR_YELLOW;
                }
                drawRect( pass_ui, close_boundA, background_color );
                drawRect( pass_ui, close_boundB, COLOR_BLACK );
                
                drawRect( pass_ui, textureID_home_storage_x, addRadius( close_boundB, -4.0f ), COLOR_WHITE );
            }
        }
    }
    
    drawInventory( pass_ui, appState );
    
    if( state->storage_isActive ) {
        if( ( state->isHolding_inventory ) || ( state->isHolding_storage ) ) {
            INVENTORY_ITEM item = state->isHolding_item;
            
            vec2 dim   = Vec2( 40.0f, 40.0f );
            rect bound = rectTLD( state->cursor_pos, dim );
            drawItem( pass_ui, appState->font, item.itemID, bound, COLOR_WHITE );
        }
        
        { // draw cursor
            vec2 dim = Vec2( 24.0f, 24.0f );
            rect bound = rectCD( state->cursor_pos, dim );
            drawRect( pass_ui, textureID_home_storage_cursor, bound, COLOR_WHITE );
        }
    }
}

internal void
finalizeHome( MEMORY * tempMemory, AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
    HOME_STATE * state = &appState->homeState;
    
    if( state->storage_isActive ) {
    } else {
        if( state->storage_timer >= HOME__OPEN_STORAGE_TARGET_TIME ) {
            state->storage_isActive = true;
            state->storage_timer    = 0.0f;
            state->cursor_pos       = getCenter( appState->play_bound );
            
            vec2 pos = cameraRel( appState, Vec2( 1.2f, 0.2f ) );
            vec2 dim = Vec2( 400.0f, 300.0f );
            rect bound = rectCD( pos, dim );
            
            PLAYER_STATE * player = &appState->playerState;
            player->position = getTL( bound ) + Vec2( -120.0f, -60.0f );
        }
        if( state->exit_timer >= HOME__EXIT_BOUND_TARGET_TIME ) {
            appState->gameMode = gameMode_game;
            startGame( tempMemory, appState, audioSystem, false );
            // TODO: tutorial
            // TODO: level/chapter select
        }
    }
}