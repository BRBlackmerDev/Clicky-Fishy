
internal mat4
genTransform( vec3 position, vec3 scale, quat orientation ) {
    mat4 result = mat4_translate( position ) * mat4_toRotation( orientation ) * mat4_scale( scale );
    return result;
}

internal void
playAudio( AUDIO_SYSTEM * audioSystem, ASSET_ID audioID, flo32 volume ) {
	AUDIO_DATA audio = getAudio( audioSystem->assetSystem, audioID );
    playAudio( audioSystem, audio, volume );
}

internal void
playMusic( MUSIC_SYSTEM * musicSystem, ASSET_ID musicID, flo32 volume ) {
	AUDIO_DATA music = getAudio( musicSystem->assetSystem, musicID );
    playMusic( musicSystem, music, volume );
}

internal void
drawCylinder( RENDERER * renderer, RENDER_PASS_ID passID, vec3 position, flo32 height, flo32 radius, vec4 color ) {
    int32 segment_count = 16;
    
    flo32 sin_lookup[ 16 + 1 ] = {};
    flo32 cos_lookup[ 16 + 1 ] = {};
    for( int32 segment_index = 0; segment_index <= segment_count; segment_index++ ) {
        flo32 radians = ( ( flo32 )segment_index / ( flo32 )segment_count ) * PI * 2.0f;
        sin_lookup[ segment_index ] = sinf( radians ) * radius;
        cos_lookup[ segment_index ] = cosf( radians ) * radius;
    }
    
    VERTEX1_TASK vertexTask = Vertex1Task( renderer );
    
    for( int32 segment_index = 0; segment_index < segment_count; segment_index++ ) {
        vec3 pos[ 4 ] = {
            position + Vec3( cos_lookup[ segment_index     ], 0.0f,   -sin_lookup[ segment_index     ] ),
            position + Vec3( cos_lookup[ segment_index + 1 ], 0.0f,   -sin_lookup[ segment_index + 1 ] ),
            position + Vec3( cos_lookup[ segment_index     ], height, -sin_lookup[ segment_index     ] ),
            position + Vec3( cos_lookup[ segment_index + 1 ], height, -sin_lookup[ segment_index + 1 ] ),
        };
        
        vec3 normal = getNormal( cross( pos[ 1 ] - pos[ 0 ], pos[ 2 ] - pos[ 0 ] ) );
        addQuad( &vertexTask, pos[ 0 ], pos[ 1 ], pos[ 2 ], pos[ 3 ], normal );
    }
    
    vec3 base_position = position + Vec3( 0.0f, height, 0.0f );
    for( int32 segment_index = 0; segment_index < segment_count; segment_index++ ) {
        vec3 pos[ 3 ] = {
            base_position,
            base_position + Vec3( cos_lookup[ segment_index     ], 0.0f, -sin_lookup[ segment_index     ] ),
            base_position + Vec3( cos_lookup[ segment_index + 1 ], 0.0f, -sin_lookup[ segment_index + 1 ] ),
        };
        vec3 normal = getNormal( cross( pos[ 1 ] - pos[ 0 ], pos[ 2 ] - pos[ 0 ] ) );
        addTri( &vertexTask, pos[ 0 ], pos[ 1 ], pos[ 2 ], normal );
    }
    
    addVertex( renderer, 1, vertexTask.vertex_count );
    addObject( renderer, passID, RenderObject( meshDrawType_triangles, vertexTask.vertex_index, vertexTask.vertex_count, assetID_texture_whiteTexture, color ) );
}

internal void
addTri( VERTEX1_TASK * vertexTask, vec3 * pos ) {
    vec3 normal = getNormal( cross( pos[ 1 ] - pos[ 0 ], pos[ 2 ] - pos[ 0 ] ) );
    addTri( vertexTask, pos[ 0 ], pos[ 1 ], pos[ 2 ], normal );
}

internal void
drawQuad( RENDERER * renderer, RENDER_PASS_ID passID, vec3 position, vec3 xAxis, vec3 yAxis, vec4 color ) {
    vec3 pos[ 4 ] = {
        position,
        position + xAxis,
        position + yAxis,
        position + xAxis + yAxis,
    };
    vec3 normal = getNormal( cross( xAxis, yAxis ) );
    
    VERTEX1_TASK vertexTask = Vertex1Task( renderer );
    addQuad( &vertexTask, pos[ 0 ], pos[ 1 ], pos[ 2 ], pos[ 3 ], normal );
    
    addVertex( renderer, 1, vertexTask.vertex_count );
    addObject( renderer, passID, RenderObject( meshDrawType_triangles, vertexTask.vertex_index, vertexTask.vertex_count, assetID_texture_whiteTexture, color ) );
}

internal flo32
distSq_pointToLine( vec3 point, vec3 vertex0, vec3 vertex1 ) {
    vec3  vector0 = point   - vertex0;
    vec3  vector1 = vertex1 - vertex0;
    
    flo32 t      = clamp01( dot( vector0, vector1 ) / getLengthSq( vector1 ) );
    flo32 result = getLengthSq( point - ( vertex0 + vector1 * t ) );
    return result;
}

internal flo32
dist_pointToLine( vec3 point, vec3 vertex0, vec3 vertex1 ) {
    flo32 result = sqrtf( distSq_pointToLine( point, vertex0, vertex1 ) );
    return result;
}

internal vec3
getClosestPoint_point_lineSegment( vec3 C, vec3 A, vec3 B ) {
    vec3 AB = B - A;
    vec3 AC = C - A;
    
    flo32 t      = clamp01( dot( AC, AB ) / dot( AB, AB ) );
    vec3  result = A + AB * t;
    return result;
}

internal void
drawSprite( RENDERER * renderer, RENDER_PASS_ID passID, ASSET_ID textureID, rect bound, vec4 color ) {
    //uint32 bufferID = vertexBufferID_vertex1;
    //int32 vertex_index    = renderer->vertex_count[ bufferID ];
    //int32 vertex_count    = 4;
	//VERTEX1 * vertex = ( ( VERTEX1 * )renderer->vertex_ptr[ bufferID ] ) + vertex_index;
    
    vec3 pos[ 4 ] = {
        Vec3( bound.left,  bound.bottom, 0.0f ),
        Vec3( bound.right, bound.bottom, 0.0f ),
        Vec3( bound.left,  bound.top, 0.0f ),
        Vec3( bound.right, bound.top, 0.0f ),
    };
    
    vec3 normal = Vec3( 0.0f, 0.0f, 1.0f );
    //*vertex++ = Vertex1( pos[ 0 ], normal, Vec2( 0.0f, 0.0f ) );
    //*vertex++ = Vertex1( pos[ 1 ], normal, Vec2( 1.0f, 0.0f ) );
    //*vertex++ = Vertex1( pos[ 2 ], normal, Vec2( 0.0f, 1.0f ) );
    //*vertex++ = Vertex1( pos[ 3 ], normal, Vec2( 1.0f, 1.0f ) );
    
    VERTEX1_TASK vertexTask = Vertex1Task( renderer );
    addQuad( &vertexTask, pos[ 0 ], pos[ 1 ], pos[ 2 ], pos[ 3 ], normal );
    
    //addVertex( renderer, bufferID, vertex_count );
    //addObject( renderer, passID, RenderObject( meshDrawType_triangleStrip, vertex_index, vertex_count, textureID, color ) );
    addVertex( renderer, 1, vertexTask.vertex_count );
    addObject( renderer, passID, RenderObject( meshDrawType_triangles, vertexTask.vertex_index, vertexTask.vertex_count, textureID, color ) );
}