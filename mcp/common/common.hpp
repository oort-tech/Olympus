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

#include <secp256k1.h>

#include <libdevcrypto/Common.h>

// #define crypto_cipher_len crypto_box_MACBYTES
// #define crypto_sign_len crypto_sign_BYTES

#define STR(s) STR_TEMP(s)
#define STR_TEMP(s) #s

namespace mcp
{
	// some utility functions
	std::string uint64_to_hex(uint64_t);
	std::string uint64_to_hex_nofill(uint64_t);
	std::string uint256_to_hex_nofill(uint256_t);
	bool hex_to_uint64(std::string const &, uint64_t &, bool show_base = false);
	bool hex_to_uint256(std::string const & value_a, uint256_t & target_a, bool show_base = false);
	std::string bytes_to_hex(dev::bytes const & b);
	int from_hex_char(char c) noexcept;
	bool hex_to_bytes(std::string const & s, dev::bytes & out);

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

	class key_pair
	{
	public:
		key_pair() = default;
		key_pair(dev::Secret const & seed);
		~key_pair();

		static key_pair create();

		// get the secret key.
		dev::Secret const& secret() const { return m_secret; }

		// get the public key.
		dev::Public const& pub() const { return m_public; }

		// get the public key compressed
		dev::PublicCompressed const& pub_comp() const { return m_public_comp; }

		// get the account's address
		dev::Address const& account() const { return m_account; }
		
		bool operator==(key_pair const& _c) const { return m_public == _c.m_public; }
		bool operator!=(key_pair const& _c) const { return m_public != _c.m_public; }
		bool flag = false;

	private:
		dev::Secret m_secret;
		dev::Public m_public;
		dev::PublicCompressed m_public_comp;
		dev::Address m_account;
	};

	class nonce
	{
	public:
		static nonce get()
		{
			static nonce s;
			random_pool.GenerateBlock(s.data.data(), s.data.size());
			return s;
		}

		nonce() { resize(); };
		nonce(dev::bytes bytes_a) { resize(); dev::bytesConstRef(&bytes_a).copyTo(dev::bytesRef(&data)); };
		dev::bytesConstRef ref() const { return dev::bytesConstRef(&data); }
		dev::bytesRef ref() { return dev::bytesRef(&data); }
		std::string to_string() const { return dev::toHex(ref()); };
		enum { size = 24 };
	private:
		dev::bytes data;
		void resize() { data.resize(size); }
	};

	namespace encry
	{
		int get_encryption_key(dev::Secret &key, const unsigned char* pk, const size_t pkLen, const dev::Secret &sk);

		int encryption(unsigned char *c, const unsigned char *m,
			unsigned long long mlen, const unsigned char *n,
			const unsigned char *ek);

		int dencryption(unsigned char *m, const unsigned char *c,
			unsigned long long clen, const unsigned char *n,
			const unsigned char *ek);

		bool verify(dev::h256 const &pkSlice, dev::Signature const &sig, dev::h256 const &hash);
	}
}
