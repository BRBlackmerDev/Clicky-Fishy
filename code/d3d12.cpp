
//----------
// TODO: DISPLAY_SETTINGS : clean up
// TODO: VERTEX1 : how should I organize this? This data is dependent on the shader.
//----------
#include <dxgi1_4.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <d3dcompiler.h>

struct DIRECTX_12_DISPLAY_SETTINGS {
    IDXGIFactory2 * dxgiFactory;
    IDXGIAdapter  * adapter;
    
    DXGI_FORMAT       format;
    D3D_FEATURE_LEVEL featureLevel;
    
    uint32 width;
    uint32 height;
    uint32 refreshRate_numerator;
    uint32 refreshRate_denominator;
};

struct VERTEX1 {
    vec2 position;
    vec2 texCoord;
};

internal VERTEX1
Vertex1( vec2 pos, vec2 tex ) {
    VERTEX1 result = {};
    result.position = pos;
    result.texCoord = tex;
    return result;
}

struct VERTEX1_BUFFER {
    D3D12_VERTEX_BUFFER_VIEW vertexView;
    D3D12_INDEX_BUFFER_VIEW   indexView;
    
    //#define VERTEX_COUNT  ( 131072 )
    //#define  INDEX_COUNT  ( 262144 )
    uint32 maxVertex;
    uint32 maxIndex;
    uint32 nVertex;
    uint32 nIndex;
    VERTEX1 * vertex_start;
    uint32  *  index_start;
    
};

internal VERTEX1_BUFFER
Vertex1Buffer( ID3D12Device2 * device, uint32 maxVertex, uint32 maxIndex = 0 ) {
    VERTEX1_BUFFER result = {};
    result.maxVertex = maxVertex;
    result.maxIndex  = maxIndex;
    
    uint32 vertex_size = sizeof( VERTEX1 );
    uint32  index_size = sizeof( uint32  );
    uint32 bufferSize = maxVertex * vertex_size + maxIndex * index_size;
    
    ID3D12Resource * uploadBuffer = 0;
    
    D3D12_HEAP_PROPERTIES prop_dest = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD );
    D3D12_RESOURCE_DESC   desc_dest = CD3DX12_RESOURCE_DESC::Buffer( bufferSize );
    
    HRESULT status = device->CreateCommittedResource( &prop_dest, D3D12_HEAP_FLAG_NONE, &desc_dest, D3D12_RESOURCE_STATE_GENERIC_READ, 0, IID_PPV_ARGS( &uploadBuffer ) );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to create the dest resource to upload the data!\n" );
    }
    
    // Map the buffer
    void * data = 0;
    D3D12_RANGE range = {};
    uploadBuffer->Map( 0, &range, &data ); 
    
    uint8 * data_start = ( uint8 * )data;
    uint8 * data_at    = data_start;
    
    data_at += ( ( uint64 )data_at % sizeof( flo32 ) );
    result.vertex_start  = ( VERTEX1 * )data_at;
    uint32 vertex_offset = ( uint32 )( data_at - data_start );
    data_at += ( maxVertex * vertex_size );
    
    result.vertexView.BufferLocation = uploadBuffer->GetGPUVirtualAddress() + vertex_offset;
    result.vertexView.StrideInBytes  = vertex_size;
    
    if( maxIndex > 0 ) {
        data_at += ( ( uint64 )data_at % sizeof( uint32 ) );
        result.index_start  = ( uint32 * )data_at;
        uint32 index_offset = ( uint32 )( data_at - data_start );
        data_at += ( maxIndex * index_size );
        
        result.indexView.BufferLocation = uploadBuffer->GetGPUVirtualAddress() + index_offset;
        result.indexView.Format         = DXGI_FORMAT_R32_UINT;
    }
    
    return result;
}

enum TEXTURE_ID {
    textureID_defaultTexture,
    textureID_whiteTexture,
    textureID_font,
    
    textureID_home_background,
    textureID_home_storage,
    textureID_home_storage_x,
    textureID_home_storage_cursor,
    textureID_inventoryIcon_open,
    textureID_inventoryIcon_close,
    
    textureID_background_left,
    textureID_background_front,
    textureID_background_right,
    textureID_background_back,
    
    textureID_background_cave_exit,
    textureID_background_sandToCave,
    textureID_background_caveWall,
    textureID_background_cave_rock,
    textureID_background_crate,
    
    textureID_breakable_plank,
    
    textureID_exitArrow,
    
    textureID_cave_wall01,
    
    textureID_player_idle,
    textureID_player_bite_open,
    textureID_player_bite_close,
    textureID_player_parry,
    textureID_player_stunned,
    textureID_player_bellyUp,
    textureID_healAttempts,
    
    textureID_entity_start,
    
    textureID_fish,
    textureID_fish_eye,
    
    textureID_goldfish,
    
    textureID_dogfish_idle,
    textureID_dogfish_dying,
    textureID_dogfish_stunned,
    textureID_dogfish_biting,
    textureID_dogfish_lungePrep,
    textureID_dogfish_lunging,
    textureID_dogfish_shakingFree,
    textureID_dogfish_injury01,
    textureID_dogfish_injury02,
    textureID_dogfish_injury03,
    
    textureID_swordfish_idle,
    textureID_swordfish_head,
    textureID_swordfish_tail,
    textureID_swordfish_stunnedByBite,
    textureID_swordfish_stunned_hitFrame,
    textureID_swordfish_stunned_lo,
    textureID_swordfish_stabbing,
    textureID_swordfish_stabbing_background,
    textureID_swordfish_angry,
    textureID_swordfish_blinkTransition,
    textureID_swordfish_dying,
    textureID_swordfish_slash,
    textureID_swordfish_slashing_up,
    textureID_swordfish_slashing_down,
    
    textureID_urchin_idle,
    textureID_urchinSpine_idle,
    
    textureID_archer_idle,
    textureID_archer_stunned,
    textureID_bubble_idle,
    textureID_bubblet_idle,
    
    textureID_parrotfish_idle,
    textureID_parrotfish_stunned,
    textureID_parrotfish_headbutt,
    textureID_parrotfish_windup,
    
    textureID_turtle_idle,
    textureID_turtle_shadow,
    
    textureID_smTurtle_idle,
    
    textureID_cage_idle,
    textureID_cage_fish,
    
    textureID_net_idle,
    textureID_net_fish,
    textureID_net_block,
    
    textureID_tower_idle,
    textureID_tower_crumble,
    
    textureID_peasant_idle,
    textureID_peasant_stunned,
    
    textureID_entity_end,
    
    textureID_explosion01,
    textureID_explosion02,
    textureID_explosion03,
    textureID_smExplosion01,
    
    textureID_count,
};

enum MODEL_ID {
    modelID_line,
    modelID_rect,
    modelID_rectOutline,
    modelID_circle,
    modelID_circleOutline,
    
    modelID_count,
};

struct RENDER_OBJECT {
    flo32      depth;
    MODEL_ID   modelID;
    TEXTURE_ID textureID;
    mat4       transform;
    vec4       modColor;
};

struct RENDER_OBJECT_LIST {
    uint32 nObjects;
    uint32 maxObjects;
    RENDER_OBJECT * object;
};

struct TEXT_RENDER_OBJECT {
    uint32 bVertex;
    uint32 nVertex;
    vec4   modColor;
};

struct TEXT_RENDER_OBJECT_LIST {
    uint32 nObjects;
    uint32 maxObjects;
    TEXT_RENDER_OBJECT * object;
};

