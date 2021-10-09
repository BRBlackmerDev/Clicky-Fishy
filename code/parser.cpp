
// TODO: file parser should not modify the contents of the file memory
struct FILE_PARSER {
    char * start;
    char * at;
    uint32 size;
};

internal FILE_PARSER
FileParser( void * memory, int32 size ) {
    FILE_PARSER result = { ( char * )memory, ( char * )memory, (uint32)size };
    return result;
}

internal boo32
hasTextRemaining( FILE_PARSER * parser ) {
    boo32 result = ( ( parser->at - parser->start ) < parser->size );
    return result;
}

internal boo32
isWhiteSpace( char c ) {
    boo32 result = ( c == 0 ) || ( c == ' ' ) || ( c == '\n' ) || ( c == '\t' ) || ( c == '\r' );
    return result;
}

internal void
eatWhiteSpace( FILE_PARSER * parser ) {
    while( hasTextRemaining( parser ) && ( isWhiteSpace( *parser->at ) ) ) { *parser->at++ = 0; }
}

internal char * 
parseToken( FILE_PARSER * parser ) {
    char * result = 0;
    
    while( hasTextRemaining( parser ) && ( isWhiteSpace( *parser->at ) ) ) { *parser->at++ = 0; }
    
    if( hasTextRemaining( parser ) ) { result = parser->at; }
    while( hasTextRemaining( parser ) && ( !isWhiteSpace( *parser->at ) ) ) { *parser->at++; }
    while( hasTextRemaining( parser ) && (  isWhiteSpace( *parser->at ) ) ) { *parser->at++ = 0; }
    
    return result;
}

internal char * 
parseLine( FILE_PARSER * parser ) {
    while( isWhiteSpace( *parser->at ) ) { *parser->at++ = 0; }
    
    char * result = parser->at;
    while( ( *parser->at != 0 ) && ( *parser->at != '\n' ) && ( *parser->at != '\r' ) ) { parser->at++; }
    while( isWhiteSpace( *parser->at ) ) { *parser->at++ = 0; }
    
    return result;
}

internal uint32
parseU32( FILE_PARSER * parser ) {
    char * token = parseToken( parser );
    uint32 result = strtoul( token, 0, 0 );
    return result;
}

internal uint32
parseU32( const char * token ) {
    uint32 result = strtoul( token, 0, 0 );
    return result;
}

internal int32
parseS32( FILE_PARSER * parser ) {
    char * token = parseToken( parser );
    int32 result = strtol( token, 0, 0 );
    return result;
}

internal flo32
parseF32( FILE_PARSER * parser ) {
    char * token = parseToken( parser );
    flo32 result = strtof( token, 0 );
    return result;
}

internal vec3
parseV3( FILE_PARSER * parser ) {
    vec3 result = {};
    for( int32 elem_index = 0; elem_index < 3; elem_index++ ) {
        result.elem[ elem_index ] = parseF32( parser );
    }
    return result;
}