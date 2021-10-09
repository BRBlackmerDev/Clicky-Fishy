
internal void
reset( WORLD_SYSTEM * world ) {
    memset( world, 0, sizeof( WORLD_SYSTEM ) );
    world->camera_cellID = INT_MAX;
}

internal void
addObstacle( GAME_CONTROL_STATE * state, WORLD_SYSTEM * world, OBSTACLE obs ) {
    obs.bound = toWorldPos( state, obs.bound );
    
    Assert( world->nObs < WORLD_SYSTEM__OBSTACLE_MAX_COUNT );
    world->obs[ world->nObs++ ] = obs;
}

internal void
addBackground( GAME_CONTROL_STATE * state, WORLD_SYSTEM * world, BACKGROUND back ) {
    back.position  = toWorldPos( state, back.position );
    back.scale    *= state->world_scale;
    
    Assert( world->nBack < WORLD_SYSTEM__BACKGROUND_MAX_COUNT );
    world->back[ world->nBack++ ] = back;
}

internal void
addBackgroundRect( GAME_CONTROL_STATE * state, WORLD_SYSTEM * world, TEXTURE_ID textureID, rect bound, vec4 modColor = COLOR_WHITE, flo32 depth = 0.0f ) {
    BACKGROUND back = {};
    back.modelID = modelID_rect;
    back.textureID = textureID;
    back.position  = getCenter ( bound );
    back.scale     = getHalfDim( bound );
    back.modColor  = modColor;
    back.depth     = depth;
    addBackground( state, world, back );
}

internal void
addCaveRock( GAME_CONTROL_STATE * state, WORLD_SYSTEM * world, vec2 position, vec2 scale, flo32 radians ) {
    BACKGROUND back = {};
    back.modelID   = modelID_rect;
    back.textureID = textureID_background_cave_rock;
    back.position  = position;
    back.scale     = scale;
    back.radians   = radians;
    back.modColor  = COLOR_GRAY( 0.3f );
    back.depth     = 0.0f;
    addBackground( state, world, back );
}

internal void
addCaveWall( GAME_CONTROL_STATE * state, WORLD_SYSTEM * world, vec2 position, vec2 scale, flo32 radians ) {
    BACKGROUND back = {};
    back.modelID   = modelID_rect;
    back.textureID = textureID_background_caveWall;
    back.position  = position;
    back.scale     = scale;
    back.radians   = radians;
    back.modColor  = COLOR_GRAY( 0.35f );
    back.depth     = 0.9998f;
    addBackground( state, world, back );
}

internal void
tileBackgroundRect( GAME_CONTROL_STATE * state, WORLD_SYSTEM * world, uint32 nRow, uint32 nCol, TEXTURE_ID textureID, rect bound, vec4 modColor = COLOR_WHITE ) {
    vec2 dim = getDim( bound );
    flo32 width  = ( dim.x / ( flo32)nCol );
    flo32 height = ( dim.y / ( flo32)nRow );
    vec2 bPos = getTL( bound );
    for( uint32 iRow = 0; iRow < nRow; iRow++ ) {
        vec2 atPos = bPos;
        for( uint32 iCol = 0; iCol < nCol; iCol++ ) {
            rect R = rectTLD( atPos, Vec2( width, height ) );
            addBackgroundRect( state, world, textureID, R, modColor );
            
            atPos.x += width;
        }
        bPos.y -= height;
    }
}

internal void
genBackgroundTiles( GAME_CONTROL_STATE * state, WORLD_SYSTEM * world, LINE_SEG * obs, uint32 nObs ) {
    rect R = RectMin();
    
    for( uint32 iObs = 0; iObs < nObs; iObs++ ) {
        LINE_SEG line = obs[ iObs ];
        vec2 P = line.P;
        vec2 Q = line.Q;
        
        R.left   = minValue( R.left,   P.x );
        R.bottom = minValue( R.bottom, P.y );
        R.right  = maxValue( R.right,  P.x );
        R.top    = maxValue( R.top,    P.y );
        R.left   = minValue( R.left,   Q.x );
        R.bottom = minValue( R.bottom, Q.y );
        R.right  = maxValue( R.right,  Q.x );
        R.top    = maxValue( R.top,    Q.y );
    }
    R = addRadius( R, 0.5f );
    
    uint32 nRow = ( uint32 )( getHeight( R ) / 2.5f ) + 1;
    uint32 nCol = ( uint32 )( getWidth ( R ) / 2.5f ) + 1;
    tileBackgroundRect( state, world, nRow, nCol, textureID_background_caveWall, R, COLOR_GRAY( 0.4f ) );
}