struct MODEL_DATA {
    uint32 nIndex;
    uint32 bIndex;
    uint32 bVertex;
};

struct TEXTURE_RESOURCE {
    ID3D12Resource * buffer;
    ID3D12Resource * buffer_intr;
};

struct DIRECTX_12_RENDER_PASS {
    VERTEX1_BUFFER * textBuffer;
    VERTEX1_BUFFER * modelBuffer;
    
    TEXT_RENDER_OBJECT_LIST       textList;
    RENDER_OBJECT_LIST       triObjectList;
    RENDER_OBJECT_LIST      lineObjectList;
};

internal void
BubbleSort( RENDER_OBJECT_LIST * list ) {
    if( list->nObjects > 1 ) {
        boo32 doSort = true;
        while( doSort ) {
            doSort = false;
            
            for( uint32 iObj = 0; iObj < ( list->nObjects - 1 ); iObj++ ) {
                if( list->object[ iObj ].depth < list->object[ iObj + 1 ].depth ) {
                    _swap( RENDER_OBJECT, list->object[ iObj ], list->object[ iObj + 1 ] );
                    doSort = true;
                }
            }
        }
    }
}

struct DIRECTX_12_RENDERER {
    HWND window_handle;
    
    boo32 isInitialized;
    
    ID3D12Device2             * device;
    IDXGISwapChain4           * swapChain;
    ID3D12DescriptorHeap      * RTVDescriptorHeap;
    ID3D12DescriptorHeap      * DSVDescriptorHeap;
    ID3D12DescriptorHeap      * SRVHeap;
    uint32                      RTVDescriptorSize;
    uint32                      currentBackBufferIndex;
    
#define DIRECTX_12_RENDERER__N_SWAP_CHAIN_BUFFERS  ( 3 )
    ID3D12Resource            * backBuffers      [ DIRECTX_12_RENDERER__N_SWAP_CHAIN_BUFFERS ];
    
    ID3D12CommandQueue        * commandQueue;
    ID3D12CommandAllocator    * commandAllocator;
    ID3D12GraphicsCommandList * commandList;
    
    ID3D12Fence               * fence;
    HANDLE                      fenceEvent;
    uint64                      fenceValue;
    
    ID3D12PipelineState * PSS_tri_depth_noBlend;
    ID3D12PipelineState * PSS_tri_noDepth_blend;
    ID3D12PipelineState * PSS_line_noDepth_noBlend;
    ID3D12PipelineState * PSS_line_noDepth_blend;
    ID3D12RootSignature * rootSignature;
    
    boo32            texture_isLoaded[ textureID_count ];
    TEXTURE_RESOURCE texture_resource[ textureID_count ];
    
    DIRECTX_12_RENDER_PASS pass_game;
    DIRECTX_12_RENDER_PASS pass_ui;
    //TEXT_RENDER_OBJECT_LIST textList;
    //RENDER_OBJECT_LIST triObjectList;
    //RENDER_OBJECT_LIST lineObjectList;
    
    MODEL_DATA     modelData[ modelID_count ];
    
    VERTEX1_BUFFER textBuffer;
    VERTEX1_BUFFER modelBuffer;
};

internal void
D3D12_EnableDebug() {
    //---------- Enable Debug Layer
#if DEBUG_BUILD
    ID3D12Debug * debugInterface = 0;
    HRESULT status = D3D12GetDebugInterface( IID_PPV_ARGS( &debugInterface ) );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to enable debug interface!\n" );
    }
    debugInterface->EnableDebugLayer();
#endif // DEBUG_BUILD
    //---------- END Enable Debug Layer
}

internal DIRECTX_12_DISPLAY_SETTINGS
getDisplay( MEMORY * tempMemory ) {
    //---------- Query DXGI Adapter
    IDXGIFactory2 * dxgiFactory = 0;
    
    uint32 factoryFlags = 0;
#if DEBUG_BUILD
    factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
    
    // NOTE: IDXGIFactory contains all the functions I need, but IDXGIFactory2 is required to retrieve a debug factory.
    HRESULT status = CreateDXGIFactory2( factoryFlags, IID_PPV_ARGS( &dxgiFactory ) );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to create the DXGI factory!\n" );
    }
    
    // NOTE: If I use IDXGIFactory to create the swap chain, how do I associate the swap chain with the window handle? Does it need to be associated? The window handle is in the swap chain description, passed during swap chain creation.
    
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    
    IDXGIAdapter * adapter_temp = 0;
    IDXGIAdapter * adapter      = 0;
    
    uint32 iAdapter  = 0;
    uint64 maxMemory = 0;
    boo32  enumerateAdapters = true;
    while( enumerateAdapters ) {
        status = dxgiFactory->EnumAdapters( iAdapter, &adapter_temp );
        if( SUCCEEDED( status ) ) {
            status = D3D12CreateDevice( adapter_temp, featureLevel, __uuidof( ID3D12Device ), 0 );
            if( SUCCEEDED( status ) ) {
                DXGI_ADAPTER_DESC adapterDesc = {};
                adapter_temp->GetDesc( &adapterDesc );
                
                if( adapterDesc.DedicatedVideoMemory > maxMemory ) {
                    adapter   = adapter_temp;
                    maxMemory = adapterDesc.DedicatedVideoMemory;
                }
            }
        } else {
            enumerateAdapters = false;
        }
        
        iAdapter++;
    }
    
    if( !adapter ) {
        OutputDebugString( "ERROR! Failed to enumerate any display adapters!\n" );
    }
    
    
    IDXGIOutput * adapterOutput = 0;
    status = adapter->EnumOutputs( 0, &adapterOutput );
    if( FAILED(  status ) ) {
        OutputDebugString( "ERROR! Failed to enumerate adapter outputs!\n" );
    }
    
    uint32 nDisplayModes = 0;
    DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
    status = adapterOutput->GetDisplayModeList( format, 0, &nDisplayModes, 0 );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to get the count of display modes!\n" );
    }
    
    // TODO: Use tempMemory for this list!
#define DISPLAY_MODE_LIST__MAX_COUNT  ( 128 )
    Assert( nDisplayModes <= DISPLAY_MODE_LIST__MAX_COUNT );
    
    DXGI_MODE_DESC * displayModeList = _pushArray( tempMemory, DXGI_MODE_DESC, nDisplayModes );
    status = adapterOutput->GetDisplayModeList( format, 0, &nDisplayModes, displayModeList );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to get the list of display modes!\n" );
    }
    
    uint32 maxWidth  = 0;
    uint32 maxHeight = 0;
    flo32  maxFPS    = 0.0f;
    int32 displayMode_index = -1;
    // This will return the display mode with the highest resolution and highest framerate. A feature for later would be to list the possible display modes to the user and have them select one.
    for( int32 iDisplayMode = nDisplayModes - 1; iDisplayMode > 0; iDisplayMode-- ) {
        DXGI_MODE_DESC * displayMode = displayModeList + iDisplayMode;
        
        DXGI_RATIONAL refresh = displayMode->RefreshRate;
        flo32 fps = ( flo32 )refresh.Numerator / ( flo32 )refresh.Denominator;
        
        boo32 case0 = ( displayMode->Width > maxWidth );
        boo32 case1 = ( displayMode->Width == maxWidth ) && ( displayMode->Height > maxHeight );
        boo32 case2 =  ( displayMode->Width == maxWidth ) && ( displayMode->Height == maxHeight ) && ( fps > maxFPS );
        // NOTE: Case2 will prefer 60.0fps over 59.94fps. There isn't any difference between the two, as far as I can tell.
        
        if( ( case0 ) || ( case1 ) || ( case2 ) ) {
            maxWidth  = displayMode->Width;
            maxHeight = displayMode->Height;
            maxFPS    = fps;
            displayMode_index = iDisplayMode;
        }
    }
    
    DIRECTX_12_DISPLAY_SETTINGS result = {};
    if( displayMode_index > -1 ) {
        DXGI_MODE_DESC * displayMode = displayModeList + displayMode_index;
        
        result.dxgiFactory = dxgiFactory;
        result.adapter     = adapter;
        
        result.format       = format;
        result.featureLevel = featureLevel;
        
        result.width  = displayMode->Width;
        result.height = displayMode->Height;
        result.refreshRate_numerator   = displayMode->RefreshRate.Numerator;
        result.refreshRate_denominator = displayMode->RefreshRate.Denominator;
    } else {
        OutputDebugString( "ERROR! Failed to find a valid display mode!\n" );
    }
    //---------- END Query DXGI Adapter
    
    _popArray( tempMemory, DXGI_MODE_DESC, nDisplayModes );
    
    return result;
}

