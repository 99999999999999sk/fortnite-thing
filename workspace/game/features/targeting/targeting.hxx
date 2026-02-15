#include <chrono>

namespace targeting {
	class c_targeting {
	public:
		void reset( ) {
			m_closest_distance = DBL_MAX;
			m_best_target_score = DBL_MAX;
			m_target_player = std::move( lists::c_actor{ } );
			m_control_points.clear( );
			m_current_progress = 0.f;
			m_current_velocity_x = 0.0f;
			m_current_velocity_y = 0.0f;
			m_prev_velocity_x = 0.0f;
			m_prev_velocity_y = 0.0f;
			m_prev_target_x = 0.0f;
			m_prev_target_y = 0.0f;
			cleanup_target_history( );
		}

		void tick( ) {
			if ( g_world->m_is_lobby )
				return;

			this->get_weapon_config( );
			this->get_close_config( );
			this->render( );

			XINPUT_STATE xinput_state;
			bool trigger_pressed = false;
			if ( XInputGetState( 0 , &xinput_state ) == ERROR_SUCCESS ) {
				trigger_pressed = ( xinput_state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD );
			}

			auto is_pressing = ( GetAsyncKeyState( m_aim_key ) & 0x8000 ) || trigger_pressed;
			if ( is_pressing ) {
				this->aim_lock( );
			}
			else {
				this->reset( );
			}
		}

		void process_player( lists::c_actor player ) {
			if ( !is_player_valid( player ) )
				return;

			if ( m_no_target_switch && m_target_player.m_actor ) {
				return;
			}

			auto screen_position = player.m_head_screen;
			auto dx = screen_position.x - g_drawing->m_width_center;
			auto dy = screen_position.y - g_drawing->m_height_center;
			if ( !screen_position.in_circle( m_aimbot_fov ) ) return;

			auto crosshair_distance = sqrt( dx * dx + dy * dy );

			if ( m_smart_targeting ) {
				update_target_history( player );

				auto final_score = calculate_smart_score( player, crosshair_distance );
				if ( final_score < m_best_target_score ) {
					m_best_target_score = final_score;
					m_target_player = std::move( player );
				}
			}
			else {
				if ( crosshair_distance < m_closest_distance ) {
					m_closest_distance = crosshair_distance;
					m_target_player = std::move( player );
				}
			}
		}

		void aim_lock( ) {
			if ( !m_target_player.m_actor )
				return;

			if ( m_no_target_switch ) {
				if ( !is_player_valid( m_target_player ) ) {
					this->reset( );
					return;
				}
			}

			auto mesh = m_target_player.m_mesh;
			if ( !mesh ) return;

			auto bone_array = m_target_player.m_bone_array;
			if ( !bone_array.is_valid( ) ) return;

			auto aim_location = this->get_aim_location( );
			if ( !aim_location.is_valid( ) ) return;

			if ( m_aim_prediction[ 0 ] ) {
				this->delta_prediction( aim_location );
			}
			if ( m_aim_prediction[ 1 ] ) {
				this->bullet_prediction( aim_location );
			}

			auto aim_screen = engine::project( aim_location );
			if ( !aim_screen.is_valid( ) ) return;
			if ( m_inverted_vertical_axis ) {
				aim_screen.y = -aim_screen.y;
			}

			if ( !m_aim_curve ) {
				auto draw_list = ImGui::GetBackgroundDrawList( );

				if ( m_target_line ) {
					draw_list->AddLine(
						aim_screen.vec( ),
						ImVec2( g_drawing->m_width_center, g_drawing->m_height_center ),
						ImColor( 207, 135, 159, 255 )
					);
				}

				if ( m_target_dot ) {
					auto max_radius = 6.0f;
					auto min_radius = 2.0f;
					auto distance = m_target_player.m_distance;
					auto scaled_radius = max_radius - ( distance * 0.05f );
					auto dot_radius = std::clamp( scaled_radius, min_radius, max_radius );

					draw_list->AddCircleFilled(
						aim_screen.vec( ),
						dot_radius,
						ImColor( 207, 135, 159, 255 ),
						12
					);
				}
			}

			auto smoothed_location = this->smooth_location( aim_location, aim_screen );
			auto aim_rotation = this->get_aim_rotation( smoothed_location );

			move_mouse( aim_rotation );
		}

