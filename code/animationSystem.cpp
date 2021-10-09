
internal void
reset( ANIMATION_SYSTEM * state ) {
    memset( state, 0, sizeof( ANIMATION_SYSTEM ) - sizeof( STRING_BUFFER ) );
    state->stringBuffer.nStr        = 0;
    state->stringBuffer.memory.used = 0;
}

internal ANIMATION *
newAnimation( ANIMATION_SYSTEM * state, MEMORY * memory ) {
    Assert( state->nAnimation < ANIMATION_SYSTEM__ANIMATION_MAX_COUNT );
    
    Assert( !state->curr_animation    );
    Assert( !state->curr_subanimation );
    
    _writem( memory, uint32, state->nAnimation );
    
    ANIMATION * animation = state->animation + state->nAnimation++;
    animation->bSubAnimation = state->nSubAnimation;
    animation->bEvent        = state->nEvent;
    
    state->curr_animation = animation;
    
    return animation;
}

internal SUBANIMATION *
newSubAnimation( ANIMATION_SYSTEM * state, ANIMATION * animation ) {
    Assert( state->nSubAnimation < ANIMATION_SYSTEM__SUBANIMATION_MAX_COUNT );
    animation->nSubAnimation++;
    
    SUBANIMATION * subanimation = state->subanimation + state->nSubAnimation++;
    subanimation->bKeyframe = state->nKeyframe;
    
    Assert( state->curr_animation );
    state->curr_subanimation = subanimation;
    
    return subanimation;
}

internal void
endAnimation( ANIMATION_SYSTEM * state ) {
    Assert( state->curr_animation    );
    Assert( state->curr_subanimation );
    
    state->curr_animation    = 0;
    state->curr_subanimation = 0;
}

internal void
addKeyframe( ANIMATION_SYSTEM * state, TEXTURE_ID textureID, flo32 targetTime, vec2 position, vec2 scale, flo32 radians, vec4 modColor, boo32 isVisible = true ) {
    ANIMATION_KEYFRAME keyframe = {};
    Assert( targetTime >= 0.0f );
    keyframe.type       = keyframeType_sprite;
    keyframe.textureID  = textureID;
    keyframe.targetTime = targetTime;
    keyframe.position   = position;
    keyframe.scale      = scale;
    keyframe.radians    = radians;
    keyframe.modColor   = modColor;
    keyframe.isVisible  = isVisible;
    
    Assert( state->nKeyframe < ANIMATION_SYSTEM__KEYFRAME_MAX_COUNT );
    state->keyframe[ state->nKeyframe++ ] = keyframe;
    
    Assert( state->curr_subanimation );
    state->curr_subanimation->nKeyframe++;
}

internal void
addKeyframe( ANIMATION_SYSTEM * state, char * str, TEXT_ALIGNMENT align, flo32 targetTime, vec2 position, vec2 scale, flo32 radians, vec4 modColor, boo32 isVisible = true ) {
    ANIMATION_KEYFRAME keyframe = {};
    Assert( targetTime >= 0.0f );
    keyframe.type       = keyframeType_text;
    keyframe.str        = str;
    keyframe.align      = align;
    keyframe.targetTime = targetTime;
    keyframe.position   = position;
    keyframe.scale      = scale;
    keyframe.radians    = radians;
    keyframe.modColor   = modColor;
    keyframe.isVisible  = isVisible;
    
    Assert( state->nKeyframe < ANIMATION_SYSTEM__KEYFRAME_MAX_COUNT );
    state->keyframe[ state->nKeyframe++ ] = keyframe;
    
    Assert( state->curr_subanimation );
    state->curr_subanimation->nKeyframe++;
}

//----------
// keyframe helper functions
//----------

internal void
addHold( ANIMATION_SYSTEM * state, TEXTURE_ID textureID, flo32 startTime, flo32 endTime, vec2 position, vec2 scale, flo32 radians, vec4 modColor, boo32 endVisibility ) {
    addKeyframe( state, textureID, startTime, position, scale, radians, modColor, true  );
    addKeyframe( state, textureID, endTime,   position, scale, radians, modColor, endVisibility );
}

internal void
addHold( ANIMATION_SYSTEM * state, char * str, TEXT_ALIGNMENT align, flo32 startTime, flo32 endTime, vec2 position, vec2 scale, flo32 radians, vec4 modColor, boo32 endVisibility ) {
    addKeyframe( state, str, align, startTime, position, scale, radians, modColor, true  );
    addKeyframe( state, str, align, endTime,   position, scale, radians, modColor, endVisibility );
}

