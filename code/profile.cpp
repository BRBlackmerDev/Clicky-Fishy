
#ifdef	STD_INCLUDE_DECL

//----------
// profile declarations
//----------

#define FUNCTION_NAME_MAX_LENGTH  48
struct PROFILE_EVENT {
	char functionName[ FUNCTION_NAME_MAX_LENGTH + 1 ];
	uint64 clock_start;
	uint64 clock_end;
	uint32 threadID;
   
#define PROFILE__SUBCLOCK_COUNT  ( 8 )
   uint64  subclock[ PROFILE__SUBCLOCK_COUNT ];
   uint32 nSubclock;
};

#define PROFILE_EVENT_LIST_MAX_COUNT  16384
struct PROFILE_EVENT_LIST {
	PROFILE_EVENT event[ PROFILE_EVENT_LIST_MAX_COUNT ];
	uint64 frameClockStart;
	int32 count;
};

struct PROFILE_NODE {
	char functionName[ FUNCTION_NAME_MAX_LENGTH ];
	uint64 clock_total;
	uint32 count_total;
   
   uint64  subclock[ PROFILE__SUBCLOCK_COUNT ];
   uint32 nSubclock;
};

#define PROFILE_NODE_LIST_MAX_COUNT  128
struct PROFILE_NODE_LIST {
	PROFILE_NODE node[ PROFILE_NODE_LIST_MAX_COUNT ];
	int32 count;
};

#define PROFILE_EVENT_STACK_MAX_COUNT  16
struct PROFILE_EVENT_STACK {
	PROFILE_EVENT event[ PROFILE_EVENT_STACK_MAX_COUNT ];
	int32 count;
};

#define PROFILE_EVENT_DATABASE_MAX_COUNT  ( 5 * 60 )
struct PROFILE_EVENT_DATABASE {
	PROFILE_EVENT_LIST * eventList;
	int32 maxCount;
	int32 index;
	int32 selectIndex;
};

#if DEBUG_BUILD
#define PROFILE_FUNCTION()  PROFILE_OBJ profileObj = PROFILE_OBJ( globalVar_profileSystem.eventList_currentFrame, __FUNCTION__ )
#else
#define PROFILE_FUNCTION()
#endif

#if DEBUG_BUILD
#define PROFILE_BREAK()  profileObj.event->subclock[ profileObj.event->nSubclock++ ] = __rdtsc();
#else
#define PROFILE_BREAK()
#endif // DEBUG_BUILD

struct PROFILE_EVENT_DATABASE_CLOCKS {
	uint64 frameStart;
	uint64 platformStart;
	uint64 appStart;
	uint64 renderStart;
	uint64 frameEnd;
	int64  counter_frameStart;
	int64  counter_frameEnd;
};

class PROFILE_OBJ {
	public:
   PROFILE_EVENT * event;
   PROFILE_OBJ( PROFILE_EVENT_LIST * eventList, char * funcName );
   ~PROFILE_OBJ();
};

typedef flo32 ( GET_MS_ELAPSED )( int64 startCounter, int64 endCounter, int64 perfFrequency );

struct PROFILE_SYSTEM {
	boo32 showProfile;
	PROFILE_EVENT_DATABASE	eventDatabase;
	PROFILE_EVENT_LIST *		eventList_currentFrame;
	PROFILE_NODE_LIST * 		nodeList_currentFrame;
	
	GET_MS_ELAPSED * getMSElapsed;
	
	PROFILE_EVENT_DATABASE_CLOCKS		eventDatabase_clocks_currentFrame;
	PROFILE_EVENT_DATABASE_CLOCKS 	eventDatabase_clocks_previousFrame;
	PROFILE_EVENT_DATABASE_CLOCKS *	eventDatabase_clocks;
	
	flo32 eventDatabase_posOffset;
	flo32 eventDatabase_width;
	flo32 eventDatabase_height;
	boo32 eventDatabase_scrollActive;
	boo32 frameBreakdown_scrollActive_bar;
	boo32 frameBreakdown_scrollActive_left;
	boo32 frameBreakdown_scrollActive_right;
	flo32 frameBreakdown_scrollX_left;
	flo32 frameBreakdown_scrollX_right;
};

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