		void render( ) {
			auto draw_list = ImGui::GetBackgroundDrawList( );

			this->m_aimbot_fov = ( m_fov_radius * g_drawing->m_width_center / g_world->m_camera_fov ) / 2.0;
			draw_list->AddCircle(
				ImVec2( g_drawing->m_width_center, g_drawing->m_height_center ),
				m_aimbot_fov,
				ImColor( 255, 255, 255, 255 )
			);

			if ( m_deadzone && m_target_player.m_actor ) {
				auto aim_location = this->get_aim_location( );
				if ( aim_location.is_valid( ) ) {
					auto aim_screen = engine::project( aim_location );
					if ( aim_screen.is_valid( ) ) {
						auto distance = m_target_player.m_distance;
						auto scaled_restriction = m_restriction * ( 1.0f + ( 50.0f / max( distance, 1.0f ) ) );

						draw_list->AddCircle(
							ImVec2( aim_screen.x, aim_screen.y ),
							scaled_restriction,
							ImColor( 255, 0, 0, 100 ),
							0,
							1.0f
						);
						draw_list->AddCircleFilled(
							ImVec2( aim_screen.x, aim_screen.y ),
							scaled_restriction,
							ImColor( 255, 0, 0, 30 )
						);
					}
				}
			}

			if ( m_target_line ) {
				if ( m_aim_curve && !m_control_points.empty( ) ) {
					if ( m_target_dot ) {
						for ( size_t i = 0; i < m_control_points.size( ); i++ ) {
							draw_list->AddCircleFilled(
								m_control_points[ i ],
								3.f,
								ImColor( 163, 75, 180, 225 ),
								8
							);
						}
					}

					if ( m_control_points.size( ) >= 2 ) {
						const int segments = 20;
						auto prev_point = m_control_points[ 0 ];

						for ( int i = 1; i <= segments; i++ ) {
							auto t = static_cast< float >( i ) / segments;
							auto current_point = calculate_bezier_point( t );

							draw_list->AddLine(
								prev_point,
								current_point,
								ImColor( 220, 151, 164, 255 ),
								1.5f
							);

							prev_point = current_point;
						}
					}
				}
			}
		}

		float m_aimbot_fov = 0.f;

	private:
		double m_closest_distance = DBL_MAX;
		double m_best_target_score = DBL_MAX;
		lists::c_actor m_target_player{ };

		struct m_target_info_t {
			std::uint32_t m_player_id = 0;
			float m_current_lock_time = 0.0f;
			std::chrono::steady_clock::time_point m_last_seen = std::chrono::steady_clock::now( );
			std::chrono::steady_clock::time_point m_first_seen = std::chrono::steady_clock::now( );
			float m_total_lock_time = 0.0f;
			int m_lock_count = 0;
			float m_average_distance = 0.0f;
			float m_last_distance = 0.0f;
			bool m_was_visible = false;
			engine::f_vector m_last_position{};
			engine::f_vector m_velocity{};
			float m_threat_level = 0.0f;
		};

		std::unordered_map<std::uint32_t, m_target_info_t> m_target_history;
		std::vector<std::uint32_t> m_recent_targets;
		float m_current_progress = 0.f;
		bool m_positive_curve = false;
		std::vector<ImVec2> m_control_points{ };

		float m_current_velocity_x = 0.0f;
		float m_current_velocity_y = 0.0f;
		float m_prev_velocity_x = 0.0f;
		float m_prev_velocity_y = 0.0f;
		float m_prev_target_x = 0.0f;
		float m_prev_target_y = 0.0f;

		void move_mouse( engine::f_rotator aim_rotation ) {
			auto player_controller = g_world->m_player_controller;
			if ( !player_controller )
				return;

			player_controller->weapon_recoil_offset( aim_rotation );
		}