internal void
addGroup( WORLD_SYSTEM * world, OBSTACLE_GROUP group ) {
    Assert( world->nObsGroup < WORLD_SYSTEM__OBSTACLE_GROUP_MAX_COUNT );
    world->obsGroup[ world->nObsGroup++ ] = group;
}

internal void
addGroup( WORLD_SYSTEM * world, BACKGROUND_GROUP group ) {
    Assert( world->nBackGroup < WORLD_SYSTEM__BACKGROUND_GROUP_MAX_COUNT );
    world->backGroup[ world->nBackGroup++ ] = group;
}

internal void
initWorldSystem( MEMORY * memory, APP_STATE * appState ) {
    WORLD_SYSTEM * world = &appState->worldSystem;
    
    rect bound = Rect( FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX );
    for( uint32 iObs = 0; iObs < world->nObs; iObs++ ) {
        OBSTACLE obs = world->obs[ iObs ];
        
        bound.left   = minValue( bound.left,   obs.bound.P.x );
        bound.bottom = minValue( bound.bottom, obs.bound.P.y );
        bound.right  = maxValue( bound.right,  obs.bound.P.x );
        bound.top    = maxValue( bound.top,    obs.bound.P.y );
        bound.left   = minValue( bound.left,   obs.bound.Q.x );
        bound.bottom = minValue( bound.bottom, obs.bound.Q.y );
        bound.right  = maxValue( bound.right,  obs.bound.Q.x );
        bound.top    = maxValue( bound.top,    obs.bound.Q.y );
    }
    
    // TODO: This only expects background rectangles! This needs to be expanded to account for the fact that a BACKGROUND object can expect any MESH_ID.
    for( uint32 iBack = 0; iBack < world->nBack; iBack++ ) {
        BACKGROUND back = world->back[ iBack ];
        
        rect R = rectCHD( back.position, back.scale );
        bound.left   = minValue( R.left,   bound.left   );
        bound.bottom = minValue( R.bottom, bound.bottom );
        bound.right  = maxValue( R.right,  bound.right  );
        bound.top    = maxValue( R.top,    bound.top    );
    }
    bound = addRadius( bound, 128.0f );
    
    vec2 offset = -getBL( bound );
    bound = addOffset( bound, offset );
    
    int32 maxX    = ( int32 )( bound.right / WORLD_SYSTEM__CELL_WIDTH  ) + 1;
    int32 maxY    = ( int32 )( bound.top   / WORLD_SYSTEM__CELL_HEIGHT ) + 1;
    int32 maxCell = maxX * maxY;
    
    { // OBSTACLES
        if( world->nObs > 0 ) {
            uint32 * cellID = _pushArray_clear( memory, uint32, world->nObs );
            for( uint32 iObs = 0; iObs < world->nObs; iObs++ ) {
                OBSTACLE obs = world->obs[ iObs ];
                
                int32 cellPX = ( int32 )( ( obs.bound.P.x + offset.x ) / WORLD_SYSTEM__CELL_WIDTH  );
                int32 cellPY = ( int32 )( ( obs.bound.P.y + offset.y ) / WORLD_SYSTEM__CELL_HEIGHT );
                int32 cellQX = ( int32 )( ( obs.bound.Q.x + offset.x ) / WORLD_SYSTEM__CELL_WIDTH  );
                int32 cellQY = ( int32 )( ( obs.bound.Q.y + offset.y ) / WORLD_SYSTEM__CELL_HEIGHT );
                
                Assert( cellPX > -1 );
                Assert( cellPY > -1 );
                Assert( cellQX > -1 );
                Assert( cellQY > -1 );
                
                Assert( abs( cellPX - cellQX ) <= 1 );
                Assert( abs( cellPY - cellQY ) <= 1 );
                
                int32 cellX = minValue( cellPX, cellQX );
                int32 cellY = minValue( cellPY, cellQY );
                cellID[ iObs ] = cellX * maxY + cellY;
            }
            
            boo32 doSort = true;
            while( doSort ) {
                doSort = false;
                for( uint32 iter = 0; iter < ( world->nObs - 1 ); iter++ ) {
                    if( cellID[ iter ] > cellID[ iter + 1 ] ) {
                        _swap( uint32, cellID[ iter ], cellID[ iter + 1 ] );
                        _swap( OBSTACLE, world->obs[ iter ], world->obs[ iter + 1 ] );
                        doSort = true;
                    }
                }
            }
            
            Assert( world->nObs > 0 );
            
            uint32 atIndex = 0;
            OBSTACLE_GROUP group = {};
            group.obsID = cellID[ atIndex ];
            group.bObs  = atIndex;
            while( atIndex < ( world->nObs - 1 ) ) {
                if( cellID[ atIndex ] != cellID[ atIndex + 1 ] ) {
                    group.nObs = atIndex - group.bObs + 1;
                    addGroup( world, group );
                    
                    group.obsID = cellID[ atIndex + 1 ];
                    group.bObs  = atIndex + 1;
                }
                
                atIndex++;
            }
            group.nObs = world->nObs - group.bObs;
            addGroup( world, group );
            
            _popArray( memory, uint32, world->nObs );
        }
    }
    
    { // BACKGROUND
        uint32 * cellID = _pushArray_clear( memory, uint32, world->nBack );
        for( uint32 iBack = 0; iBack < world->nBack; iBack++ ) {
            BACKGROUND back = world->back[ iBack ];
            
            rect R = rectCHD( back.position, back.scale );
            vec2 P = getBL( R );
            vec2 Q = getTR( R );
            
            int32 cellPX = ( int32 )( ( P.x + offset.x ) / WORLD_SYSTEM__CELL_WIDTH  );
            int32 cellPY = ( int32 )( ( P.y + offset.y ) / WORLD_SYSTEM__CELL_HEIGHT );
            int32 cellQX = ( int32 )( ( Q.x + offset.x ) / WORLD_SYSTEM__CELL_WIDTH  );
            int32 cellQY = ( int32 )( ( Q.y + offset.y ) / WORLD_SYSTEM__CELL_HEIGHT );
            
            Assert( cellPX > -1 );
            Assert( cellPY > -1 );
            Assert( cellQX > -1 );
            Assert( cellQY > -1 );
            
            Assert( abs( cellPX - cellQX ) <= 1 );
            Assert( abs( cellPY - cellQY ) <= 1 );
            
            int32 cellX = minValue( cellPX, cellQX );
            int32 cellY = minValue( cellPY, cellQY );
            cellID[ iBack ] = cellX * maxY + cellY;
        }
        
        boo32 doSort = true;
        while( doSort ) {
            doSort = false;
            for( uint32 iter = 0; iter < ( world->nBack - 1 ); iter++ ) {
                if( cellID[ iter ] > cellID[ iter + 1 ] ) {
                    _swap( uint32, cellID[ iter ], cellID[ iter + 1 ] );
                    _swap( BACKGROUND, world->back[ iter ], world->back[ iter + 1 ] );
                    doSort = true;
                }
            }
        }
        
        Assert( world->nBack > 0 );
        
        uint32 atIndex = 0;
        BACKGROUND_GROUP group = {};
        group.backID = cellID[ atIndex ];
        group.bBack  = atIndex;
        while( atIndex < ( world->nBack - 1 ) ) {
            if( cellID[ atIndex ] != cellID[ atIndex + 1 ] ) {
                group.nBack = atIndex - group.bBack + 1;
                addGroup( world, group );
                
                group.backID = cellID[ atIndex + 1 ];
                group.bBack  = atIndex + 1;
            }
            
            atIndex++;
        }
        group.nBack = world->nBack - group.bBack;
        addGroup( world, group );
        
        _popArray( memory, uint32, world->nBack );
    }
    
    world->grid_offset = offset;
    world->grid_maxX   = maxX;
    world->grid_maxY   = maxY;
    
    uint32 breakHere = 10;
}

