namespace player {
	void render( lists::c_actor const player ) {
		auto draw_list = ImGui::GetBackgroundDrawList( );

		if ( config::aimbot::enabled ) {
			g_targeting->process_player( player );
		}

		if ( config::visuals::enabled ) {
			if ( config::radar::enabled ) {
				g_radar->render( player );
			}

			auto head_world = player.m_head_location;
			auto head_screen = player.m_head_screen;
			if ( head_screen.in_screen( ) ) {
				auto origin = player.m_bounds.orgin;
				auto extent = player.m_bounds.box_extent;

				engine::f_vector2d min_pos( DBL_MAX, DBL_MAX );
				engine::f_vector2d max_pos( DBL_MIN, DBL_MIN );
				for ( int i = 0; i < 8; i++ ) {
					auto corner = engine::f_vector(
						( i & 1 ) ? extent.x : -extent.x,
						( i & 2 ) ? extent.y : -extent.y,
						( i & 4 ) ? extent.z : -extent.z
					);

					auto box_origin = origin + corner;
					auto origin_2d = engine::project( box_origin );
					if ( !origin_2d.is_valid( ) ) continue;

					min_pos.x = min( min_pos.x, origin_2d.x );
					min_pos.y = min( min_pos.y, origin_2d.y );
					max_pos.x = max( max_pos.x, origin_2d.x );
					max_pos.y = max( max_pos.y, origin_2d.y );
				}

				engine::f_vector2d size( max_pos - min_pos );
				engine::f_vector2d bottom_middle( min_pos.x + ( size.x / 2.0 ), max_pos.y );
				engine::f_vector2d top_middle( min_pos.x + ( size.x / 2.0 ), min_pos.y );

				auto corner_width = size.x / 4.0;
				auto corner_height = size.y / 7.0;

				auto font_size = ImGui::GetFontSize( );
				auto top_text_offset = font_size + 2.0;
				auto bottom_text_offset = 1.0;

				if ( config::visuals::full_box ) {
					draw_list->AddLine( ImVec2( min_pos.x, min_pos.y ), ImVec2( min_pos.x + size.x, min_pos.y ), ImColor( 142, 130, 209, 255 ), 1.5f );
					draw_list->AddLine( ImVec2( min_pos.x + size.x, min_pos.y ), ImVec2( min_pos.x + size.x, min_pos.y + size.y ), ImColor( 142, 130, 209, 255 ), 1.5f );
					draw_list->AddLine( ImVec2( min_pos.x + size.x, min_pos.y + size.y ), ImVec2( min_pos.x, min_pos.y + size.y ), ImColor( 142, 130, 209, 255 ), 1.5f );
					draw_list->AddLine( ImVec2( min_pos.x, min_pos.y + size.y ), ImVec2( min_pos.x, min_pos.y ), ImColor( 142, 130, 209, 255 ), 1.5f );
				}
				else if ( config::visuals::corner_box ) {
					draw_list->AddLine( ImVec2( min_pos.x, min_pos.y ), ImVec2( min_pos.x, min_pos.y + corner_height ), ImColor( 142, 130, 209, 255 ), 1.5f );
					draw_list->AddLine( ImVec2( min_pos.x, min_pos.y ), ImVec2( min_pos.x + corner_width, min_pos.y ), ImColor( 142, 130, 209, 255 ), 1.5f );

					draw_list->AddLine( ImVec2( min_pos.x + size.x - corner_width, min_pos.y ), ImVec2( min_pos.x + size.x, min_pos.y ), ImColor( 142, 130, 209, 255 ), 1.5f );
					draw_list->AddLine( ImVec2( min_pos.x + size.x, min_pos.y ), ImVec2( min_pos.x + size.x, min_pos.y + corner_height ), ImColor( 142, 130, 209, 255 ), 1.5f );

					draw_list->AddLine( ImVec2( min_pos.x, min_pos.y + size.y - corner_height ), ImVec2( min_pos.x, min_pos.y + size.y ), ImColor( 142, 130, 209, 255 ), 1.5f );
					draw_list->AddLine( ImVec2( min_pos.x, min_pos.y + size.y ), ImVec2( min_pos.x + corner_width, min_pos.y + size.y ), ImColor( 142, 130, 209, 255 ), 1.5f );

					draw_list->AddLine( ImVec2( min_pos.x + size.x - corner_width, min_pos.y + size.y ), ImVec2( min_pos.x + size.x, min_pos.y + size.y ), ImColor( 142, 130, 209, 255 ), 1.5f );
					draw_list->AddLine( ImVec2( min_pos.x + size.x, min_pos.y + size.y - corner_height ), ImVec2( min_pos.x + size.x, min_pos.y + size.y ), ImColor( 142, 130, 209, 255 ), 1.5f );
				}
				else if ( config::visuals::threed_box ) {
					engine::f_vector corners[ 8 ] = {
						origin + engine::f_vector( -extent.x, -extent.y, -extent.z ),
						origin + engine::f_vector( extent.x, -extent.y, -extent.z ),
						origin + engine::f_vector( -extent.x, extent.y, -extent.z ),
						origin + engine::f_vector( extent.x, extent.y, -extent.z ),
						origin + engine::f_vector( -extent.x, -extent.y, extent.z ),
						origin + engine::f_vector( extent.x, -extent.y, extent.z ),
						origin + engine::f_vector( -extent.x, extent.y, extent.z ),
						origin + engine::f_vector( extent.x, extent.y, extent.z )
					};

					engine::f_vector2d screen_corners[ 8 ];
					bool valid = true;
					for ( int i = 0; i < 8; i++ ) {
						screen_corners[ i ] = engine::project( corners[ i ] );
						if ( !screen_corners[ i ].is_valid( ) ) valid = false;
					}

					if ( valid ) {
						const ImColor box_color( 142, 130, 209, 255 );
						const float thickness = 1.5f;

						draw_list->AddLine( screen_corners[ 0 ].vec( ), screen_corners[ 1 ].vec( ), box_color, thickness );
						draw_list->AddLine( screen_corners[ 1 ].vec( ), screen_corners[ 3 ].vec( ), box_color, thickness );
						draw_list->AddLine( screen_corners[ 3 ].vec( ), screen_corners[ 2 ].vec( ), box_color, thickness );
						draw_list->AddLine( screen_corners[ 2 ].vec( ), screen_corners[ 0 ].vec( ), box_color, thickness );

						draw_list->AddLine( screen_corners[ 4 ].vec( ), screen_corners[ 5 ].vec( ), box_color, thickness );
						draw_list->AddLine( screen_corners[ 5 ].vec( ), screen_corners[ 7 ].vec( ), box_color, thickness );
						draw_list->AddLine( screen_corners[ 7 ].vec( ), screen_corners[ 6 ].vec( ), box_color, thickness );
						draw_list->AddLine( screen_corners[ 6 ].vec( ), screen_corners[ 4 ].vec( ), box_color, thickness );

						draw_list->AddLine( screen_corners[ 0 ].vec( ), screen_corners[ 4 ].vec( ), box_color, thickness );
						draw_list->AddLine( screen_corners[ 1 ].vec( ), screen_corners[ 5 ].vec( ), box_color, thickness );
						draw_list->AddLine( screen_corners[ 2 ].vec( ), screen_corners[ 6 ].vec( ), box_color, thickness );
						draw_list->AddLine( screen_corners[ 3 ].vec( ), screen_corners[ 7 ].vec( ), box_color, thickness );
					}
				}

				if ( config::visuals::filled ) {
					draw_list->AddRectFilled(
						min_pos.vec( ),
						max_pos.vec( ),
						ImColor( 142, 130, 209, 80 )
					);
				}

				if ( config::visuals::snaplines ) {
					draw_list->AddLine( bottom_middle.vec( ), ImVec2( g_drawing->m_width_center, g_drawing->m_height ), ImColor( 142, 130, 209, 255 ) );
				}

				if ( config::visuals::movement ) {
					auto text_format = g_drawing->get_movement_name( player.m_movement_mode );
					if ( !text_format.empty( ) ) {
						auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

						auto text_pos = ImVec2(
							top_middle.x - ( text_size.x / 2.0f ),
							top_middle.y - top_text_offset
						);

						auto text_color = g_drawing->get_movement_color( player.m_movement_mode );
						auto outline_color = ImColor( 0, 0, 0, 255 );

						for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
							for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
								if ( dx == 0.0f && dy == 0.0f ) continue;
								draw_list->AddText(
									ImVec2( text_pos.x + dx, text_pos.y + dy ),
									outline_color,
									text_format.c_str( )
								);
							}
						}

						draw_list->AddText( text_pos, text_color, text_format.c_str( ) );
						top_text_offset += text_size.y;
					}
				}

				if ( config::visuals::season_rank ) {
					auto text_format = g_drawing->get_rank_name( player.m_player_rank );
					if ( !text_format.empty( ) ) {
						auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

						auto text_pos = ImVec2(
							top_middle.x - ( text_size.x / 2.0f ),
							top_middle.y - top_text_offset
						);

						auto text_color = g_drawing->get_rank_color( player.m_player_rank );
						auto outline_color = ImColor( 0, 0, 0, 255 );

						for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
							for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
								if ( dx == 0.0f && dy == 0.0f ) continue;
								draw_list->AddText(
									ImVec2( text_pos.x + dx, text_pos.y + dy ),
									outline_color,
									text_format.c_str( )
								);
							}
						}

						draw_list->AddText( text_pos, text_color, text_format.c_str( ) );
						top_text_offset += text_size.y;
					}
				}

				if ( config::visuals::platform ) {
					if ( !player.m_is_bot ) {
						auto text_format = std::format( "{}", g_drawing->get_platform_name( player.m_platform_name ) );
						auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

						auto text_pos = ImVec2(
							top_middle.x - ( text_size.x / 2.0f ),
							top_middle.y - top_text_offset
						);

						auto text_color = g_drawing->get_platform_color( player.m_platform_name );
						auto outline_color = ImColor( 0, 0, 0, 255 );

						for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
							for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
								if ( dx == 0.0f && dy == 0.0f ) continue;
								draw_list->AddText(
									ImVec2( text_pos.x + dx, text_pos.y + dy ),
									outline_color,
									text_format.c_str( )
								);
							}
						}

						draw_list->AddText( text_pos, text_color, text_format.c_str( ) );
						top_text_offset += text_size.y;
					}
				}

				if ( config::visuals::wounded ) {
					if ( player.m_is_downed ) {
						auto text_format = std::string( "Wounded" );
						auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

						auto text_pos = ImVec2(
							top_middle.x - ( text_size.x / 2.0f ),
							top_middle.y - top_text_offset
						);

						auto text_color = ImColor( 66, 67, 227, 255 );
						auto outline_color = ImColor( 0, 0, 0, 255 );

						for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
							for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
								if ( dx == 0.0f && dy == 0.0f ) continue;
								draw_list->AddText(
									ImVec2( text_pos.x + dx, text_pos.y + dy ),
									outline_color,
									text_format.c_str( )
								);
							}
						}

						draw_list->AddText( text_pos, text_color, text_format.c_str( ) );
						top_text_offset += text_size.y;
					}
				}

				if ( config::visuals::display_name ) {
					auto text_format = std::format( "{} ({:.1f}m)", player.m_player_name, player.m_distance );
					auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

					auto text_pos = ImVec2(
						top_middle.x - ( text_size.x / 2.0f ),
						top_middle.y - top_text_offset
					);

					auto text_color = ImColor( 142, 130, 209, 255 );
					auto outline_color = ImColor( 0, 0, 0, 255 );

					for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
						for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
							if ( dx == 0.0f && dy == 0.0f ) continue;
							draw_list->AddText(
								ImVec2( text_pos.x + dx, text_pos.y + dy ),
								outline_color,
								text_format.c_str( )
							);
						}
					}

					draw_list->AddText( text_pos, text_color, text_format.c_str( ) );
					top_text_offset += text_size.y;
				}

				if ( config::visuals::targeting_bar ) {
					auto camera_location = player.m_camera_location;
					auto camera_rotation = player.m_camera_rotation;

					auto direction = g_world->m_head_location - camera_location;
					direction = direction / sqrt( direction.length( ) );

					auto local_direction = camera_rotation.unrotate_vector( direction );
					auto yaw = atan2( local_direction.y, local_direction.x ) * ( 180.0 / std::numbers::pi );
					auto pitch = atan2( local_direction.z, sqrt( local_direction.x * local_direction.x + local_direction.y * local_direction.y ) ) * ( 180.0 / std::numbers::pi );

					const auto bar_height = 4.7f;
					const auto bar_width = size.x;
					const auto bar_y = max_pos.y + 2.0f;

					draw_list->AddRectFilledMultiColor(
						ImVec2( min_pos.x, bar_y ),
						ImVec2( min_pos.x + bar_width, bar_y + bar_height ),
						ImColor( 41, 41, 41, 255 ),
						ImColor( 35, 35, 35, 255 ),
						ImColor( 35, 35, 35, 255 ),
						ImColor( 41, 41, 41, 255 )
					);

					auto angle_diff = std::abs( yaw );
					if ( angle_diff < 90.0 ) {
						auto targeting_percentage = angle_diff / 90.0;

						auto filled_width = bar_width * targeting_percentage;

						const int segments = 20;
						const float segment_width = filled_width / segments;

						for ( int i = 0; i < segments; ++i ) {
							float current_x = i * segment_width;
							float progress = static_cast< float >( i ) / ( segments - 1 );

							float red = std::lerp( 1.0f, 0.0f, progress );
							float green = std::lerp( 0.0f, 1.0f, progress );
							float alpha = std::lerp( 0.8f, 1.0f, progress );

							draw_list->AddRectFilledMultiColor(
								ImVec2( min_pos.x + current_x, bar_y ),
								ImVec2( min_pos.x + current_x + segment_width, bar_y + bar_height ),
								ImColor( red, green, 0.0f, alpha * 0.8f ),
								ImColor( red, green, 0.0f, alpha ),
								ImColor( red, green, 0.0f, alpha ),
								ImColor( red, green, 0.0f, alpha * 0.8f )
							);
						}

						if ( targeting_percentage > 0.95f ) {
							const float glow_width = 5.0f;
							draw_list->AddRectFilledMultiColor(
								ImVec2( min_pos.x + filled_width - glow_width, bar_y ),
								ImVec2( min_pos.x + filled_width, bar_y + bar_height ),
								ImColor( 0.0f, 1.0f, 0.0f, 0.0f ),
								ImColor( 0.0f, 1.0f, 0.0f, 0.3f ),
								ImColor( 0.0f, 1.0f, 0.0f, 0.3f ),
								ImColor( 0.0f, 1.0f, 0.0f, 0.0f )
							);
						}

						draw_list->AddRect(
							ImVec2( min_pos.x - 1, bar_y - 1 ),
							ImVec2( min_pos.x + bar_width + 1, bar_y + bar_height + 1 ),
							ImColor( 0, 0, 0, 100 ),
							0.0f,
							0,
							1.0f
						);
						draw_list->AddRect(
							ImVec2( min_pos.x, bar_y ),
							ImVec2( min_pos.x + bar_width, bar_y + bar_height ),
							ImColor( 0, 0, 0, 255 ),
							0.0f,
							0,
							1.0f
						);

						bottom_text_offset += bar_height + 2.0f;
					}
				}

				if ( config::visuals::weapon_name ) {
					if ( player.m_weapon ) {
						if ( !player.m_weapon_name.empty( ) ) {
							auto text_color = g_drawing->get_tier_color( player.m_weapon_rarity );
							auto text_format = player.m_weapon_ammo ? std::format( "{} ({})", player.m_weapon_name, player.m_weapon_ammo ) :
								std::format( "{}", player.m_weapon_name );
							if ( text_format.contains( "Pickaxe" ) )
								text_color = ImColor( 102, 102, 102, 255 );

							auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );
							auto text_pos = ImVec2(
								bottom_middle.x - ( text_size.x / 2.0f ),
								bottom_middle.y + bottom_text_offset
							);

							auto outline_color = ImColor( 0, 0, 0, 255 );
							for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
								for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
									if ( dx == 0.0f && dy == 0.0f ) continue;
									draw_list->AddText(
										ImVec2( text_pos.x + dx, text_pos.y + dy ),
										outline_color,
										text_format.c_str( )
									);
								}
							}

							draw_list->AddText( text_pos, text_color, text_format.c_str( ) );
							bottom_text_offset += text_size.y;
						}
					}
				}

				if ( config::visuals::level ) {
					auto player_level = player.m_player_level;
					if ( player_level && !g_world->m_is_lobby ) {
						auto text_format = std::format( "Level {}", player_level );
						auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

						auto text_pos = ImVec2(
							bottom_middle.x - ( text_size.x / 2.0f ),
							bottom_middle.y + bottom_text_offset
						);

						auto text_color = ImColor( 209, 135, 57, 255 );
						auto outline_color = ImColor( 0, 0, 0, 255 );

						for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
							for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
								if ( dx == 0.0f && dy == 0.0f ) continue;
								draw_list->AddText(
									ImVec2( text_pos.x + dx, text_pos.y + dy ),
									outline_color,
									text_format.c_str( )
								);
							}
						}

						draw_list->AddText( text_pos, text_color, text_format.c_str( ) );
						bottom_text_offset += text_size.y;
					}
				}

				if ( config::visuals::kills ) {
					auto player_kills = player.m_player_kills;
					if ( player_kills && !g_world->m_is_lobby ) {
						auto text_format = std::format( "Kills {}", player_kills );
						auto text_size = ImGui::CalcTextSize( text_format.c_str( ) );

						auto text_pos = ImVec2(
							bottom_middle.x - ( text_size.x / 2.0f ),
							bottom_middle.y + bottom_text_offset
						);

						auto text_color = ImColor( 217, 70, 70, 255 );
						auto outline_color = ImColor( 0, 0, 0, 255 );

						for ( auto dx = -1.0f; dx <= 1.0f; dx++ ) {
							for ( auto dy = -1.0f; dy <= 1.0f; dy++ ) {
								if ( dx == 0.0f && dy == 0.0f ) continue;
								draw_list->AddText(
									ImVec2( text_pos.x + dx, text_pos.y + dy ),
									outline_color,
									text_format.c_str( )
								);
							}
						}

						draw_list->AddText( text_pos, text_color, text_format.c_str( ) );
						bottom_text_offset += text_size.y;
					}
				}

				if ( config::visuals::view_line ) {
					auto rotation = player.m_player_rotation;
					auto direction = rotation.get_forward_vector( );

					auto view_direction = player.m_head_location + ( direction * 180.0 );
					auto view_screen = engine::project( view_direction );

					draw_list->AddLine(
						head_screen.vec( ),
						view_screen.vec( ),
						ImColor( 134, 49, 213, 255 )
					);
				}

				if ( config::visuals::movement_tracers ) {
					auto& traces = g_player_traces[ player.m_actor ].get_traces( );
					for ( auto i = 1; i < traces.size( ); i++ ) {
						auto prev_position = traces[ i - 1 ].m_position;
						auto curr_position = traces[ i ].m_position;

						auto prev_screen = engine::project( prev_position );
						auto curr_screen = engine::project( curr_position );

						if ( prev_screen.is_valid( ) && curr_screen.is_valid( ) ) {
							draw_list->AddLine(
								prev_screen.vec( ),
								curr_screen.vec( ),
								ImColor( 167, 46, 160, 255 )
							);
						}
					}
				}

				if ( config::visuals::skeleton ) {
					for ( const auto& pair : player.m_skeleton_pairs ) {
						auto world_location_first = pair.first;
						auto world_location_second = pair.second;

						auto screen_position_first = engine::project( world_location_first );
						auto screen_position_second = engine::project( world_location_second );

						draw_list->AddLine( screen_position_first.vec( ), screen_position_second.vec( ), ImColor( 142, 130, 209, 255 ) );
					}
				}
			}

			if ( config::visuals::fov_arrows && !g_world->m_is_lobby ) {
				auto aimbot_fov = g_targeting->m_aimbot_fov;
				if ( !head_screen.in_circle( aimbot_fov ) ) {
					auto delta_seconds = ImGui::GetIO( ).DeltaTime;
					auto angle = atan2(
						player.m_head_screen.y - g_drawing->m_height_center,
						player.m_head_screen.x - g_drawing->m_width_center
					);

					engine::f_vector2d arrow_position{
						g_drawing->m_width_center + ( aimbot_fov + 12.0f ) * cos( angle ),
						g_drawing->m_height_center + ( aimbot_fov + 12.0f ) * sin( angle )
					};

					auto rotation_angle = angle - delta_seconds;
					engine::f_vector2d arrow_vertex[ 3 ]{
						{
							arrow_position.x + cos( rotation_angle ) * 15.0f,
							arrow_position.y + sin( rotation_angle ) * 15.0f
						},
						{
							arrow_position.x + cos( rotation_angle - 1.5f ) * 12.0f,
							arrow_position.y + sin( rotation_angle - 1.5f ) * 12.0f
						},
						{
							arrow_position.x + cos( rotation_angle + 1.5f ) * 12.0f,
							arrow_position.y + sin( rotation_angle + 1.5f ) * 12.0f
						}
					};

					auto draw_list = ImGui::GetBackgroundDrawList( );
					for ( float t = 0; t <= 1.0f; t += 0.1f ) {
						engine::f_vector2d left = {
							arrow_vertex[ 0 ].x + ( arrow_vertex[ 1 ].x - arrow_vertex[ 0 ].x ) * t,
							arrow_vertex[ 0 ].y + ( arrow_vertex[ 1 ].y - arrow_vertex[ 0 ].y ) * t
						};

						engine::f_vector2d right = {
							arrow_vertex[ 0 ].x + ( arrow_vertex[ 2 ].x - arrow_vertex[ 0 ].x ) * t,
							arrow_vertex[ 0 ].y + ( arrow_vertex[ 2 ].y - arrow_vertex[ 0 ].y ) * t
						};

						draw_list->AddLine(
							left.vec( ),
							right.vec( ),
							ImColor( 142, 130, 209, 255 ),
							1.5f
						);
					}

					draw_list->AddLine( arrow_vertex[ 0 ].vec( ), arrow_vertex[ 1 ].vec( ), ImColor( 0, 0, 0, 255 ), 2.5f );
					draw_list->AddLine( arrow_vertex[ 1 ].vec( ), arrow_vertex[ 2 ].vec( ), ImColor( 0, 0, 0, 255 ), 2.5f );
					draw_list->AddLine( arrow_vertex[ 2 ].vec( ), arrow_vertex[ 0 ].vec( ), ImColor( 0, 0, 0, 255 ), 2.5f );
				}
			}
		}
	}
}