		engine::f_rotator get_aim_rotation( engine::f_vector aim_location ) {
			auto aim_rotation = engine::find_look_at_rotation( g_world->m_camera_location, aim_location );
			aim_rotation -= g_world->m_camera_rotation;
			aim_rotation.normalize( );

			if ( m_aim_acceleration ) {
				auto distance = std::sqrt( aim_rotation.yaw * aim_rotation.yaw + aim_rotation.pitch * aim_rotation.pitch );
				auto current_speed = std::sqrt( m_current_velocity_x * m_current_velocity_x + m_current_velocity_y * m_current_velocity_y );

				if ( distance < 0.1f ) {
					m_current_velocity_x = 0.0f;
					m_current_velocity_y = 0.0f;
					m_prev_velocity_x = 0.0f;
					m_prev_velocity_y = 0.0f;
					m_prev_target_x = 0.0f;
					m_prev_target_y = 0.0f;
				}
				else {
					auto target_x = aim_rotation.yaw;
					auto target_y = aim_rotation.pitch;
					auto smoothed_target_x = m_prev_target_x + ( target_x - m_prev_target_x ) * 0.5f;
					auto smoothed_target_y = m_prev_target_y + ( target_y - m_prev_target_y ) * 0.5f;

					auto velocity_diff_x = smoothed_target_x - m_current_velocity_x;
					auto velocity_diff_y = smoothed_target_y - m_current_velocity_y;

					auto current_dir_x = current_speed > 0.001f ? m_current_velocity_x / current_speed : 0.0f;
					auto current_dir_y = current_speed > 0.001f ? m_current_velocity_y / current_speed : 0.0f;
					auto target_dir_x = distance > 0.001f ? smoothed_target_x / distance : 0.0f;
					auto target_dir_y = distance > 0.001f ? smoothed_target_y / distance : 0.0f;
					auto direction_alignment = ( current_dir_x * target_dir_x + current_dir_y * target_dir_y );

					auto target_speed = min( distance, m_max_velocity );
					auto speed_ratio = current_speed / target_speed;
					auto speed_transition = std::pow( 1.0f - speed_ratio, 2.0f );

					auto direction_transition = ( 1.0f + direction_alignment ) * 0.5f;

					auto delta_time = ImGui::GetIO( ).DeltaTime;
					auto time_factor = min( delta_time * ImGui::GetIO( ).Framerate, 1.0f );

					auto transition_factor = speed_transition * direction_transition;
					auto adaptive_acceleration = m_acceleration * ( 1.0f + transition_factor ) * time_factor;

					auto new_velocity_x = m_current_velocity_x + velocity_diff_x * adaptive_acceleration * delta_time;
					auto new_velocity_y = m_current_velocity_y + velocity_diff_y * adaptive_acceleration * delta_time;

					m_current_velocity_x = m_prev_velocity_x + ( new_velocity_x - m_prev_velocity_x ) * 0.7f;
					m_current_velocity_y = m_prev_velocity_y + ( new_velocity_y - m_prev_velocity_y ) * 0.7f;

					auto new_speed = std::sqrt( m_current_velocity_x * m_current_velocity_x + m_current_velocity_y * m_current_velocity_y );
					if ( new_speed > target_speed ) {
						auto scale = target_speed / new_speed;
						m_current_velocity_x *= scale;
						m_current_velocity_y *= scale;
					}

					if ( std::abs( m_current_velocity_x ) < 0.001f ) m_current_velocity_x = 0.0f;
					if ( std::abs( m_current_velocity_y ) < 0.001f ) m_current_velocity_y = 0.0f;

					m_prev_velocity_x = m_current_velocity_x;
					m_prev_velocity_y = m_current_velocity_y;
					m_prev_target_x = smoothed_target_x;
					m_prev_target_y = smoothed_target_y;
				}

				aim_rotation.yaw = m_current_velocity_x;
				aim_rotation.pitch = m_current_velocity_y;
			}

			return aim_rotation;
		}