#if DEBUG_BUILD
#define PROFILE_SYSTEM__SIZE_IN_BYTES	( ( ( PROFILE_EVENT_DATABASE_MAX_COUNT + 1 ) * sizeof( PROFILE_EVENT_LIST ) ) + sizeof( PROFILE_NODE_LIST ) + ( PROFILE_EVENT_DATABASE_MAX_COUNT * sizeof( PROFILE_EVENT_DATABASE_CLOCKS ) ) )
PROFILE_SYSTEM globalVar_profileSystem = {};

//----------
// profile functions
//----------

internal uint32
getThreadID() {
	uint8 * thread_memory = ( uint8 * )__readgsqword( 0x30 );
	uint32  threadID = *( ( uint32 * )( thread_memory + 0x48 ) );
	return threadID;
}

internal void
setString( char * srcStr, char * destStr ) {
	char * dest = destStr;
	char * src = srcStr;
	while( *src ) {
		*dest++ = *src++;
	}
	*dest = 0;
}

internal boo32
matchString( char * string0, char * string1 ) {
   boo32 doMatch = true;
   while( ( *string0 ) && ( *string1 ) ) {
      if( *string0 != *string1 ) { doMatch = false; }
      string0++;
      string1++;
   }
   if( ( *string0 ) || ( *string1 ) ) { doMatch = false; }
   return doMatch;
}

internal boo32
matchSegment( char * string0, char * string1, uint32 nChar ) {
   boo32 doMatch = true;
   for( uint32 iChar = 0; iChar < nChar; iChar++ ) {
      if( string0[ iChar ] != string1[ iChar ] ) { doMatch = false; }
   }
   return doMatch;
}

internal PROFILE_EVENT *
newEvent( PROFILE_EVENT_LIST * profileList ) {
	int32 event_index = InterlockedIncrement( ( volatile long * )&profileList->count ) - 1;
	PROFILE_EVENT * event = profileList->event + event_index;
	Assert( profileList->count < PROFILE_EVENT_LIST_MAX_COUNT );
	event->clock_start = 0;
	event->clock_end = 0;
   event->nSubclock = 0;
	for( int32 iter = 0; iter < FUNCTION_NAME_MAX_LENGTH; iter++ ) {
		event->functionName[ iter ] = 0;
	}
	return event;
}

internal PROFILE_NODE *
newNode( PROFILE_NODE_LIST * profileList ) {
	PROFILE_NODE * node = profileList->node + profileList->count++;
	Assert( profileList->count < PROFILE_NODE_LIST_MAX_COUNT );
	node->clock_total = 0;
	node->count_total = 0;
   node->nSubclock   = 0;
   memset( node->subclock, 0, sizeof( uint64 ) * PROFILE__SUBCLOCK_COUNT );
	for( int32 iter = 0; iter < FUNCTION_NAME_MAX_LENGTH; iter++ ) {
		node->functionName[ iter ] = 0;
	}
	return node;
}

