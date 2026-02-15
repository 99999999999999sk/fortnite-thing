#pragma once

namespace tracers {
    struct tracer_t {
        engine::f_vector m_position;
        std::chrono::steady_clock::time_point m_timestamp;
    };

    class c_tracers {
    private:
        std::vector<tracer_t> m_traces;
        const float m_max_age = 2.5f;

    public:
        void add_trace( const engine::f_vector& position ) {
            auto now = std::chrono::steady_clock::now( );
            m_traces.emplace_back( std::move( tracer_t{ position, now } ) );

            cleanup( );
        }

        void cleanup( ) {
            auto now = std::chrono::steady_clock::now( );
            m_traces.erase(
                std::remove_if( m_traces.begin( ), m_traces.end( ),
                    [ & ]( const tracer_t& point ) {
                        auto age = std::chrono::duration_cast< std::chrono::duration<float> >(
                            now - point.m_timestamp ).count( );
                        return age > m_max_age;
                    }
                ),
                m_traces.end( )
            );
        }

        const std::vector<tracer_t>& get_traces( ) const {
            return m_traces;
        }
    };
}