internal void
addHold( ANIMATION_SYSTEM * state, char * str, TEXT_ALIGNMENT align, flo32 startTime, flo32 endTime, vec2 position, flo32 scale ) {
    addKeyframe( state, str, align, startTime, position, Vec2( scale, scale ), 0.0f, COLOR_BLACK, true  );
    addKeyframe( state, str, align, endTime,   position, Vec2( scale, scale ), 0.0f, COLOR_BLACK, false );
}

internal ANIMATION_KEYFRAME *
duplicateLastKey( ANIMATION_SYSTEM * state ) {
    Assert( state->nKeyframe < ANIMATION_SYSTEM__KEYFRAME_MAX_COUNT );
    state->keyframe[ state->nKeyframe ] = state->keyframe[ state->nKeyframe - 1 ];
    ANIMATION_KEYFRAME * result = state->keyframe + state->nKeyframe++;
    
    Assert( state->curr_animation    );
    Assert( state->curr_subanimation );
    state->curr_subanimation->nKeyframe++;
    
    return result;
}

internal void
nextKey( ANIMATION_SYSTEM * state, flo32 targetTime, vec2 position ) {
    ANIMATION_KEYFRAME * newKey = duplicateLastKey( state );
    newKey->targetTime = targetTime;
    newKey->position   = position;
}

internal void
nextKey( ANIMATION_SYSTEM * state, flo32 targetTime, vec2 position, boo32 isVisible ) {
    ANIMATION_KEYFRAME * newKey = duplicateLastKey( state );
    newKey->targetTime = targetTime;
    newKey->position   = position;
    newKey->isVisible  = isVisible;
}

internal void
nextKey( ANIMATION_SYSTEM * state, flo32 targetTime, vec2 position, vec2 scale ) {
    ANIMATION_KEYFRAME * newKey = duplicateLastKey( state );
    newKey->targetTime = targetTime;
    newKey->position   = position;
    newKey->scale      = scale;
}

internal void
nextKey( ANIMATION_SYSTEM * state, flo32 targetTime, char * str, vec2 position, boo32 isVisible ) {
    ANIMATION_KEYFRAME * newKey = duplicateLastKey( state );
    newKey->str        = str;
    newKey->targetTime = targetTime;
    newKey->position   = position;
    newKey->isVisible  = isVisible;
}

//----------
//----------

internal void
addEvent( ANIMATION_SYSTEM * state, ANIMATION_EVENT event ) {
    Assert( event.targetTime >= 0.0f );
    
    Assert( state->nEvent < ANIMATION_SYSTEM__EVENT_MAX_COUNT );
    state->event[ state->nEvent++ ] = event;
    
    Assert( state->curr_animation );
    state->curr_animation->nEvent++;
}

internal void
addAudio( ANIMATION_SYSTEM * state, flo32 targetTime, AUDIO_ID audioID, flo32 volume ) {
    ANIMATION_EVENT event = {};
    event.type       = animEventType_audio;
    event.targetTime = targetTime;
    event.audioID    = audioID;
    event.volume     = volume;
    
    addEvent( state, event );
}

internal void
addCameraShake( ANIMATION_SYSTEM * state, flo32 startTime, flo32 frequency, vec2 offsetInPixels, flo32 up, flo32 hold, flo32 down ) {
    ANIMATION_EVENT event = {};
    event.type               = animEventType_cameraShake;
    event.targetTime         = startTime;
    event.cam_upTime         = up;
    event.cam_holdTime       = hold;
    event.cam_downTime       = down;
    event.cam_frequency      = frequency;
    event.cam_offsetInPixels = offsetInPixels;
    
    addEvent( state, event );
}

internal void
addAnimShake( ANIMATION_SYSTEM * state, flo32 startTime, flo32 frequency, vec2 offsetInPixels, flo32 up, flo32 hold, flo32 down ) {
    ANIMATION_EVENT event = {};
    event.type               = animEventType_animShake;
    event.targetTime         = startTime;
    event.cam_upTime         = up;
    event.cam_holdTime       = hold;
    event.cam_downTime       = down;
    event.cam_frequency      = frequency;
    event.cam_offsetInPixels = offsetInPixels;
    
    addEvent( state, event );
}

