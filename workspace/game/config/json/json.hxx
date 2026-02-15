#pragma once

namespace config {
	namespace json {
		using json = nlohmann::json;

		json to_json( ) {
			json j;
			j[ "aimbot" ] = {
				{ "enabled", aimbot::enabled },
				{ "weapon_configs", aimbot::weapon_configs },
				{ "close_aim", aimbot::close_aim },
				{ "shotgun_only", aimbot::shotgun_only },
				{ "linear_aim", { aimbot::linear_aim[ 0 ], aimbot::linear_aim[ 1 ], aimbot::linear_aim[ 2 ], aimbot::linear_aim[ 3 ], aimbot::linear_aim[ 4 ], aimbot::linear_aim[ 5 ], aimbot::linear_aim[ 6 ] } },
				{ "no_target_switch", { aimbot::no_target_switch[ 0 ], aimbot::no_target_switch[ 1 ], aimbot::no_target_switch[ 2 ], aimbot::no_target_switch[ 3 ], aimbot::no_target_switch[ 4 ], aimbot::no_target_switch[ 5 ], aimbot::no_target_switch[ 6 ] } },
				{ "inverted_vertical_axis", { aimbot::inverted_vertical_axis[ 0 ], aimbot::inverted_vertical_axis[ 1 ], aimbot::inverted_vertical_axis[ 2 ], aimbot::inverted_vertical_axis[ 3 ], aimbot::inverted_vertical_axis[ 4 ], aimbot::inverted_vertical_axis[ 5 ], aimbot::inverted_vertical_axis[ 6 ] } },
				{ "fov_circle", { aimbot::fov_circle[ 0 ], aimbot::fov_circle[ 1 ], aimbot::fov_circle[ 2 ], aimbot::fov_circle[ 3 ], aimbot::fov_circle[ 4 ], aimbot::fov_circle[ 5 ], aimbot::fov_circle[ 6 ] } },
				{ "target_line", { aimbot::target_line[ 0 ], aimbot::target_line[ 1 ], aimbot::target_line[ 2 ], aimbot::target_line[ 3 ], aimbot::target_line[ 4 ], aimbot::target_line[ 5 ] } },
				{ "target_dot", { aimbot::target_dot[ 0 ], aimbot::target_dot[ 1 ], aimbot::target_dot[ 2 ], aimbot::target_dot[ 3 ], aimbot::target_dot[ 4 ], aimbot::target_dot[ 5 ] } },
				{ "aim_curve", { aimbot::aim_curve[ 0 ], aimbot::aim_curve[ 1 ], aimbot::aim_curve[ 2 ], aimbot::aim_curve[ 3 ], aimbot::aim_curve[ 4 ], aimbot::aim_curve[ 5 ] } },
				{ "fov_radius", { aimbot::fov_radius[ 0 ], aimbot::fov_radius[ 1 ], aimbot::fov_radius[ 2 ], aimbot::fov_radius[ 3 ], aimbot::fov_radius[ 4 ], aimbot::fov_radius[ 5 ], aimbot::fov_radius[ 6 ] } },
				{ "deadzone", { aimbot::deadzone[ 0 ], aimbot::deadzone[ 1 ], aimbot::deadzone[ 2 ], aimbot::deadzone[ 3 ], aimbot::deadzone[ 4 ], aimbot::deadzone[ 5 ], aimbot::deadzone[ 6 ] } },
				{ "aim_prediction", {
					{ aimbot::aim_prediction[ 0 ][ 0 ], aimbot::aim_prediction[ 0 ][ 1 ] },
					{ aimbot::aim_prediction[ 1 ][ 0 ], aimbot::aim_prediction[ 1 ][ 1 ] },
					{ aimbot::aim_prediction[ 2 ][ 0 ], aimbot::aim_prediction[ 2 ][ 1 ] },
					{ aimbot::aim_prediction[ 3 ][ 0 ], aimbot::aim_prediction[ 3 ][ 1 ] },
					{ aimbot::aim_prediction[ 4 ][ 0 ], aimbot::aim_prediction[ 4 ][ 1 ] },
					{ aimbot::aim_prediction[ 5 ][ 0 ], aimbot::aim_prediction[ 5 ][ 1 ] }
				} },
				{ "smoothing", { aimbot::smoothing[ 0 ], aimbot::smoothing[ 1 ], aimbot::smoothing[ 2 ], aimbot::smoothing[ 3 ], aimbot::smoothing[ 4 ], aimbot::smoothing[ 5 ], aimbot::smoothing[ 6 ] } },
				{ "aim_bone", {
					{ aimbot::aim_bone[ 0 ][ 0 ], aimbot::aim_bone[ 0 ][ 1 ], aimbot::aim_bone[ 0 ][ 2 ], aimbot::aim_bone[ 0 ][ 3 ] },
					{ aimbot::aim_bone[ 1 ][ 0 ], aimbot::aim_bone[ 1 ][ 1 ], aimbot::aim_bone[ 1 ][ 2 ], aimbot::aim_bone[ 1 ][ 3 ] },
					{ aimbot::aim_bone[ 2 ][ 0 ], aimbot::aim_bone[ 2 ][ 1 ], aimbot::aim_bone[ 2 ][ 2 ], aimbot::aim_bone[ 2 ][ 3 ] },
					{ aimbot::aim_bone[ 3 ][ 0 ], aimbot::aim_bone[ 3 ][ 1 ], aimbot::aim_bone[ 3 ][ 2 ], aimbot::aim_bone[ 3 ][ 3 ] },
					{ aimbot::aim_bone[ 4 ][ 0 ], aimbot::aim_bone[ 4 ][ 1 ], aimbot::aim_bone[ 4 ][ 2 ], aimbot::aim_bone[ 4 ][ 3 ] },
					{ aimbot::aim_bone[ 5 ][ 0 ], aimbot::aim_bone[ 5 ][ 1 ], aimbot::aim_bone[ 5 ][ 2 ], aimbot::aim_bone[ 5 ][ 3 ] },
					{ aimbot::aim_bone[ 6 ][ 0 ], aimbot::aim_bone[ 6 ][ 1 ], aimbot::aim_bone[ 6 ][ 2 ], aimbot::aim_bone[ 6 ][ 3 ] }
				} },
				{ "aim_key", { aimbot::aim_key[ 0 ], aimbot::aim_key[ 1 ], aimbot::aim_key[ 2 ], aimbot::aim_key[ 3 ], aimbot::aim_key[ 4 ], aimbot::aim_key[ 5 ], aimbot::aim_key[ 6 ] } },
				{ "restriction", { aimbot::restriction[ 0 ], aimbot::restriction[ 1 ], aimbot::restriction[ 2 ], aimbot::restriction[ 3 ], aimbot::restriction[ 4 ], aimbot::restriction[ 5 ], aimbot::restriction[ 6 ] } },
				{ "smart_targeting", { aimbot::smart_targeting[ 0 ], aimbot::smart_targeting[ 1 ], aimbot::smart_targeting[ 2 ], aimbot::smart_targeting[ 3 ], aimbot::smart_targeting[ 4 ], aimbot::smart_targeting[ 5 ] } },
				{ "override_general", { aimbot::override_general[ 0 ], aimbot::override_general[ 1 ], aimbot::override_general[ 2 ], aimbot::override_general[ 3 ], aimbot::override_general[ 4 ], aimbot::override_general[ 5 ] } },
				{ "ignore_downed", { aimbot::ignore_downed[ 0 ], aimbot::ignore_downed[ 1 ], aimbot::ignore_downed[ 2 ], aimbot::ignore_downed[ 3 ], aimbot::ignore_downed[ 4 ], aimbot::ignore_downed[ 5 ] } },
				{ "ignore_bots", { aimbot::ignore_bots[ 0 ], aimbot::ignore_bots[ 1 ], aimbot::ignore_bots[ 2 ], aimbot::ignore_bots[ 3 ], aimbot::ignore_bots[ 4 ], aimbot::ignore_bots[ 5 ] } },
				{ "only_visible", { aimbot::only_visible[ 0 ], aimbot::only_visible[ 1 ], aimbot::only_visible[ 2 ], aimbot::only_visible[ 3 ], aimbot::only_visible[ 4 ], aimbot::only_visible[ 5 ] } },
				{ "aim_acceleration", { aimbot::aim_acceleration[ 0 ], aimbot::aim_acceleration[ 1 ], aimbot::aim_acceleration[ 2 ], aimbot::aim_acceleration[ 3 ], aimbot::aim_acceleration[ 4 ], aimbot::aim_acceleration[ 5 ] } },
				{ "max_velocity", { aimbot::max_velocity[ 0 ], aimbot::max_velocity[ 1 ], aimbot::max_velocity[ 2 ], aimbot::max_velocity[ 3 ], aimbot::max_velocity[ 4 ], aimbot::max_velocity[ 5 ] } },
				{ "acceleration", { aimbot::acceleration[ 0 ], aimbot::acceleration[ 1 ], aimbot::acceleration[ 2 ], aimbot::acceleration[ 3 ], aimbot::acceleration[ 4 ], aimbot::acceleration[ 5 ] } },
				{ "deceleration", { aimbot::deceleration[ 0 ], aimbot::deceleration[ 1 ], aimbot::deceleration[ 2 ], aimbot::deceleration[ 3 ], aimbot::deceleration[ 4 ], aimbot::deceleration[ 5 ] } },
				{ "velocity_threshold", { aimbot::velocity_threshold[ 0 ], aimbot::velocity_threshold[ 1 ], aimbot::velocity_threshold[ 2 ], aimbot::velocity_threshold[ 3 ], aimbot::velocity_threshold[ 4 ], aimbot::velocity_threshold[ 5 ] } },
				{ "max_distance", { aimbot::max_distance[ 0 ], aimbot::max_distance[ 1 ], aimbot::max_distance[ 2 ], aimbot::max_distance[ 3 ], aimbot::max_distance[ 4 ], aimbot::max_distance[ 5 ], aimbot::max_distance[ 6 ] } }
			};

			j[ "triggerbot" ] = {
				{ "enabled", triggerbot::enabled },
			};

			j[ "visuals" ] = {
				{ "enabled", visuals::enabled },
				{ "full_box", visuals::full_box },
				{ "corner_box", visuals::corner_box },
				{ "threed_box", visuals::threed_box },
				{ "filled", visuals::filled },
				{ "skeleton", visuals::skeleton },
				{ "snaplines", visuals::snaplines },
				{ "weapon_name", visuals::weapon_name },
				{ "display_name", visuals::display_name },
				{ "season_rank", visuals::season_rank },
				{ "platform", visuals::platform },
				{ "level", visuals::level },
				{ "targeting_bar", visuals::targeting_bar },
				{ "damage", visuals::damage },
				{ "movement", visuals::movement },
				{ "kills", visuals::kills },
				{ "wounded", visuals::wounded },
				{ "fov_arrows", visuals::fov_arrows },
				{ "view_line", visuals::view_line },
				{ "stencil", visuals::stencil },
				{ "movement_tracers", visuals::movement_tracers },
				{ "max_distance", visuals::max_distance }
			};

			j[ "world" ] = {
				{ "enabled", world::enabled },
				{ "loot_tier", { world::loot_tier[ 0 ], world::loot_tier[ 1 ], world::loot_tier[ 2 ], world::loot_tier[ 3 ], world::loot_tier[ 4 ] } },
				{ "mark_pickups", world::mark_pickups },
				{ "show_distance", world::show_distance },
				{ "max_distance", world::max_distance },
				{ "max_pickups", world::max_pickups }
			};

			j[ "radar" ] = {
				{ "enabled", radar::enabled },
				{ "distance", radar::distance },
				{ "opacity", radar::opacity },
				{ "position_x", radar::position_x },
				{ "position_y", radar::position_y },
				{ "size", radar::size },
				{ "range", radar::range },
				{ "grid", radar::grid },
				{ "grid_divisions", radar::grid_divisions },
				{ "line_of_sight", radar::line_of_sight },
				{ "los_fill", radar::los_fill },
				{ "rotation_mode", radar::rotation_mode },
				{ "show_pickups", radar::show_pickups }
			};

			j[ "misc" ] = {
				{ "enabled", misc::enabled },
				{ "vsync", misc::vsync },
				{ "inventory", misc::inventory },
				{ "spectators", misc::spectators },
				{ "tracers", misc::tracers },
				{ "performance_counter", misc::performance_counter },
				{ "stream_proof", misc::stream_proof },
				{ "bullet_tracers", misc::bullet_tracers },
				{ "max_tracers", misc::max_tracers },
				{ "fadeout_time", misc::fadeout_time }
			};

			return j;
		}