internal void
collateEvents( PROFILE_EVENT_LIST * eventList, PROFILE_NODE_LIST * nodeList ) {
	PROFILE_EVENT_STACK _eventStack = {};
	PROFILE_EVENT_STACK * eventStack = &_eventStack;
	
	for( int32 event_index = 0; event_index < eventList->count; event_index++ ) {
		PROFILE_EVENT * event = eventList->event + event_index;
		
		PROFILE_NODE * currentNode = 0;
		boo32 isInList = false;
		
		for( int32 node_index = 0; node_index < nodeList->count && !isInList; node_index++ ) {
			PROFILE_NODE * node = nodeList->node + node_index;
			
			if( matchString( event->functionName, node->functionName ) ) {
				isInList = true;
				currentNode = node;
			}
		}
		
		if( !isInList ) {
			currentNode = newNode( nodeList );
			setString( event->functionName, currentNode->functionName );
		}
		Assert( currentNode );
		currentNode->count_total++;
		uint64 clock_total = event->clock_end - event->clock_start;
		currentNode->clock_total += clock_total;
      
      currentNode->nSubclock = event->nSubclock;
      for( uint32 iSubclock = 0; iSubclock < currentNode->nSubclock; iSubclock++ ) {
         currentNode->subclock[ iSubclock ] += ( event->subclock[ iSubclock ] - event->clock_start );
      }
		
		for( int32 iter = eventStack->count - 1; iter >= 0; iter-- ) {
			PROFILE_EVENT * stackEvent = eventStack->event + iter;
			
			if( event->clock_start > stackEvent->clock_end ) {
				eventStack->count--;
			}
		}
		
		if( eventStack->count > 0 ) {
			PROFILE_EVENT * parentEvent = eventStack->event + eventStack->count - 1;
			for( int32 node_index = 0; node_index < nodeList->count; node_index++ ) {
				PROFILE_NODE * node = nodeList->node + node_index;
				
				if( matchString( parentEvent->functionName, node->functionName ) ) {
					node->clock_total -= clock_total;
					break;
				}
			}
		}
		
		PROFILE_EVENT * pushEvent = eventStack->event + eventStack->count++;
		Assert( eventStack->count < PROFILE_EVENT_STACK_MAX_COUNT );
		*pushEvent = *event;
	}
	
	boo32 isSorted = true;
	do {
		isSorted = true;
		for( int32 node_index = 0; node_index < nodeList->count - 1; node_index++ ) {
			PROFILE_NODE * nodeA = nodeList->node + node_index;
			PROFILE_NODE * nodeB = nodeList->node + node_index + 1;
			
			if( nodeA->clock_total < nodeB->clock_total ) {
				PROFILE_NODE _temp = {};
				PROFILE_NODE * temp = &_temp;
				
				setString( nodeA->functionName, temp->functionName );
				temp->clock_total = nodeA->clock_total;
				temp->count_total = nodeA->count_total;
				
				setString( nodeB->functionName, nodeA->functionName );
				nodeA->clock_total = nodeB->clock_total;
				nodeA->count_total = nodeB->count_total;
				
				setString( temp->functionName, nodeB->functionName );
				nodeB->clock_total = temp->clock_total;
				nodeB->count_total = temp->count_total;
				
				isSorted = false;
			}
		}
	} while( !isSorted );
}

PROFILE_OBJ::PROFILE_OBJ( PROFILE_EVENT_LIST * eventList, char * funcName ) {
	event = newEvent( eventList );
	setString( funcName, event->functionName );
	event->threadID = getThreadID();
	event->clock_start = __rdtsc();
}

PROFILE_OBJ::~PROFILE_OBJ() {
	event->clock_end = __rdtsc();
}