internal void
addAnimation( ANIMATION_SYSTEM * state, ACTIVE_ANIMATION animation ) {
    Assert( state->nAnim < ANIMATION_SYSTEM__ACTIVE_ANIMATION_MAX_COUNT );
    
    for( uint32 index = 0; index < ANIMATION_SYSTEM__ACTIVE_ANIMATION_MAX_COUNT; index++ ) {
        ACTIVE_ANIMATION * anim = state->anim + index;
        if( !anim->isActive ) {
            *anim = animation;
            state->nAnim++;
            break;
        }
    }
}

internal void
playAnimation( ANIMATION_SYSTEM * state, ANIMATION animation ) {
    Assert( ( animation.nSubAnimation > 0 ) || ( animation.nEvent > 0 ) );
    
    ACTIVE_ANIMATION anim = {};
    anim.isActive    = true;
    anim.shouldLoop  = animation.shouldLoop;
    anim.isCameraRel = animation.isCameraRel;
    
    for( uint32 iSub = 0; iSub < animation.nSubAnimation; iSub++ ) {
        SUBANIMATION subanimation = state->subanimation[ animation.bSubAnimation + iSub ];
        
        ACTIVE_SUBANIMATION sub = {};
        sub.keyframeA = state->keyframe[ subanimation.bKeyframe ];
        sub.keyframeB = sub.keyframeA;
        if( subanimation.nKeyframe > 1 ) {
            sub.keyframeB = state->keyframe[ subanimation.bKeyframe + 1 ];
        }
        
        sub.firstKeyframe = subanimation.bKeyframe;
        sub.atKeyframe    = subanimation.bKeyframe + 1;
        sub.lastKeyframe  = subanimation.bKeyframe + subanimation.nKeyframe;
        
        Assert( anim.nSubAnim < ANIMATION_SYSTEM__ACTIVE_SUBANIMATION_COUNT );
        anim.subAnim[ anim.nSubAnim++ ] = sub;
    }
    
    anim.firstEvent = animation.bEvent;
    anim.atEvent    = animation.bEvent;
    anim.lastEvent  = animation.bEvent + animation.nEvent;
    
    addAnimation( state, anim );
}

internal void
updateAnimation( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, flo32 dt ) {
    ANIMATION_SYSTEM * state = &appState->animationSystem;
    
    for( uint32 iAnim = 0; iAnim < ANIMATION_SYSTEM__ACTIVE_ANIMATION_MAX_COUNT; iAnim++ ) {
        ACTIVE_ANIMATION * anim = state->anim + iAnim;
        if( anim->isActive ) {
            boo32 isValid = ( anim->atEvent < anim->lastEvent );
            while( isValid ) {
                ANIMATION_EVENT event = state->event[ anim->atEvent ];
                if( anim->timer >= event.targetTime ) {
                    switch( event.type ) {
                        case animEventType_audio: {
                            playAudio( audioSystem, appState->audio, event.audioID, event.volume );
                        } break;
                        
                        case animEventType_cameraShake: {
                            initShake( &appState->cameraState.cameraShake, event.cam_upTime, event.cam_holdTime, event.cam_downTime, event.cam_frequency, event.cam_offsetInPixels );
                        } break;
                        
                        case animEventType_animShake: {
                            anim->shake_isActive = true;
                            initShake( &anim->shake, event.cam_upTime, event.cam_holdTime, event.cam_downTime, event.cam_frequency, event.cam_offsetInPixels );
                        } break;
                    }
                    anim->atEvent++;
                    isValid = ( anim->atEvent < anim->lastEvent );
                } else {
                    isValid = false;
                }
            }
            
            anim->timer += dt;
            
            if( anim->shake_isActive ) {
                updateShake( &anim->shake, dt );
            }
            
            //DISPLAY_VALUE( flo32, anim->timer );
            //DISPLAY_VALUE( uint32, anim->atKeyframe );
            //DISPLAY_VALUE( uint32, anim->lastKeyframe );
            //DISPLAY_VALUE( uint32, anim->atEvent );
            //DISPLAY_VALUE( uint32, anim->lastEvent );
        }
    }
}

