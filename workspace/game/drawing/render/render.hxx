namespace drawing {
	void render( ) {
		while ( g_drawing->m_msg.message != WM_QUIT ) {
			if ( PeekMessageA( &g_drawing->m_msg, g_drawing->m_window_handle, 0, 0, PM_REMOVE ) ) {
				TranslateMessage( &g_drawing->m_msg );
				DispatchMessage( &g_drawing->m_msg );
			}

			if ( GetAsyncKeyState( VK_INSERT ) & 1 )
				g_drawing->m_menu_active = !g_drawing->m_menu_active;

			g_drawing->update_affinity( );
			g_drawing->new_frame( );
			g_drawing->draw_menu( );

			if ( config::radar::enabled ) {
				g_radar->draw_background( );
			}

			if ( config::misc::enabled ) {
				if ( config::misc::performance_counter ) {
					debug::debug_info_t perf_info;
					perf_info.type = debug::DEBUG_TYPE_PERFORMANCE;
					perf_info.header = "MakFN Performance";
					perf_info.metrics[ "FPS" ] = ImGui::GetIO( ).Framerate;
					g_debug->draw( perf_info );
				}

				if ( config::misc::spectators ) {
					static const std::string box_id = "spectators_list";

					auto spectators = g_world->m_spectators;
					if ( spectators.is_valid( ) ) {
						debug::debug_info_t spec_info;
						spec_info.id = box_id;
						spec_info.type = debug::DEBUG_TYPE_ENTITIES;
						spec_info.header = "Spectators";

						for ( auto idx = 0ul; idx < spectators.size( ); idx++ ) {
							auto player_state = spectators.get( idx );
							if ( !player_state ) continue;

							spec_info.entries.emplace_back(
								std::format( "{} ({})", "Spectator", player_state->player_id( ) ),
								player_state->get_player_name( false )
							);
						}

						g_debug->draw( spec_info );
					}
				}

				if ( config::misc::bullet_tracers ) {
					auto draw_list = ImGui::GetForegroundDrawList( );

					for ( auto it = g_world->m_tracer_list.begin( ); it != g_world->m_tracer_list.end( );) {
						auto& tracer = *it;
						tracer.m_lifetime += ImGui::GetIO( ).DeltaTime;
						tracer.m_alpha = max( 0.0f, 1.0f - ( tracer.m_lifetime / config::misc::fadeout_time ) );

						if ( tracer.m_alpha <= 0.0f ) {
							it = g_world->m_tracer_list.erase( it );
							continue;
						}

						if ( tracer.m_trail_points.size( ) < 5 ) {
							tracer.m_trail_points.push_back( tracer.m_end_location );
						}

						float thickness = std::clamp( 2.0 - ( tracer.m_start_location.distance_to( tracer.m_end_location ) / 1000.0 ), 0.5, 2.0 );

						auto start_screen = engine::world_to_screen( tracer.m_start_location );
						auto end_screen = engine::world_to_screen( tracer.m_end_location );
						if ( start_screen.is_valid( ) && end_screen.is_valid( ) ) {
							for ( size_t i = 0; i < tracer.m_trail_points.size( ) - 1; i++ ) {
								auto trail_start = engine::world_to_screen( tracer.m_trail_points[ i ] );
								auto trail_end = engine::world_to_screen( tracer.m_trail_points[ i + 1 ] );

								if ( trail_start.is_valid( ) && trail_end.is_valid( ) ) {
									float trail_alpha = tracer.m_alpha * ( 1.0f - ( float )i / tracer.m_trail_points.size( ) );
									ImColor trail_color = ImColor( 195, 177, 225, 255 );
									trail_color.Value.w = trail_alpha;

									draw_list->AddLine(
										trail_start.vec( ),
										trail_end.vec( ),
										trail_color,
										thickness * ( 1.0f - ( float )i / tracer.m_trail_points.size( ) )
									);
								}
							}

							ImColor tracer_color = ImColor( 144, 154, 251, 255 );
							tracer_color.Value.w = tracer.m_alpha;
							draw_list->AddLine(
								start_screen.vec( ),
								end_screen.vec( ),
								tracer_color,
								thickness
							);

							draw_list->AddCircle(
								end_screen.vec( ),
								3.0f,
								tracer_color,
								0,
								thickness * 2.0f
							);
						}
						++it;
					}
				}

				g_world->loop_actors( player::render );
				g_world->loop_pickups( enviroment::render );

				if ( config::aimbot::enabled ) {
					g_targeting->tick( );
				}
			}

			g_drawing->draw_frame( );
			g_debug->reset_all( );
		}

		g_drawing->destroy( );
	}
}