internal DIRECTX_12_RENDERER
initDirectX12( DIRECTX_12_DISPLAY_SETTINGS displaySettings, HWND window_handle ) {
    HRESULT status = 0;
    
    //---------- Create DirectX12 Device
    ID3D12Device2 * device = 0;
    status = D3D12CreateDevice( displaySettings.adapter, displaySettings.featureLevel, IID_PPV_ARGS( &device ) );
    // NOTE: D3D12CreateDevice() is throwing the following exception: Exception thrown at 0x00007FFF65F9A799 in fish.exe: Microsoft C++ exception: _com_error at memory location 0x00000058964F7960.
    // 0x00007FFF65F9A799 is an instruction address outside my code.
    // 0x00000058964F7960 is a stack address (i think) from within D3D12CreateDevice()
    // These addresses are close to my current instruction and stack address, but coming after. This leads me to believe the error is coming from inside D3D12CreateDevice(), but I have not encountered this kind of thing before and I don't have access to the D3D12CreateDevice() source.
    // I believe this error only started occurring after I updated my display drivers.
    // This exception is not preventing the device from being create, so I will just ignore it until I have more experience.
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to create the D3D12 device!\n" );
    }
    //---------- END Create DirectX12 Device
    
    //---------- Enable Debug Messages
#if DEBUG_BUILD
    //#if 0
    ID3D12InfoQueue * infoQueue = 0;
    status = device->QueryInterface( __uuidof( ID3D12InfoQueue ), ( void ** )&infoQueue );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to query the Info Queue interface from the D3D12 device!\n" );
    }
    
    infoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE );
    infoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_ERROR,      TRUE );
    infoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_WARNING,    TRUE );
    
    // Suppress whole categories of messages
    //D3D12_MESSAGE_CATEGORY Categories[] = {};
    
    // Suppress messages based on their severity level
    D3D12_MESSAGE_SEVERITY Severities[] = {
        D3D12_MESSAGE_SEVERITY_INFO
    };
    
    // Suppress individual messages by their ID
    D3D12_MESSAGE_ID DenyIds[] = {
        D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
        D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
        D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
    };
    
    D3D12_INFO_QUEUE_FILTER NewFilter = {};
    // NewFilter.DenyList.NumCategories = _arrayCount( Categories );
    // NewFilter.DenyList.pCategoryList = Categories;
    NewFilter.DenyList.NumSeverities  = _arrayCount( Severities );
    NewFilter.DenyList.pSeverityList  = Severities;
    NewFilter.DenyList.NumIDs         = _arrayCount( DenyIds );
    NewFilter.DenyList.pIDList        = DenyIds;
    
    status = infoQueue->PushStorageFilter( &NewFilter );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to push the error filters onto the info queue!\n" );
    }
#endif // DEBUG_BUILD
    //---------- END Enable Debug Messages
    
    //---------- Create Command Queue
    ID3D12CommandQueue * commandQueue = 0;
    
    D3D12_COMMAND_QUEUE_DESC commandQueue_desc = {};
    commandQueue_desc.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT;
    commandQueue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    commandQueue_desc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
    commandQueue_desc.NodeMask = 0;
    
    status = device->CreateCommandQueue( &commandQueue_desc, IID_PPV_ARGS( &commandQueue ) );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to create the command queue!\n" );
    }
    //---------- END Create Command Queue
    
    //---------- Create Swap Chain
#define DIRECTX_12_RENDERER__N_SWAP_CHAIN_BUFFERS  ( 3 )
    uint32 nBuffers = DIRECTX_12_RENDERER__N_SWAP_CHAIN_BUFFERS;
    
    DXGI_SWAP_CHAIN_DESC swapChain_desc = {};
    swapChain_desc.BufferDesc.Width  = displaySettings.width;
    swapChain_desc.BufferDesc.Height = displaySettings.height;
    swapChain_desc.BufferDesc.Format = displaySettings.format;
    swapChain_desc.BufferDesc.RefreshRate.Numerator   = displaySettings.refreshRate_numerator;
    swapChain_desc.BufferDesc.RefreshRate.Denominator = displaySettings.refreshRate_denominator;
    swapChain_desc.SampleDesc   = { 1, 0 };
    swapChain_desc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChain_desc.BufferCount  = nBuffers; 
    swapChain_desc.OutputWindow = window_handle;
    swapChain_desc.Windowed     = true;
    swapChain_desc.SwapEffect   = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChain_desc.Flags        = 0;
    // NOTE: DXGI_SWAP_CHAIN_DESC1 has options to specify the scaling mode (if the size of the backbuffer does not match the output target) and the alpha mode. Both of these options would be good to investigate in the future.
    
    IDXGISwapChain  * swapChain_temp = 0;
    IDXGISwapChain4 * swapChain      = 0;
    status = displaySettings.dxgiFactory->CreateSwapChain( commandQueue, &swapChain_desc, &swapChain_temp );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to create the swap chain from the window handle!\n" );
    }
    
    // Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
    // will be handled manually.
    status = displaySettings.dxgiFactory->MakeWindowAssociation( window_handle, DXGI_MWA_NO_ALT_ENTER );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to disable DXGI handling of Alt+Enter fullscreen toggle!" );
    }
    
    status = swapChain_temp->QueryInterface( __uuidof( IDXGISwapChain4 ), ( void ** )&swapChain );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to query the DXGISwapChain4 interface from IDXGISwapChain1!\n" );
    }
    //---------- END Create Swap Chain
    
    //---------- Create Descriptor Heap
    ID3D12DescriptorHeap *  descriptorHeap = 0;
    
    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeap_desc = {};
    descriptorHeap_desc.NumDescriptors = nBuffers;
    descriptorHeap_desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    
    status = device->CreateDescriptorHeap( &descriptorHeap_desc, IID_PPV_ARGS( &descriptorHeap ) );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to create the descriptor heap!\n" );
    }
    //---------- END Create Descriptor Heap
    
    //---------- Create Render Target Views
    ID3D12Resource * backBuffers[ DIRECTX_12_RENDERER__N_SWAP_CHAIN_BUFFERS ] = {};
    
    uint32 rtvDescriptorSize = device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle( descriptorHeap->GetCPUDescriptorHandleForHeapStart() );
    
    for( uint32 iRenderTarget = 0; iRenderTarget < nBuffers; iRenderTarget++ ) {
        ID3D12Resource * backBuffer = 0;
        status = swapChain->GetBuffer( iRenderTarget, IID_PPV_ARGS( &backBuffer ) );
        if( FAILED( status ) ) {
            OutputDebugString( "ERROR! Failed to get the Ith buffer from the swap chain!\n" );
        }
        
        device->CreateRenderTargetView( backBuffer, 0, rtvHandle );
        backBuffers[ iRenderTarget ] = backBuffer;
        
        rtvHandle.Offset( rtvDescriptorSize );
    }
    //---------- END Create Render Target Views
    
    //---------- Create Command Allocator
    ID3D12CommandAllocator * commandAllocator = 0;
    status = device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &commandAllocator ) );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to create the command allocator for the command list!\n" );
    }
    //---------- END Create Command Allocator
    
    //---------- Create Command List
    ID3D12GraphicsCommandList * commandList = 0;
    status = device->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, 0, IID_PPV_ARGS( &commandList ) );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to create the command list!\n" );
    }
    
    status = commandList->Close();
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to close the command list for use!\n" );
    }
    //---------- END Create Command List
    
    //---------- Create Fence
    ID3D12Fence * fence = 0;
    status = device->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &fence ) );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to create the fence object!\n" );
    }
    //---------- END Create Fence
    
    //---------- Create Fence Event
    HANDLE fenceEvent = CreateEvent( 0, 0, 0, 0 );
    if( !fenceEvent ) {
        OutputDebugString( "ERROR! Failed to create the wait event for the fence!\n" );
    }
    //---------- END Create Fence
    
    DIRECTX_12_RENDERER result = {};
    result.window_handle = window_handle;
    
    result.isInitialized = true;
    
    result.device                 = device;
    result.swapChain              = swapChain;
    result.RTVDescriptorHeap      = descriptorHeap;
    result.RTVDescriptorSize      = rtvDescriptorSize;
    result.currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
    
    for( uint32 iRenderTarget = 0; iRenderTarget < nBuffers; iRenderTarget++ ) {
        result.backBuffers      [ iRenderTarget ] = backBuffers      [ iRenderTarget ];
    }
    
    result.commandQueue     = commandQueue;
    result.commandAllocator = commandAllocator;
    result.commandList      = commandList;
    
    result.fence      = fence;
    result.fenceEvent = fenceEvent;
    
    return result;
}