		void bullet_prediction( engine::f_vector& aim_location ) const {
			auto projectile_speed = g_world->m_projectile_speed;
			auto projectile_gravity = g_world->m_projectile_gravity;

			if ( g_world->m_acknowledged_pawn->current_weapon( ) ) {
				if ( projectile_gravity && projectile_speed ) {
					auto gravity = fabs( projectile_gravity * -980.0f );
					auto distance = g_world->m_camera_location.distance( aim_location );
					auto time = distance / projectile_speed;

					aim_location.x -= time;
					aim_location.y -= time;
					aim_location.z -= time + ( 0.5f * gravity * time * time );
				}
			}
		}

		void delta_prediction( engine::f_vector& aim_location ) const {
			aim_location.x += m_target_player.m_velocity.x;
			aim_location.y += m_target_player.m_velocity.y;
			aim_location.z += m_target_player.m_velocity.z;
		}

		engine::f_vector smooth_location( engine::f_vector aim_location, engine::f_vector2d aim_screen ) {
			if ( !aim_screen.is_valid( ) || m_smoothing <= 0 ) {
				return aim_location;
			}

			auto dx = aim_screen.x - g_drawing->m_width_center;
			auto dy = aim_screen.y - g_drawing->m_height_center;

			auto start_point = ImVec2( g_drawing->m_width_center, g_drawing->m_height_center );
			auto end_point = ImVec2( aim_screen.x, aim_screen.y );

			if ( m_deadzone ) {
				auto distance = m_target_player.m_distance;
				auto scaled_restriction = m_restriction * ( 1.0f + ( 50.0f / max( distance, 1.0f ) ) );
				auto screen_distance = sqrt( dx * dx + dy * dy );
				if ( screen_distance < scaled_restriction ) {
					dx = 0;
					dy = 0;
				}
			}

			if ( m_aim_curve ) {
				if ( m_control_points.empty( ) ||
					( m_control_points.size( ) >= 2 &&
						( abs( m_control_points.back( ).x - end_point.x ) > 8.0f ||
							abs( m_control_points.back( ).y - end_point.y ) > 8.0f ) ) ) {

					m_control_points.clear( );
					m_control_points.push_back( start_point );

					auto distance = sqrt( dx * dx + dy * dy );
					auto num_control_points = min( 3, static_cast< int >( distance ) );

					for ( int i = 0; i < num_control_points; i++ ) {
						auto progress = ( i + 1.0f ) / ( num_control_points + 1.0f );
						auto offset_strength = distance * 0.2f * ( m_positive_curve ? 1.0f : -1.0f );

						auto perpX = -dy;
						auto perpY = dx;
						auto norm = sqrt( perpX * perpX + perpY * perpY );

						if ( norm > 0.001f ) {
							perpX /= norm;
							perpY /= norm;
						}

						auto bell_curve = sin( progress * std::numbers::pi );
						auto control_point = ImVec2(
							start_point.x + dx * progress + perpX * offset_strength * bell_curve,
							start_point.y + dy * progress + perpY * offset_strength * bell_curve
						);

						m_control_points.push_back( control_point );
					}

					m_control_points.push_back( end_point );

					if ( rand( ) % 1000 == 0 ) {
						m_positive_curve = !m_positive_curve;
					}

					m_current_progress = 0.0f;
				}

				auto progress_step = 1.0f / ( m_smoothing * 3.0f + 5.0f );
				m_current_progress += progress_step;
				m_current_progress = std::clamp( m_current_progress, 0.0f, 1.0f );

				auto max_step = 100.0f / ( m_smoothing * 1.5f + 1.0f );
				auto distance = std::sqrt( dx * dx + dy * dy );
				if ( distance > max_step ) {
					dx = ( dx / distance ) * max_step;
					dy = ( dy / distance ) * max_step;
				}

				dx = std::clamp( dx, ( double )-max_step, ( double )max_step );
				dy = std::clamp( dy, ( double )-max_step, ( double )max_step );
			}
			else {
				m_control_points.clear( );
				m_current_progress = 0.0f;

				auto max_step = 100.0f / ( m_smoothing + 1.0f );
				auto distance = std::sqrt( dx * dx + dy * dy );
				if ( distance > max_step ) {
					dx = ( dx / distance ) * max_step;
					dy = ( dy / distance ) * max_step;
				}

				dx = std::clamp( dx, ( double )-max_step, ( double )max_step );
				dy = std::clamp( dy, ( double )-max_step, ( double )max_step );
			}

			auto apply_random = [ ]( double& value, bool positive ) {
				auto abs_val = static_cast< int >( std::abs( value ) );
				auto rand_val = 0;

				if ( abs_val >= 32 ) rand_val = rand( ) % 21;
				else if ( abs_val > 16 ) rand_val = rand( ) % 8;
				else if ( abs_val > 8 ) rand_val = rand( ) % 4;
				else if ( abs_val > 3 ) rand_val = rand( ) % 2;
				else if ( abs_val > 1 ) rand_val = rand( ) % 1;

				if ( positive ) value += rand_val;
				else value -= rand_val;
				};

			apply_random( dx, m_positive_curve );
			apply_random( dy, m_positive_curve );

			auto smoothed_screen = engine::f_vector2d(
				g_drawing->m_width_center + dx,
				g_drawing->m_height_center + dy
			);

			auto aim_direction = engine::deproject( smoothed_screen );
			auto aim_distance = g_world->m_camera_location.distance_to( aim_location );
			return g_world->m_camera_location + ( aim_direction * aim_distance );
		}