internal boo32
CellIsValid( WORLD_SYSTEM * world, int32 cX, int32 cY ) {
    boo32 isValidX = ( cX > -1 ) && ( cX < ( int32 )world->grid_maxX );
    boo32 isValidY = ( cY > -1 ) && ( cY < ( int32 )world->grid_maxY );
    boo32 result = ( isValidX ) && ( isValidY );
    return result;
}

internal void
updateWorldSystem( APP_STATE * appState, flo32 dt ) {
    WORLD_SYSTEM * world  = &appState->worldSystem;
    CAMERA_STATE * camera = &appState->cameraState;
    OBSTACLE_STATE   * obstacleState   = &appState->obstacleState;
    BACKGROUND_STATE * backgroundState = &appState->backgroundState;
    
    int32 cellX = ( uint32 )( ( camera->at.x + world->grid_offset.x ) / WORLD_SYSTEM__CELL_WIDTH );
    int32 cellY = ( uint32 )( ( camera->at.y + world->grid_offset.y ) / WORLD_SYSTEM__CELL_WIDTH );
    boo32 isValidX = ( cellX > -2 ) && ( cellX < ( int32 )( world->grid_maxX + 1 ) );
    boo32 isValidY = ( cellY > -2 ) && ( cellY < ( int32 )( world->grid_maxY + 1 ) );
    if( ( isValidX ) && ( isValidY ) ) {
        uint32 iCell = cellX * world->grid_maxY + cellY;
        
        if( iCell != world->camera_cellID ) {
            int32 dX[ 9 ] = { -1, -1, -1,  0, 0, 0,  1, 1, 1 };
            int32 dY[ 9 ] = { -1,  0,  1, -1, 0, 1, -1, 0, 1 };
            
            obstacleState->nObstacle     = 0;
            backgroundState->nBackground = 0;
            
            for( uint32 iter = 0; iter < 9; iter++ ) {
                int32 cX = cellX + dX[ iter ];
                int32 cY = cellY + dY[ iter ];
                if( CellIsValid( world, cX, cY ) ) {
                    uint32 cID = cX * world->grid_maxY + cY;
                    
                    { // OBSTACLE
                        int32 group_index = -1;
                        for( uint32 iGroup = 0; ( group_index == -1 ) && ( iGroup < world->nObsGroup ); iGroup++ ) {
                            // TODO: the group list is sorted, so this can be faster.
                            OBSTACLE_GROUP group = world->obsGroup[ iGroup ];
                            if( group.obsID == cID ) {
                                group_index = iGroup;
                            }
                        }
                        
                        if( group_index > -1 ) {
                            OBSTACLE_GROUP group = world->obsGroup[ group_index ];
                            
                            OBSTACLE * src = world->obs + group.bObs;
                            for( uint32 iObs = 0; iObs < group.nObs; iObs++ ) {
                                OBSTACLE obs = src[ iObs ];
                                addObstacle( appState, obs );
                            }
                        }
                    }
                    
                    { // BACKGROUND
                        int32 group_index = -1;
                        for( uint32 iGroup = 0; ( group_index == -1 ) && ( iGroup < world->nBackGroup ); iGroup++ ) {
                            // TODO: the group list is sorted, so this can be faster.
                            BACKGROUND_GROUP group = world->backGroup[ iGroup ];
                            if( group.backID == cID ) {
                                group_index = iGroup;
                            }
                        }
                        
                        if( group_index > -1 ) {
                            BACKGROUND_GROUP group = world->backGroup[ group_index ];
                            
                            BACKGROUND * src = world->back + group.bBack;
                            for( uint32 iBack = 0; iBack < group.nBack; iBack++ ) {
                                BACKGROUND back = src[ iBack ];
                                addBackground( appState, back );
                            }
                        }
                    }
                }
            }
        }
        
        world->camera_cellID = iCell;
    }
}

internal void
drawWorldSystem( RENDER_PASS * pass, APP_STATE * appState ) {
    WORLD_SYSTEM * world = &appState->worldSystem;
    
#if 0    
    vec2 bPos = -world->grid_offset;
    for( uint32 iRow = 0; ( iRow < world->grid_maxY + 1 ); iRow++ ) {
        vec2 P = bPos + Vec2( 0.0f, WORLD_SYSTEM__CELL_HEIGHT * ( flo32 )iRow );
        vec2 Q = P + Vec2( WORLD_SYSTEM__CELL_WIDTH * ( flo32 )world->grid_maxX, 0.0f );
        drawLine( pass, P, Q, COLOR_RED );
    }
    for( uint32 iCol = 0; ( iCol < world->grid_maxX + 1 ); iCol++ ) {
        vec2 P = bPos + Vec2( WORLD_SYSTEM__CELL_WIDTH * ( flo32 )iCol, 0.0f );
        vec2 Q = P + Vec2( 0.0f, WORLD_SYSTEM__CELL_HEIGHT * ( flo32 )world->grid_maxY );
        drawLine( pass, P, Q, COLOR_RED );
    }
#endif
}