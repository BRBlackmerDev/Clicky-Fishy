
//----------
// TODO: think about rotating 3d background
//  - This doesn't affect gameplay, so I'm putting it off until later. Technically, it could interfere with gameplay, but shouldn't.
// TODO: think about formatting text drawing:
//  - what about something like:
//    STR( format_string, args... )
//  - this would have global access to a tempMemory buffer, and return the char *
// TODO: how should I handle different aspect ratios
// TODO: fix app_bound so that it isn't overlapped by the item panel
//----------

//#define APP_permMemorySize		_MB( 256 )
#define APP_permMemorySize        _GB( 1   )
#define APP_tempMemorySize		_MB( 256 )

#define APP_isFullscreen  ( 0 )


// TODO: think about player art, esp. flipping along the x-axis

// TODO: tune fish size
// TODO: tune fish spawn target time
// TODO: fish spawn paths should not overlap!
// TODO: fish name
// TODO: fish should move using velocity and accel, not tPosition
// TODO: fish should bob up and down slightly when idling
// TODO: tune fish click area (in-depth debugging: show last click relative to position and bitmap)
// TODO: i like when fish suddenly randomly decide to dart across the screen, but they should usually stay close to their base point
// TODO: randomize: idle times, movement speed, etc.
// TODO: fix fish art to allow different colored fish
// TODO: tune fish colors
// TODO: fix fish spawn and movement to allow an envelope around the edge that they can't move through (unless they are despawning)
// TODO: how many different fish colors should there be?
//  - how many skittles colors are there?
// TODO: tune fish points
// TODO: achievement for only eating fish of a single color: 'I only like the green ones!' (or something...)
// TODO: when eating fish of the same color, how does the score multiplier transfer to other fish?
//  - obviously, eating a fish of a different color would restart the score multiplier. it depends on the scoring chain that you are currently in. (eg. same color, rainbow sequence)
// TODO: think about how and when fish spawn:
//  - probably when the screen is cleared, as one example
// TODO: how should i handle color chains when the player eats multiple fish? different colors? different sizes?
// TODO: how should point particles appear for multiple fish? particle for each fish? or one combined particle for the bite?
//  - there should probably be additional bonus points for eating multiple fish at the same time. so like, one particle for each fish and an additional particle for the combo
// TODO: should the score multiplier be flo32 or uint32?
#define SPAWN_TARGET_TIME      ( 0.5f )
#define SPAWN_AND_EXIT_MARGIN  ( 32.0f )

#define PLAY_BOUND_SAFE_MARGIN  ( 40.0f )

// TODO: better art for score display!
// TODO: better art for score particles
// TODO: tune particle display target time
#define SCORE_PARTICLE_TARGET_TIME  ( 1.0f )

// TODO: adjust score multiplier target time based on number of fish ate
#define PLAYER__SCORE_MULTIPLIER__TARGET_TIME  ( 5.0f )

// TODO: make achievements look nice
#define SECRET_SHOW_TARGET_TIME  ( 6.0f )

// TODO: make items look nice
// TODO: make item panel look nice
// TODO: items should appear randomly when corresponding fish are eaten
// TODO: item should always occur first time conditions are met (corresponding fish are eaten)
// TODO: items should appear as bubbles and float upward until they pop (ie. disappear)
// TODO: fix items so they don't appear off screen or drift off the screen
#define ITEM_RADIUS    ( 24.0f )
#define ITEM_OFFSET    ( 80.0f )
#define ITEM_SHOW_TARGET_TIME  ( 5.0f )

#define ITEM_TOOLBAR_ITEM_DIM  ( 40.0f )

// TODO: fix audio clipping, probably make it where I can stop and restart a sound

#define SCHOOL_OF_FISH_TARGET_TIME  ( 22.0f )

#define SECRET_GOLDFISH_SPAWN_TARGET_TIME   ( 2.0f )
#define SECRET_SECOND_GOLDFISH_TARGET_TIME  ( 2.0f )
#define SECRET_GOLDFISH_SNIPE_TARGET_TIME   ( 5.0f )