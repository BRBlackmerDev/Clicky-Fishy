
internal EVENT_TRIGGER
EventTrigger( uint32 eventID, rect bound ) {
    EVENT_TRIGGER result = {};
    result.eventID = eventID;
    result.bound   = bound;
    return result;
}

internal void
addEventTrigger( APP_STATE * appState, EVENT_TRIGGER eventTrigger ) {
    EVENT_TRIGGER_STATE * state = &appState->eventTriggerState;
    
    Assert( state->nEventTrigger < EVENT_TRIGGER_MAX_COUNT );
    state->eventTrigger[ state->nEventTrigger++ ] = eventTrigger;
}

internal void
drawEventTrigger( RENDER_PASS * pass, APP_STATE * appState, boo32 showDebug = false ) {
    EVENT_TRIGGER_STATE * state = &appState->eventTriggerState;
    
#if 0    
    for( uint32 iEventTrigger = 0; iEventTrigger < state->nEventTrigger; iEventTrigger++ ) {
        EVENT_TRIGGER eventTrigger = state->eventTrigger[ iEventTrigger ];
        drawRectOutline( pass, eventTrigger.bound, COLOR_GREEN );
    }
#endif
}
