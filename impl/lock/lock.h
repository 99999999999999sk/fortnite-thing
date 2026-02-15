class srw_lock_exception : public std::runtime_error {
public:
    explicit srw_lock_exception( const char* msg ) : std::runtime_error( msg ) { }
};

class srw_lock {
private:
    mutable SRWLOCK m_lock;

public:
    srw_lock( ) noexcept {
        InitializeSRWLock( &m_lock );
    }

    srw_lock( const srw_lock& ) = delete;
    srw_lock& operator=( const srw_lock& ) = delete;
    srw_lock( srw_lock&& ) = delete;
    srw_lock& operator=( srw_lock&& ) = delete;

    ~srw_lock( ) = default;

    void lock_shared( ) const noexcept {
        AcquireSRWLockShared( &m_lock );
    }

    void unlock_shared( ) const noexcept {
        ReleaseSRWLockShared( &m_lock );
    }

    void lock_exclusive( ) const noexcept {
        AcquireSRWLockExclusive( &m_lock );
    }

    void unlock_exclusive( ) const noexcept {
        ReleaseSRWLockExclusive( &m_lock );
    }

    bool try_lock_shared( ) const noexcept {
        return TryAcquireSRWLockShared( &m_lock ) != FALSE;
    }

    bool try_lock_exclusive( ) const noexcept {
        return TryAcquireSRWLockExclusive( &m_lock ) != FALSE;
    }

    template<typename Rep, typename Period>
    bool try_lock_shared_for( const std::chrono::duration<Rep, Period>& timeout ) const {
        auto start = std::chrono::steady_clock::now( );
        auto end = start + timeout;

        DWORD sleep_ms = 1;
        const DWORD max_sleep = 16;

        while ( std::chrono::steady_clock::now( ) < end ) {
            if ( try_lock_shared( ) ) {
                return true;
            }

            Sleep( sleep_ms );
            sleep_ms = min( sleep_ms * 2, max_sleep );
        }

        return false;
    }

    template<typename Rep, typename Period>
    bool try_lock_exclusive_for( const std::chrono::duration<Rep, Period>& timeout ) const {
        auto start = std::chrono::steady_clock::now( );
        auto end = start + timeout;

        DWORD sleep_ms = 1;
        const DWORD max_sleep = 16;

        while ( std::chrono::steady_clock::now( ) < end ) {
            if ( try_lock_exclusive( ) ) {
                return true;
            }

            Sleep( sleep_ms );
            sleep_ms = min( sleep_ms * 2, max_sleep );
        }

        return false;
    }

    SRWLOCK* native_handle( ) noexcept {
        return &m_lock;
    }

    const SRWLOCK* native_handle( ) const noexcept {
        return &m_lock;
    }
};

class srw_shared_lock {
private:
    const srw_lock* m_lock;
    bool m_owns_lock;

public:
    struct defer_lock_t { };
    struct try_to_lock_t { };
    struct adopt_lock_t { };

    static constexpr defer_lock_t defer_lock{};
    static constexpr try_to_lock_t try_to_lock{};
    static constexpr adopt_lock_t adopt_lock{};

    explicit srw_shared_lock( const srw_lock& lock )
        : m_lock( &lock ), m_owns_lock( true ) {
        m_lock->lock_shared( );
    }

    srw_shared_lock( const srw_lock& lock, defer_lock_t ) noexcept
        : m_lock( &lock ), m_owns_lock( false ) {
    }

    srw_shared_lock( const srw_lock& lock, try_to_lock_t )
        : m_lock( &lock ), m_owns_lock( false ) {
        m_owns_lock = m_lock->try_lock_shared( );
    }

    srw_shared_lock( const srw_lock& lock, adopt_lock_t ) noexcept
        : m_lock( &lock ), m_owns_lock( true ) {
    }