internal void
clear( DIRECTX_12_RENDERER * renderer, flo32 * color ) {
    uint32 index = renderer->currentBackBufferIndex;
    ID3D12Resource         * backBuffer       = renderer->backBuffers[ index ];
    ID3D12CommandAllocator * commandAllocator = renderer->commandAllocator;
    
    commandAllocator->Reset();
    renderer->commandList->Reset( commandAllocator, 0 );
    
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition( backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET );
    
    renderer->commandList->ResourceBarrier( 1, &barrier );
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv( renderer->RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
                                      renderer->currentBackBufferIndex, renderer->RTVDescriptorSize );
    
    renderer->commandList->ClearRenderTargetView( rtv, color, 0, 0 );
}

internal void
Signal( ID3D12CommandQueue * commandQueue, ID3D12Fence * fence, uint64 * fenceValue ) {
    uint64 fenceValueForSignal = ++( *fenceValue );
    
    HRESULT status = commandQueue->Signal( fence, fenceValueForSignal );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to issue the signal command for this fence!\n" );
    }
}

internal void
WaitForFenceValue( ID3D12Fence * fence, uint64 fenceValue, HANDLE fenceEvent,
                  uint32 milliseconds = 0xFFFFFFFF ) {
    if( fence->GetCompletedValue() < fenceValue ) {
        HRESULT status = fence->SetEventOnCompletion( fenceValue, fenceEvent );
        if( FAILED( status ) ) {
            OutputDebugString( "ERROR! Failed to associate the fence event with the fence for waiting!\n" );
        }
        WaitForSingleObject( fenceEvent, ( DWORD )milliseconds );
    }
}

internal void
present( DIRECTX_12_RENDERER * renderer ) {
    uint32 index = renderer->currentBackBufferIndex;
    ID3D12Resource * backBuffer = renderer->backBuffers[ index ];
    
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition( backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    renderer->commandList->ResourceBarrier( 1, &barrier );
    
    HRESULT status = renderer->commandList->Close();
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to close the command list for rendering!\n" );
    }
    
    ID3D12CommandList * const commandLists[] = { renderer->commandList };
    renderer->commandQueue->ExecuteCommandLists( 1, commandLists );
    Signal( renderer->commandQueue, renderer->fence, &renderer->fenceValue );
    
    status = renderer->swapChain->Present( 1, 0 );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Renderer failed when presenting the render view!\n" );
    }
    
    renderer->currentBackBufferIndex = renderer->swapChain->GetCurrentBackBufferIndex();
    
    WaitForFenceValue( renderer->fence, renderer->fenceValue, renderer->fenceEvent );
}

internal void
updateBufferResource( DIRECTX_12_RENDERER * renderer, ID3D12Resource ** dest, ID3D12Resource ** intr, uint32 nElem, uint32 elemSize, void * data ) {
    if( data ) {
        ID3D12Device2 * device = renderer->device;
        
        uint32 bufferSize = nElem * elemSize;
        
        D3D12_HEAP_PROPERTIES prop_dest = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_DEFAULT );
        D3D12_HEAP_PROPERTIES prop_intr = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD  );
        D3D12_RESOURCE_DESC   desc_dest = CD3DX12_RESOURCE_DESC::Buffer( bufferSize );
        D3D12_RESOURCE_DESC   desc_intr = CD3DX12_RESOURCE_DESC::Buffer( bufferSize );
        
        HRESULT status = device->CreateCommittedResource( &prop_dest, D3D12_HEAP_FLAG_NONE, &desc_dest, D3D12_RESOURCE_STATE_COPY_DEST, 0, IID_PPV_ARGS( dest ) );
        if( FAILED( status ) ) {
            OutputDebugString( "ERROR! Failed to create the dest resource to upload the data!\n" );
        }
        
        status = device->CreateCommittedResource( &prop_intr, D3D12_HEAP_FLAG_NONE, &desc_intr, D3D12_RESOURCE_STATE_GENERIC_READ, 0, IID_PPV_ARGS( intr ) );
        if( FAILED( status ) ) {
            OutputDebugString( "ERROR! Failed to create the Intermediate Resource to upload the Buffer Data!\n" );
        }
        
        D3D12_SUBRESOURCE_DATA subresourceData = {};
        subresourceData.pData      = data;
        subresourceData.RowPitch   = bufferSize;
        subresourceData.SlicePitch = subresourceData.RowPitch;
        
        UpdateSubresources( renderer->commandList, *dest, *intr, 0, 0, 1, &subresourceData );
    }
}

internal D3D12_GPU_DESCRIPTOR_HANDLE
getGPUHandle( ID3D12Device2 * device, ID3D12DescriptorHeap * heap, uint32 slotIndex ) {
    uint32 handleSize = device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
    CD3DX12_GPU_DESCRIPTOR_HANDLE result( heap->GetGPUDescriptorHandleForHeapStart() );
    for( uint32 iSlot = 0; iSlot < slotIndex; iSlot++ ) {
        result.Offset( handleSize );
    }
    return result;
}

internal D3D12_CPU_DESCRIPTOR_HANDLE
getCPUHandle( ID3D12Device2 * device, ID3D12DescriptorHeap * heap, uint32 slotIndex ) {
    uint32 handleSize = device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
    CD3DX12_CPU_DESCRIPTOR_HANDLE result( heap->GetCPUDescriptorHandleForHeapStart() );
    for( uint32 iSlot = 0; iSlot < slotIndex; iSlot++ ) {
        result.Offset( handleSize );
    }
    return result;
}

