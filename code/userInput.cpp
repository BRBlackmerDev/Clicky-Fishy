
#ifdef	STD_INCLUDE_DECL

//----------
// mouse declarations
//----------

enum MOUSE_BUTTON_ID {
	mouseButton_left,
	mouseButton_right,
	mouseButton_middle,
	
	mouseButton_count,
};

struct MOUSE_BUTTON_STATE {
	boo32 isDown;
	int32 halfTransitionCount;
};

struct MOUSE_STATE {
	MOUSE_BUTTON_STATE button[ mouseButton_count ];
	vec2  position;
    vec2  prevPosition;
    
    uint8 flags;
	int32 wheelClicks;
};

//----------
// keyboard declarations
//----------

enum KEY_STATE_CODE {
	keyCode_null,
	
	keyCode_backspace,
	keyCode_shift,
	keyCode_control,
	keyCode_alt,
	keyCode_capsLock,
	keyCode_escape,
	keyCode_pageUp,
	keyCode_pageDown,
	keyCode_end,
	keyCode_home,
	keyCode_left,
	keyCode_right,
	keyCode_up,
	keyCode_down,
	keyCode_insert,
	keyCode_delete,
	keyCode_enter,
	keyCode_tab,
	keyCode_space,
	keyCode_0,
	keyCode_1,
	keyCode_2,
	keyCode_3,
	keyCode_4,
	keyCode_5,
	keyCode_6,
	keyCode_7,
	keyCode_8,
	keyCode_9,
	keyCode_a,
	keyCode_b,
	keyCode_c,
	keyCode_d,
	keyCode_e,
	keyCode_f,
	keyCode_g,
	keyCode_h,
	keyCode_i,
	keyCode_j,
	keyCode_k,
	keyCode_l,
	keyCode_m,
	keyCode_n,
	keyCode_o,
	keyCode_p,
	keyCode_q,
	keyCode_r,
	keyCode_s,
	keyCode_t,
	keyCode_u,
	keyCode_v,
	keyCode_w,
	keyCode_x,
	keyCode_y,
	keyCode_z,
	keyCode_period,
	keyCode_comma,
	keyCode_quote,
	keyCode_semicolon,
	keyCode_equal,
	keyCode_dash,
	keyCode_tilde,
	keyCode_forwardSlash,
	keyCode_backSlash,
	keyCode_openBracket,
	keyCode_closeBracket,
	keyCode_num0,
	keyCode_num1,
	keyCode_num2,
	keyCode_num3,
	keyCode_num4,
	keyCode_num5,
	keyCode_num6,
	keyCode_num7,
	keyCode_num8,
	keyCode_num9,
    keyCode_numAdd,
    keyCode_numSub,
    keyCode_numMul,
    keyCode_numDiv,
    keyCode_numPeriod,
    keyCode_F1,
    keyCode_F2,
    keyCode_F3,
    keyCode_F4,
    keyCode_F5,
    keyCode_F6,
    keyCode_F7,
    keyCode_F8,
    keyCode_F9,
    keyCode_F10,
    keyCode_F11,
    keyCode_F12,
	
	keyCode_count,
};

struct KEY_STATE {
    uint16 keyCode;
    uint8  isDown;
	uint8  flags;
};

#define KEYBOARD_FLAGS__CONTROL  ( 0x01 )
#define KEYBOARD_FLAGS__ALT      ( 0x02 )
#define KEYBOARD_FLAGS__SHIFT    ( 0x04 )

#define KEYBOARD_STATE__MAX_EVENT_COUNT  ( 16 )
struct KEYBOARD_STATE {
	uint8    flags;
    uint32   nEvents;
    KEY_STATE event[ KEYBOARD_STATE__MAX_EVENT_COUNT ];
};

//----------
// controller declarations
//----------

enum CONTROLLER_BUTTON_ID {
	controllerButton_dPad_up,
	controllerButton_dPad_down,
	controllerButton_dPad_left,
	controllerButton_dPad_right,
    
    controllerButton_start,
    controllerButton_back,
    
    controllerButton_thumb_left,
    controllerButton_thumb_right,
    controllerButton_shoulder_left,
    controllerButton_shoulder_right,
    
    controllerButton_A,
    controllerButton_B,
    controllerButton_X,
    controllerButton_Y,
	
	controllerButton_count,
};

struct CONTROLLER_BUTTON_STATE {
	boo32 isDown;
	int32 halfTransitionCount;
};

struct CONTROLLER_STATE {
	CONTROLLER_BUTTON_STATE button[ controllerButton_count ];
};

#endif	// STD_INCLUDE_DECL
#ifdef	STD_INCLUDE_FUNC

//----------
// mouse functions
//----------

internal void
MOUSE_endOfFrame( MOUSE_STATE * mouse ) {
	mouse->wheelClicks  = 0;
    //mouse->prevPosition = mouse->position;
	for( uint32 buttonIndex = 0; buttonIndex < mouseButton_count; buttonIndex++ ) {
		MOUSE_BUTTON_STATE * button = &mouse->button[ buttonIndex ];
		button->halfTransitionCount = 0;
	}
}

