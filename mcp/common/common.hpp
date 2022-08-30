#pragma once

#include <libdevcore/Common.h>
#include <libdevcore/CommonData.h>
#include <mcp/common/numbers.hpp>
// #include <sodium.h>

#include <functional>
#include <mutex>
#include <thread>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <condition_variable>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>

#include <secp256k1-vrf.h>

#include <libdevcrypto/Common.h>

#define STR(s) STR_TEMP(s)
#define STR_TEMP(s) #s

namespace mcp
{
    template <typename... Args>
    class Signal
    {
    public:
		using Callback = std::function<void(Args...)>;

        void add(Callback const & _h)
        {
            observers.push_back(_h);
        }
        void operator() (Args... args)
        {
            for (auto & i : observers)
            {
                i(args...);
            }
        }
	private:
        std::vector<Callback> observers;
    };

    class error_message
    {
    public:
        error_message() :
            error(false)
        {
        }
        bool error;
        std::string message;
    };

	/*
	* a noop lockable concept that can be used in place of std::mutex
	*/
	class null_lock {
	public:
		void lock() {}
		void unlock() {}
		bool try_lock() { return true; }
	};

    template <typename T, class Lock = null_lock>
    class mru_list
    {
    public:
		mru_list(size_t const &capacity_a) :
			m_capacity(capacity_a),
			m_lock(std::make_shared<Lock>())
		{
		};

        void add(T const &item)
		{
			std::lock_guard<Lock> lock(*m_lock);
			auto result(m_container.push_front(item));
			if (result.second && m_container.size() > m_capacity)
			{
				m_container.pop_back();
			}
		};

        bool contains(T const &item) const
		{
			std::lock_guard<Lock> lock(*m_lock);
			return m_container.template get<1>().find(item) != m_container.template get<1>().end();
		};

        size_t size() const
		{
			return m_container.size();
		};

    private:
        boost::multi_index_container<
            T,
            boost::multi_index::indexed_by<
                boost::multi_index::sequenced<>,
                boost::multi_index::hashed_unique<boost::multi_index::identity<T>>>>
            m_container;
        size_t m_capacity;
		std::shared_ptr<Lock> m_lock;
    };

	class fast_steady_clock
	{
	public:
		fast_steady_clock() : thread([this]() { run(); }) {}
		~fast_steady_clock() { stop(); }
		void run()
		{
			while (!stopped)
			{
				_timer = std::chrono::steady_clock::now();
				std::unique_lock<std::mutex> lock(mutex);
				condition.wait_for(lock, std::chrono::milliseconds(500));
			}
		}
		void stop() 
		{ 
			stopped = true; 
			if (thread.joinable())
			{
				thread.join();
			}
		}
		std::chrono::steady_clock::time_point now() { return _timer; }
		uint64_t now_since_epoch() {return std::chrono::duration_cast<std::chrono::milliseconds>(_timer.time_since_epoch()).count();}
	private:
		std::chrono::steady_clock::time_point _timer;
		std::mutex mutex;
		std::condition_variable condition;
		bool stopped = false;
		std::thread thread;
	};

	namespace encry
	{
		int encryption(unsigned char *c, const unsigned char *m,
			unsigned long long mlen, const unsigned char *n,
			const unsigned char *ek);

		int dencryption(unsigned char *m, const unsigned char *c,
			unsigned long long clen, const unsigned char *n,
			const unsigned char *ek);

		secp256k1_context const* get_secp256k1_ctx();
	}
}
