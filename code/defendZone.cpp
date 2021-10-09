
internal void
updateDefendZone( AUDIO_SYSTEM * audioSystem, APP_STATE * appState, flo32 dt ) {
   DEFEND_ZONE_STATE * state = &appState->defendZoneState;
   if( state->isActive ) {
      
   }
}

internal void
drawDefendZone( RENDER_PASS * pass, APP_STATE * appState, DEFEND_ZONE_STATE * state, boo32 showDebug = false ) {
   if( state->isActive ) {
      // TODO: use modColor to show when it is taking damage.
      drawQuad( pass, state->textureID, state->position + state->texture_offset, state->texture_scale, COLOR_WHITE );
      
      rect bound = rectCHD( state->position + Vec2( 0.0f, -20.0f ), Vec2( state->halfDim.x * 0.6f, 20.0f ) );
      drawHealthBar( pass, bound, state->health, state->maxHealth );
      
      if( showDebug ) {
         rect coll = rectCHD( state->position, state->halfDim );
         drawRectOutline( pass, coll, COLOR_RED );
      }
   }
}

internal void
finalizeDefendZone( AUDIO_SYSTEM * audioSystem, APP_STATE * appState ) {
   DEFEND_ZONE_STATE * state = &appState->defendZoneState;
   
   if( ( state->isActive ) && ( !appState->gameOver ) ) {
      rect coll = rectCHD( state->position, state->halfDim );
      
      BUBBLET_STATE * bubbletState = &appState->bubbletState;
      for( uint32 iFish = 0; iFish < BUBBLET_MAX_COUNT; iFish++ ) {
         BUBBLET * bubblet = bubbletState->bubblet + iFish;
         if( ( bubblet->isActive ) && ( isInBound( bubblet->position, coll ) ) ) {
            killBubblet( audioSystem, appState, bubblet, bubblet->position );
            
            state->health = maxValue( state->health - 1, 0 );
         }
      }
      
      if( state->health <= 0 ) {
         PLAYER_STATE * player = &appState->playerState;
         player->health = 0;
      }
   }
}