    template<typename Rep, typename Period>
    srw_shared_lock( const srw_lock& lock, const std::chrono::duration<Rep, Period>& timeout )
        : m_lock( &lock ), m_owns_lock( false ) {
        m_owns_lock = m_lock->try_lock_shared_for( timeout );
    }

    ~srw_shared_lock( ) {
        if ( m_owns_lock ) {
            m_lock->unlock_shared( );
        }
    }

    srw_shared_lock( const srw_shared_lock& ) = delete;
    srw_shared_lock& operator=( const srw_shared_lock& ) = delete;

    srw_shared_lock( srw_shared_lock&& other ) noexcept
        : m_lock( other.m_lock ), m_owns_lock( other.m_owns_lock ) {
        other.m_lock = nullptr;
        other.m_owns_lock = false;
    }

    srw_shared_lock& operator=( srw_shared_lock&& other ) noexcept {
        if ( this != &other ) {
            if ( m_owns_lock ) {
                m_lock->unlock_shared( );
            }
            m_lock = other.m_lock;
            m_owns_lock = other.m_owns_lock;
            other.m_lock = nullptr;
            other.m_owns_lock = false;
        }
        return *this;
    }

    void lock( ) {
        if ( !m_lock ) {
            throw srw_lock_exception( "No associated lock" );
        }
        if ( m_owns_lock ) {
            throw srw_lock_exception( "Already owns lock" );
        }
        m_lock->lock_shared( );
        m_owns_lock = true;
    }

    bool try_lock( ) {
        if ( !m_lock ) {
            throw srw_lock_exception( "No associated lock" );
        }
        if ( m_owns_lock ) {
            throw srw_lock_exception( "Already owns lock" );
        }
        m_owns_lock = m_lock->try_lock_shared( );
        return m_owns_lock;
    }

    template<typename Rep, typename Period>
    bool try_lock_for( const std::chrono::duration<Rep, Period>& timeout ) {
        if ( !m_lock ) {
            throw srw_lock_exception( "No associated lock" );
        }
        if ( m_owns_lock ) {
            throw srw_lock_exception( "Already owns lock" );
        }
        m_owns_lock = m_lock->try_lock_shared_for( timeout );
        return m_owns_lock;
    }

    void unlock( ) {
        if ( !m_owns_lock ) {
            throw srw_lock_exception( "Doesn't own lock" );
        }
        m_lock->unlock_shared( );
        m_owns_lock = false;
    }

    void release( ) noexcept {
        m_owns_lock = false;
    }

    bool owns_lock( ) const noexcept {
        return m_owns_lock;
    }

    explicit operator bool( ) const noexcept {
        return m_owns_lock;
    }

    const srw_lock* mutex( ) const noexcept {
        return m_lock;
    }
};

class srw_unique_lock {
private:
    const srw_lock* m_lock;
    bool m_owns_lock;

public:
    struct defer_lock_t { };
    struct try_to_lock_t { };
    struct adopt_lock_t { };

    static constexpr defer_lock_t defer_lock{};
    static constexpr try_to_lock_t try_to_lock{};
    static constexpr adopt_lock_t adopt_lock{};

    explicit srw_unique_lock( const srw_lock& lock )
        : m_lock( &lock ), m_owns_lock( true ) {
        m_lock->lock_exclusive( );
    }

    srw_unique_lock( const srw_lock& lock, defer_lock_t ) noexcept
        : m_lock( &lock ), m_owns_lock( false ) {
    }

    srw_unique_lock( const srw_lock& lock, try_to_lock_t )
        : m_lock( &lock ), m_owns_lock( false ) {
        m_owns_lock = m_lock->try_lock_exclusive( );
    }

    srw_unique_lock( const srw_lock& lock, adopt_lock_t ) noexcept
        : m_lock( &lock ), m_owns_lock( true ) {
    }

