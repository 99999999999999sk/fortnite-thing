#pragma once

namespace config {
	namespace aimbot {
		const int max_weapon_configs = 5;
		const int max_close_aim = 6;
		int weapon_configs = 0;

		bool enabled = true;
		bool close_aim = false;
		bool shotgun_only = true;
		bool linear_aim[ max_close_aim ] = { false, false, false, false, false, false };
		bool no_target_switch[ max_close_aim ] = { true, true, true, true, true, true };
		bool inverted_vertical_axis[ max_close_aim ] = { false, false, false, false, false, false };
		bool fov_circle[ max_close_aim ] = { true, true, true, true, true, true };
		bool target_line[ max_weapon_configs ] = { true, true, true, true, true };
		bool target_dot[ max_weapon_configs ] = { false, false, false, false, false };
		bool aim_curve[ max_weapon_configs ] = { true, true, true, true, true };
		float fov_radius[ max_close_aim ] = { 80, 80, 80, 80, 80, 120 };
		bool deadzone[ max_close_aim ] = { true, true, true, true, true, true };
		bool aim_prediction[ max_weapon_configs ][ 5 ] = { {false}, {false}, {false}, {false}, {false} };
		int smoothing[ max_close_aim ] = { 7, 7, 7, 7, 7, 5 };
		bool aim_bone[ max_close_aim ][ 5 ] = {
			{true, false, false, false, false},
			{true, false, false, false, false},
			{true, false, false, false, false},
			{true, false, false, false, false},
			{true, false, false, false, false},
			{true, false, false, false, false}
		};
		int aim_key[ max_close_aim ] = { VK_RBUTTON, VK_RBUTTON, VK_RBUTTON, VK_RBUTTON, VK_RBUTTON, VK_RBUTTON };
		float restriction[ max_close_aim ] = { 6, 6, 6, 6, 6, 6 };
		bool smart_targeting[ max_weapon_configs ] = { true, true, true, true, true };
		bool override_general[ max_weapon_configs ] = { false, false, false, false, false };
		bool ignore_downed[ max_weapon_configs ] = { true, true, true, true, true };
		bool ignore_bots[ max_weapon_configs ] = { false, false, false, false, false };
		bool only_visible[ max_weapon_configs ] = { false, false, false, false, false };
		bool aim_acceleration[ max_weapon_configs ] = { true, true, true, true, true };
		float max_velocity[ max_weapon_configs ] = { 100.0f, 100.0f, 100.0f, 100.0f, 100.0f };
		float acceleration[ max_weapon_configs ] = { 50.0f, 50.0f, 50.0f, 50.0f, 50.0f };
		float deceleration[ max_weapon_configs ] = { 30.0f, 30.0f, 30.0f, 30.0f, 30.0f };
		float velocity_threshold[ max_weapon_configs ] = { 0.2f, 0.2f, 0.2f, 0.2f, 0.2f };
		int max_distance[ max_close_aim ] = { 300, 300, 300, 300, 300, 300 };
	}

	namespace triggerbot {
		bool enabled = false;
	}

	namespace visuals {
		bool enabled = true;
		bool full_box = true;
		bool corner_box = false;
		bool threed_box = false;
		bool filled = true;
		bool skeleton = false;
		bool snaplines = false;
		bool weapon_name = true;
		bool display_name = true;
		bool season_rank = false;
		bool platform = true;
		bool level = true;
		bool targeting_bar = true;
		bool damage = false;
		bool movement = true;
		bool kills = true;
		bool wounded = true;
		bool fov_arrows = true;
		bool view_line = false;
		bool stencil = false;
		bool movement_tracers = true;
		float max_distance = 200.f;
	}

	namespace world {
		bool enabled = true;
		bool loot_tier[ 5 ] = { true, true, true, true, true };
		bool mark_pickups = false;
		bool show_distance = true;
		float max_distance = 100.f;
		int max_pickups = 300;
	}

	namespace radar {
		bool enabled = true;
		bool distance = true;
		float opacity = 150;
		int position_x = 100;
		int position_y = 500;
		int size = 220;
		int range = 100;
		bool grid = true;
		int grid_divisions = 8;
		bool line_of_sight = false;
		bool los_fill = true;
		bool rotation_mode = true;
		bool show_pickups = false;
	}

	namespace misc {
		bool enabled = true;
		bool vsync = false;
		bool inventory = false;
		bool spectators = true;
		bool tracers = false;
		bool performance_counter = true;
		bool controller_support = false;
		bool stream_proof = false;
		bool bullet_tracers = true;
		int max_tracers = 5;
		float fadeout_time = 2.0f;
	}

	namespace configs {
		bool enabled = true;
	}

	namespace exploits {
	}

	namespace debug {
		float box_width = 300.0f;
		float box_spacing = 10.0f;
		float start_y = 50.0f;

		ImColor bg_color = ImColor( 30, 30, 35, 240 );
		ImColor border_color = ImColor( 70, 70, 80, 255 );
		ImColor inactive_color = ImColor( 50, 50, 55, 180 );
		ImColor entry_bg_color = ImColor( 40, 40, 45, 120 );
		ImColor accent_color = ImColor( 100, 150, 255, 255 );
		ImColor header_bg_top = ImColor( 60, 60, 70, 255 );
		ImColor header_bg_bottom = ImColor( 45, 45, 55, 255 );
		ImColor header_color = ImColor( 255, 255, 255, 255 );
		ImColor text_color = ImColor( 255, 255, 255, 255 );
		ImColor text_secondary_color = ImColor( 180, 180, 190, 255 );
		float event_timeout = 5.0f;
	}

	void apply( ) {
		world::loot_tier[ 0 ] = true;
		world::loot_tier[ 1 ] = true;
		world::loot_tier[ 2 ] = true;
		world::loot_tier[ 3 ] = true;
		world::loot_tier[ 4 ] = true;
	}
}