internal void
drawAnimation( RENDER_PASS * pass, APP_STATE * appState ) {
    ANIMATION_SYSTEM * state = &appState->animationSystem;
    for( uint32 iAnim = 0; iAnim < ANIMATION_SYSTEM__ACTIVE_ANIMATION_MAX_COUNT; iAnim++ ) {
        ACTIVE_ANIMATION * anim = state->anim + iAnim;
        if( anim->isActive ) {
            for( uint32 iSub = 0; iSub < anim->nSubAnim; iSub++ ) {
                ACTIVE_SUBANIMATION * subAnim = anim->subAnim + iSub;
                ANIMATION_KEYFRAME keyA = subAnim->keyframeA;
                ANIMATION_KEYFRAME keyB = subAnim->keyframeB;
                
                if( ( anim->timer >= keyA.targetTime ) && ( keyA.isVisible ) ) {
                    TEXTURE_ID textureID = keyA.textureID;
                    
                    flo32 t = clamp01( ( anim->timer - keyA.targetTime ) / ( keyB.targetTime - keyA.targetTime ) );
                    
                    flo32 camera_height = getHeight( appState->cameraBound );
                    
                    vec2 posA = keyA.position;
                    vec2 posB = keyB.position;
                    if( anim->isCameraRel ) {
                        posA = getBL( appState->cameraBound ) + posA * camera_height;
                        posB = getBL( appState->cameraBound ) + posB * camera_height;
                    }
                    vec2  pos = lerp( posA, t, posB );
                    if( anim->shake_isActive ) {
                        pos += getOffset( &anim->shake );
                    }
                    
                    flo32 rad   = lerp( keyA.radians,  t, keyB.radians  );
                    vec2  sca   = lerp( keyA.scale,    t, keyB.scale    );
                    vec4  color = lerp( keyA.modColor, t, keyB.modColor );
                    
                    switch( keyA.type ) {
                        case keyframeType_sprite: {
                            drawQuad( pass, textureID, pos, rad, sca, color );
                        } break;
                        
                        case keyframeType_text: {
                            rect bound = getBound( appState->font, keyA.str, keyA.align, pos, sca );
                            bound = addRadius( bound, 20.0f );
                            drawRect( pass, textureID_whiteTexture, bound, COLOR_WHITE );
                            drawString( pass, appState->font, keyA.str, pos, keyA.align, sca, color );
                        } break;
                    }
                }
            }
        }
    }
}

internal void
finalizeAnimation( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
    ANIMATION_SYSTEM * state = &appState->animationSystem;
    
    for( uint32 iAnim = 0; iAnim < ANIMATION_SYSTEM__ACTIVE_ANIMATION_MAX_COUNT; iAnim++ ) {
        ACTIVE_ANIMATION * anim = state->anim + iAnim;
        if( anim->isActive ) {
            for( uint32 iSub = 0; iSub < anim->nSubAnim; iSub++ ) {
                ACTIVE_SUBANIMATION * subAnim = anim->subAnim + iSub;
                if( anim->timer > subAnim->keyframeB.targetTime ) {
                    subAnim->keyframeA = subAnim->keyframeB;
                    subAnim->atKeyframe++;
                    if( subAnim->atKeyframe < subAnim->lastKeyframe ) {
                        subAnim->keyframeB = state->keyframe[ subAnim->atKeyframe ];
                    }
                }
            }
        }
    }
    
    for( uint32 iAnim = 0; iAnim < ANIMATION_SYSTEM__ACTIVE_ANIMATION_MAX_COUNT; iAnim++ ) {
        ACTIVE_ANIMATION * anim = state->anim + iAnim;
        if( anim->isActive ) {
            boo32 isComplete = true;
            for( uint32 iSub = 0; iSub < anim->nSubAnim; iSub++ ) {
                ACTIVE_SUBANIMATION * subAnim = anim->subAnim + iSub;
                if( subAnim->atKeyframe < subAnim->lastKeyframe ) {
                    isComplete = false;
                }
            }
            if( ( isComplete ) && ( anim->atEvent >= anim->lastEvent ) ) {
                if( anim->shouldLoop ) {
                    anim->timer = 0.0f;
                    
                    for( uint32 iSub = 0; iSub < anim->nSubAnim; iSub++ ) {
                        ACTIVE_SUBANIMATION * sub = anim->subAnim + iSub;
                        sub->atKeyframe = sub->firstKeyframe;
                        
                        sub->keyframeA = state->keyframe[ sub->atKeyframe ];
                        sub->keyframeB = sub->keyframeA;
                        if( ( sub->lastKeyframe - sub->firstKeyframe ) > 1 ) {
                            sub->keyframeB = state->keyframe[ sub->atKeyframe + 1 ];
                        }
                        sub->atKeyframe++;
                    }
                    
                    anim->atEvent = anim->firstEvent;
                } else {
                    anim->isActive = false;
                    state->nAnim--;
                }
            }
        }
    }
}