
internal void
spawnItem( ITEM_STATE * state, ITEM_ID itemID, vec2 pos, rect bound, uint32 value = 0 ) {
    vec2 offset = RandomDirection2D() * ITEM_OFFSET;
    vec2 dest   = pos + offset;
    
    if( ( dest.x <= bound.left   ) || ( dest.x >= bound.right ) ) {
        offset.x = -offset.x;
    }
    if( ( dest.y <= bound.bottom ) || ( dest.y >= bound.top   ) ) {
        offset.y = -offset.y;
    }
    dest = pos + offset;
    
    addItem( state, itemID, dest );
}

internal void
updateItems( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
    ITEM_STATE * state  = &appState->itemState;
    PLAYER_STATE  * player = &appState->playerState;
    FISH_ATE_LIST * list   = &appState->fishAteList;
    
    if( list->attemptBite ) {
        flo32 radius   = ITEM_BITE_RADIUS;
        flo32 radiusSq = radius * radius;
        
        uint32 newCount = 0;
        for( uint32 iItem = 0; iItem < state->nItem; iItem++ ) {
            GAME_ITEM item = state->item[ iItem ];
            
            flo32 distSq = getLengthSq( player->position - item.position );
            if( distSq <= radiusSq ) {
                appState->nThingsAte++;
                
                switch( item.itemID ) {
                    case itemID_heal: {
                        // TODO: heal audio
                        // TODO: heal particles
                        
                        player->health = minValue( player->health + PLAYER_HEAL_AMOUNT, 1.0f );
                    } break;
                    
                    case itemID_pointMultiplier: {
                        stopAudio( audioSystem, audioID_scoreMultiplier );
                        
                        playAudio( audioSystem, appState->audio, audioID_bite4, 0.5f );
                        playAudio( audioSystem, appState->audio, audioID_scoreMultiplier, 1.0f );
                    } break;
                    
                    case itemID_slowDownTime: {
                        playAudio( audioSystem, appState->audio, audioID_bite4, 0.5f );
                    } break;
                    
                    case itemID_strongBite: {
                        stopAudio( audioSystem, audioID_scoreMultiplier );
                        
                        playAudio( audioSystem, appState->audio, audioID_bite4, 0.5f );
                        playAudio( audioSystem, appState->audio, audioID_scoreMultiplier, 1.0f );
                    } break;
                }
            } else {
                state->item[ newCount++ ] = item;
            }
        }
        state->nItem = newCount;
    }
    if( player->doSlap ) {
        flo32 radius   = ITEM_SLAP_RADIUS;
        flo32 radiusSq = radius * radius;
        
        uint32 newCount = 0;
        for( uint32 iItem = 0; iItem < state->nItem; iItem++ ) {
            GAME_ITEM item = state->item[ iItem ];
            
            flo32 distSq = getLengthSq( player->position - item.position );
            if( distSq <= radiusSq ) {
                boo32 sendToStorage = true;
                int32 iEmpty        = -1;
                
                ITEM_INVENTORY * inventory = &appState->item_inventory;
                for( uint32 iSlot = 0; iSlot < inventory->nSlots; iSlot++ ) {
                    INVENTORY_ITEM * slot = inventory->item + iSlot;
                    if( ( iEmpty == -1 ) && ( slot->itemID == itemID_empty ) ) {
                        iEmpty = iSlot;
                    }
                    if( ( slot->itemID == item.itemID ) && ( slot->nItem < slot->maxItem ) ) {
                        slot->nItem++;
                        sendToStorage = false;
                    }
                }
                
                if( sendToStorage ) {
                    if( iEmpty > -1 ) {
                        inventory->item[ iEmpty ] = InventoryItem( item.itemID, 1 );
                    } else {
                        ITEM_STORAGE * storage = &appState->item_storage;
                        storage->nItem[ item.itemID ]++;
                        
                        inventory->toInventory_timer = ITEM_INVENTORY__TO_INVENTORY_ICON_TARGET_TIME;
                        inventory->toInventory_timer = 1.3f;
                    }
                }
                
                // play audio if in current inventory
                // play audio if sent to storage
                // update animation in item toolbar
            } else {
                state->item[ newCount++ ] = item;
            }
        }
        state->nItem = newCount;
    }
    
    uint32 typeAte[ fishType_count ] = {};
    
    for( uint32 iFish = 0; iFish < list->nFishAte; iFish++ ) {
        FISH_ATE fish = list->fishAte[ iFish ];
        typeAte[ fish.type ]++;
    }
    
    uint32 ate_rainbow = list->nAte[ fishType_rainbow ];
    if( ate_rainbow >= 2 ) {
        rect bound = addRadius( appState->cameraBound, -40.0f );
        spawnItem( state, itemID_pointMultiplier, player->position, bound, ate_rainbow );
    }
    
    if( typeAte[ fishType_goldfish ] >= 1 ) {
        uint32 value = rand() % 4;
        if( value == 0 ) {
            rect bound = addRadius( appState->cameraBound, -40.0f );
            spawnItem( state, itemID_slowDownTime, player->position, bound );
        }
    }
    
    if( typeAte[ fishType_dogfish ] >= 1 ) {
        uint32 value = rand() % 4;
        if( value == 0 ) {
            rect bound = addRadius( appState->cameraBound, -40.0f );
            spawnItem( state, itemID_strongBite, player->position, bound );
        }
    }
}