		bool is_player_valid( lists::c_actor player ) const {
			return player.m_actor && ( !m_ignore_downed || !player.m_is_downed ) &&
				( !m_only_visible || player.m_is_visible ) &&
				( !m_ignore_bots || !player.m_is_bot ) &&
				player.m_distance < m_max_distance;
		}

		ImVec2 calculate_bezier_point( float t ) const {
			if ( m_control_points.empty( ) ) {
				return ImVec2( g_drawing->m_width_center, g_drawing->m_height_center );
			}

			if ( m_control_points.size( ) == 1 ) {
				return m_control_points[ 0 ];
			}

			auto points = m_control_points;
			for ( size_t j = 1; j < points.size( ); j++ ) {
				for ( size_t i = 0; i < points.size( ) - j; i++ ) {
					points[ i ].x = ( 1 - t ) * points[ i ].x + t * points[ i + 1 ].x;
					points[ i ].y = ( 1 - t ) * points[ i ].y + t * points[ i + 1 ].y;
				}
			}

			return points[ 0 ];
		}

		engine::f_vector get_aim_location( ) const {
			std::vector<engine::e_bone> bone_pool;

			auto mesh = m_target_player.m_mesh;
			if ( !mesh ) return {};

			auto bone_array = m_target_player.m_bone_array;
			if ( !bone_array.is_valid( ) ) return {};

			if ( m_aim_bone[ 0 ] ) {
				bone_pool.push_back( engine::e_bone::vb_head_fx );
			}
			if ( m_aim_bone[ 1 ] ) {
				bone_pool.push_back( engine::e_bone::neck_01 );
			}
			if ( m_aim_bone[ 2 ] ) {
				bone_pool.push_back( engine::e_bone::neck_02 );
			}
			if ( m_aim_bone[ 3 ] ) {
				bone_pool.push_back( engine::e_bone::spine_01 );
			}

			if ( !bone_pool.empty( ) ) {
				if ( m_linear_aim ) {
					auto start_bone = mesh->get_bone_location( bone_array, bone_pool.front( ) );
					auto end_bone = mesh->get_bone_location( bone_array, bone_pool.back( ) );

					static float last_time = 0.0f;
					static float current_t = 0.0f;
					static bool increasing = true;

					auto current_time = static_cast< float >( std::chrono::duration_cast< std::chrono::milliseconds >(
						std::chrono::steady_clock::now( ).time_since_epoch( )
					).count( ) ) / 1000.0f;

					if ( last_time == 0.0f ) {
						last_time = current_time;
					}

					float delta_time = current_time - last_time;
					last_time = current_time;

					float distance = start_bone.distance_to( end_bone );
					float speed = std::clamp( 1.0f / ( distance * 0.1f ), 0.1f, 2.0f );

					if ( increasing ) {
						current_t += delta_time * speed;
						if ( current_t >= 1.0f ) {
							current_t = 1.0f;
							increasing = false;
						}
					}
					else {
						current_t -= delta_time * speed;
						if ( current_t <= 0.0f ) {
							current_t = 0.0f;
							increasing = true;
						}
					}

					float smooth_t = current_t * current_t * ( 3.0f - 2.0f * current_t );

					return engine::f_vector(
						start_bone.x + ( end_bone.x - start_bone.x ) * smooth_t,
						start_bone.y + ( end_bone.y - start_bone.y ) * smooth_t,
						start_bone.z + ( end_bone.z - start_bone.z ) * smooth_t
					);
				}
				else {
					auto index = rand( ) % bone_pool.size( );
					return mesh->get_bone_location( bone_array, bone_pool[ index ] );
				}
			}

			return {};
		}

