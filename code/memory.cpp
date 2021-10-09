
#ifdef	STD_INCLUDE_DECL

struct MEMORY {
	void * base;
	uint64 size;
	uint64 used;
   
#define MEMORY__SNAP_COUNT  ( 8 )
   uint32 snap_index;
   uint64 snapUsed[ MEMORY__SNAP_COUNT ];
};

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

//----------
// memory functions
//----------

internal MEMORY
Memory( void * base, uint64 size ) {
	MEMORY result = { base, size };
	return result;
}

internal void
reset( MEMORY * memory ) {
	memset( memory->base, 0, memory->size );
	memory->used = 0;
}

#define _pushType( memory, type ) ( type * )pushSize_( memory, sizeof( type ) )
#define _pushArray( memory, type, count ) ( type * )pushSize_( memory, sizeof( type ) * count )
#define _pushSize( memory, size ) pushSize_( memory, size )

internal void *
pushSize_( MEMORY * memory, uint64 size ) {
	Assert( memory->used + size <= memory->size );
	void * result = ( uint8 * )memory->base + memory->used;
	memory->used += size;
	return result;
}

#define _pushType_clear( memory, type ) ( type * )pushSize_clear( memory, sizeof( type ) )
#define _pushArray_clear( memory, type, count ) ( type * )pushSize_clear( memory, sizeof( type ) * count )
#define _pushSize_clear( memory, size ) pushSize_clear( memory, size )

internal void *
pushSize_clear( MEMORY * memory, uint64 size ) {
	void * result = pushSize_( memory, size );
	memset( result, 0, size );
	return result;
}

#define _popType(  memory, type ) 			popSize_( memory, sizeof( type ) )
#define _popArray( memory, type, count ) 	popSize_( memory, sizeof( type ) * count )
#define _popSize(  memory, size ) 			popSize_( memory, size )

internal void
popSize_( MEMORY * memory, uint64 size ) {
	Assert( memory->used - size >= 0 );
	memory->used -= size;
}

inline void
pushValue( MEMORY * memory, uint8 value ) {
	Assert( memory->used + sizeof( uint8 ) <= memory->size );
	uint8 * slot = ( uint8 * )memory->base + memory->used;
	*slot = value;
	memory->used += sizeof( uint8 );
}

inline uint8 *
getUsed( MEMORY * memory ) {
   uint8 * result = ( uint8 * )memory->base + memory->used;
   return result;
}

internal MEMORY
subArena( MEMORY * parentArena, int32 size ) {
	MEMORY result = {};
	result.base = _pushSize_clear( parentArena, size );
	result.size = size;
	return result;
}

internal MEMORY
subArena( MEMORY * parentArena ) {
   MEMORY result = subArena( parentArena, ( int32 )( parentArena->size - parentArena->used ) );
   return result;
}

internal MEMORY *
SnapMemory( MEMORY * memory ) {
   Assert( memory->snap_index < MEMORY__SNAP_COUNT );
   memory->snapUsed[ memory->snap_index++ ] = memory->used;
   
   MEMORY * result = memory;
   return result;
}

internal uint8 *
getSnapBase( MEMORY * memory ) {
   Assert( memory->snap_index > 0 );
   uint8 * result = ( uint8 * )memory->base + memory->snapUsed[ memory->snap_index - 1 ];
   return result;
}

internal uint32
getSnapUsed( MEMORY * memory ) {
   Assert( memory->snap_index > 0 );
   uint32 result = ( uint32 )( memory->used - memory->snapUsed[ memory->snap_index - 1 ] );
   return result;
}

internal void
popToSnap( MEMORY * memory ) {
   Assert( memory->snap_index > 0 );
   memory->used = memory->snapUsed[ --memory->snap_index ];
}

#endif	// STD_INCLUDE_FUNC