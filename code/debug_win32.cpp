
//----------
// debug declarations
//----------

#if DEBUG_BUILD

internal void CONSOLE_STRING( const char * string );

struct DEBUG_SYSTEM {
    // TODO: Currently, DEBUG_SYSTEM only draws text, so *pass expects a pointer to the UI render pass.
    RENDER_PASS * pass;
    FONT        * font;
    vec2       atPos;
    flo32      advanceHeight;
    boo32      log_isEnabled;
};
DEBUG_SYSTEM globalVar_debugSystem = {};

#define DISPLAY_VALUE( type, name ) {\
char __debug_string[ 2048 ] = {};\
serialize_##type( __debug_string, #name, name );\
DISPLAY_STRING( __debug_string );\
}
#define CONSOLE_VALUE( type, name ) {\
char __debug_string[ 2048 ] = {};\
serialize_##type( __debug_string, #name, name );\
strcat( __debug_string, "\n" );\
OutputDebugString( __debug_string );\
}
#define CONSOLE_VALUE_HEX( type, name ) {\
char __debug_string[ 2048 ] = {};\
serialize_hex_##type( __debug_string, #name, name );\
strcat( __debug_string, "\n" );\
OutputDebugString( __debug_string );\
}

#else
#define DISPLAY_STRING( str )
#define DISPLAY_VALUE( type, name )
#define CONSOLE_STRING( str )

internal void PROFILE_string( char * string ) {}
internal void PROFILE_tooltip( char * string, vec2 position ) {}

#endif

//----------
// debug functions
//----------

#if DEBUG_BUILD
#define DEBUG_SYSTEM__MAX_VERTEX_COUNT		( 2097152 )
#define DEBUG_SYSTEM__MAX_MEMORY_SIZE    _MB( 2 )
#define DEBUG_SYSTEM__SIZE_IN_BYTES  		( sizeof( RENDER_SYSTEM ) + DEBUG_SYSTEM__MAX_MEMORY_SIZE )

internal void
CONSOLE_STRING( const char * string ) {
	OutputDebugString( string );
	OutputDebugString( "\n" );
}

#if 0
internal void
DEBUG_addTask( ASSET_ID textureID, rect bound, vec4 color ) {
    RENDERER * renderer = globalVar_debugSystem.renderer;
    
    int32 vertex_index    = renderer->vertex_count[ 0 ];
    int32 vertex_count    = 4;
	DEBUG_VERTEX * vertex = ( ( DEBUG_VERTEX * )renderer->vertex_ptr[ 0 ] ) + vertex_index;
    
    vec2 pos[ 4 ] = {
        Vec2( bound.left,  bound.bottom ),
        Vec2( bound.right, bound.bottom ),
        Vec2( bound.left,  bound.top ),
        Vec2( bound.right, bound.top ),
    };
    
    vec2 tex[ 4 ] = {
        Vec2( 0.0f, 0.0f ),
        Vec2( 1.0f, 0.0f ),
        Vec2( 0.0f, 1.0f ),
        Vec2( 1.0f, 1.0f ),
    };
    
    *vertex++ = DebugVertex( pos[ 0 ], tex[ 0 ] );
    *vertex++ = DebugVertex( pos[ 1 ], tex[ 1 ] );
    *vertex++ = DebugVertex( pos[ 2 ], tex[ 2 ] );
    *vertex++ = DebugVertex( pos[ 3 ], tex[ 3 ] );
    
    addVertex( renderer, 0, vertex_count );
    addObject( renderer, renderPassID_debug, RenderObject( meshDrawType_triangleStrip, vertex_index, vertex_count, textureID, color ) );
}
#endif

internal void
drawString( RENDER_PASS * pass, FONT * font, const char * string, vec2 position, vec2 scale, vec4 color ) {
    VERTEX1_BUFFER * buffer = pass->textBuffer;
    VERTEX1 * vertex = buffer->vertex_start + buffer->nVertex;
    
    uint32 bVertex = buffer->nVertex;
    
    int32 nChar = ( int32 )strlen( string );
    
    vec2 atPos = position;
    for( int32 iChar = 0; iChar < nChar; iChar++ ) {
        char c = string[ iChar ];
        if( c != ' ' ) {
            int32 char_index = c - FONT__START_CHAR;
            FONT_CHAR _char = font->alphabet[ char_index ];
            
            vec2 offset = _char.offset * scale;
            vec2 dim    = _char.dim    * scale;
            
            flo32 left   = atPos.x + offset.x;
            flo32 right  = left    + dim.x;
            flo32 top    = atPos.y + offset.y;
            flo32 bottom = top     - dim.y;
            rect bound = Rect( left, bottom, right, top );
            
            vec2 pos[ 4 ] = {
                Vec2( bound.left,  bound.bottom ),
                Vec2( bound.right, bound.bottom ),
                Vec2( bound.left,  bound.top ),
                Vec2( bound.right, bound.top ),
            };
            
            vec2 tex[ 4 ] = {
                Vec2( _char.texCoord_min.x, _char.texCoord_min.y ),
                Vec2( _char.texCoord_max.x, _char.texCoord_min.y ),
                Vec2( _char.texCoord_min.x, _char.texCoord_max.y ),
                Vec2( _char.texCoord_max.x, _char.texCoord_max.y ),
            };
            
            *vertex++ = Vertex1( pos[ 0 ], tex[ 0 ] );
            *vertex++ = Vertex1( pos[ 1 ], tex[ 1 ] );
            *vertex++ = Vertex1( pos[ 2 ], tex[ 2 ] );
            *vertex++ = Vertex1( pos[ 3 ], tex[ 3 ] );
            *vertex++ = Vertex1( pos[ 2 ], tex[ 2 ] );
            *vertex++ = Vertex1( pos[ 1 ], tex[ 1 ] );
            
            buffer->nVertex += 6;
            
            Assert( buffer->nVertex < buffer->maxVertex );
            
            atPos.x += ( _char.advanceWidth * scale.x );
        } else {
            atPos.x += ( font->advanceWidth_space * scale.x );
        }
    }
    
    TEXT_RENDER_OBJECT object = {};
    object.bVertex  = bVertex;
    object.nVertex  = buffer->nVertex - bVertex;
    object.modColor = color;
    
    TEXT_RENDER_OBJECT_LIST * textList = &pass->textList;
    Assert( textList->nObjects < textList->maxObjects );
    textList->object[ textList->nObjects++ ] = object;
}