internal void
uploadTexture( DIRECTX_12_RENDERER * renderer, TEXTURE_ID textureID, uint32 width, uint32 height, void * data ) {
    TEXTURE_RESOURCE * resource = renderer->texture_resource + textureID;
    if( data ) {
        ID3D12Device2 * device = renderer->device;
        
        if( resource->buffer ) {
            D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition( resource->buffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST );
            renderer->commandList->ResourceBarrier( 1, &barrier );
        } else {
            ID3D12Resource * textureBuffer_intr = 0;
            ID3D12Resource * textureBuffer      = 0;
            
            D3D12_RESOURCE_DESC textureDesc = {};
            textureDesc.MipLevels = 1;
            textureDesc.Format    = DXGI_FORMAT_R8G8B8A8_UNORM;
            textureDesc.Width     = width;
            textureDesc.Height    = height;
            textureDesc.Flags     = D3D12_RESOURCE_FLAG_NONE;
            textureDesc.DepthOrArraySize   = 1;
            textureDesc.SampleDesc.Count   = 1;
            textureDesc.SampleDesc.Quality = 0;
            textureDesc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            
            D3D12_HEAP_PROPERTIES prop_dest = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_DEFAULT );
            HRESULT status = device->CreateCommittedResource( &prop_dest, D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, 0, IID_PPV_ARGS( &textureBuffer ) );
            if( FAILED( status ) ) {
                OutputDebugString( "ERROR! Failed to create the dest resource to upload the texture data!\n" );
            }
            
            uint64 bufferSize = GetRequiredIntermediateSize( textureBuffer, 0, 1 );
            
            D3D12_HEAP_PROPERTIES prop_intr = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD  );
            D3D12_RESOURCE_DESC   desc_intr = CD3DX12_RESOURCE_DESC::Buffer( bufferSize );
            status = device->CreateCommittedResource( &prop_intr, D3D12_HEAP_FLAG_NONE, &desc_intr, D3D12_RESOURCE_STATE_GENERIC_READ, 0, IID_PPV_ARGS( &textureBuffer_intr ) );
            if( FAILED( status ) ) {
                OutputDebugString( "ERROR! Failed to create the intermediate resource to upload the texture data!\n" );
            }
            
            resource->buffer      = textureBuffer;
            resource->buffer_intr = textureBuffer_intr;
        }
        
        D3D12_SUBRESOURCE_DATA subresourceData = {};
        subresourceData.pData      = data;
        subresourceData.RowPitch   = width  * sizeof( uint32 );
        subresourceData.SlicePitch = height * width * sizeof( uint32 );
        
        UpdateSubresources( renderer->commandList, resource->buffer, resource->buffer_intr, 0, 0, 1, &subresourceData );
        
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition( resource->buffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE );
        renderer->commandList->ResourceBarrier( 1, &barrier );
        
        D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
        SRVDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
        SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        SRVDesc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
        SRVDesc.Texture2D.MipLevels     = 1;
        
        D3D12_CPU_DESCRIPTOR_HANDLE handle = getCPUHandle( renderer->device, renderer->SRVHeap, ( uint32 )textureID );
        renderer->device->CreateShaderResourceView( resource->buffer, &SRVDesc, handle );
        renderer->texture_isLoaded[ textureID ] = true;
    }
}

internal CD3DX12_BLEND_DESC
Blend( boo32 enableBlend ) {
    CD3DX12_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable  = false;
    blendDesc.IndependentBlendEnable = false;
    blendDesc.RenderTarget[ 0 ].BlendEnable   = enableBlend;
    blendDesc.RenderTarget[ 0 ].LogicOpEnable = false;
    // NOTE: This blend function is for pre-multiplied alpha.
    blendDesc.RenderTarget[ 0 ].SrcBlend       = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[ 0 ].DestBlend      = D3D12_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[ 0 ].BlendOp        = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[ 0 ].SrcBlendAlpha  = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[ 0 ].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[ 0 ].BlendOpAlpha   = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[ 0 ].LogicOp               = D3D12_LOGIC_OP_NOOP;
    blendDesc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    
    return blendDesc;
}

internal CD3DX12_DEPTH_STENCIL_DESC
DepthStencil( boo32 enableDepth ) {
    CD3DX12_DEPTH_STENCIL_DESC DSDesc = {};
    DSDesc.DepthEnable                  = enableDepth;
    DSDesc.DepthWriteMask               = D3D12_DEPTH_WRITE_MASK_ALL;
    DSDesc.DepthFunc                    = D3D12_COMPARISON_FUNC_LESS;
    DSDesc.StencilEnable                = false;
    DSDesc.StencilReadMask              = D3D12_DEFAULT_STENCIL_READ_MASK;
    DSDesc.StencilWriteMask             = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    DSDesc.FrontFace.StencilFailOp      = D3D12_STENCIL_OP_KEEP;
    DSDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    DSDesc.FrontFace.StencilPassOp      = D3D12_STENCIL_OP_KEEP;
    DSDesc.FrontFace.StencilFunc        = D3D12_COMPARISON_FUNC_ALWAYS;
    DSDesc.BackFace.StencilFailOp       = D3D12_STENCIL_OP_KEEP;
    DSDesc.BackFace.StencilDepthFailOp  = D3D12_STENCIL_OP_KEEP;
    DSDesc.BackFace.StencilPassOp       = D3D12_STENCIL_OP_KEEP;
    DSDesc.BackFace.StencilFunc         = D3D12_COMPARISON_FUNC_ALWAYS;
    
    return DSDesc;
}

struct PIPELINE_STATE_STREAM {
    CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE        RootSignature;
    CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT          InputLayout;
    CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY    PrimitiveTopologyType;
    CD3DX12_PIPELINE_STATE_STREAM_VS                    VertexShader;
    CD3DX12_PIPELINE_STATE_STREAM_PS                    PixelShader;
    CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC            BlendDesc;
    CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT  DepthStencilFormat;
    CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RenderTargetFormats;
    CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER            Rasterizer;
    CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL         DepthStencil;
};

internal ID3D12PipelineState *
createPSS( ID3D12Device2 * device, PIPELINE_STATE_STREAM * PSS ) {
    D3D12_PIPELINE_STATE_STREAM_DESC PSSDesc = {
        sizeof( PIPELINE_STATE_STREAM ), PSS
    };
    
    ID3D12PipelineState * pipelineState = 0;
    HRESULT status = device->CreatePipelineState( &PSSDesc, IID_PPV_ARGS( &pipelineState ) );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to create the pipeline state!\n" );
    }
    return pipelineState;
}