		bool from_json( const json& j ) {
			try {
				if ( j.contains( "aimbot" ) ) {
					const auto& a = j[ "aimbot" ];
					aimbot::enabled = a[ "enabled" ];
					aimbot::weapon_configs = a[ "weapon_configs" ];
					aimbot::close_aim = a[ "close_aim" ];
					aimbot::shotgun_only = a[ "shotgun_only" ];

					const auto& linear_aim = a[ "linear_aim" ];
					if ( linear_aim.is_array( ) && linear_aim.size( ) >= 7 ) {
						for ( int i = 0; i < 7; i++ ) {
							aimbot::linear_aim[ i ] = linear_aim[ i ];
						}
					}

					const auto& no_target_switch = a[ "no_target_switch" ];
					if ( no_target_switch.is_array( ) && no_target_switch.size( ) >= 7 ) {
						for ( int i = 0; i < 7; i++ ) {
							aimbot::no_target_switch[ i ] = no_target_switch[ i ];
						}
					}

					const auto& inverted_vertical_axis = a[ "inverted_vertical_axis" ];
					if ( inverted_vertical_axis.is_array( ) && inverted_vertical_axis.size( ) >= 7 ) {
						for ( int i = 0; i < 7; i++ ) {
							aimbot::inverted_vertical_axis[ i ] = inverted_vertical_axis[ i ];
						}
					}

					const auto& fov_circle = a[ "fov_circle" ];
					if ( fov_circle.is_array( ) && fov_circle.size( ) >= 7 ) {
						for ( int i = 0; i < 7; i++ ) {
							aimbot::fov_circle[ i ] = fov_circle[ i ];
						}
					}

					const auto& target_line = a[ "target_line" ];
					if ( target_line.is_array( ) && target_line.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::target_line[ i ] = target_line[ i ];
						}
					}

					const auto& target_dot = a[ "target_dot" ];
					if ( target_dot.is_array( ) && target_dot.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::target_dot[ i ] = target_dot[ i ];
						}
					}

					const auto& aim_curve = a[ "aim_curve" ];
					if ( aim_curve.is_array( ) && aim_curve.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::aim_curve[ i ] = aim_curve[ i ];
						}
					}

