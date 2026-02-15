namespace engine {
    bool f_vector2d::in_rect(
        double radius
    ) const {
        return g_drawing->m_width_center >= ( g_drawing->m_width_center - radius ) && g_drawing->m_width_center <= ( g_drawing->m_width_center + radius ) &&
            y >= ( y - radius ) && y <= ( y + radius );
    }

    bool f_vector2d::in_circle(
        double radius
    ) const {
        if ( in_rect( radius ) ) {
            auto dx = g_drawing->m_width_center - x; dx *= dx;
            auto dy = g_drawing->m_height_center - y; dy *= dy;
            return dx + dy <= radius * radius;
        } return false;
    }

    bool f_vector2d::in_screen( ) const {
        if ( this->x < g_drawing->m_width && this->y < g_drawing->m_height )
            return true;
        return false;
    }

    f_rotator find_look_at_rotation( f_vector& start, f_vector& target ) {
        f_vector direction = target - start;
        direction = direction / sqrt( direction.length( ) );

        auto yaw = atan2( direction.y, direction.x ) * ( 180.0 / std::numbers::pi );
        auto pitch = atan2( direction.z, sqrt( direction.x * direction.x + direction.y * direction.y ) ) * ( 180.0 / std::numbers::pi );

        return f_rotator( pitch, yaw, 0.0 );
    }

    f_matrix get_rotation_matrix( const f_rotator& rotation ) {
        f_matrix matrix = {};

        const double pitch = rotation.pitch * std::numbers::pi / 180.0;
        const double yaw = rotation.yaw * std::numbers::pi / 180.0;
        const double roll = rotation.roll * std::numbers::pi / 180.0;

        const double sp = sin( pitch );
        const double cp = cos( pitch );
        const double sy = sin( yaw );
        const double cy = cos( yaw );
        const double sr = sin( roll );
        const double cr = cos( roll );

        matrix.x_plane.x = cp * cy;
        matrix.x_plane.y = cp * sy;
        matrix.x_plane.z = sp;

        matrix.y_plane.x = sr * sp * cy - cr * sy;
        matrix.y_plane.y = sr * sp * sy + cr * cy;
        matrix.y_plane.z = -sr * cp;

        matrix.z_plane.x = -( cr * sp * cy + sr * sy );
        matrix.z_plane.y = cy * sr - cr * sp * sy;
        matrix.z_plane.z = cr * cp;

        matrix.w_plane.w = 1.0;

        return matrix;
    }

    f_matrix to_rotation_matrix( f_rotator& rotation ) {
        f_matrix matrix = {};

        auto rad_pitch = ( rotation.pitch * std::numbers::pi / 180.f );
        auto rad_yaw = ( rotation.yaw * std::numbers::pi / 180.f );
        auto rad_roll = ( rotation.roll * std::numbers::pi / 180.f );

        auto sin_pitch = sin( rad_pitch );
        auto cos_pitch = cos( rad_pitch );

        auto sin_yaw = sin( rad_yaw );
        auto cos_yaw = cos( rad_yaw );

        auto sin_roll = sin( rad_roll );
        auto cos_roll = cos( rad_roll );

        matrix.x_plane.x = cos_pitch * cos_yaw;
        matrix.x_plane.y = cos_pitch * sin_yaw;
        matrix.x_plane.z = sin_pitch;
        matrix.x_plane.w = 0.f;

        matrix.y_plane.x = sin_roll * sin_pitch * cos_yaw - cos_roll * sin_yaw;
        matrix.y_plane.y = sin_roll * sin_pitch * sin_yaw + cos_roll * cos_yaw;
        matrix.y_plane.z = -sin_roll * cos_pitch;
        matrix.y_plane.w = 0.f;

        matrix.z_plane.x = -( cos_roll * sin_pitch * cos_yaw + sin_roll * sin_yaw );
        matrix.z_plane.y = cos_yaw * sin_roll - cos_roll * sin_pitch * sin_yaw;
        matrix.z_plane.z = cos_roll * cos_pitch;
        matrix.z_plane.w = 0.f;

        matrix.w_plane.w = 1.f;
        return matrix;
    }

    f_vector2d project( f_vector& world_location ) {
        auto matrix = to_rotation_matrix( g_world->m_camera_rotation );

        auto axis_x = f_vector( matrix.x_plane.x, matrix.x_plane.y, matrix.x_plane.z );
        auto axis_y = f_vector( matrix.y_plane.x, matrix.y_plane.y, matrix.y_plane.z );
        auto axis_z = f_vector( matrix.z_plane.x, matrix.z_plane.y, matrix.z_plane.z );

        auto delta = (
            world_location - g_world->m_camera_location
            );

        auto transform = f_vector(
            delta.vector_scalar( axis_y ),
            delta.vector_scalar( axis_z ),
            delta.vector_scalar( axis_x )
        );

        transform.z = ( transform.z < 1.f ) ? 1.f : transform.z;

        auto fov_radians = tan( g_world->m_camera_fov * std::numbers::pi / 360.f );

        return f_vector2d(
            g_drawing->m_width_center + transform.x * ( g_drawing->m_width_center / fov_radians ) / transform.z,
            g_drawing->m_height_center - transform.y * ( g_drawing->m_width_center / fov_radians ) / transform.z
        );
    }

    f_vector2d world_to_screen( f_vector world_location ) {
        auto matrix = get_rotation_matrix( g_world->m_camera_rotation );

        f_vector x_axis = {
            matrix.x_plane.x,
            matrix.x_plane.y,
            matrix.x_plane.z
        };

        f_vector y_axis = {
            matrix.y_plane.x,
            matrix.y_plane.y,
            matrix.y_plane.z
        };

        f_vector z_axis = {
            matrix.z_plane.x,
            matrix.z_plane.y,
            matrix.z_plane.z
        };

        auto delta = world_location - g_world->m_camera_location;

        f_vector transform = {
            delta.vector_scalar( y_axis ),
            delta.vector_scalar( z_axis ),
            delta.vector_scalar( x_axis )
        };

        if ( transform.z < 0.01 )
            transform.z = 0.01;

        double fov_multiplier = g_drawing->m_width_center / tan( g_world->m_camera_fov * std::numbers::pi / 360.0 );
        return f_vector2d(
            g_drawing->m_width_center + transform.x * fov_multiplier / transform.z,
            g_drawing->m_height_center - transform.y * fov_multiplier / transform.z
        );
    }

    f_vector deproject( f_vector2d& screen_position ) {
        auto fov_radians = std::tan( g_world->m_camera_fov * std::numbers::pi / 360.0 );
        auto view_space = f_vector(
            ( screen_position.x - g_drawing->m_width_center ) * fov_radians / g_drawing->m_width_center,
            ( g_drawing->m_height_center - screen_position.y ) * fov_radians / g_drawing->m_width_center,
            1.0
        );

        auto matrix = to_rotation_matrix( g_world->m_camera_rotation );

        auto axis_x = f_vector( matrix.x_plane.x, matrix.x_plane.y, matrix.x_plane.z );
        auto axis_y = f_vector( matrix.y_plane.x, matrix.y_plane.y, matrix.y_plane.z );
        auto axis_z = f_vector( matrix.z_plane.x, matrix.z_plane.y, matrix.z_plane.z );

        auto world_dir = axis_y * view_space.x + axis_z * view_space.y + axis_x * view_space.z;
        return world_dir.normalized( );
    }
}