internal void
createShader( DIRECTX_12_RENDERER * renderer, DIRECTX_12_DISPLAY_SETTINGS displaySettings ) {
    renderer->commandAllocator->Reset();
    renderer->commandList->Reset( renderer->commandAllocator, 0 );
    
    ID3D12Device2 * device = renderer->device;
    
    //---------- Create Vertex and Pixel Shaders
    const char * shader_vertexCode = "\
                                                                                                                                                                         cbuffer MatrixBuffer : register( b0 ) {\
                                                                                                                                                                            matrix camera_transform;\
                                                                                                                                                                            matrix model_transform;\
                                                                                                                                                                         };\
                                                                                                                                                                         \
                                                                                                                                                                         struct VertexInputType {\
                                                                                                                                                                            float4 position : POSITION;\
                                                                                                                                                                            float2 texCoord : TEXCOORD0;\
                                                                                                                                                                         };\
                                                                                                                                                                         \
                                                                                                                                                                         struct PixelInputType {\
                                                                                                                                                                            float4 position : SV_POSITION;\
                                                                                                                                                                            float2 texCoord : TEXCOORD0;\
                                                                                                                                                                         };\
                                                                                                                                                                         \
                                                                                                                                                                         PixelInputType main( VertexInputType input ) {\
                                                                                                                                                                            PixelInputType output;\
                                                                                                                                                                            input.position.z = 0.0f;\
                                                                                                                                                                            input.position.w = 1.0f;\
                                                                                                                                                                            output.position = mul( mul( camera_transform, model_transform ), input.position );\
                                                                                                                                                                            output.texCoord = input.texCoord;\
                                                                                                                                                                         return output;\
                                                                                                                                                                         }";
    
    const char * shader_pixelCode = "\
                                                                                                                                                                            cbuffer ColorBuffer : register( b1 ) {\
                                                                                                                                                                               float4 modColor;\
                                                                                                                                                                            };\
                                                                                                                                                                            \
                                                                                                                                                                         struct PixelInputType {\
                                                                                                                                                                            float4 position : SV_POSITION;\
                                                                                                                                                                            float2 texCoord : TEXCOORD0;\
                                                                                                                                                                         };\
                                                                                                                                                                         \
                                                                                                                                                                            SamplerState texSampler : register( s0 );\
                                                                                                                                                                            Texture2D    tex        : register( t0 );\
                                                                                                                                                                            \
                                                                                                                                                                         float4 main( PixelInputType input ) : SV_TARGET {\
                                                                                                                                                                            float4 texel = modColor * tex.Sample( texSampler, input.texCoord );\
                                                                                                                                                                            return texel;\
                                                                                                                                                                         }";
    
    ID3DBlob * errorMessage = 0;
    ID3DBlob * vertexShaderBlob = 0;
    HRESULT status = D3DCompile( shader_vertexCode, strlen( shader_vertexCode ), 0, 0, 0, "main", "vs_5_1", D3DCOMPILE_ENABLE_STRICTNESS, 0, &vertexShaderBlob, &errorMessage );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! RENDERER failed to compile the vertex shader!\n" );
        char * error = ( char * )( errorMessage->GetBufferPointer() );
        InvalidCodePath;
    }
    
    ID3DBlob * pixelShaderBlob  = 0;
    status = D3DCompile( shader_pixelCode, strlen( shader_pixelCode ), 0, 0, 0, "main", "ps_5_1", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pixelShaderBlob, &errorMessage );
    if( status != S_OK ) {
        OutputDebugString( "ERROR! RENDERER failed to compile the pixel shader!\n" );
        char * error = ( char * )( errorMessage->GetBufferPointer() );
        InvalidCodePath;
    }
    //---------- END Create Vertex and Pixel Shaders
    
    VERTEX1 vertex[] = {
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
        1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
        1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
    };
    uint32 nVertex = 4;
    uint32 vertex_size = sizeof( VERTEX1 );
    uint32 vertex_bufferSize = vertex_size * nVertex;
    
    uint32 index[] = {
        0, 1, 2,
        3, 2, 1,
    };
    uint32 nIndex = 6;
    uint32 index_size = sizeof( uint32 );
    uint32 index_bufferSize = index_size * nIndex;
    
    uint32 texture0[ 4 * 4 ] = {
        0xFF000000, 0xFF000000,   0xFFFFFFFF, 0xFFFFFFFF,
        0xFF000000, 0xFF000000,   0xFFFFFFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFFFFFF,   0xFF000000, 0xFF000000,
        0xFFFFFFFF, 0xFFFFFFFF,   0xFF000000, 0xFF000000,
    };
    /*
    uint32 texture1[ 4 * 4 ] = {
       0xFFFF0000, 0xFFFF0000,   0xFF0000FF, 0xFF0000FF,
       0xFFFF0000, 0xFFFF0000,   0xFF0000FF, 0xFF0000FF,
       0xFF0000FF, 0xFF0000FF,   0xFFFF0000, 0xFFFF0000,
       0xFF0000FF, 0xFF0000FF,   0xFFFF0000, 0xFFFF0000,
    };
    */
    uint32 texture_width  = 4;
    uint32 texture_height = 4;
    
    /*
    //---------- Create Single Buffer for Vertices and Indices
    D3D12_VERTEX_BUFFER_VIEW vertexBuffer_view_model = {};
    D3D12_INDEX_BUFFER_VIEW   indexBuffer_view_model = {};
    { // for drawing models
       uint32 bufferSize = _KB( 64 );
       
       ID3D12Resource * uploadBuffer = 0;
       
       D3D12_HEAP_PROPERTIES prop_dest = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD );
       D3D12_RESOURCE_DESC   desc_dest = CD3DX12_RESOURCE_DESC::Buffer( bufferSize );
       
       status = device->CreateCommittedResource( &prop_dest, D3D12_HEAP_FLAG_NONE, &desc_dest, D3D12_RESOURCE_STATE_GENERIC_READ, 0, IID_PPV_ARGS( &uploadBuffer ) );
       if( FAILED( status ) ) {
          OutputDebugString( "ERROR! Failed to create the dest resource to upload the data!\n" );
       }
       
       // Map the buffer
       void * data = 0;
       D3D12_RANGE range = {};
       uploadBuffer->Map( 0, &range, &data ); 
       
       uint8 * data_start = ( uint8 * )data;
       uint8 * data_at    = data_start;
       
       // Upload the vertices
       // -- align the data pointer for the vertex data
       data_at += ( ( uint64 )data_at % sizeof( flo32 ) );
       
       // -- upload the vertex data
       uint32 vertex_offset = ( uint32 )( data_at - data_start );
       memcpy( data_at, vertex, vertex_bufferSize ); 
       data_at += vertex_bufferSize;
       
       // -- create the vertex buffer view
       vertexBuffer_view_model.BufferLocation = uploadBuffer->GetGPUVirtualAddress() + vertex_offset;
       vertexBuffer_view_model.StrideInBytes  = vertex_size;
       vertexBuffer_view_model.SizeInBytes    = sizeof( vertex );
       
       // Upload the indices
       // -- align the data pointer for the index data
       data_at += ( ( uint64 )data_at % sizeof( uint32 ) );
       
       // -- upload the index data
       uint32 index_offset = ( uint32 )( data_at - data_start );
       memcpy( data_at, index, index_bufferSize ); 
       data_at += index_bufferSize;
       
       // -- create the index buffer view
       indexBuffer_view_model.BufferLocation = uploadBuffer->GetGPUVirtualAddress() + index_offset;
       indexBuffer_view_model.Format         = DXGI_FORMAT_R32_UINT;
       indexBuffer_view_model.SizeInBytes    = sizeof( index );
       
       MODEL_LIST * modelList = &renderer->modelList;
       
       MODEL_DATA data0 = {};
       data0.nIndex  = 6;
       data0.bIndex  = 0;
       data0.bVertex = 0;
       modelList->data[ modelID_rect ] = data0;
    }
    //---------- END Create Single Buffer for Vertices and Indices
    */
    
    //---------- Create Descriptor Heap for Textures
    ID3D12DescriptorHeap * SRVDescriptorHeap = 0;
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = textureID_count;
    srvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    status = device->CreateDescriptorHeap( &srvHeapDesc, IID_PPV_ARGS( &SRVDescriptorHeap ) );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to create the descriptor heap for the textures!\n" );
    }
    //---------- END Create Descriptor Heap for Textures
    
    //---------- Create Descriptor Heap for Depth-Stencil View
    ID3D12DescriptorHeap * DSVDescriptorHeap = 0;
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    status = device->CreateDescriptorHeap( &dsvHeapDesc, IID_PPV_ARGS( &DSVDescriptorHeap ) );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to create the descriptor heap for the depth-stencil buffer!\n" );
    }
    //---------- END Create Descriptor Heap for Depth-Stencil View
    
    //---------- Create Depth/Stencil View
    uint32 width  = displaySettings.width;
    uint32 height = displaySettings.height;
    ID3D12Resource * depthBuffer = 0;
    
    D3D12_CLEAR_VALUE optimizedClearValue = {};
    optimizedClearValue.Format       = DXGI_FORMAT_D32_FLOAT;
    optimizedClearValue.DepthStencil = { 1.0f, 0 };
    
    D3D12_HEAP_PROPERTIES prop = CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_DEFAULT );
    D3D12_RESOURCE_DESC   desc = CD3DX12_RESOURCE_DESC::Tex2D( DXGI_FORMAT_D32_FLOAT, width, height,
                                                              1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL ); 
    status = device->CreateCommittedResource( &prop, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &optimizedClearValue,
                                             IID_PPV_ARGS( &depthBuffer ) );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to create the commited resource for the depth/stencil view!\n" );
    }
    
    D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
    dsv.Format        = DXGI_FORMAT_D32_FLOAT;
    dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    
    device->CreateDepthStencilView( depthBuffer, &dsv, DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart() );
    //---------- END Create Depth/Stencil View
    
    //---------- Create Vertex Input Layout
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };
    uint32 inputLayout_count = _arrayCount( inputLayout );
    //---------- END Create Vertex Input Layout
    
    //---------- Create Sampler Description
    D3D12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC( 0, D3D12_FILTER_MIN_MAG_MIP_POINT );
    samplerDesc.BorderColor      = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    //---------- END Create Sampler Description
    
    //---------- Create Root Signature
    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    status = device->CheckFeatureSupport( D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof( featureData ) );
    if( FAILED( status ) ) {
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }
    
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    
    D3D12_ROOT_PARAMETER1 rootParams[ 3 ] = {};
    
    rootParams[ 0 ].ParameterType    = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    rootParams[ 0 ].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParams[ 0 ].Constants.ShaderRegister = 0;
    rootParams[ 0 ].Constants.RegisterSpace  = 0;
    rootParams[ 0 ].Constants.Num32BitValues = 32;
    
    rootParams[ 1 ].ParameterType    = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    rootParams[ 1 ].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParams[ 1 ].Constants.ShaderRegister = 1;
    rootParams[ 1 ].Constants.RegisterSpace  = 0;
    rootParams[ 1 ].Constants.Num32BitValues = 4;
    
    D3D12_DESCRIPTOR_RANGE1 descRange = {};
    descRange.RangeType          = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descRange.NumDescriptors     = 1;
    descRange.BaseShaderRegister = 0;
    descRange.RegisterSpace      = 0;
    descRange.OffsetInDescriptorsFromTableStart = 0;
    
    rootParams[ 2 ].ParameterType    = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParams[ 2 ].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParams[ 2 ].DescriptorTable.NumDescriptorRanges = 1;
    rootParams[ 2 ].DescriptorTable.pDescriptorRanges   = &descRange;
    
    uint32 rootParams_count = _arrayCount( rootParams );
    
    D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
    rootSigDesc.Desc_1_1.NumParameters     = rootParams_count;
    rootSigDesc.Desc_1_1.pParameters       = rootParams;
    rootSigDesc.Desc_1_1.NumStaticSamplers = 1;
    rootSigDesc.Desc_1_1.pStaticSamplers   = &samplerDesc;
    rootSigDesc.Desc_1_1.Flags             = rootSignatureFlags;
    
    ID3DBlob * rootSignatureBlob = 0;
    ID3DBlob * errorBlob         = 0;
    status = D3DX12SerializeVersionedRootSignature( &rootSigDesc, featureData.HighestVersion, &rootSignatureBlob, &errorBlob );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to serialize the root signature!\n" );
    }
    
    ID3D12RootSignature * rootSignature = 0;
    status = device->CreateRootSignature( 0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS( &rootSignature ) );
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to create the root signature!\n" );
    }
    //---------- END Create Root Signature
    
    //---------- Create Pipeline State
    PIPELINE_STATE_STREAM PSS = {};
    
    D3D12_RT_FORMAT_ARRAY renderTargetFormats = {};
    renderTargetFormats.NumRenderTargets = 1;
    renderTargetFormats.RTFormats[ 0 ]   = DXGI_FORMAT_R8G8B8A8_UNORM;
    
    CD3DX12_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
    rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
    rasterizerDesc.FrontCounterClockwise = true;
    rasterizerDesc.DepthBias             = 0;
    rasterizerDesc.DepthBiasClamp        = 0.0f;
    rasterizerDesc.SlopeScaledDepthBias  = 0.0f;
    rasterizerDesc.DepthClipEnable       = true;
    // NOTE: REMEMBER! This is not a depth-buffer clip! This is related to clip-space coords!
    rasterizerDesc.MultisampleEnable     = false;
    rasterizerDesc.AntialiasedLineEnable = false;
    rasterizerDesc.ForcedSampleCount     = 0;
    rasterizerDesc.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    
    PSS.RootSignature         = rootSignature;
    PSS.InputLayout           = { inputLayout, inputLayout_count };
    PSS.VertexShader          = CD3DX12_SHADER_BYTECODE( vertexShaderBlob );
    PSS.PixelShader           = CD3DX12_SHADER_BYTECODE( pixelShaderBlob  );
    PSS.DepthStencilFormat    = DXGI_FORMAT_D32_FLOAT;
    PSS.RenderTargetFormats   = renderTargetFormats;
    PSS.Rasterizer            = rasterizerDesc;
    
    PSS.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    PSS.DepthStencil = DepthStencil( true );
    PSS.BlendDesc    = Blend( false );
    ID3D12PipelineState * PSS_tri_depth_noBlend = createPSS( device, &PSS );
    
    PSS.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    PSS.DepthStencil = DepthStencil( false );
    PSS.BlendDesc    = Blend( true );
    ID3D12PipelineState * PSS_tri_noDepth_blend = createPSS( device, &PSS );
    
    PSS.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    PSS.DepthStencil = DepthStencil( false );
    PSS.BlendDesc    = Blend( false );
    ID3D12PipelineState * PSS_line_noDepth_noBlend = createPSS( device, &PSS );
    
    PSS.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    PSS.DepthStencil = DepthStencil( false );
    PSS.BlendDesc    = Blend( true );
    ID3D12PipelineState * PSS_line_noDepth_blend = createPSS( device, &PSS );
    
    //---------- END Create Pipeline State
    
    renderer->PSS_tri_depth_noBlend   = PSS_tri_depth_noBlend;
    renderer->PSS_tri_noDepth_blend   = PSS_tri_noDepth_blend;
    renderer->PSS_line_noDepth_noBlend = PSS_line_noDepth_noBlend;
    renderer->PSS_line_noDepth_blend   = PSS_line_noDepth_blend;
    renderer->rootSignature = rootSignature;
    renderer->modelBuffer        = Vertex1Buffer( device, 131072, 131072 );
    renderer->textBuffer         = Vertex1Buffer( device, 131072 );
    renderer->DSVDescriptorHeap  = DSVDescriptorHeap;
    renderer->SRVHeap            = SRVDescriptorHeap;
    
    //---------- Execute the Command List
    status = renderer->commandList->Close();
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to close the command list for rendering!\n" );
    }
    
    ID3D12CommandList * const commandLists[] = { renderer->commandList };
    renderer->commandQueue->ExecuteCommandLists( 1, commandLists );
    Signal( renderer->commandQueue, renderer->fence, &renderer->fenceValue );
    WaitForFenceValue( renderer->fence, renderer->fenceValue, renderer->fenceEvent );
    //---------- END Execute the Command List
    
    /*
    // Load the vertex shader.
    ID3DBlob * vertexShaderBlob = 0;
    HRESULT status = D3DReadFileToBlob( L"VertexShader.cso", &vertexShaderBlob );
    if( FAILED( status ) ) {
       OutputDebugString( "ERROR! Failed to create/load the vertex shader!\n" );
    }
    
    // Load the pixel shader.
    ID3DBlob * pixelShaderBlob = 0;
    status = D3DReadFileToBlob( L"PixelShader.cso", &pixelShaderBlob );
    if( FAILED( status ) ) {
       OutputDebugString( "ERROR! Failed to create/load the pixel shader!\n" );
    }
    */
}

