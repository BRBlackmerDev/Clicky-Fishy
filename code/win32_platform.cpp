
internal FILE_DATA
win32_readFile( MEMORY * memory, const char * saveDir, const char * filename ) {
	FILE_DATA result = {};
	
	char filePath[ 2048 ] = {};
	if( saveDir ) {
		sprintf( filePath, "%s/%s", saveDir, filename );
	} else {
		sprintf( filePath, "%s", filename );
	}
	
	HANDLE file = CreateFile( filePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
	if( file != INVALID_HANDLE_VALUE ) {
		LARGE_INTEGER _fileSize = {};
		if( GetFileSizeEx( file, &_fileSize ) ) {
            uint32 fileSize = ( uint32 )_fileSize.QuadPart;
            uint8 * dest = ( uint8 * )_pushSize( memory, fileSize );
			
			DWORD bytesRead = {};
            BOOL  status    = ReadFile( file, dest, fileSize, &bytesRead, 0 );
            if( ( status ) && ( fileSize == bytesRead ) ) {
                result.contents = dest;
                result.size     = fileSize;
			} else {
				_popSize( memory, result.size );
                
                char debug[ 128 ] {};
                sprintf( debug, "ERROR! File: %s : unable to read contents!\n", filename );
                OutputDebugString( debug );
			}
			CloseHandle( file );
		} else {
            char debug[ 128 ] = {};
            sprintf( debug, "ERROR! File: %s : does not contain any data!\n", filename );
            OutputDebugString( debug );
		}
	} else {
        char debug[ 128 ] = {};
        sprintf( debug, "ERROR! File: %s : unable to open!\n", filename );
        OutputDebugString( debug );
	}
	
	return result;
}

internal boo32
win32_writeFile( const char * saveDir, const char * filename, void * data, uint32 size ) {
	boo32 result = false;
	
	char filePath[ 2048 ] = {};
	if( saveDir ) {
		sprintf( filePath, "%s/%s", saveDir, filename );
	} else {
		sprintf( filePath, "%s", filename );
	}
    
	HANDLE fileHandle = CreateFile( filePath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0 );
	if( fileHandle != INVALID_HANDLE_VALUE ) {
		DWORD bytesWritten;
		if( WriteFile( fileHandle, data, size, &bytesWritten, 0 ) ) {
			result = ( bytesWritten == size );
		}
        
		CloseHandle( fileHandle );
	}
    
	return result;
}

uint32 frame_counter = 0;
internal void
win32_outputMessage( char * header, char * message ) {
    char str[ 64 ] = {};
    sprintf( str, "%10u : %8s : %s\n", frame_counter, header, message );
    OutputDebugString( str );
}

internal void
win32_logMessage( uint32 message, boo32 fromWndProc ) {
    char * header0 = "WndProc";
    char * header1 = "my";
    
    char * header = ( fromWndProc ? header0 : header1 );
    
    switch( message ) {
        case WM_ACTIVATE: {
            // Sent to both the window being activated and the window being deactivated. If the windows use the same input queue, the message is sent synchronously, first to the window procedure of the top-level window being deactivated, then to the window procedure of the top-level window being activated. If the windows use different input queues, the message is sent asynchronously, so the window is activated immediately.
            win32_outputMessage( header, "WM_ACTIVATE" );
        } break;
        
        case WM_ACTIVATEAPP: {
            // Sent when the active window changes. Sent to both the activated window and the deactivated window.
            win32_outputMessage( header, "WM_ACTIVATEAPP" );
        } break;
        
        case WM_CAPTURECHANGED: {
            // Sent to the window that is losing the mouse capture.
            win32_outputMessage( header, "WM_CAPTURECHANGED" );
        } break;
        
        case WM_CLOSE: {
            // Sent as a signal that a window or an application should terminate.
            win32_outputMessage( header, "WM_CLOSE" );
        } break;
        
        case WM_DESTROY: {
            // Sent when a window is being destroyed. It is sent to the window procedure of the window being destroyed after the window is removed from the screen.
            win32_outputMessage( header, "WM_DESTROY" );
        } break;
        
        case WM_GETMINMAXINFO: {
            // Sent to a window when its size or position is about to change.
            win32_outputMessage( header, "WM_GETMINMAXINFO" );
        } break;
        
        case WM_GETICON: {
            // Sent to a window to retrieve the handle to the large or small icon associated with the window.
            win32_outputMessage( header, "WM_GETICON" );
        } break;
        
        case WM_IME_SETCONTEXT: {
            // Sent to an application when a window is activated. A window receives this message through its WindowProc function.
            win32_outputMessage( header, "WM_IME_SETCONTEXT" );
        } break;
        
        case WM_IME_NOTIFY: {
            // Sent to an application to notify it of changes to the IME window. A window receives this message through its WindowProc function.
            win32_outputMessage( header, "WM_IME_NOTIFY" );
        } break;
        
        case WM_KILLFOCUS: {
            // Sent to a window immediately before it loses the keyboard focus.
            win32_outputMessage( header, "WM_KILLFOCUS" );
        } break;
        
        case WM_MOUSEMOVE: {
            // Posted to a window when the cursor moves. If the mouse is not captured, the message is posted to the window that contains the cursor. Otherwise, the message is posted to the window that has captured the mouse.
            // win32_outputMessage( header, "WM_MOUSEMOVE" );
        } break;
        
        case WM_NCACTIVATE: {
            // Sent to a window when its nonclient area needs to be changed to indicate an active or inactive state.
            win32_outputMessage( header, "WM_NCACTIVATE" );
        } break;
        
        case WM_NCCREATE: {
            // Sent when a window is first created. Sent prior to the WM_CREATE message.
            win32_outputMessage( header, "WM_NCCREATE" );
        } break;
        
        case WM_NCCALCSIZE: {
            // Sent when the size and position of a window's client area must be calculated.
            win32_outputMessage( header, "WM_NCCALCSIZE" );
        } break;
        
        case WM_NCDESTROY: {
            // Notifies a window that its nonclient area is being destroyed. The DestroyWindow function sends the WM_NCDESTROY message to the window following the WM_DESTROY message.WM_DESTROY is used to free the allocated memory object associated with the window. The WM_NCDESTROY message is sent after the child windows have been destroyed. In contrast, WM_DESTROY is sent before the child windows are destroyed. A window receives this message through its WindowProc function.
            win32_outputMessage( header, "WM_NCDESTROY" );
        } break;
        
        case WM_NCHITTEST: {
            // Sent to a window to determine what part of the window corresponds to a particular screen coordinate. Sent, for example, in response to mouse cursor movement or when a mouse button is pressed/released.
            // win32_outputMessage( header, "WM_NCHITTEST" );
        } break;
        
        case WM_NCLBUTTONDOWN: {
            // Sent to a window when the user presses the left mouse button while the cursor is within the nonclient area of the window.
            win32_outputMessage( header, "WM_NCLBUTTONDOWN" );
        } break;
        
        case WM_NCMOUSEMOVE: {
            // Posted to a window when the cursor is moved within the nonclient area of the window. This message is posted to the window that contains the cursor. If a window has captured the mouse, this message is not posted.
            // win32_outputMessage( header, "WM_NCMOUSEMOVE" );
        } break;
        
        case WM_PAINT: {
            // Sent when the system or another application makes a request to paint a portion of the application's window.
            win32_outputMessage( header, "WM_PAINT" );
        } break;
        
        case WM_QUIT: {
            // Indicates a request to terminate an application, and is generated when the application calls the PostQuitMessage function.
            win32_outputMessage( header, "WM_QUIT" );
        } break;
        
        case WM_SETCURSOR: {
            // Sent if the mouse cursor moves, but the input is not captured by the window.
            // win32_outputMessage( header, "WM_SETCURSOR" );
        } break;
        
        case WM_SYSCOMMAND: {
            // A window receives this message when the user chooses a command from the Window menu (formerly known as the system or control menu) or when the user chooses the maximize button, minimize button, restore button, or close button.
            win32_outputMessage( header, "WM_SYSCOMMAND" );
        } break;
		
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP: {
            win32_outputMessage( header, "Windows Key Message" );
            // ERROR! Keyboard message was passed directly to WindowProc! All keyboard messages should be handled directly by the win32_processPendingMessages()!
        } break;
        
        case WM_TIMER: {
            // Posted to the installing thread's message queue when a timer expires. The message is posted by the GetMessage or PeekMessage function.
            // win32_outputMessage( header, "WM_TIMER" );
        } break;
        
        case WM_WINDOWPOSCHANGED: {
            // Sent to a window whose size, position, or place in the Z order has changed as a result of a call to the SetWindowPos function or another window-management function.
            win32_outputMessage( header, "WM_WINDOWPOSCHANGED" );
        } break;
        
        case WM_WINDOWPOSCHANGING: {
            // Sent to a window whose size, position, or place in the Z order is about to change as a result of a call to the SetWindowPos function or another window-management function.
            win32_outputMessage( header, "WM_WINDOWPOSCHANGING" );
        } break;
        
        default: {
            char str[ 8 ] = {};
            sprintf( str, "0x%04X", ( uint32 )message );
            win32_outputMessage( header, str );
        } break;
    }
}

LRESULT CALLBACK
win32_WindowProc( HWND window, uint32 message, WPARAM wParam, LPARAM lParam ) {
    // win32_logMessage( message, true );
	LRESULT result = 0;
    switch( message ) {
        case WM_DESTROY: {
            PostQuitMessage( 0 );
        } break;
        
        default: {
            result = DefWindowProc( window, message, wParam, lParam );
        } break;
    }
    return result;
}

internal void
win32_processPendingMessages( PLATFORM * platform, HWND windowHandle ) {
    KEYBOARD_STATE * keyboard = &platform->keyboard;
    keyboard->nEvents = 0;
    
    MOUSE_STATE * mouse = &platform->mouse;
    MOUSE_endOfFrame( mouse );
    
	MSG message = {};
    while( PeekMessage( &message, 0, 0, 0, PM_REMOVE ) ) {
        // win32_logMessage( message.message, false );
        
        switch( message.message ) {
            case WM_QUIT: {
                platform->isRunning = false;
            } break;
            
			case WM_MOUSEWHEEL: {
				int16 wParam_hi = ( ( message.wParam >> 16 ) & 0xFFFF );
				int32 wheelClicks = wParam_hi / 120;
				mouse->wheelClicks = wheelClicks;
			} break;
			
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP: {
				addButtonEvent( mouse, mouseButton_left, ( message.wParam & MK_LBUTTON ), keyboard->flags );
			} break;
			
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP: {
				addButtonEvent( mouse, mouseButton_middle, ( message.wParam & MK_MBUTTON ), keyboard->flags );
			} break;
			
			case WM_RBUTTONUP:
			case WM_RBUTTONDOWN: {
				addButtonEvent( mouse, mouseButton_right, ( message.wParam & MK_RBUTTON ), keyboard->flags );
			} break;
			
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_KEYDOWN:
			case WM_KEYUP: {
                boo32 wasDown = ( ( message.lParam & ( 1 << 30 ) ) != 0 );
				boo32 isDown  = ( ( message.lParam & ( 1 << 31 ) ) == 0 );
				if( isDown != wasDown ) {
                    switch( message.wParam ) {
						case VK_BACK: { addKeyEvent( keyboard, keyCode_backspace, isDown, keyboard->flags ); } break;
						case VK_TAB: 		{ addKeyEvent( keyboard, keyCode_tab, isDown, keyboard->flags ); } break;
						case VK_RETURN:	{ addKeyEvent( keyboard, keyCode_enter, isDown, keyboard->flags ); } break;
						case VK_SHIFT: 	{
                            if( isDown ) {
                                keyboard->flags |=  KEYBOARD_FLAGS__SHIFT;
                            } else {
                                keyboard->flags &= ~KEYBOARD_FLAGS__SHIFT;
                            }
							addKeyEvent( keyboard, keyCode_shift, isDown, keyboard->flags );
						} break;
						case VK_CONTROL: {
                            if( isDown ) {
                                keyboard->flags |=  KEYBOARD_FLAGS__CONTROL;
                            } else {
                                keyboard->flags &= ~KEYBOARD_FLAGS__CONTROL;
                            }
							addKeyEvent( keyboard, keyCode_control, isDown, keyboard->flags );
						} break;
						case VK_MENU: 		{
                            if( isDown ) {
                                keyboard->flags |=  KEYBOARD_FLAGS__ALT;
                            } else {
                                keyboard->flags &= ~KEYBOARD_FLAGS__ALT;
                            }
                            addKeyEvent( keyboard, keyCode_alt, isDown, keyboard->flags );
                        } break;
						case VK_CAPITAL: 	{ addKeyEvent( keyboard, keyCode_capsLock, 	isDown, keyboard->flags ); } break;
						case VK_ESCAPE: 	{ addKeyEvent( keyboard, keyCode_escape, 		isDown, keyboard->flags ); } break;
						case VK_SPACE: 	{ addKeyEvent( keyboard, keyCode_space, 		isDown, keyboard->flags ); } break;
						case VK_PRIOR: 	{ addKeyEvent( keyboard, keyCode_pageUp, 		isDown, keyboard->flags ); } break;
						case VK_NEXT: 		{ addKeyEvent( keyboard, keyCode_pageDown, 	isDown, keyboard->flags ); } break;
						case VK_END: 		{ addKeyEvent( keyboard, keyCode_end, 			isDown, keyboard->flags ); } break;
						case VK_HOME: 		{ addKeyEvent( keyboard, keyCode_home, 		isDown, keyboard->flags ); } break;
						case VK_LEFT: 		{ addKeyEvent( keyboard, keyCode_left, 		isDown, keyboard->flags ); } break;
						case VK_RIGHT: 	{ addKeyEvent( keyboard, keyCode_right, 		isDown, keyboard->flags ); } break;
						case VK_UP: 		{ addKeyEvent( keyboard, keyCode_up, 			isDown, keyboard->flags ); } break;
						case VK_DOWN: 		{ addKeyEvent( keyboard, keyCode_down,       isDown, keyboard->flags ); } break;
						case VK_INSERT: 	{ addKeyEvent( keyboard, keyCode_insert, 		isDown, keyboard->flags ); } break;
						case VK_DELETE: 	{ addKeyEvent( keyboard, keyCode_delete, 		isDown, keyboard->flags ); } break;
						case 0x30: { addKeyEvent( keyboard, keyCode_0, isDown, keyboard->flags ); } break;
						case 0x31: { addKeyEvent( keyboard, keyCode_1, isDown, keyboard->flags ); } break;
						case 0x32: { addKeyEvent( keyboard, keyCode_2, isDown, keyboard->flags ); } break;
						case 0x33: { addKeyEvent( keyboard, keyCode_3, isDown, keyboard->flags ); } break;
						case 0x34: { addKeyEvent( keyboard, keyCode_4, isDown, keyboard->flags ); } break;
						case 0x35: { addKeyEvent( keyboard, keyCode_5, isDown, keyboard->flags ); } break;
						case 0x36: { addKeyEvent( keyboard, keyCode_6, isDown, keyboard->flags ); } break;
						case 0x37: { addKeyEvent( keyboard, keyCode_7, isDown, keyboard->flags ); } break;
						case 0x38: { addKeyEvent( keyboard, keyCode_8, isDown, keyboard->flags ); } break;
						case 0x39: { addKeyEvent( keyboard, keyCode_9, isDown, keyboard->flags ); } break;
						case 0x41: { addKeyEvent( keyboard, keyCode_a, isDown, keyboard->flags ); } break;
						case 0x42: { addKeyEvent( keyboard, keyCode_b, isDown, keyboard->flags ); } break;
						case 0x43: { addKeyEvent( keyboard, keyCode_c, isDown, keyboard->flags ); } break;
						case 0x44: { addKeyEvent( keyboard, keyCode_d, isDown, keyboard->flags ); } break;
						case 0x45: { addKeyEvent( keyboard, keyCode_e, isDown, keyboard->flags ); } break;
						case 0x46: { addKeyEvent( keyboard, keyCode_f, isDown, keyboard->flags ); } break;
						case 0x47: { addKeyEvent( keyboard, keyCode_g, isDown, keyboard->flags ); } break;
						case 0x48: { addKeyEvent( keyboard, keyCode_h, isDown, keyboard->flags ); } break;
						case 0x49: { addKeyEvent( keyboard, keyCode_i, isDown, keyboard->flags ); } break;
						case 0x4A: { addKeyEvent( keyboard, keyCode_j, isDown, keyboard->flags ); } break;
						case 0x4B: { addKeyEvent( keyboard, keyCode_k, isDown, keyboard->flags ); } break;
						case 0x4C: { addKeyEvent( keyboard, keyCode_l, isDown, keyboard->flags ); } break;
						case 0x4D: { addKeyEvent( keyboard, keyCode_m, isDown, keyboard->flags ); } break;
						case 0x4E: { addKeyEvent( keyboard, keyCode_n, isDown, keyboard->flags ); } break;
						case 0x4F: { addKeyEvent( keyboard, keyCode_o, isDown, keyboard->flags ); } break;
						case 0x50: { addKeyEvent( keyboard, keyCode_p, isDown, keyboard->flags ); } break;
						case 0x51: { addKeyEvent( keyboard, keyCode_q, isDown, keyboard->flags ); } break;
						case 0x52: { addKeyEvent( keyboard, keyCode_r, isDown, keyboard->flags ); } break;
						case 0x53: { addKeyEvent( keyboard, keyCode_s, isDown, keyboard->flags ); } break;
						case 0x54: { addKeyEvent( keyboard, keyCode_t, isDown, keyboard->flags ); } break;
						case 0x55: { addKeyEvent( keyboard, keyCode_u, isDown, keyboard->flags ); } break;
						case 0x56: { addKeyEvent( keyboard, keyCode_v, isDown, keyboard->flags ); } break;
						case 0x57: { addKeyEvent( keyboard, keyCode_w, isDown, keyboard->flags ); } break;
						case 0x58: { addKeyEvent( keyboard, keyCode_x, isDown, keyboard->flags ); } break;
						case 0x59: { addKeyEvent( keyboard, keyCode_y, isDown, keyboard->flags ); } break;
						case 0x5A: { addKeyEvent( keyboard, keyCode_z, isDown, keyboard->flags ); } break;
						case VK_OEM_1: { addKeyEvent( keyboard, keyCode_semicolon, isDown, keyboard->flags ); } break;
						case VK_OEM_PLUS: { addKeyEvent( keyboard, keyCode_equal, isDown, keyboard->flags ); } break;
						case VK_OEM_COMMA: { addKeyEvent( keyboard, keyCode_comma, isDown, keyboard->flags ); } break;
						case VK_OEM_MINUS: { addKeyEvent( keyboard, keyCode_dash, isDown, keyboard->flags ); } break;
						case VK_OEM_PERIOD: { addKeyEvent( keyboard, keyCode_period, isDown, keyboard->flags ); } break;
						case VK_OEM_2: { addKeyEvent( keyboard, keyCode_forwardSlash, isDown, keyboard->flags ); } break;
						case VK_OEM_3: { addKeyEvent( keyboard, keyCode_tilde, isDown, keyboard->flags ); } break;
						case VK_OEM_4: { addKeyEvent( keyboard, keyCode_openBracket, isDown, keyboard->flags ); } break;
						case VK_OEM_5: { addKeyEvent( keyboard, keyCode_backSlash, isDown, keyboard->flags ); } break;
						case VK_OEM_6: { addKeyEvent( keyboard, keyCode_closeBracket, isDown, keyboard->flags ); } break;
						case VK_OEM_7: { addKeyEvent( keyboard, keyCode_quote, isDown, keyboard->flags ); } break;
						case VK_NUMPAD0: { addKeyEvent( keyboard, keyCode_num0, isDown, keyboard->flags ); } break;
						case VK_NUMPAD1: { addKeyEvent( keyboard, keyCode_num1, isDown, keyboard->flags ); } break;
						case VK_NUMPAD2: { addKeyEvent( keyboard, keyCode_num2, isDown, keyboard->flags ); } break;
						case VK_NUMPAD3: { addKeyEvent( keyboard, keyCode_num3, isDown, keyboard->flags ); } break;
						case VK_NUMPAD4: {addKeyEvent( keyboard, keyCode_num4, isDown, keyboard->flags ); } break;
						case VK_NUMPAD5: { addKeyEvent( keyboard, keyCode_num5, isDown, keyboard->flags ); } break;
						case VK_NUMPAD6: { addKeyEvent( keyboard, keyCode_num6, isDown, keyboard->flags ); } break;
						case VK_NUMPAD7: { addKeyEvent( keyboard, keyCode_num7, isDown, keyboard->flags ); } break;
						case VK_NUMPAD8: { addKeyEvent( keyboard, keyCode_num8, isDown, keyboard->flags ); } break;
						case VK_NUMPAD9: { addKeyEvent( keyboard, keyCode_num9, isDown, keyboard->flags ); } break;
						case VK_MULTIPLY: { addKeyEvent( keyboard, keyCode_numMul, isDown, keyboard->flags ); } break;
						case VK_ADD: { addKeyEvent( keyboard, keyCode_numAdd, isDown, keyboard->flags ); } break;
						case VK_DECIMAL: { addKeyEvent( keyboard, keyCode_numPeriod, isDown, keyboard->flags ); } break;
						case VK_SUBTRACT: { addKeyEvent( keyboard, keyCode_numSub, isDown, keyboard->flags ); } break;
						case VK_DIVIDE: { addKeyEvent( keyboard, keyCode_numDiv, isDown, keyboard->flags ); } break;
						case VK_F1:      { addKeyEvent( keyboard, keyCode_F1,   isDown, keyboard->flags ); } break;
						case VK_F2:      { addKeyEvent( keyboard, keyCode_F2,   isDown, keyboard->flags ); } break;
						case VK_F3:      { addKeyEvent( keyboard, keyCode_F3,   isDown, keyboard->flags ); } break;
						case VK_F4:      { addKeyEvent( keyboard, keyCode_F4,   isDown, keyboard->flags ); } break;
						case VK_F5:      { addKeyEvent( keyboard, keyCode_F5,   isDown, keyboard->flags ); } break;
						case VK_F6:      { addKeyEvent( keyboard, keyCode_F6,   isDown, keyboard->flags ); } break;
						case VK_F7:      { addKeyEvent( keyboard, keyCode_F7,   isDown, keyboard->flags ); } break;
						case VK_F8:      { addKeyEvent( keyboard, keyCode_F8,   isDown, keyboard->flags ); } break;
						case VK_F9:      { addKeyEvent( keyboard, keyCode_F9,   isDown, keyboard->flags ); } break;
						case VK_F10:     { addKeyEvent( keyboard, keyCode_F10,  isDown, keyboard->flags ); } break;
						case VK_F11:     { addKeyEvent( keyboard, keyCode_F11,  isDown, keyboard->flags ); } break;
						case VK_F12:     { addKeyEvent( keyboard, keyCode_F12,  isDown, keyboard->flags ); } break;
						default: {
							char string[ 128 ] = {};
							sprintf( string, "Key message received, but not processed: %lu %s\n", (uint32)message.wParam, ( isDown ? "PRESSED" : "RELEASED" ) );
							OutputDebugString( string );
						} break;
					};
				}
			} break;
            
            default: {
                TranslateMessage( &message );
                DispatchMessage ( &message );
            } break;
        }
	}
}

internal void
initPass( RENDERER * renderer, RENDER_PASS * pass, MEMORY * memory, uint32 nTri, uint32 nLine, uint32 nText ) {
    pass->triObjectList.maxObjects  = nTri;
    pass->lineObjectList.maxObjects = nLine;
    pass->textList.maxObjects       = nText;
    
    pass->triObjectList.object  = _pushArray( memory,      RENDER_OBJECT, nTri  );
    pass->lineObjectList.object = _pushArray( memory,      RENDER_OBJECT, nLine );
    pass->textList.object       = _pushArray( memory, TEXT_RENDER_OBJECT, nText );
    
    pass->modelBuffer = &renderer->modelBuffer;
    pass->textBuffer  = &renderer->textBuffer;
}

internal void
renderPass( RENDERER * renderer, RENDER_PASS * pass, mat4 camera_transform ) {
    ID3D12GraphicsCommandList * commandList = renderer->commandList;
    
    { // draw models
        VERTEX1_BUFFER * buffer = &renderer->modelBuffer;
        commandList->IASetVertexBuffers( 0, 1, &buffer->vertexView );
        commandList->IASetIndexBuffer  ( &buffer->indexView );
        
        commandList->SetGraphicsRoot32BitConstants( 0, 16, camera_transform.elem, 0 );
        
        { // triangle models
            RENDER_OBJECT_LIST * objectList = &pass->triObjectList;
            
            commandList->SetPipelineState( renderer->PSS_tri_noDepth_blend );
            commandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
            for( uint32 iObject = 0; iObject < objectList->nObjects; iObject++ ) {
                RENDER_OBJECT object    = objectList->object[ iObject ];
                MODEL_DATA    modelData = renderer->modelData[ object.modelID ];
                
                commandList->SetGraphicsRoot32BitConstants( 0, 16, object.transform.elem, 16 );
                
                TEXTURE_ID textureID = textureID_defaultTexture;
                if( renderer->texture_isLoaded[ object.textureID ] ) {
                    textureID = object.textureID;
                }
                D3D12_GPU_DESCRIPTOR_HANDLE handle = getGPUHandle( renderer->device, renderer->SRVHeap, textureID );
                commandList->SetGraphicsRootDescriptorTable( 2, handle );
                
                commandList->SetGraphicsRoot32BitConstants( 1, 4, object.modColor.elem, 0 );
                
                commandList->DrawIndexedInstanced( modelData.nIndex, 1, modelData.bIndex, modelData.bVertex, 0 );
            }
            objectList->nObjects = 0;
        } // END triangle models
        
        { // line models
            RENDER_OBJECT_LIST * objectList = &pass->lineObjectList;
            
            commandList->SetPipelineState( renderer->PSS_line_noDepth_blend );
            commandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_LINESTRIP );
            for( uint32 iObject = 0; iObject < objectList->nObjects; iObject++ ) {
                RENDER_OBJECT object    = objectList->object[ iObject ];
                MODEL_DATA    modelData = renderer->modelData[ object.modelID ];
                
                commandList->SetGraphicsRoot32BitConstants( 0, 16, object.transform.elem, 16 );
                
                Assert( object.textureID == textureID_whiteTexture );
                D3D12_GPU_DESCRIPTOR_HANDLE handle = getGPUHandle( renderer->device, renderer->SRVHeap, object.textureID );
                commandList->SetGraphicsRootDescriptorTable( 2, handle );
                
                commandList->SetGraphicsRoot32BitConstants( 1, 4, object.modColor.elem, 0 );
                
                commandList->DrawIndexedInstanced( modelData.nIndex, 1, modelData.bIndex, modelData.bVertex, 0 );
            }
            objectList->nObjects = 0;
        } // END line models
    }
    
    { // draw overlay text
        commandList->SetPipelineState( renderer->PSS_tri_noDepth_blend );
        
        VERTEX1_BUFFER * buffer = &renderer->textBuffer;
        buffer->vertexView.SizeInBytes = sizeof( VERTEX1 ) * buffer->nVertex;
        
        commandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
        commandList->IASetVertexBuffers( 0, 1, &buffer->vertexView );
        
        D3D12_GPU_DESCRIPTOR_HANDLE handle = getGPUHandle( renderer->device, renderer->SRVHeap, textureID_font );
        commandList->SetGraphicsRootDescriptorTable( 2, handle );
        
        //mat4 camera_transform = mat4_orthographic( app_bound );
        commandList->SetGraphicsRoot32BitConstants( 0, 16, camera_transform.elem, 0 );
        
        mat4 model_transform = mat4_identity();
        commandList->SetGraphicsRoot32BitConstants( 0, 16, model_transform.elem, 16 );
        
        TEXT_RENDER_OBJECT_LIST * objectList = &pass->textList;
        for( uint32 iObject = 0; iObject < objectList->nObjects; iObject++ ) {
            TEXT_RENDER_OBJECT object = objectList->object[ iObject ];
            
            commandList->SetGraphicsRoot32BitConstants( 1, 4, object.modColor.elem, 0 );
            
            commandList->DrawInstanced( object.nVertex, 1, object.bVertex, 0 );
        }
        objectList->nObjects = 0;
    }
}