internal void
drawItems( RENDER_PASS * pass, APP_STATE * appState ) {
    ITEM_STATE * state = &appState->itemState;
    
    const char * label[ itemID_count ] = {
        "2x",
        "SLOW",
        "BITE",
    };
    
    for( uint32 iItem = 0; iItem < state->nItem; iItem++ ) {
        GAME_ITEM item = state->item[ iItem ];
        switch( item.itemID ) {
            case itemID_heal: {
                vec2 posA = item.position + ITEM_HEAL_BITMAP_OFFSET;
                drawQuad( pass, textureID_healAttempts, posA, ITEM_HEAL_BITMAP_SCALE, COLOR_WHITE );
                drawQuad( pass, textureID_bubble_idle, item.position, ITEM_BUBBLE_BITMAP_SCALE, COLOR_WHITE );
            } break;
            
            case itemID_pointMultiplier: {
                drawCircle( pass, item.position, ITEM_RADIUS, COLOR_WHITE );
                
                char str[ 4 ] = {};
                //sprintf( str, "%ux", item.multiplier );
                drawString( pass, appState->font, str, item.position, textAlign_center, COLOR_BLACK );
            } break;
            
            default: {
                drawCircle( pass, item.position, ITEM_RADIUS, COLOR_WHITE );
                drawString( pass, appState->font, label[ item.itemID ], item.position, textAlign_center, COLOR_BLACK );
                
            } break;
        }
    }
}

internal void
finalizeItems( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
#if 0    
    ITEM_STATE * state = &appState->itemState;
    
    rect bound = addRadius( appState->cameraBound, 50.0f );
    
    uint32 newCount = 0;
    for( uint32 iItem = 0; iItem < state->nItem; iItem++ ) {
        GAME_ITEM item = state->item[ iItem ];
        if( isInBound( item.position, bound ) ) {
            state->item[ newCount++ ] = item;
        }
    }
    state->nItem = newCount;
#endif
}

internal void
moveStorageToInventory( APP_STATE * appState ) {
    ITEM_INVENTORY * inventory = &appState->item_inventory;
    ITEM_STORAGE   * storage   = &appState->item_storage;
    
    uint32 minHeal = 3;
    uint32 nHeal   = 0;
    for( uint32 iSlot = 0; iSlot < inventory->nSlots; iSlot++ ) {
        INVENTORY_ITEM * item = inventory->item + iSlot;
        if( item->itemID == itemID_heal ) {
            nHeal++;
        }
    }
    nHeal += storage->nItem[ itemID_heal ];
    if( nHeal < minHeal ) {
        storage->nItem[ itemID_heal ] += ( minHeal - nHeal );
    }
    
    for( uint32 iSlot = 0; iSlot < inventory->nSlots; iSlot++ ) {
        INVENTORY_ITEM * item = inventory->item + iSlot;
        
        uint32 canMoveA = item->maxItem - item->nItem;
        uint32 canMoveB = storage->nItem[ item->itemID ];
        uint32 canMove = minValue( canMoveA, canMoveB );
        
        item->nItem += canMove;
    }
}