					const auto& fov_radius = a[ "fov_radius" ];
					if ( fov_radius.is_array( ) && fov_radius.size( ) >= 7 ) {
						for ( int i = 0; i < 7; i++ ) {
							aimbot::fov_radius[ i ] = fov_radius[ i ];
						}
					}

					const auto& deadzone = a[ "deadzone" ];
					if ( deadzone.is_array( ) && deadzone.size( ) >= 7 ) {
						for ( int i = 0; i < 7; i++ ) {
							aimbot::deadzone[ i ] = deadzone[ i ];
						}
					}

					const auto& aim_prediction = a[ "aim_prediction" ];
					if ( aim_prediction.is_array( ) && aim_prediction.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							const auto& pred = aim_prediction[ i ];
							if ( pred.is_array( ) && pred.size( ) >= 2 ) {
								aimbot::aim_prediction[ i ][ 0 ] = pred[ 0 ];
								aimbot::aim_prediction[ i ][ 1 ] = pred[ 1 ];
							}
						}
					}

					const auto& smoothing = a[ "smoothing" ];
					if ( smoothing.is_array( ) && smoothing.size( ) >= 7 ) {
						for ( int i = 0; i < 7; i++ ) {
							aimbot::smoothing[ i ] = smoothing[ i ];
						}
					}

