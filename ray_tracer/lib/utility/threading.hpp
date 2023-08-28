#pragma once

#include <future>
#include <optional>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace utl
{
	template<class _Ty>
	class future
	{
    public:
        future() noexcept = default;

        future(std::future<_Ty>&& _future) noexcept : __future(std::move(_future))
        {
        }

        bool has_value() const noexcept
        {
            return __future.valid() || __current;
        }

        _Ty& get() noexcept
        {
            if (__future.valid())
                __current = __future.get();
            return *__current;
        }

        constexpr explicit operator bool() const noexcept { return has_value(); }
        constexpr operator _Ty& () const noexcept { return *get(); }

        _Ty& operator*() noexcept { return get(); }
        _Ty& operator->() noexcept { return get(); }

        bool operator==(const future& rhs) const noexcept
        {
            return __future == rhs.__future && __current == rhs.__current;
        }

        bool operator!=(const future& rhs) const noexcept
        {
            return !operator==(rhs);
        }

    private:
        std::future<_Ty> __future;
        std::optional<_Ty> __current;
	};

    class threadworker
    {
    public:
        threadworker()
        {
            _thread = std::thread(&threadworker::loop, this);
        }

        ~threadworker()
        {
            if (_thread.joinable())
            {
                wait();
                _queue_mutex.lock();
                _destroying = true;
                _condition.notify_one();
                _queue_mutex.unlock();
                _thread.join();
            }
        }

        template<class _Lambda>
        void push(_Lambda&& function)
        {
            std::lock_guard<std::mutex> lock(_queue_mutex);
            _work_queue.push(std::move(function));
            _condition.notify_one();
        }

        template<class _Lambda, class... _Types>
        void push(_Lambda&& function, _Types&& ...args)
        {
            push([function, ...args = std::forward<_Types>(args)] { function(std::forward<_Types>(args)...); });
        }

        void wait()
        {
            std::unique_lock<std::mutex> lock(_queue_mutex);
            _condition.wait(lock, [this]() { return _work_queue.empty(); });
        }

        bool is_free()
        {
            return _waiting;
        }
    private:
        void loop()
        {
            while (true)
            {
                utl::function<void()> work;
                {
                    std::unique_lock<std::mutex> lock_begin(_queue_mutex);
                    _condition.wait(lock_begin, [this]
                        { return !_work_queue.empty() || _destroying; });
                    if (_destroying)
                        break;
                    work = _work_queue.front();
                    _waiting = false;
                }

                work();

                {
                    std::lock_guard<std::mutex> lock_end(_queue_mutex);
                    _work_queue.pop();
                    _waiting = true;
                    _condition.notify_one();
                }
            }
        }
    private:
        std::atomic<bool> _destroying{ false };
        std::atomic<bool> _waiting{ true };
        std::thread _thread;
        std::queue<utl::function<void()>> _work_queue;
        std::mutex _queue_mutex;
        std::condition_variable _condition;
    };
}