internal void
initInventoryCharge( APP_STATE * appState  ) {
    ITEM_INVENTORY * inventory  = &appState->item_inventory;
    INVENTORY_ITEM * activeItem = inventory->item + inventory->activeSlot;
    if( ( activeItem->itemID != itemID_empty ) && ( activeItem->nItem > 0 ) ) {
        inventory->charge_isActive = true;
        inventory->charge_timer    = 0.0f;
        inventory->charge_targetTime = PLAYER_FIRST_HEAL_TARGET_TIME;
        // TODO: get target time based on itemID
    }
}

internal void
updateInventory( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, MOUSE_STATE * mouse, flo32 dt ) {
    ITEM_INVENTORY * inventory = &appState->item_inventory;
    inventory->toInventory_timer = maxValue( inventory->toInventory_timer - dt, 0.0f );
    
    if( inventory->charge_isActive ) {
        inventory->charge_timer += dt;
        if( inventory->charge_timer >= inventory->charge_targetTime ) {
            inventory->charge_timer = 0.0f;
            
            INVENTORY_ITEM * activeItem = inventory->item + inventory->activeSlot;
            activeItem->nItem--;
            
            if( activeItem->nItem == 0 ) {
                inventory->charge_isActive = false;
            }
            // TODO: update isActive and targetTime for multi-use
            
            switch( activeItem->itemID ) {
                case itemID_heal: {
                    PLAYER_STATE * player = &appState->playerState;
                    player->health      = minValue( player->health + PLAYER_HEAL_AMOUNT, 1.0f );
                    player->attemptHeal = true;
                    
                    inventory->charge_targetTime = PLAYER_MULTI_HEAL_TARGET_TIME;
                    
                    // TODO: add heal audio
                    // TODO: add heal particles
                } break;
                
                default: {
                    InvalidCodePath;
                } break;
            }
        }
    }
    if( wasReleased( mouse, mouseButton_middle ) ) {
        inventory->charge_isActive = false;
    }
}

internal void
drawItem( RENDER_PASS * pass, FONT * font, ITEM_ID itemID, rect bound, vec4 modColor ) {
    switch( itemID ) {
        case itemID_empty: {
            drawString( pass, font, "EMPTY", getCenter( bound ), textAlign_center, Vec2( 1.5f, 1.5f ), COLOR_WHITE );
        } break;
        
        case itemID_pointMultiplier: {
            drawString( pass, font, "POINT", getCenter( bound ), textAlign_center, Vec2( 1.5f, 1.5f ), COLOR_WHITE );
        } break;
        
        case itemID_slowDownTime: {
            drawString( pass, font, "SLOW", getCenter( bound ), textAlign_center, Vec2( 1.5f, 1.5f ), COLOR_WHITE );
        } break;
        
        case itemID_strongBite: {
            drawString( pass, font, "BITE", getCenter( bound ), textAlign_center, Vec2( 1.5f, 1.5f ), COLOR_WHITE );
        } break;
        
        case itemID_heal: {
            drawRect( pass, textureID_healAttempts, bound, modColor );
        } break;
        
        default: {
            InvalidCodePath;
        } break;
    }
}