					const auto& aim_bone = a[ "aim_bone" ];
					if ( aim_bone.is_array( ) && aim_bone.size( ) >= 7 ) {
						for ( int i = 0; i < 7; i++ ) {
							const auto& bones = aim_bone[ i ];
							if ( bones.is_array( ) && bones.size( ) >= 4 ) {
								for ( int j = 0; j < 4; j++ ) {
									aimbot::aim_bone[ i ][ j ] = bones[ j ];
								}
							}
						}
					}

					const auto& aim_key = a[ "aim_key" ];
					if ( aim_key.is_array( ) && aim_key.size( ) >= 7 ) {
						for ( int i = 0; i < 7; i++ ) {
							aimbot::aim_key[ i ] = aim_key[ i ];
						}
					}

					const auto& restriction = a[ "restriction" ];
					if ( restriction.is_array( ) && restriction.size( ) >= 7 ) {
						for ( int i = 0; i < 7; i++ ) {
							aimbot::restriction[ i ] = restriction[ i ];
						}
					}

					const auto& smart_targeting = a[ "smart_targeting" ];
					if ( smart_targeting.is_array( ) && smart_targeting.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::smart_targeting[ i ] = smart_targeting[ i ];
						}
					}

					const auto& override_general = a[ "override_general" ];
					if ( override_general.is_array( ) && override_general.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::override_general[ i ] = override_general[ i ];
						}
					}

					const auto& ignore_downed = a[ "ignore_downed" ];
					if ( ignore_downed.is_array( ) && ignore_downed.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::ignore_downed[ i ] = ignore_downed[ i ];
						}
					}

					const auto& ignore_bots = a[ "ignore_bots" ];
					if ( ignore_bots.is_array( ) && ignore_bots.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::ignore_bots[ i ] = ignore_bots[ i ];
						}
					}

					const auto& only_visible = a[ "only_visible" ];
					if ( only_visible.is_array( ) && only_visible.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::only_visible[ i ] = only_visible[ i ];
						}
					}

					const auto& aim_acceleration = a[ "aim_acceleration" ];
					if ( aim_acceleration.is_array( ) && aim_acceleration.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::aim_acceleration[ i ] = aim_acceleration[ i ];
						}
					}

					const auto& max_velocity = a[ "max_velocity" ];
					if ( max_velocity.is_array( ) && max_velocity.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::max_velocity[ i ] = max_velocity[ i ];
						}
					}

					const auto& acceleration = a[ "acceleration" ];
					if ( acceleration.is_array( ) && acceleration.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::acceleration[ i ] = acceleration[ i ];
						}
					}

					const auto& deceleration = a[ "deceleration" ];
					if ( deceleration.is_array( ) && deceleration.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::deceleration[ i ] = deceleration[ i ];
						}
					}

					const auto& velocity_threshold = a[ "velocity_threshold" ];
					if ( velocity_threshold.is_array( ) && velocity_threshold.size( ) >= 6 ) {
						for ( int i = 0; i < 6; i++ ) {
							aimbot::velocity_threshold[ i ] = velocity_threshold[ i ];
						}
					}

					const auto& max_distance = a[ "max_distance" ];
					if ( max_distance.is_array( ) && max_distance.size( ) >= 7 ) {
						for ( int i = 0; i < 7; i++ ) {
							aimbot::max_distance[ i ] = max_distance[ i ];
						}
					}
				}

				if ( j.contains( "triggerbot" ) ) {
					const auto& t = j[ "triggerbot" ];
					triggerbot::enabled = t[ "triggerbot" ];
				}

				if ( j.contains( "visuals" ) ) {
					const auto& v = j[ "visuals" ];
					visuals::enabled = v[ "enabled" ];
					visuals::full_box = v[ "full_box" ];
					visuals::corner_box = v[ "corner_box" ];
					visuals::threed_box = v[ "threed_box" ];
					visuals::filled = v[ "filled" ];
					visuals::skeleton = v[ "skeleton" ];
					visuals::snaplines = v[ "snaplines" ];
					visuals::weapon_name = v[ "weapon_name" ];
					visuals::display_name = v[ "display_name" ];
					visuals::season_rank = v[ "season_rank" ];
					visuals::platform = v[ "platform" ];
					visuals::level = v[ "level" ];
					visuals::targeting_bar = v[ "targeting_bar" ];
					visuals::damage = v[ "damage" ];
					visuals::movement = v[ "movement" ];
					visuals::kills = v[ "kills" ];
					visuals::wounded = v[ "wounded" ];
					visuals::fov_arrows = v[ "fov_arrows" ];
					visuals::view_line = v[ "view_line" ];
					visuals::stencil = v[ "stencil" ];
					visuals::movement_tracers = v[ "movement_tracers" ];
					visuals::max_distance = v[ "max_distance" ];
				}

				if ( j.contains( "world" ) ) {
					const auto& w = j[ "world" ];
					world::enabled = w[ "enabled" ];

					const auto& tiers = w[ "loot_tier" ];
					if ( tiers.is_array( ) && tiers.size( ) >= 5 ) {
						world::loot_tier[ 0 ] = tiers[ 0 ];
						world::loot_tier[ 1 ] = tiers[ 1 ];
						world::loot_tier[ 2 ] = tiers[ 2 ];
						world::loot_tier[ 3 ] = tiers[ 3 ];
						world::loot_tier[ 4 ] = tiers[ 4 ];
					}

					world::mark_pickups = w[ "mark_pickups" ];
					world::show_distance = w[ "show_distance" ];
					world::max_distance = w[ "max_distance" ];
					world::max_pickups = w[ "max_pickups" ];
				}

				if ( j.contains( "radar" ) ) {
					const auto& r = j[ "radar" ];
					radar::enabled = r[ "enabled" ];
					radar::distance = r[ "distance" ];
					radar::opacity = r[ "opacity" ];
					radar::position_x = r[ "position_x" ];
					radar::position_y = r[ "position_y" ];
					radar::size = r[ "size" ];
					radar::range = r[ "range" ];
					radar::grid = r[ "grid" ];
					radar::grid_divisions = r[ "grid_divisions" ];
					radar::line_of_sight = r[ "line_of_sight" ];
					radar::los_fill = r[ "los_fill" ];
					radar::rotation_mode = r[ "rotation_mode" ];
					radar::show_pickups = r[ "show_pickups" ];
				}

				if ( j.contains( "misc" ) ) {
					const auto& m = j[ "misc" ];
					misc::enabled = m[ "enabled" ];
					misc::vsync = m[ "vsync" ];
					misc::inventory = m[ "inventory" ];
					misc::spectators = m[ "spectators" ];
					misc::tracers = m[ "tracers" ];
					misc::performance_counter = m[ "performance_counter" ];
					misc::stream_proof = m[ "stream_proof" ];
					misc::bullet_tracers = m[ "bullet_tracers" ];
					misc::max_tracers = m[ "max_tracers" ];
					misc::fadeout_time = m[ "fadeout_time" ];
				}

				return true;
			}
			catch ( const std::exception& e ) {
				return false;
			}
		}

		bool save( const std::string& filename ) {
			try {
				std::ofstream file( filename );
				if ( !file.is_open( ) ) return false;

				json j = to_json( );
				file << j.dump( 4 );
				return true;
			}
			catch ( const std::exception& e ) {
				return false;
			}
		}

		bool load( const std::string& filename ) {
			try {
				std::ifstream file( filename );
				if ( !file.is_open( ) ) return false;

				json j;
				file >> j;
				return from_json( j );
			}
			catch ( const std::exception& e ) {
				return false;
			}
		}
	}

	class c_config {
	public:
		c_config( ) { }
		c_config( const c_config& ) = delete;
		c_config& operator=( const c_config& ) = delete;

		std::vector<std::string> get_configs( ) {
			return m_available_configs;
		}

		void get_available_configs( ) {
			m_available_configs.clear( );
			std::filesystem::path config_dir = get_config_directory( );
			if ( !std::filesystem::exists( config_dir ) ) {
				std::filesystem::create_directories( config_dir );
				return;
			}

			for ( const auto& entry : std::filesystem::directory_iterator( config_dir ) ) {
				if ( entry.path( ).extension( ) == ".json" ) {
					m_available_configs.push_back( entry.path( ).stem( ).string( ) );
				}
			}
		}

		bool save_config( const std::string& name ) {
			std::filesystem::path config_dir = get_config_directory( );
			if ( !std::filesystem::exists( config_dir ) ) {
				std::filesystem::create_directories( config_dir );
			}
			std::filesystem::path config_path = config_dir / ( name + ".json" );
			return json::save( config_path.string( ) );
		}

		bool load_config( const std::string& name ) {
			std::filesystem::path config_path = get_config_directory( ) / ( name + ".json" );
			return json::load( config_path.string( ) );
		}

		bool delete_config( const std::string& name ) {
			std::filesystem::path config_path = get_config_directory( ) / ( name + ".json" );
			if ( std::filesystem::exists( config_path ) ) {
				return std::filesystem::remove( config_path );
			}
			return false;
		}

		bool rename_config( const std::string& old_name, const std::string& new_name ) {
			std::filesystem::path old_path = get_config_directory( ) / ( old_name + ".json" );
			std::filesystem::path new_path = get_config_directory( ) / ( new_name + ".json" );

			if ( std::filesystem::exists( old_path ) && !std::filesystem::exists( new_path ) ) {
				std::filesystem::rename( old_path, new_path );
				return true;
			}
			return false;
		}

		bool export_config( const std::string& name, const std::string& path ) {
			std::filesystem::path export_path = path;
			if ( export_path.extension( ) != ".json" ) {
				export_path += ".json";
			}
			return json::save( export_path.string( ) );
		}

		bool import_config( const std::string& path ) {
			std::filesystem::path import_path = path;
			if ( !std::filesystem::exists( import_path ) ) {
				return false;
			}
			return json::load( import_path.string( ) );
		}

		void reset( ) {
			aimbot::enabled = true;
			aimbot::weapon_configs = 0;
			aimbot::close_aim = false;
			aimbot::shotgun_only = true;

			for ( int i = 0; i < aimbot::max_weapon_configs; ++i ) {
				aimbot::target_line[ i ] = true;
				aimbot::target_dot[ i ] = false;
				aimbot::aim_curve[ i ] = true;
				aimbot::smart_targeting[ i ] = true;
				aimbot::override_general[ i ] = false;
				aimbot::ignore_downed[ i ] = true;
				aimbot::ignore_bots[ i ] = false;
				aimbot::only_visible[ i ] = false;
				aimbot::aim_acceleration[ i ] = true;
				aimbot::max_velocity[ i ] = 100.0f;
				aimbot::acceleration[ i ] = 50.0f;
				aimbot::deceleration[ i ] = 30.0f;
				aimbot::velocity_threshold[ i ] = 0.2f;
				for ( int j = 0; j < 5; ++j ) {
					aimbot::aim_prediction[ i ][ j ] = false;
				}
			}

			for ( int i = 0; i < aimbot::max_close_aim; ++i ) {
				aimbot::linear_aim[ i ] = false;
				aimbot::no_target_switch[ i ] = true;
				aimbot::inverted_vertical_axis[ i ] = false;
				aimbot::fov_circle[ i ] = true;
				aimbot::fov_radius[ i ] = 80;
				aimbot::deadzone[ i ] = true;
				aimbot::smoothing[ i ] = 7;
				aimbot::aim_key[ i ] = VK_RBUTTON;
				aimbot::restriction[ i ] = 6;
				aimbot::max_distance[ i ] = 300;
				for ( int j = 0; j < 5; ++j ) {
					aimbot::aim_bone[ i ][ j ] = ( j == 0 );
				}
			}

			aimbot::max_distance[ aimbot::max_close_aim - 1 ] = 35;

			triggerbot::enabled = false;

			visuals::enabled = true;
			visuals::full_box = true;
			visuals::corner_box = false;
			visuals::threed_box = false;
			visuals::filled = true;
			visuals::skeleton = false;
			visuals::snaplines = false;
			visuals::weapon_name = true;
			visuals::display_name = true;
			visuals::season_rank = false;
			visuals::platform = true;
			visuals::level = true;
			visuals::targeting_bar = true;
			visuals::damage = false;
			visuals::movement = true;
			visuals::kills = true;
			visuals::wounded = true;
			visuals::fov_arrows = true;
			visuals::view_line = false;
			visuals::stencil = false;
			visuals::movement_tracers = true;
			visuals::max_distance = 200.f;

			world::enabled = true;
			world::mark_pickups = false;
			world::show_distance = true;
			world::max_distance = 100.f;
			world::max_pickups = 300;
			for ( int i = 0; i < 5; i++ ) {
				world::loot_tier[ i ] = true;
			}

			radar::enabled = true;
			radar::distance = true;
			radar::opacity = 150;
			radar::position_x = 100;
			radar::position_y = 500;
			radar::size = 220;
			radar::range = 100;
			radar::grid = true;
			radar::grid_divisions = 8;
			radar::line_of_sight = false;
			radar::los_fill = true;
			radar::rotation_mode = true;
			radar::show_pickups = false;

			misc::enabled = true;
			misc::vsync = false;
			misc::inventory = false;
			misc::spectators = true;
			misc::tracers = false;
			misc::performance_counter = true;
			misc::stream_proof = false;
			misc::bullet_tracers = true;
			misc::max_tracers = 5;
			misc::fadeout_time = 2.0f;
		}

		void open_config_folder( ) {
			auto config_dir = get_config_directory( );
			ShellExecuteA( NULL, "open", config_dir.string( ).c_str( ), NULL, NULL, SW_SHOWNORMAL );
		}

	private:
		std::vector<std::string> m_available_configs;

		std::filesystem::path get_config_directory( ) {
			char exe_path[ MAX_PATH ];
			GetModuleFileNameA( NULL, exe_path, MAX_PATH );
			std::filesystem::path exe_dir = std::filesystem::path( exe_path ).parent_path( );

			return exe_dir / "configs";
		}
	};
}