int32 CALLBACK
WinMain( HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int32 windowShowCode ) {
    srand( ( uint32 )time( 0 ) );
    
    uint32 memory_size = 0;
    memory_size += sizeof( APP_STATE );
    memory_size += sizeof( PLATFORM  );
    memory_size += APP_permMemorySize;
    memory_size += APP_tempMemorySize;
    
    MEMORY platformMemory = {};
    platformMemory.size = memory_size;
    platformMemory.base = VirtualAlloc( 0, memory_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
    
    APP_STATE * appState = _pushType( &platformMemory, APP_STATE );
    PLATFORM  * platform = _pushType( &platformMemory, PLATFORM  );
    platform->targetSec  = 1.0f / APP_targetFPS;
    
    platform->permMemory = subArena( &platformMemory, APP_permMemorySize );
    platform->tempMemory = subArena( &platformMemory, APP_tempMemorySize );
    platform->readFile   = win32_readFile;
    platform->writeFile  = win32_writeFile;
    
    D3D12_EnableDebug();
    DIRECTX_12_DISPLAY_SETTINGS displaySettings = getDisplay( &platform->tempMemory );
    
    WNDCLASS windowClass = {};
    windowClass.lpfnWndProc   = win32_WindowProc;
    windowClass.hInstance     = instance;
    windowClass.lpszClassName = "WindowClass";
    windowClass.hCursor       = LoadCursor( 0, IDC_ARROW );
    
    if( RegisterClass( &windowClass ) ) {
        // uint32 window_flags = WS_OVERLAPPEDWINDOW; // Windowed
        uint32 window_flags = ( WS_VISIBLE | WS_EX_TOPMOST | WS_POPUP ); // Fullscreen
        HWND window = CreateWindowEx( 0, "WindowClass", "Caption", window_flags, 0, 0, displaySettings.width, displaySettings.height, 0, 0, instance, 0 );
        
        if( window ) {
            platform->renderer = initDirectX12( displaySettings, window );
            
            RENDERER     * renderer    = &platform->renderer;
            MEMORY       * permMemory  = &platform->permMemory;
            MEMORY       * tempMemory  = &platform->tempMemory;
            AUDIO_SYSTEM * audioSystem = &platform->audioSystem;
            
            createShader( renderer, displaySettings );
            
            ShowWindow( window, SW_SHOW );
            
            platform->isRunning = true;
            
            FONT font = loadFont( 16.0f );
            
            resetCommandList( renderer );
            
            RENDER_PASS * pass_game = &renderer->pass_game;
            RENDER_PASS * pass_ui   = &renderer->pass_ui;
            
            initPass( renderer, pass_game, permMemory, 1024, 1024, 512 );
            initPass( renderer, pass_ui,   permMemory,  128,  128, 512 );
            renderer->modelData[ modelID_line          ] = genLine         ( renderer );
            renderer->modelData[ modelID_rect          ] = genRect         ( renderer );
            renderer->modelData[ modelID_rectOutline   ] = genRectOutline  ( renderer );
            renderer->modelData[ modelID_circle        ] = genCircle       ( renderer );
            renderer->modelData[ modelID_circleOutline ] = genCircleOutline( renderer );
            
            { // create and upload white texture
                uint32 whiteTexture = 0xFFFFFFFF;
                uploadTexture( renderer, textureID_whiteTexture, 1, 1, &whiteTexture );
            }
            { // create and upload default debug texture
                uint32 defaultTexture[ 256 * 256 ] = {};
                
                uint32 width  = 256;
                uint32 height = 256;
                uint32 nTexel = width * height;
                //memset( defaultTexture, 0x000000FF, nTexel * sizeof( uint32 ) );
                for( uint32 iter = 0; iter < nTexel; iter++ ) {
                    uint32 iRow = ( iter / 8192 );
                    uint32 iCol = ( iter / 32 ) % 8;
                    
                    if( ( ( ( iRow + iCol ) % 2 ) == 0 ) ) {
                        defaultTexture[ iter ] = 0xFFFF00FF;
                    } else {
                        defaultTexture[ iter ] = 0xFF444444;
                    }
                }
                uploadTexture( renderer, textureID_defaultTexture, width, height, &defaultTexture );
            }
            
            uploadTexture( renderer, textureID_font, font.texture_width, font.texture_height, font.texture_data );
            
            executeCommandList( renderer );
            
            WIN32_AUDIO win32_audio = initAudio( window, audioSystem, permMemory );
            
            vec2 app_dim   = Vec2( ( flo32 )displaySettings.width, ( flo32 )displaySettings.height );
            vec2 app_halfDim = app_dim * 0.5f;
            rect app_bound = rectBLD( Vec2( 0.0f, 0.0f ), app_dim );
            
            appState->app_dim     = app_dim;
            appState->app_halfDim = app_halfDim;
            appState->app_bound   = app_bound;
            appState->font        = &font;
            
            vec2 debugSystem_basePos = getTL( app_bound ) + Vec2( 10.0f, -10.0f );
            // TODO: Currently, the debug system is only drawing stationary text after the UI is drawn, so I'm just attaching it to the end of the UI render pass. Otherwise, this would have its own render pass.
            globalVar_debugSystem.pass          = pass_ui;
            globalVar_debugSystem.font          = &font;
            globalVar_debugSystem.advanceHeight = font.advanceHeight;
            
            MOUSE_STATE    * mouse    = &platform->mouse;
            KEYBOARD_STATE * keyboard = &platform->keyboard;
            
            ShowCursor( false );
            while( platform->isRunning ) {
                
                //globalVar_debugSystem.atPos = debugSystem_basePos;
                globalVar_debugSystem.atPos = getBL( appState->play_bound ) + Vec2( 10.0f, 10.0f );
                
                win32_processPendingMessages( platform, window );
                
                resetCommandList( renderer );
                
                if( wasPressed( keyboard, keyCode_F4, ( KEYBOARD_FLAGS__ALT ) ) ) {
                    PostQuitMessage( 0 );
                }
                
                POINT mouse_position = {};
                GetCursorPos( &mouse_position );
                flo32 mouseX = ( flo32 )mouse_position.x;
                flo32 mouseY = ( flo32 )( displaySettings.height - 1 - mouse_position.y );
                
                mouse->prevPosition = mouse->position;
                mouse->position     = Vec2( mouseX, mouseY );
                
#if 0
                if( globalVar_debugSystem.log_isEnabled ) {
                    CONSOLE_VALUE( uint32, frame_counter );
                }
                if( wasPressed( keyboard, keyCode_l, KEYBOARD_FLAGS__CONTROL ) ) {
                    globalVar_debugSystem.log_isEnabled = !globalVar_debugSystem.log_isEnabled;
                }
#endif
                appState->frame_counter = frame_counter;
                
                appState->editor_isActive = false;
                if( appState->editor_isActive ) {
                    EDITOR_updateAndRender( appState, platform );
                } else {
                    APP_updateAndRender( appState, platform );
                    updateAudio( &win32_audio, audioSystem );
                }
                if( appState->resetMousePos ) {
                    int32 X = appState->mouse_baseX;
                    int32 Y = displaySettings.height - 1 - appState->mouse_baseY;
                    SetCursorPos( X, Y );
                }
                
                ID3D12GraphicsCommandList * commandList = renderer->commandList;
                
                ID3D12Resource * backBuffer = renderer->backBuffers[ renderer->currentBackBufferIndex ];
                D3D12_CPU_DESCRIPTOR_HANDLE RTV = CD3DX12_CPU_DESCRIPTOR_HANDLE( renderer->RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), renderer->currentBackBufferIndex, renderer->RTVDescriptorSize );
                D3D12_CPU_DESCRIPTOR_HANDLE DSV = renderer->DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
                
                D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition( backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET );
                commandList->ResourceBarrier( 1, &barrier );
                
                //flo32 color[ 4 ] = { 1.0f, 0.0f, 1.0f, 1.0f };
                flo32 color[ 4 ] = { 0.0f, 0.0f, 0.0f, 1.0f };
                commandList->ClearRenderTargetView( RTV, color, 0, 0 );
                commandList->ClearDepthStencilView( DSV, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, 0 );
                
                commandList->SetPipelineState        ( renderer->PSS_tri_depth_noBlend );
                commandList->SetGraphicsRootSignature( renderer->rootSignature );
                
                commandList->SetDescriptorHeaps( 1, &renderer->SRVHeap );
                
                flo32 width  = ( flo32 )displaySettings.width;
                flo32 height = ( flo32 )displaySettings.height;
                D3D12_VIEWPORT viewport = { 0.0f, 0.0f, width, height, 0.0f, 1.0f };
                D3D12_RECT     scissor  = { 0, 0, ( LONG )width, ( LONG )height };
                
                commandList->RSSetViewports   ( 1, &viewport );
                commandList->RSSetScissorRects( 1, &scissor  );
                
                commandList->OMSetRenderTargets( 1, &RTV, 0, &DSV );
                
                // render pass : game
                // render pass : ui
                
                BubbleSort( &pass_game->triObjectList );
                
                mat4 camera_transform = {};
                
                if( appState->editor_isActive ) {
                    EDITOR_STATE * editor = &appState->editorState;
                    
                    rect R = rectCD( editor->camera_pos, editor->camera_dim );
                    camera_transform = mat4_orthographic( R );
                } else {
                    rect play_bound = appState->play_bound;
                    vec2 center = getCenter( play_bound );
                    flo32 tLeft    = ( center.x -    app_bound.left ) / ( center.x -    play_bound.left );
                    flo32 tRight   = ( app_bound.right -   center.x ) / ( play_bound.right -   center.x );
                    flo32 tBottom  = ( center.y -  app_bound.bottom ) / ( center.y -  play_bound.bottom );
                    flo32 tTop     = ( app_bound.top -     center.y ) / ( play_bound.top -     center.y );
                    
                    rect cameraBound = appState->cameraBound;
                    vec2 cameraPos   = getCenter( cameraBound );
                    cameraBound.left   = cameraPos.x - ( cameraPos.x -    cameraBound.left ) * tLeft;
                    cameraBound.right  = cameraPos.x + ( cameraBound.right -   cameraPos.x ) * tRight;
                    cameraBound.bottom = cameraPos.y - ( cameraPos.y -  cameraBound.bottom ) * tBottom;
                    cameraBound.top    = cameraPos.y + ( cameraBound.top -     cameraPos.y ) * tTop;
                    
                    camera_transform = mat4_orthographic( cameraBound );
                    //camera_transform = mat4_orthographic( rectCD( getCenter( cameraBound ), getDim( cameraBound ) * 8.0f ) );
                    //camera_transform = mat4_orthographic( rectCD( getCenter( cameraBound ) + Vec2( getWidth( cameraBound ), -getHeight( cameraBound ) ) * 2.0f, getDim( cameraBound ) * 8.0f ) );
                }
                renderPass( renderer, pass_game, camera_transform );
                
                camera_transform = mat4_orthographic( appState->app_bound );
                renderPass( renderer, pass_ui,   camera_transform );
                
                renderer->textBuffer.nVertex = 0;
                
#if 0
                { // draw models
                    VERTEX1_BUFFER * buffer = &renderer->modelBuffer;
                    commandList->IASetVertexBuffers( 0, 1, &buffer->vertexView );
                    commandList->IASetIndexBuffer  ( &buffer->indexView );
                    
                    commandList->SetGraphicsRoot32BitConstants( 0, 16, camera_transform.elem, 0 );
                    
                    { // triangle models
                        RENDER_OBJECT_LIST * objectList = &renderer->triObjectList;
                        
                        commandList->SetPipelineState( renderer->PSS_tri_noDepth_blend );
                        commandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
                        for( uint32 iObject = 0; iObject < objectList->nObjects; iObject++ ) {
                            RENDER_OBJECT object    = objectList->object[ iObject ];
                            MODEL_DATA    modelData = renderer->modelData[ object.modelID ];
                            
                            commandList->SetGraphicsRoot32BitConstants( 0, 16, object.transform.elem, 16 );
                            
                            TEXTURE_ID textureID = textureID_defaultTexture;
                            if( renderer->texture_isLoaded[ object.textureID ] ) {
                                textureID = object.textureID;
                            }
                            D3D12_GPU_DESCRIPTOR_HANDLE handle = getGPUHandle( renderer->device, renderer->SRVHeap, textureID );
                            commandList->SetGraphicsRootDescriptorTable( 2, handle );
                            
                            commandList->SetGraphicsRoot32BitConstants( 1, 4, object.modColor.elem, 0 );
                            
                            commandList->DrawIndexedInstanced( modelData.nIndex, 1, modelData.bIndex, modelData.bVertex, 0 );
                        }
                        objectList->nObjects = 0;
                    } // END triangle models
                    
                    { // line models
                        RENDER_OBJECT_LIST * objectList = &renderer->lineObjectList;
                        
                        commandList->SetPipelineState( renderer->PSS_line_noDepth_blend );
                        commandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_LINESTRIP );
                        for( uint32 iObject = 0; iObject < objectList->nObjects; iObject++ ) {
                            RENDER_OBJECT object    = objectList->object[ iObject ];
                            MODEL_DATA    modelData = renderer->modelData[ object.modelID ];
                            
                            commandList->SetGraphicsRoot32BitConstants( 0, 16, object.transform.elem, 16 );
                            
                            Assert( object.textureID == textureID_whiteTexture );
                            D3D12_GPU_DESCRIPTOR_HANDLE handle = getGPUHandle( renderer->device, renderer->SRVHeap, object.textureID );
                            commandList->SetGraphicsRootDescriptorTable( 2, handle );
                            
                            commandList->SetGraphicsRoot32BitConstants( 1, 4, object.modColor.elem, 0 );
                            
                            commandList->DrawIndexedInstanced( modelData.nIndex, 1, modelData.bIndex, modelData.bVertex, 0 );
                        }
                        objectList->nObjects = 0;
                    } // END line models
                }
                
                /*
                { // draw lines in world space
                   commandList->SetPipelineState( renderer->PSS_line_noDepth_blend );
                   
                   mat4 camera_transform = genCameraTransform( &camera, app_dim );
                   commandList->SetGraphicsRoot32BitConstants( 0, 16, camera_transform.elem, 0 );
                   mat4 model_transform = mat4_identity();
                   commandList->SetGraphicsRoot32BitConstants( 0, 16, model_transform.elem, 16 );
                   
                   VERTEX1_BUFFER * buffer   = &renderer->lineBuffer_world;
                   buffer->vertexView.SizeInBytes = sizeof( VERTEX1 ) * buffer->nVertex;
                   commandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_LINELIST );
                   commandList->IASetVertexBuffers( 0, 1, &buffer->vertexView );
                   
                   D3D12_GPU_DESCRIPTOR_HANDLE handle = getGPUHandle( renderer->device, renderer->SRVHeap, textureID_whiteTexture );
                   commandList->SetGraphicsRootDescriptorTable( 2, handle );
                   
                   LINE_LIST      * lineList = &renderer->lineList_world;
                   for( uint32 iLine = 0; iLine < lineList->nLines; iLine++ ) {
                      LINE_OBJECT line = lineList->line[ iLine ];
                      
                      vec4 modColor = line.modColor;
                      modColor.rgb *= modColor.a;
                      
                      commandList->SetGraphicsRoot32BitConstants( 1, 4, modColor.elem, 0 );
                      commandList->DrawInstanced( line.nSegments * 2, 1, line.bVertex, 0 );
                   }
                }
                
                //addV2( dstr, Vec2( mouseX, mouseY ) );
                //write( debugSystem, dstr );
                //drawString( renderer, &font, str, getTL( app_bound ) + Vec2( 10.0f, -100.0f ) );
                
                { // draw quads on overlay
                   VERTEX1_BUFFER * buffer   = &renderer->quadBuffer_overlay;
                   QUAD_LIST      * quadList = &renderer->quadList_overlay;
                   commandList->SetPipelineState( renderer->PSS_noDepth_blend );
                   
                   mat4 camera_transform = mat4_orthographic( app_bound, 0.0f, 1.0f );
                   commandList->SetGraphicsRoot32BitConstants( 0, 16, camera_transform.elem, 0 );
                   mat4 model_transform = mat4_identity();
                   commandList->SetGraphicsRoot32BitConstants( 0, 16, model_transform.elem, 16 );
                   
                   buffer->vertexView.SizeInBytes = sizeof( VERTEX1 ) * buffer->nVertex;
                   commandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
                   commandList->IASetVertexBuffers( 0, 1, &buffer->vertexView );
                   
                   
                   for( uint32 iQuad = 0; iQuad < quadList->nQuads; iQuad++ ) {
                      QUAD_OBJECT quad = quadList->quad[ iQuad ];
                      
                      D3D12_GPU_DESCRIPTOR_HANDLE handle = getGPUHandle( renderer->device, renderer->SRVHeap, quad.textureID );
                      commandList->SetGraphicsRootDescriptorTable( 2, handle );
                      
                      commandList->SetGraphicsRoot32BitConstants( 1, 4, quad.modColor.elem, 0 );
                      
                      commandList->DrawInstanced( quad.nQuads * 6, 1, quad.bVertex, 0 );
                   }
                }
                
                { // draw lines on overlay
                   VERTEX1_BUFFER * buffer   = &renderer->lineBuffer_overlay;
                   LINE_LIST      * lineList = &renderer->lineList_overlay;
                   commandList->SetPipelineState( renderer->PSS_line_noDepth_noBlend );
                   
                   mat4 camera_transform = mat4_orthographic( app_bound, 0.0f, 1.0f );
                   commandList->SetGraphicsRoot32BitConstants( 0, 16, camera_transform.elem, 0 );
                   mat4 model_transform = mat4_identity();
                   commandList->SetGraphicsRoot32BitConstants( 0, 16, model_transform.elem, 16 );
                   
                   buffer->vertexView.SizeInBytes = sizeof( VERTEX1 ) * buffer->nVertex;
                   commandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_LINELIST );
                   commandList->IASetVertexBuffers( 0, 1, &buffer->vertexView );
                   
                   D3D12_GPU_DESCRIPTOR_HANDLE handle = getGPUHandle( renderer->device, renderer->SRVHeap, textureID_whiteTexture );
                   commandList->SetGraphicsRootDescriptorTable( 2, handle );
                   
                   for( uint32 iLine = 0; iLine < lineList->nLines; iLine++ ) {
                      LINE_OBJECT line = lineList->line[ iLine ];
                      
                      commandList->SetGraphicsRoot32BitConstants( 1, 4, line.modColor.elem, 0 );
                      
                      commandList->DrawInstanced( line.nSegments * 2, 1, line.bVertex, 0 );
                   }
                }
                */
                
                { // draw overlay text
                    commandList->SetPipelineState( renderer->PSS_tri_noDepth_blend );
                    
                    VERTEX1_BUFFER * buffer = &renderer->textBuffer;
                    buffer->vertexView.SizeInBytes = sizeof( VERTEX1 ) * buffer->nVertex;
                    
                    commandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
                    commandList->IASetVertexBuffers( 0, 1, &buffer->vertexView );
                    
                    D3D12_GPU_DESCRIPTOR_HANDLE handle = getGPUHandle( renderer->device, renderer->SRVHeap, textureID_font );
                    commandList->SetGraphicsRootDescriptorTable( 2, handle );
                    
                    mat4 camera_transform = mat4_orthographic( app_bound );
                    commandList->SetGraphicsRoot32BitConstants( 0, 16, camera_transform.elem, 0 );
                    
                    mat4 model_transform = mat4_identity();
                    commandList->SetGraphicsRoot32BitConstants( 0, 16, model_transform.elem, 16 );
                    
                    TEXT_RENDER_OBJECT_LIST * objectList = &renderer->textList;
                    for( uint32 iObject = 0; iObject < objectList->nObjects; iObject++ ) {
                        TEXT_RENDER_OBJECT object = objectList->object[ iObject ];
                        
                        commandList->SetGraphicsRoot32BitConstants( 1, 4, object.modColor.elem, 0 );
                        
                        commandList->DrawInstanced( object.nVertex, 1, object.bVertex, 0 );
                    }
                    objectList->nObjects = 0;
                    renderer->textBuffer.nVertex = 0;
                }
#endif
                present( renderer );
                frame_counter++;
            }
        }
    }
    
    return 0;
}