		float calculate_smart_score( const lists::c_actor& player, float crosshair_distance ) {
			auto player_id = player.m_player_id;
			auto distance = player.m_distance;

			auto distance_score = get_distance_priority( distance );
			auto crosshair_score = crosshair_distance * 0.7f;
			auto persistence_bonus = get_persistence_bonus( player_id );
			auto threat_score = get_threat_score( player );
			auto visibility_bonus = get_visibility_bonus( player );
			auto health_bonus = get_health_bonus( player );
			auto weapon_bonus = get_weapon_threat_bonus( player );

			auto final_score = crosshair_score +
				( distance_score * -0.4f ) +
				persistence_bonus +
				( threat_score * -0.15f ) +
				( visibility_bonus * -0.1f ) +
				( health_bonus * -0.15f ) +
				( weapon_bonus * -0.1f );

			return final_score;
		}

		float get_persistence_bonus( std::uint32_t player_id ) {
			if ( m_target_history.count( player_id ) ) {
				auto& history = m_target_history[ player_id ];

				if ( player_id == m_target_player.m_player_id ) {
					auto lock_bonus = -min( history.m_current_lock_time * 5.0f, 25.0f );
					auto consistency_bonus = -min( history.m_lock_count * 2.0f, 10.0f );
					return lock_bonus + consistency_bonus;
				}

				if ( history.m_current_lock_time > 0.0f ) {
					return 5.0f;
				}
			}
			return 0.0f;
		}

		float get_threat_score( const lists::c_actor& player ) {
			auto threat = 0.0f;

			if ( player.m_player_level > 0 ) {
				threat += min( player.m_player_level / 100.0f, 1.0f ) * 10.0f;
			}

			if ( player.m_player_kills > 0 ) {
				threat += min( player.m_player_kills * 2.0f, 15.0f );
			}

			if ( player.m_is_bot ) {
				threat *= 0.5f;
			}

			return threat;
		}

		float get_visibility_bonus( const lists::c_actor& player ) {
			if ( player.m_is_visible ) {
				return 15.0f;
			}
			return 0.0f;
		}

		float get_health_bonus( const lists::c_actor& player ) {
			if ( player.m_is_downed ) {
				return 20.0f;
			}
			return 0.0f;
		}

		float get_weapon_threat_bonus( const lists::c_actor& player ) {
			if ( !player.m_weapon ) return 0.0f;

			switch ( player.m_weapon_rarity ) {
			case engine::e_fort_rarity::legendary: return 15.0f;
			case engine::e_fort_rarity::epic: return 10.0f;
			case engine::e_fort_rarity::rare: return 5.0f;
			case engine::e_fort_rarity::uncommon: return 2.0f;
			default: return 0.0f;
			}
		}