internal void
updateProfile( PROFILE_SYSTEM * profileSystem, PLATFORM * platform, int64 perfFrequency, RENDERER * renderer, rect client_bound ) {
	if( globalVar_profileSystem.showProfile ) {
		vec2  eventDatabase_basePos 	= getBL( client_bound ) + Vec2( profileSystem->eventDatabase_posOffset, profileSystem->eventDatabase_posOffset );
		rect  eventDatabase_bound		= rectBLD( eventDatabase_basePos, Vec2( profileSystem->eventDatabase_width, profileSystem->eventDatabase_height ) );
		flo32 eventDatabase_slotWidth = getWidth( eventDatabase_bound ) / ( flo32 )PROFILE_EVENT_DATABASE_MAX_COUNT;
		
		MOUSE_STATE * mouse = &platform->mouse;
		rect frameBreakdown_scrollBound = rectBLD( getTL( eventDatabase_bound ) + Vec2( 0.0f, 4.0f ), Vec2( getWidth( eventDatabase_bound ), 12.0f ) );
		
		vec2 scrollBar_pos = Vec2( profileSystem->frameBreakdown_scrollX_left, frameBreakdown_scrollBound.bottom );
		vec2 scrollBar_dim = Vec2( profileSystem->frameBreakdown_scrollX_right - profileSystem->frameBreakdown_scrollX_left, getHeight( frameBreakdown_scrollBound ) );
		rect frameBreakdown_scrollBarBound = rectBLD( scrollBar_pos, scrollBar_dim );
		rect frameBreakdown_leftBound  = rectBLD( getBL( frameBreakdown_scrollBarBound ) + Vec2( -2.0f, 0.0f ), Vec2( 4.0f, scrollBar_dim.y ) );
		rect frameBreakdown_rightBound = rectBLD( getBR( frameBreakdown_scrollBarBound ) + Vec2( -2.0f, 0.0f ), Vec2( 4.0f, scrollBar_dim.y ) );
		
		rect eventDatabase_resizeBound = rectBLD( getTL( eventDatabase_bound ) + Vec2( -2.0f, -2.0f ), Vec2( getWidth( eventDatabase_bound ) + 4.0f, 4.0f ) );
		if( profileSystem->eventDatabase_scrollActive ) {
			profileSystem->eventDatabase_height = maxValue( mouse->position.y - eventDatabase_bound.bottom, 100.0f );
		}
		if( profileSystem->frameBreakdown_scrollActive_left ) {
			profileSystem->frameBreakdown_scrollX_left  = clamp( mouse->position.x, profileSystem->eventDatabase_posOffset, profileSystem->frameBreakdown_scrollX_right - 40.0f );
		}
		if( profileSystem->frameBreakdown_scrollActive_right ) {
			profileSystem->frameBreakdown_scrollX_right = clamp( mouse->position.x, profileSystem->frameBreakdown_scrollX_left + 40.0f, frameBreakdown_scrollBound.right );
		}
		if( profileSystem->frameBreakdown_scrollActive_bar ) {
			flo32 width = profileSystem->frameBreakdown_scrollX_right - profileSystem->frameBreakdown_scrollX_left;
			
			flo32 left  = mouse->position.x - width * 0.5f;
			flo32 right = mouse->position.x + width * 0.5f;
			if( left < profileSystem->eventDatabase_posOffset ) {
				left  = profileSystem->eventDatabase_posOffset;
				right = profileSystem->eventDatabase_posOffset + width;
			}
			if( right > frameBreakdown_scrollBound.right ) {
				right = frameBreakdown_scrollBound.right;
				left  = frameBreakdown_scrollBound.right - width;
			}
			
			profileSystem->frameBreakdown_scrollX_left  = left;
			profileSystem->frameBreakdown_scrollX_right = right;
		}
		if( isInBound( mouse->position, eventDatabase_resizeBound ) ) {
			if( wasPressed( mouse, mouseButton_left ) ) {
				profileSystem->eventDatabase_scrollActive = true;
			}
		}
		if( isInBound( mouse->position, frameBreakdown_leftBound ) ) {
			if( wasPressed( mouse, mouseButton_left ) ) {
				profileSystem->frameBreakdown_scrollActive_left = true;
			}
		} else if( isInBound( mouse->position, frameBreakdown_rightBound ) ) {
			if( wasPressed( mouse, mouseButton_left ) ) {
				profileSystem->frameBreakdown_scrollActive_right = true;
			}
		} else if( isInBound( mouse->position, frameBreakdown_scrollBarBound ) ) {
			if( wasPressed( mouse, mouseButton_left ) ) {
				profileSystem->frameBreakdown_scrollActive_bar = true;
			}
		}
		if( wasReleased( mouse, mouseButton_left ) ) {
			profileSystem->eventDatabase_scrollActive        = false;
			profileSystem->frameBreakdown_scrollActive_bar   = false;
			profileSystem->frameBreakdown_scrollActive_left  = false;
			profileSystem->frameBreakdown_scrollActive_right = false;
		}
		scrollBar_pos = Vec2( profileSystem->frameBreakdown_scrollX_left, frameBreakdown_scrollBound.bottom );
		scrollBar_dim = Vec2( profileSystem->frameBreakdown_scrollX_right - profileSystem->frameBreakdown_scrollX_left, getHeight( frameBreakdown_scrollBound ) );
		frameBreakdown_scrollBarBound = rectBLD( scrollBar_pos, scrollBar_dim );
		frameBreakdown_leftBound  = rectBLD( getBL( frameBreakdown_scrollBarBound ) + Vec2( -2.0f, 0.0f ), Vec2( 4.0f, scrollBar_dim.y ) );
		frameBreakdown_rightBound = rectBLD( getBR( frameBreakdown_scrollBarBound ) + Vec2( -2.0f, 0.0f ), Vec2( 4.0f, scrollBar_dim.y ) );
		
		int32 collateIndex = globalVar_profileSystem.eventDatabase.index;
		if( wasPressed( mouse, mouseButton_right ) ) {
			globalVar_profileSystem.eventDatabase.selectIndex = -1;
		}
		if( globalVar_profileSystem.eventDatabase.selectIndex != -1 ) {
			collateIndex = globalVar_profileSystem.eventDatabase.selectIndex;
		}
		if( isInBound( mouse->position, eventDatabase_bound ) ) {
			int32 hoverIndex = ( int32 )( ( mouse->position.x - eventDatabase_bound.left ) / eventDatabase_slotWidth );
			if( wasPressed( mouse, mouseButton_left ) ) {
				globalVar_profileSystem.eventDatabase.selectIndex = hoverIndex;
				collateIndex = hoverIndex;
			}
			if( globalVar_profileSystem.eventDatabase.selectIndex == -1 ) {
				collateIndex = hoverIndex;
			}
		}
		PROFILE_EVENT_LIST * collateList = globalVar_profileSystem.eventDatabase.eventList + collateIndex;
		PROFILE_EVENT_DATABASE_CLOCKS * collateClocks = globalVar_profileSystem.eventDatabase_clocks + collateIndex;
      
		collateEvents( collateList, globalVar_profileSystem.nodeList_currentFrame );
		
#define CLOCK_BOUND_MAX_STACK_COUNT  16
		uint64 clockBound_stack[ CLOCK_BOUND_MAX_STACK_COUNT ] = {};
		int32 stack_count = 0;
		
		flo32 frameBreakdown_minWidth = 40.0f;
		flo32 frameBreakdown_maxWidth = getWidth( frameBreakdown_scrollBound );
		flo32 frameBreakdown_ratio = ( profileSystem->frameBreakdown_scrollX_right - profileSystem->frameBreakdown_scrollX_left - frameBreakdown_minWidth ) / ( frameBreakdown_maxWidth - frameBreakdown_minWidth );
		// DISPLAY_VALUE( flo32, frameBreakdown_ratio );
		
		vec2	client_dim = getDim( client_bound );
		flo32 client_width = client_dim.x;
		flo32 profile_minWidth = client_dim.x;
		flo32 profile_maxWidth = profile_minWidth * 10.0f;
		flo32 profile_width   = lerp( profile_maxWidth, frameBreakdown_ratio, profile_minWidth );
		
		vec2  profile_basePos = Vec2( 0.0f, eventDatabase_bound.top + 20.0f );
		flo32 row_height = 20.0f;
		
		flo32 valid_width = getWidth( frameBreakdown_scrollBound ) - ( profileSystem->frameBreakdown_scrollX_right - profileSystem->frameBreakdown_scrollX_left );
		if( valid_width > 0.0f ) {
			flo32 offset_ratio = ( profileSystem->frameBreakdown_scrollX_left - frameBreakdown_scrollBound.left ) / valid_width;
			profile_basePos.x -= ( profile_width - client_width ) * offset_ratio;
		}
		uint64 profile_frameClockStart = collateList->frameClockStart;
		uint64 profile_clocksPerFrame = 60000000;
		
		rect clear_boundA = Rect( client_bound.left, client_bound.bottom, client_bound.right, eventDatabase_bound.top );
		DEBUG_drawRect( clear_boundA, Vec4( 0.0f, 0.0f, 0.0f, 0.95f ) );
		
		DEBUG_drawRect( eventDatabase_bound, COLOR_GRAY( 0.80f ) );
		for( int32 database_index = 0; database_index < PROFILE_EVENT_DATABASE_MAX_COUNT; database_index++ ) {
			PROFILE_EVENT_DATABASE_CLOCKS clock = globalVar_profileSystem.eventDatabase_clocks[ database_index ];
			
			rect boundA = {};
			boundA.left  = eventDatabase_basePos.x + eventDatabase_slotWidth * ( flo32 )database_index;
			boundA.right = boundA.left + eventDatabase_slotWidth;
			boundA.bottom = eventDatabase_basePos.y + ( ( flo32 )( clock.platformStart	- clock.frameStart ) / ( flo32 )profile_clocksPerFrame ) * profileSystem->eventDatabase_height;
			boundA.top 	 = eventDatabase_basePos.y + ( ( flo32 )( clock.appStart 		- clock.frameStart ) / ( flo32 )profile_clocksPerFrame ) * profileSystem->eventDatabase_height;
			DEBUG_drawRect( boundA, Vec4( 0.0f, 0.5f, 0.0f, 1.0f ) );
			
			rect boundB = {};
			boundB.left  = eventDatabase_basePos.x + eventDatabase_slotWidth * ( flo32 )database_index;
			boundB.right = boundB.left + eventDatabase_slotWidth;
			boundB.bottom = eventDatabase_basePos.y + ( ( flo32 )( clock.appStart		- clock.frameStart ) / ( flo32 )profile_clocksPerFrame ) * profileSystem->eventDatabase_height;
			boundB.top 	 = eventDatabase_basePos.y + ( ( flo32 )( clock.renderStart	- clock.frameStart ) / ( flo32 )profile_clocksPerFrame ) * profileSystem->eventDatabase_height;
			DEBUG_drawRect( boundB, Vec4( 0.0f, 0.0f, 0.5f, 1.0f ) );
			
			rect boundC = {};
			boundC.left  = eventDatabase_basePos.x + eventDatabase_slotWidth * ( flo32 )database_index;
			boundC.right = boundC.left + eventDatabase_slotWidth;
			boundC.bottom = eventDatabase_basePos.y + ( ( flo32 )( clock.renderStart	- clock.frameStart ) / ( flo32 )profile_clocksPerFrame ) * profileSystem->eventDatabase_height;
			boundC.top 	 = eventDatabase_basePos.y + ( ( flo32 )( clock.frameEnd		- clock.frameStart ) / ( flo32 )profile_clocksPerFrame ) * profileSystem->eventDatabase_height;
			DEBUG_drawRect( boundC, Vec4( 0.5f, 0.0f, 0.0f, 1.0f ) );
		}
		
		rect clear_boundB = Rect( client_bound.left, eventDatabase_bound.top, client_bound.right, client_bound.top );
		DEBUG_drawRect( clear_boundB, Vec4( 0.0f, 0.0f, 0.0f, 0.95f ) );
		
		rect eventDatabase_highlightBound = rectBLD( eventDatabase_basePos + Vec2( eventDatabase_slotWidth * ( flo32 )globalVar_profileSystem.eventDatabase.index, 0.0f ), Vec2( eventDatabase_slotWidth, profileSystem->eventDatabase_height ) );
		DEBUG_drawRect( eventDatabase_highlightBound, Vec4( 0.25f, 0.25f, 0.0f, 0.25f ) );
		
		if( collateIndex != -1 ) {
			vec2 collateIndex_position = eventDatabase_basePos + Vec2( eventDatabase_slotWidth * ( flo32 )collateIndex, 0.0f );
			DEBUG_drawRect( rectBLD( collateIndex_position, Vec2( eventDatabase_slotWidth, profileSystem->eventDatabase_height ) ), Vec4( 0.5f, 0.0f, 0.0f, 0.5f ) );
		}
		
		DEBUG_drawRect( frameBreakdown_scrollBound,    COLOR_GRAY( 0.20f ) );
		DEBUG_drawRect( frameBreakdown_scrollBarBound, COLOR_GRAY( 0.50f ) );
		
		if( isInBound( mouse->position, eventDatabase_resizeBound ) ) {
			DEBUG_drawRect( eventDatabase_resizeBound, COLOR_RED );
		}
		if( isInBound( mouse->position, frameBreakdown_leftBound ) ) {
			DEBUG_drawRect( frameBreakdown_leftBound, COLOR_RED );
		} else if( isInBound( mouse->position, frameBreakdown_rightBound ) ) {
			DEBUG_drawRect( frameBreakdown_rightBound, COLOR_RED );
		} else if( isInBound( mouse->position, frameBreakdown_scrollBarBound ) ) {
			DEBUG_drawRect( frameBreakdown_scrollBarBound, COLOR_RED );
		}
		
		vec4 colorTable[ 15 ] = {};
		colorTable[  0 ] = Vec4( 0.2f, 0.2f, 0.2f, 1.0f );
		colorTable[  1 ] = Vec4( 0.4f, 0.2f, 0.2f, 1.0f );
		colorTable[  2 ] = Vec4( 0.6f, 0.2f, 0.2f, 1.0f );
		colorTable[  3 ] = Vec4( 0.2f, 0.4f, 0.2f, 1.0f );
		colorTable[  4 ] = Vec4( 0.2f, 0.6f, 0.2f, 1.0f );
		colorTable[  5 ] = Vec4( 0.2f, 0.2f, 0.4f, 1.0f );
		colorTable[  6 ] = Vec4( 0.2f, 0.2f, 0.6f, 1.0f );
		colorTable[  7 ] = Vec4( 0.4f, 0.4f, 0.2f, 1.0f );
		colorTable[  8 ] = Vec4( 0.6f, 0.6f, 0.2f, 1.0f );
		colorTable[  9 ] = Vec4( 0.4f, 0.2f, 0.4f, 1.0f );
		colorTable[ 10 ] = Vec4( 0.6f, 0.2f, 0.6f, 1.0f );
		colorTable[ 11 ] = Vec4( 0.2f, 0.4f, 0.4f, 1.0f );
		colorTable[ 12 ] = Vec4( 0.2f, 0.6f, 0.6f, 1.0f );
		colorTable[ 13 ] = Vec4( 0.4f, 0.4f, 0.4f, 1.0f );
		colorTable[ 14 ] = Vec4( 0.6f, 0.6f, 0.6f, 1.0f );
		int32 color_select = 0;
		
      int32 threadID_count = 0;
		uint32 threadID[ 12 ] = {};
		
		PROFILE_EVENT * profile_tooltip = 0;
		for( int32 event_index = 0; event_index < collateList->count; event_index++ ) {
			PROFILE_EVENT * event = collateList->event + event_index;
			
			int32 level = -1;
			for( int32 threadID_index = 0; ( level == -1 ) && ( threadID_index < threadID_count ); threadID_index++ ) {
				if( threadID[ threadID_index ] == event->threadID ) {
					level = threadID_index;
				}
			}
			if( level == -1 ) {
				threadID[ threadID_count ] = event->threadID;
				level = threadID_count++;
			}
			Assert( threadID_count < 12 );
			
			rect bound = {};
			bound.left  = profile_basePos.x + ( ( flo32 )( event->clock_start - profile_frameClockStart ) / ( flo32 )profile_clocksPerFrame ) * profile_width;
			bound.right = profile_basePos.x + ( ( flo32 )( event->clock_end   - profile_frameClockStart ) / ( flo32 )profile_clocksPerFrame ) * profile_width;
			bound.top 	 = profile_basePos.y + row_height * ( flo32 )( level + 1 );
			bound.bottom = profile_basePos.y + row_height * ( flo32 )level;
			
			if( isInBound( mouse->position, bound ) ) {
				profile_tooltip = event;
			}
			
			DEBUG_drawRect( bound, colorTable[ color_select ] );
			
			color_select = ( color_select + 2 ) % _arrayCount( colorTable );
		}
		
		if( profile_tooltip ) {
			PROFILE_tooltip( profile_tooltip->functionName, mouse->position );
		}
		
		flo32  frame_msElapsed = profileSystem->getMSElapsed( collateClocks->counter_frameStart, collateClocks->counter_frameEnd, perfFrequency );
		uint64 frame_cycElapsed = collateClocks->frameEnd    - collateClocks->frameStart;
		uint64   app_cycElapsed = collateClocks->renderStart - collateClocks->appStart;
		
		char time_stringA[ 128 ] = {};
		sprintf( time_stringA, "Prev Frame: %10.04f ms  %10I64u cyc  %6I64u MC", frame_msElapsed, frame_cycElapsed, frame_cycElapsed / 1000 );
		
		char time_stringB[ 128 ] = {};
		sprintf( time_stringB, "App Update:                %10I64u cyc  %6I64u MC  %.02f", app_cycElapsed, app_cycElapsed / 1000, ( flo32 )app_cycElapsed / ( flo32 )frame_cycElapsed * 100.0f );
		
		PROFILE_string( time_stringA );
		PROFILE_string( time_stringB );
		
		if( globalVar_profileSystem.nodeList_currentFrame->count > 0 ) {
			PROFILE_string( "------------------------|---nCyc---||-MCyc-||-count-||-%frame-||-%app-|" );
		}
		for( int32 node_index = 0; node_index < globalVar_profileSystem.nodeList_currentFrame->count; node_index++ ) {
			PROFILE_NODE * node = globalVar_profileSystem.nodeList_currentFrame->node + node_index;
			
			char profile_string[ 512 ] = {};
			sprintf( profile_string, "%-24s %10I64u  %6I64u  %7d  %8.02f  %6.02f", node->functionName, node->clock_total, node->clock_total / 1000, node->count_total, ( ( flo32 )node->clock_total / ( flo32 )frame_cycElapsed ) * 100.0f, ( ( flo32 )node->clock_total / ( flo32 )app_cycElapsed ) * 100.0f );
			PROFILE_string( profile_string );
         
         for( uint32 iSubclock = 0; iSubclock < node->nSubclock; iSubclock++ ) {
            flo32 percent = ( flo32 )node->subclock[ iSubclock ] / ( flo32 )node->clock_total;
            
            char str[ 512 ] = {};
            sprintf( str, "  %.02f", percent );
            PROFILE_string( str );
         }
         // average each subclock for the event??
         // should this only work for nodes that have a single call
		}
	} else {
		flo32  frame_msElapsed 	= profileSystem->getMSElapsed( globalVar_profileSystem.eventDatabase_clocks_previousFrame.counter_frameStart, globalVar_profileSystem.eventDatabase_clocks_previousFrame.counter_frameEnd, perfFrequency );
		uint64 frame_cycElapsed = globalVar_profileSystem.eventDatabase_clocks_previousFrame.frameEnd - globalVar_profileSystem.eventDatabase_clocks_previousFrame.frameStart;
		uint64 app_cycElapsed = globalVar_profileSystem.eventDatabase_clocks_previousFrame.renderStart - globalVar_profileSystem.eventDatabase_clocks_previousFrame.appStart;
		char time_stringA[ 128 ] = {};
		sprintf( time_stringA, "Prev Frame: %10.04f ms  %10I64u cyc  %6I64u MC", frame_msElapsed, frame_cycElapsed, frame_cycElapsed / 1000 );
		
		char time_stringB[ 128 ] = {};
		sprintf( time_stringB, "App Update:                %10I64u cyc  %6I64u MC  %.02f", app_cycElapsed, app_cycElapsed / 1000, ( flo32 )app_cycElapsed / ( flo32 )frame_cycElapsed * 100.0f );
		
		DISPLAY_STRING( time_stringA );
		DISPLAY_STRING( time_stringB );
		
		PROFILE_EVENT_LIST * database_eventList = globalVar_profileSystem.eventDatabase.eventList + globalVar_profileSystem.eventDatabase.index;
		database_eventList->count = 0;
		database_eventList->frameClockStart = globalVar_profileSystem.eventDatabase_clocks_previousFrame.frameStart;
		
		globalVar_profileSystem.eventDatabase_clocks[ globalVar_profileSystem.eventDatabase.index ] = globalVar_profileSystem.eventDatabase_clocks_previousFrame;
		
		for( int32 event_index = 0; event_index < globalVar_profileSystem.eventList_currentFrame->count; event_index++ ) {
			PROFILE_EVENT * srcEvent = globalVar_profileSystem.eventList_currentFrame->event + event_index;
			PROFILE_EVENT * destEvent = database_eventList->event + database_eventList->count++;
			Assert( database_eventList->count < PROFILE_EVENT_LIST_MAX_COUNT );
			
			*destEvent = *srcEvent;
		}
		globalVar_profileSystem.eventDatabase.index = ( globalVar_profileSystem.eventDatabase.index + 1 ) % globalVar_profileSystem.eventDatabase.maxCount;
	}
	globalVar_profileSystem.eventList_currentFrame->count = 0;
	globalVar_profileSystem.nodeList_currentFrame->count = 0;
	
	KEYBOARD_STATE * keyboard = &platform->keyboard;
	if( wasPressed( keyboard, keyCode_p ) ) {
		if( globalVar_profileSystem.showProfile ) {
			profileSystem->eventDatabase_scrollActive = false;
		}
	}
   
	
	globalVar_profileSystem.eventDatabase_clocks_currentFrame.platformStart = __rdtsc();
}

#endif // DEBUG_BUILD

#endif	// STD_INCLUDE_FUNC