internal void
resetCommandList( DIRECTX_12_RENDERER * renderer ) {
    renderer->commandAllocator->Reset();
    renderer->commandList->Reset( renderer->commandAllocator, 0 );
}

internal void
executeCommandList( DIRECTX_12_RENDERER * renderer ) {
    HRESULT status = renderer->commandList->Close();
    if( FAILED( status ) ) {
        OutputDebugString( "ERROR! Failed to close the command list for rendering!\n" );
    }
    
    ID3D12CommandList * const commandLists[] = { renderer->commandList };
    renderer->commandQueue->ExecuteCommandLists( 1, commandLists );
    Signal( renderer->commandQueue, renderer->fence, &renderer->fenceValue );
    WaitForFenceValue( renderer->fence, renderer->fenceValue, renderer->fenceEvent );
}

internal MODEL_DATA
uploadModel( VERTEX1_BUFFER * buffer, VERTEX1 * vertex, uint32 nVertex, uint32 * index, uint32 nIndex ) {
    MODEL_DATA result = {};
    result.nIndex  = nIndex;
    result.bIndex  = buffer->nIndex;
    result.bVertex = buffer->nVertex;
    
    VERTEX1 * dVertex = buffer->vertex_start + buffer->nVertex;
    memcpy( dVertex, vertex, sizeof( VERTEX1 ) * nVertex );
    buffer->nVertex += nVertex;
    Assert( buffer->nVertex < buffer->maxVertex );
    
    uint32 * dIndex = buffer->index_start + buffer->nIndex;
    memcpy( dIndex, index, sizeof( uint32 ) * nIndex );
    buffer->nIndex += nIndex;
    Assert( buffer->nIndex < buffer->maxIndex );
    
    buffer->vertexView.SizeInBytes = sizeof( VERTEX1 ) * buffer->nVertex;
    buffer->indexView.SizeInBytes  = sizeof( uint32  ) * buffer->nIndex;
    
    return result;
}