		void update_target_history( const lists::c_actor& player ) {
			auto player_id = player.m_player_id;
			auto current_time = std::chrono::steady_clock::now( );

			auto& history = m_target_history[ player_id ];
			history.m_player_id = player_id;
			history.m_last_seen = current_time;

			if ( history.m_lock_count == 0 ) {
				history.m_first_seen = current_time;
			}

			if ( history.m_last_position.is_valid( ) ) {
				auto time_diff = std::chrono::duration<double>( current_time - history.m_last_seen ).count( );
				if ( time_diff > 0.0f ) {
					auto pos_diff = player.m_base_location - history.m_last_position;
					history.m_velocity = pos_diff / time_diff;
				}
			}
			history.m_last_position = player.m_base_location;

			history.m_last_distance = player.m_distance;
			if ( history.m_average_distance == 0.0f ) {
				history.m_average_distance = player.m_distance;
			}
			else {
				history.m_average_distance = ( history.m_average_distance * 0.9f ) + ( player.m_distance * 0.1f );
			}

			history.m_was_visible = player.m_is_visible;

			if ( player_id == m_target_player.m_player_id ) {
				auto elapsed = std::chrono::duration<float>( current_time - history.m_last_seen ).count( );
				history.m_current_lock_time += elapsed;
				history.m_total_lock_time += elapsed;

				if ( history.m_current_lock_time > 0.1f && history.m_lock_count == 0 ) {
					history.m_lock_count = 1;
				}
			}
			else {
				if ( history.m_current_lock_time > 0.0f ) {
					history.m_lock_count++;
				}
				history.m_current_lock_time = 0.0f;
			}

			history.m_threat_level = calculate_threat_level( player, history );
		}

		float calculate_threat_level( const lists::c_actor& player, const m_target_info_t& history ) {
			auto threat = 0.0f;

			threat += min( player.m_player_kills * 0.1f, 1.0f );
			threat += min( player.m_player_level * 0.01f, 1.0f );

			return std::clamp( threat, 0.0f, 5.0f );
		}

		void cleanup_target_history( ) {
			auto current_time = std::chrono::steady_clock::now( );
			auto cleanup_threshold = std::chrono::seconds( 30 );

			for ( auto it = m_target_history.begin( ); it != m_target_history.end( );) {
				if ( current_time - it->second.m_last_seen > cleanup_threshold ) {
					it = m_target_history.erase( it );
				}
				else {
					++it;
				}
			}
		}

		float get_distance_priority( float distance ) {
			return std::sqrt( distance ) * 0.1f;
		}

	private:
		bool m_linear_aim = false;
		bool m_no_target_switch = false;
		bool m_inverted_vertical_axis = false;
		bool m_fov_circle = false;
		bool m_target_line = false;
		bool m_target_dot = false;
		bool m_aim_curve = false;
		float m_fov_radius = 0.f;
		bool m_deadzone = false;
		bool m_aim_prediction[ 5 ];
		int m_smoothing = 0;
		bool m_aim_bone[ 5 ]{ };
		int m_aim_key = 0;
		float m_restriction = 0.f;
		bool m_smart_targeting = false;
		bool m_ignore_downed = false;
		bool m_ignore_bots = false;
		bool m_only_visible = false;
		bool m_aim_acceleration = false;
		float m_max_velocity = 0.f;
		float m_acceleration = 0.f;
		float m_deceleration = 0.f;
		float m_velocity_threshold = 0.f;
		int m_max_distance = 0;

		void get_weapon_config( ) {
			if ( config::aimbot::override_general[ config::aimbot::weapon_configs ] ) {
				auto weapon_animation = g_world->m_weapon_animation;
				switch ( weapon_animation ) {
				case engine::e_weapon_core_animation::rifle: {
					this->apply_config( 1 );
				} break;
				case engine::e_weapon_core_animation::assault_rifle: {
					this->apply_config( 1 );
				} break;
				case engine::e_weapon_core_animation::shotgun: {
					this->apply_config( 2 );
				} break;
				case engine::e_weapon_core_animation::tactical_shotgun: {
					this->apply_config( 2 );
				} break;
				case engine::e_weapon_core_animation::machine_pistol: {
					this->apply_config( 3 );
				} break;
				case engine::e_weapon_core_animation::smg_p90: {
					this->apply_config( 3 );
				} break;
				case engine::e_weapon_core_animation::pistol: {
					this->apply_config( 4 );
				} break;
				case engine::e_weapon_core_animation::sniper_rifle: {
					this->apply_config( 5 );
				} break;
				default: {
					this->apply_config( 0 );
				} break;
				}
			}
			else
				this->apply_config( 0 );
		}