internal boo32
isDown( MOUSE_STATE * mouse, MOUSE_BUTTON_ID buttonID, uint8 flags = 0 ) {
    MOUSE_BUTTON_STATE * button = mouse->button + buttonID;
	boo32 result = ( button->isDown ) && ( mouse->flags == flags );
	return result;
}

internal boo32
wasButtonPressed( MOUSE_BUTTON_STATE * button ) {
	boo32 result = false;
	if( ( button->isDown && button->halfTransitionCount >= 1 ) ||
       ( !button->isDown && button->halfTransitionCount >= 2 ) ) {
		result = true;
	}
	return result;
}

internal boo32
wasPressed( MOUSE_STATE * mouse, MOUSE_BUTTON_ID buttonID, uint8 flags = 0 ) {
	MOUSE_BUTTON_STATE * button = &mouse->button[ buttonID ];
	boo32 result = ( wasButtonPressed( button ) ) && ( mouse->flags == flags );
	return result;
}

internal boo32
wasReleased( MOUSE_BUTTON_STATE * button ) {
	boo32 result = false;
	if( ( !button->isDown && button->halfTransitionCount >= 1 ) ||
       ( button->isDown && button->halfTransitionCount >= 2 ) ) {
		result = true;
	}
	return result;
}

internal boo32
wasReleased( MOUSE_STATE * mouse, MOUSE_BUTTON_ID buttonID ) {
	MOUSE_BUTTON_STATE * button = &mouse->button[ buttonID ];
	boo32 result = wasReleased( button );
	return result;
}

internal void
addButtonEvent( MOUSE_STATE * mouse, MOUSE_BUTTON_ID buttonID, boo32 isDown, uint8 flags ) {
    mouse->flags = flags;
    
    MOUSE_BUTTON_STATE * button = mouse->button + buttonID;
    button->isDown = isDown;
    button->halfTransitionCount++;
}

//----------
// keyboard functions
//----------

internal KEY_STATE
KeyState( KEY_STATE_CODE keyCode, uint8 isDown, uint8 flags ) {
	KEY_STATE result = {};
	result.keyCode     = ( uint16 )keyCode;
	result.isDown      = isDown;
	result.flags       = flags;
	return result;
}

internal boo32
wasPressed( KEYBOARD_STATE * keyboard, KEY_STATE_CODE keyCode, uint8 targetFlags = 0 ) {
	boo32 result = false;
	for( uint32 iEvent = 0; iEvent < keyboard->nEvents; iEvent++ ) {
		KEY_STATE event = keyboard->event[ iEvent ];
		if( ( event.keyCode == keyCode ) && ( event.isDown ) && ( event.flags == targetFlags ) ) {
			result = true;
		}
	}
	return result;
}

internal boo32
wasReleased( KEYBOARD_STATE * keyboard, KEY_STATE_CODE keyCode ) {
	boo32 result = false;
	for( uint32 iEvent = 0; iEvent < keyboard->nEvents; iEvent++ ) {
		KEY_STATE event = keyboard->event[ iEvent ];
		if( ( event.keyCode == keyCode ) && ( !event.isDown ) ) {
			result = true;
		}
	}
	return result;
}

internal void
addKeyEvent( KEYBOARD_STATE * keyboard, KEY_STATE_CODE keyCode, boo32 isDown, uint8 flags ) {
	Assert( keyboard->nEvents < KEYBOARD_STATE__MAX_EVENT_COUNT );
	keyboard->event[ keyboard->nEvents++ ] = KeyState( keyCode, ( uint8 )isDown, flags );
}

//----------
// controller functions
//----------

internal boo32
wasButtonPressed( CONTROLLER_BUTTON_STATE * button ) {
	boo32 result = false;
	if( (  button->isDown && ( button->halfTransitionCount >= 1 ) ) ||
       ( !button->isDown && ( button->halfTransitionCount >= 2 ) ) ) {
		result = true;
	}
	return result;
}

internal boo32
wasPressed( CONTROLLER_STATE * controller, CONTROLLER_BUTTON_ID buttonID ) {
	CONTROLLER_BUTTON_STATE * button = controller->button + buttonID;
	boo32 result = wasButtonPressed( button );
	return result;
}

internal boo32
wasReleased( CONTROLLER_BUTTON_STATE * button ) {
	boo32 result = false;
	if( ( !button->isDown && ( button->halfTransitionCount >= 1 ) ) ||
       (  button->isDown && ( button->halfTransitionCount >= 2 ) ) ) {
		result = true;
	}
	return result;
}

internal boo32
wasReleased( CONTROLLER_STATE * controller, CONTROLLER_BUTTON_ID buttonID ) {
	CONTROLLER_BUTTON_STATE * button = controller->button + buttonID;
	boo32 result = wasReleased( button );
	return result;
}

internal void
updateButton( CONTROLLER_STATE * controller, CONTROLLER_BUTTON_ID buttonID, boo32 isDown ) {
    CONTROLLER_BUTTON_STATE * button = controller->button + buttonID;
    if( (  button->isDown && !isDown ) ||
       ( !button->isDown &&  isDown ) ) {
        button->halfTransitionCount++;
    }
    button->isDown = isDown;
}

#endif	// STD_INCLUDE_FUNC