internal void
drawString( RENDER_PASS * pass, FONT * font, const char * string, vec2 position, TEXT_ALIGNMENT align, vec4 color ) {
    vec2 offset = getOffsetFromAlignment( font, string, Vec2( 1.0f, 1.0f ), align );
    drawString( pass, font, string, position + offset, Vec2( 1.0f, 1.0f ), color );
}

internal void
drawString( RENDER_PASS * pass, FONT * font, const char * string, vec2 position, TEXT_ALIGNMENT align, vec2 scale, vec4 color ) {
    vec2 offset = getOffsetFromAlignment( font, string, scale, align );
    drawString( pass, font, string, position + offset, scale, color );
}

internal void
DEBUG_drawString( char * string, vec2 position, vec4 color ) {
    RENDER_PASS * pass = globalVar_debugSystem.pass;
	FONT        * font = globalVar_debugSystem.font;
    
    drawString( pass, font, string, position, Vec2( 1.0f, 1.0f ), color );
}

#if 0
internal void
DEBUG_drawRect( rect bound, vec4 color ) {
	DEBUG_addTask( assetID_texture_whiteTexture, bound, color );
}
#endif

internal void
DISPLAY_STRING( char * string ) {
	DEBUG_drawString( string, globalVar_debugSystem.atPos, COLOR_BLACK );
	globalVar_debugSystem.atPos.y += globalVar_debugSystem.advanceHeight;
}

internal void
DISPLAY_STRING( char * string, vec2 position ) {
	DEBUG_drawString( string, position, COLOR_BLACK );
}

internal void
PROFILE_string( char * string ) {
	DEBUG_drawString( string, globalVar_debugSystem.atPos, COLOR_WHITE );
	globalVar_debugSystem.atPos.y -= globalVar_debugSystem.advanceHeight;
}

internal void
PROFILE_tooltip( char * string, vec2 position ) {
	DEBUG_drawString( string, position, COLOR_WHITE );
}

internal void serialize_int8  ( char * buffer, const char * name, int8   value ) {
    sprintf( buffer, "%s: %hhd",    name, value ); }
internal void serialize_uint8 ( char * buffer, const char * name, uint8  value ) {
    sprintf( buffer, "%s: %hhu",    name, value ); }
internal void serialize_int16 ( char * buffer, const char * name, int16  value ) { sprintf( buffer, "%s: %hd",    name, value ); }
internal void serialize_int32 ( char * buffer, const char * name, int32  value ) { sprintf( buffer, "%s: %d",     name, value ); }
internal void serialize_uint16( char * buffer, const char * name, uint16 value ) { sprintf( buffer, "%s: %hu",    name, value ); }
internal void serialize_uint32( char * buffer, const char * name, uint32 value ) { sprintf( buffer, "%s: %u",     name, value ); }
internal void serialize_uint64( char * buffer, const char * name, uint64 value ) { sprintf( buffer, "%s: %llu",   name, value ); }
internal void serialize_boo32 ( char * buffer, const char * name, boo32  value ) { sprintf( buffer, "%s: %d",     name, value ); }
internal void serialize_flo32 ( char * buffer, const char * name, flo32  value ) { sprintf( buffer, "%s: %0.12f",     name, value ); }
internal void serialize_vec2  ( char * buffer, const char * name, vec2   value ) { sprintf( buffer, "%s: %f, %f", name, value.x, value.y ); }
internal void serialize_vec3  ( char * buffer, const char * name, vec3   value ) { sprintf( buffer, "%s: %f, %f, %f", name, value.x, value.y, value.z ); }
internal void serialize_vec4  ( char * buffer, const char * name, vec4   value ) { sprintf( buffer, "%s: %f, %f, %f, %f", name, value.x, value.y, value.z, value.w ); }
internal void serialize_rect  ( char * buffer, const char * name, rect   value ) { sprintf( buffer, "%s: %f, %f, %f, %f", name, value.left, value.bottom, value.right, value.top ); }
internal void serialize_MEMORY ( char * buffer, const char * name, MEMORY memory ) { sprintf( buffer, "%s: base: %llx, size: %llu, used: %llu", name, ( uint64 )memory.base, memory.size, memory.used ); }

internal void serialize_hex_uint32( char * buffer, const char * name, uint32 value ) { sprintf( buffer, "%s: %X",     name, value ); }

#endif // DEBUG_BUILD