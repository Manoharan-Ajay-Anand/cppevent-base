#ifndef CPPEVENT_BASE_TASK_HPP
#define CPPEVENT_BASE_TASK_HPP

#include <coroutine>
#include <optional>
#include <exception>

namespace cppevent {

struct task {
    struct promise_type {
        task get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception() {}
        void return_void() {}
    };
};

template <typename T>
struct awaitable_task {
    struct promise_type {

        struct final_awaiter {
            bool await_ready() noexcept { return false; }

            std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> handle) {
                auto& waiting = handle.promise().m_waiting;
                if (waiting) {
                    return waiting;
                }
                return std::noop_coroutine();
            }

            void await_resume() noexcept {}
        };

        std::optional<T> m_val_opt;
        std::coroutine_handle<> m_waiting;
        std::exception_ptr m_exception;

        awaitable_task<T> get_return_object() {
            return { std::coroutine_handle<promise_type>::from_promise(*this) };
        }

        std::suspend_never initial_suspend() { return {}; }
        final_awaiter final_suspend() noexcept { return {}; }
        
        void unhandled_exception() {
            m_exception = std::current_exception();
        }
        
        void return_value(const T& val) {
            m_val_opt = val;
        }

        void return_value(T&& val) {
            m_val_opt = std::move(val);
        }
    };

    std::coroutine_handle<promise_type> m_handle;

    ~awaitable_task() {
        m_handle.destroy();
    }

    bool await_ready() noexcept {
        return m_handle.done();
    }
    
    void await_suspend(std::coroutine_handle<> handle) {
        m_handle.promise().m_waiting = handle;
    }
    
    T&& await_resume() {
        promise_type& promise = m_handle.promise();
        if (promise.m_exception) {
            std::rethrow_exception(promise.m_exception);
        }
        return std::move(promise.val.value());
    }
};

template<>
struct awaitable_task<void> {
    struct promise_type {

        struct final_awaiter {
            bool await_ready() noexcept { return false; }

            std::coroutine_handle<> await_suspend(std::coroutine_handle<promise_type> handle) {
                auto& waiting = handle.promise().m_waiting;
                if (waiting) {
                    return waiting;
                }
                return std::noop_coroutine();
            }

            void await_resume() noexcept {}
        };

        std::coroutine_handle<> m_waiting;
        std::exception_ptr m_exception;

        awaitable_task<void> get_return_object() {
            return { std::coroutine_handle<promise_type>::from_promise(*this) };
        }

        std::suspend_never initial_suspend() { return {}; }
        final_awaiter final_suspend() noexcept { return {}; }
        
        void unhandled_exception() {
            m_exception = std::current_exception();
        }
        
        void return_void() {}
    };

    std::coroutine_handle<promise_type> m_handle;

    ~awaitable_task() {
        m_handle.destroy();
    }

    bool await_ready() noexcept {
        return m_handle.done();
    }
    
    void await_suspend(std::coroutine_handle<> handle) {
        m_handle.promise().m_waiting = handle;
    }
    
    void await_resume() {
        promise_type& promise = m_handle.promise();
        if (promise.m_exception) {
            std::rethrow_exception(promise.m_exception);
        }
    }
};

}

#endif