    template<typename Rep, typename Period>
    srw_unique_lock( const srw_lock& lock, const std::chrono::duration<Rep, Period>& timeout )
        : m_lock( &lock ), m_owns_lock( false ) {
        m_owns_lock = m_lock->try_lock_exclusive_for( timeout );
    }

    ~srw_unique_lock( ) {
        if ( m_owns_lock ) {
            m_lock->unlock_exclusive( );
        }
    }

    srw_unique_lock( const srw_unique_lock& ) = delete;
    srw_unique_lock& operator=( const srw_unique_lock& ) = delete;

    srw_unique_lock( srw_unique_lock&& other ) noexcept
        : m_lock( other.m_lock ), m_owns_lock( other.m_owns_lock ) {
        other.m_lock = nullptr;
        other.m_owns_lock = false;
    }

    srw_unique_lock& operator=( srw_unique_lock&& other ) noexcept {
        if ( this != &other ) {
            if ( m_owns_lock ) {
                m_lock->unlock_exclusive( );
            }
            m_lock = other.m_lock;
            m_owns_lock = other.m_owns_lock;
            other.m_lock = nullptr;
            other.m_owns_lock = false;
        }
        return *this;
    }

    void lock( ) {
        if ( !m_lock ) {
            throw srw_lock_exception( "No associated lock" );
        }
        if ( m_owns_lock ) {
            throw srw_lock_exception( "Already owns lock" );
        }
        m_lock->lock_exclusive( );
        m_owns_lock = true;
    }

    bool try_lock( ) {
        if ( !m_lock ) {
            throw srw_lock_exception( "No associated lock" );
        }
        if ( m_owns_lock ) {
            throw srw_lock_exception( "Already owns lock" );
        }
        m_owns_lock = m_lock->try_lock_exclusive( );
        return m_owns_lock;
    }

    template<typename Rep, typename Period>
    bool try_lock_for( const std::chrono::duration<Rep, Period>& timeout ) {
        if ( !m_lock ) {
            throw srw_lock_exception( "No associated lock" );
        }
        if ( m_owns_lock ) {
            throw srw_lock_exception( "Already owns lock" );
        }
        m_owns_lock = m_lock->try_lock_exclusive_for( timeout );
        return m_owns_lock;
    }

    void unlock( ) {
        if ( !m_owns_lock ) {
            throw srw_lock_exception( "Doesn't own lock" );
        }
        m_lock->unlock_exclusive( );
        m_owns_lock = false;
    }

    void release( ) noexcept {
        m_owns_lock = false;
    }

    bool owns_lock( ) const noexcept {
        return m_owns_lock;
    }

    explicit operator bool( ) const noexcept {
        return m_owns_lock;
    }

    const srw_lock* mutex( ) const noexcept {
        return m_lock;
    }
};

using srw_lock_shared_guard = srw_shared_lock;
using srw_lock_exclusive_guard = srw_unique_lock;

namespace srw_utils {
    template<typename Func>
    auto with_shared_lock( const srw_lock& lock, Func&& func ) -> decltype( func( ) ) {
        srw_shared_lock guard( lock );
        return func( );
    }

    template<typename Func>
    auto with_exclusive_lock( const srw_lock& lock, Func&& func ) -> decltype( func( ) ) {
        srw_unique_lock guard( lock );
        return func( );
    }

    template<typename Func>
    bool try_with_shared_lock( const srw_lock& lock, Func&& func,
        std::chrono::milliseconds timeout = std::chrono::milliseconds( 100 ) ) {
        srw_shared_lock guard( lock, timeout );
        if ( guard.owns_lock( ) ) {
            func( );
            return true;
        }
        return false;
    }

    template<typename Func>
    bool try_with_exclusive_lock( const srw_lock& lock, Func&& func,
        std::chrono::milliseconds timeout = std::chrono::milliseconds( 100 ) ) {
        srw_unique_lock guard( lock, timeout );
        if ( guard.owns_lock( ) ) {
            func( );
            return true;
        }
        return false;
    }
}