		void get_close_config( ) {
			if ( config::aimbot::close_aim ) {
				auto weapon_animation = g_world->m_weapon_animation;
				if ( config::aimbot::shotgun_only && weapon_animation != engine::e_weapon_core_animation::shotgun )
					return;

				auto distance = m_target_player.m_distance;
				if ( distance > config::aimbot::max_distance[ 6 ] )
					return;

				this->apply_config( 6 );
			}
		}

		void apply_config( int weapon_config ) {
			if ( weapon_config > 5 ) {
				m_linear_aim = config::aimbot::linear_aim[ weapon_config ];
				m_no_target_switch = config::aimbot::no_target_switch[ weapon_config ];
				m_inverted_vertical_axis = config::aimbot::inverted_vertical_axis[ weapon_config ];
				m_fov_circle = config::aimbot::fov_circle[ weapon_config ];
				m_fov_radius = config::aimbot::fov_radius[ weapon_config ];
				m_deadzone = config::aimbot::deadzone[ weapon_config ];
				m_smoothing = config::aimbot::smoothing[ weapon_config ];
				memcpy( m_aim_bone, config::aimbot::aim_bone[ weapon_config ], sizeof( m_aim_bone ) );
				m_aim_key = config::aimbot::aim_key[ weapon_config ];
				m_restriction = config::aimbot::restriction[ weapon_config ];
				m_max_distance = config::aimbot::max_distance[ weapon_config ];
			}
			else {
				m_linear_aim = config::aimbot::linear_aim[ weapon_config ];
				m_no_target_switch = config::aimbot::no_target_switch[ weapon_config ];
				m_inverted_vertical_axis = config::aimbot::inverted_vertical_axis[ weapon_config ];
				m_fov_circle = config::aimbot::fov_circle[ weapon_config ];
				m_target_line = config::aimbot::target_line[ weapon_config ];
				m_target_dot = config::aimbot::target_dot[ weapon_config ];
				m_aim_curve = config::aimbot::aim_curve[ weapon_config ];
				m_fov_radius = config::aimbot::fov_radius[ weapon_config ];
				m_deadzone = config::aimbot::deadzone[ weapon_config ];
				memcpy( m_aim_prediction, config::aimbot::aim_prediction[ weapon_config ], sizeof( m_aim_prediction ) );
				m_smoothing = config::aimbot::smoothing[ weapon_config ];
				memcpy( m_aim_bone, config::aimbot::aim_bone[ weapon_config ], sizeof( m_aim_bone ) );
				m_aim_key = config::aimbot::aim_key[ weapon_config ];
				m_restriction = config::aimbot::restriction[ weapon_config ];
				m_smart_targeting = config::aimbot::smart_targeting[ weapon_config ];
				m_ignore_downed = config::aimbot::ignore_downed[ weapon_config ];
				m_ignore_bots = config::aimbot::ignore_bots[ weapon_config ];
				m_only_visible = config::aimbot::only_visible[ weapon_config ];
				m_aim_acceleration = config::aimbot::aim_acceleration[ weapon_config ];
				m_max_velocity = config::aimbot::max_velocity[ weapon_config ];
				m_acceleration = config::aimbot::acceleration[ weapon_config ];
				m_deceleration = config::aimbot::deceleration[ weapon_config ];
				m_velocity_threshold = config::aimbot::velocity_threshold[ weapon_config ];
				m_max_distance = config::aimbot::max_distance[ weapon_config ];
			}
		}
	};
}