internal MODEL_DATA
genLine( DIRECTX_12_RENDERER * renderer ) {
    VERTEX1_BUFFER * buffer = &renderer->modelBuffer;
    
    VERTEX1 vertex[ 2 ] = {
        0.0f, 0.0f,  0.0f, 0.0f,
        1.0f, 0.0f,  1.0f, 0.0f,
    };
    uint32 nVertex = _arrayCount( vertex );
    
    uint32 index[] = {
        0, 1,
    };
    uint32 nIndex = _arrayCount( index );
    
    MODEL_DATA result = uploadModel( buffer, vertex, nVertex, index, nIndex );
    return result;
}

internal MODEL_DATA
genRect( DIRECTX_12_RENDERER * renderer ) {
    VERTEX1_BUFFER * buffer = &renderer->modelBuffer;
    
    VERTEX1 vertex[ 4 ] = {
        -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,   1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
        1.0f,  1.0f,   1.0f, 1.0f,
    };
    uint32 nVertex = _arrayCount( vertex );
    
    uint32 index[] = {
        0, 1, 2,
        3, 2, 1,
    };
    uint32 nIndex = _arrayCount( index );
    
    MODEL_DATA result = uploadModel( buffer, vertex, nVertex, index, nIndex );
    return result;
}

internal MODEL_DATA
genRectOutline( DIRECTX_12_RENDERER * renderer ) {
    VERTEX1_BUFFER * buffer = &renderer->modelBuffer;
    
    VERTEX1 vertex[ 4 ] = {
        -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,   1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
        1.0f,  1.0f,   1.0f, 1.0f,
    };
    uint32 nVertex = _arrayCount( vertex );
    
    uint32 index[] = {
        0, 1, 3, 2, 0,
    };
    uint32 nIndex = _arrayCount( index );
    
    MODEL_DATA result = uploadModel( buffer, vertex, nVertex, index, nIndex );
    return result;
}

internal MODEL_DATA
genCircle( DIRECTX_12_RENDERER * renderer ) {
    VERTEX1_BUFFER * buffer = &renderer->modelBuffer;
    
    uint32 nVertex = 64;
    VERTEX1 vertex[ 64 + 1 ] = {};
    for( uint32 iVertex = 0; iVertex < nVertex; iVertex++ ) {
        flo32 radians = ( ( flo32 )iVertex / ( flo32 )nVertex ) * 2.0f * PI;
        flo32 x = cosf( radians );
        flo32 y = sinf( radians );
        
        VERTEX1 vert = {};
        vert.position = Vec2( x, y );
        vert.texCoord = {};
        vertex[ iVertex ] = vert;
    }
    
    uint32 nIndex = nVertex * 3;
    uint32 index[ 64 * 3 ] = {};
    for( uint32 iIndex = 0; iIndex < nVertex; iIndex++ ) {
        uint32 bIndex = iIndex * 3;
        index[ bIndex + 0 ] = nVertex;
        index[ bIndex + 1 ] = iIndex; // [0..63]
        index[ bIndex + 2 ] = ( iIndex + 1 ) % nVertex; // [1..64(wraps to 0)]
    }
    
    nVertex += 1;
    
    MODEL_DATA result = uploadModel( buffer, vertex, nVertex, index, nIndex );
    return result;
}

internal MODEL_DATA
genCircleOutline( DIRECTX_12_RENDERER * renderer ) {
    VERTEX1_BUFFER * buffer = &renderer->modelBuffer;
    
    uint32 nVertex = 64;
    VERTEX1 vertex[ 64 ] = {};
    for( uint32 iVertex = 0; iVertex < nVertex; iVertex++ ) {
        flo32 radians = ( ( flo32 )iVertex / ( flo32 )nVertex ) * 2.0f * PI;
        flo32 x = cosf( radians );
        flo32 y = sinf( radians );
        
        VERTEX1 vert = {};
        vert.position = Vec2( x, y );
        vert.texCoord = {};
        vertex[ iVertex ] = vert;
    }
    
    uint32 nIndex = nVertex + 1;
    uint32 index[ 64 + 1 ] = {};
    for( uint32 iIndex = 0; iIndex < nIndex - 1; iIndex++ ) {
        index[ iIndex ] = iIndex;
    }
    
    MODEL_DATA result = uploadModel( buffer, vertex, nVertex, index, nIndex );
    return result;
}