internal void
drawInventoryItem( RENDER_PASS * pass, FONT * font, INVENTORY_ITEM item, rect bound, boo32 isActiveSlot, flo32 t ) {
    rect outer = bound;
    rect inner = addRadius( outer, -2.0f );
    vec2 inner_dim = getDim( inner );
    
    vec4 backgroundColor = COLOR_GRAY( 0.2f );
    if( isActiveSlot ) {
        backgroundColor = COLOR_YELLOW;
    }
    drawRect( pass, textureID_whiteTexture, outer, backgroundColor );
    drawRect( pass, textureID_whiteTexture, inner, COLOR_GRAY( 0.15f ) );
    
    if( t > 0.0f ) {
        rect status = rectBLD( getBL( inner ), Vec2( inner_dim.x, inner_dim.y * t ) );
        drawRect( pass, textureID_whiteTexture, status, COLOR_YELLOW );
    }
    
    vec4 modColor = COLOR_WHITE;
    if( item.nItem == 0 ) {
        modColor = Vec4( 0.4f, 0.7f, 0.7f, 1.0f );
    }
    
    drawItem( pass, font, item.itemID, bound, modColor );
    
    char str[ 4 ] = {};
    sprintf( str, "%u", item.nItem );
    drawString( pass, font, str, getBR( inner ) + Vec2( -2.0f, -4.0f ), textAlign_bottomRight, Vec2( 4.0f, 4.0f ), COLOR_WHITE );
}

internal void
drawInventory( RENDER_PASS * pass, APP_STATE * appState ) {
    ITEM_INVENTORY * inventory = &appState->item_inventory;
    
    rect bound = appState->panel_bound;
    drawRect( pass, textureID_whiteTexture, bound, COLOR_BLACK );
    
    flo32 marginX = 8.0f;
    flo32 slot_width = getWidth( bound ) - marginX * 2.0f;
    vec2  slot_dim   = Vec2( slot_width, slot_width );
    flo32 advanceY = slot_dim.y + 6.0f;
    
    vec2 atPos = getTL( appState->panel_bound ) + Vec2( marginX, -4.0f );
    
    
    vec2       toInventory_dim       = Vec2( slot_width, slot_width * 0.5f );
    rect       toInventory_bound     = rectTLD( atPos, toInventory_dim );
    TEXTURE_ID toInventory_textureID = textureID_inventoryIcon_close;
    vec4       toInventory_modColor  = COLOR_WHITE;
    
    rect background_bound    = scaleRect( toInventory_bound, Vec2( 0.95f, 0.3f ) );
    vec4 background_modColor = COLOR_GRAY( 0.18f );
    if( inventory->toInventory_timer > 0.0f ) {
        flo32 targetA = 1.1f;
        flo32 targetB = 1.0f;
        
        if( inventory->toInventory_timer > 1.15f ) {
            toInventory_textureID = textureID_inventoryIcon_open;
        }
        
        flo32 t = clamp01( ( inventory->toInventory_timer - targetA ) / ITEM_INVENTORY__TO_INVENTORY_ICON_TARGET_TIME );
        flo32 add = lerp( 0.0f, t, 20.0f );
        toInventory_bound = addRadius( toInventory_bound, add );
        
        flo32 s = clamp01( inventory->toInventory_timer / targetB );
        flo32 a = lerp( 1.0f, s, 2.0f );
        toInventory_modColor = Vec4( a, a, a, 1.0f );
        background_modColor  = lerp( background_modColor, s, COLOR_GRAY( 0.5f ) );
    }
    drawRect( pass, background_bound, background_modColor );
    drawRect( pass, toInventory_textureID, toInventory_bound, toInventory_modColor );
    atPos.y -= toInventory_dim.y;
    
    for( uint32 iItem = 0; iItem < inventory->nSlots; iItem++ ) {
        rect R = rectTLD( atPos, slot_dim );
        
        boo32 isActiveSlot = ( iItem == inventory->activeSlot );
        
        flo32 targetTime = inventory->charge_targetTime;
        //if( player->multiHeal_isActive ) {
        //targetTime = PLAYER_MULTI_HEAL_TARGET_TIME;
        //}
        flo32 t = 0.0f;
        if( ( inventory->charge_isActive ) && ( isActiveSlot ) ) {
            t = clamp01( inventory->charge_timer / targetTime );
        }
        
        drawInventoryItem( pass, appState->font, inventory->item[ iItem ], R, isActiveSlot, t );
        
        atPos.y -= advanceY;
    }
}