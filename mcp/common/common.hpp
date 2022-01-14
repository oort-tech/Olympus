#pragma once

#include <libdevcore/Common.h>
#include <libdevcore/CommonData.h>
#include <mcp/common/numbers.hpp>
#include <sodium.h>

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

#define crypto_cipher_len crypto_box_MACBYTES
#define crypto_sign_len crypto_sign_BYTES

#define STR(s) STR_TEMP(s)
#define STR_TEMP(s) #s

namespace mcp
{
	// some utility functions
	std::string uint64_to_hex(uint64_t);
	bool hex_to_uint64(std::string const &, uint64_t &);
	std::string bytes_to_hex(dev::bytes const & b);
	int from_hex_char(char c) noexcept;
	bool hex_to_bytes(std::string const & s, dev::bytes & out);

    template <typename... T>
    class observer_set
    {
    public:
        void add(std::function<void(T...)> const & observer_a)
        {
            std::lock_guard<std::mutex> lock(mutex);
            observers.push_back(observer_a);
        }
        void operator() (T... args)
        {
            std::lock_guard<std::mutex> lock(mutex);
            for (auto & i : observers)
            {
                i(args...);
            }
        }
        std::mutex mutex;
        std::vector<std::function<void(T...)>> observers;
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
		key_pair(seed_key const & seed);
		~key_pair();
		static key_pair create();

		// get the secret key.
		secret_key const& secret() const { return m_secret; }

		// get the public key.
		public_key const& pub() const { return m_public; }

		// get the public key compressed
		public_key_comp const& pub_comp() const { return m_public_comp; }

		// get the account's address
		account20_struct const& account() const { return m_account; }

		bool operator==(key_pair const& _c) const { return m_public == _c.m_public; }
		bool operator!=(key_pair const& _c) const { return m_public != _c.m_public; }
		bool flag = false;

	private:
		secret_key m_secret;
		public_key m_public;
		public_key_comp m_public_comp;
		account20_struct m_account;
	};

	class nonce
	{
	public:
		static nonce get()
		{
			static nonce s;
			randombytes_buf(s.data.data(), s.data.size());
			return s;
		}

		nonce() { resize(); };
		nonce(dev::bytes bytes_a) { resize(); dev::bytesConstRef(&bytes_a).copyTo(dev::bytesRef(&data)); };
		dev::bytesConstRef ref() const { return dev::bytesConstRef(&data); }
		dev::bytesRef ref() { return dev::bytesRef(&data); }
		std::string to_string() const { return dev::toHex(ref()); };
		enum { size = crypto_box_NONCEBYTES };
	private:
		dev::bytes data;
		void resize() { data.resize(crypto_box_NONCEBYTES); }
	};

	namespace encry
	{
		//encryption curve 25519
		int encryption(unsigned char *c, const unsigned char *m,
			unsigned long long mlen, const unsigned char *n,
			const unsigned char *pk, const unsigned char *sk);

		//dencryption curve 25519
		int dencryption(unsigned char *m, const unsigned char *c,
			unsigned long long clen, const unsigned char *n,
			const unsigned char *pk, const unsigned char *sk);

		//used in-out key(seem AES), more efficiency than pk-sk
		int encryption(unsigned char *c, const unsigned char *m,
			unsigned long long mlen, const unsigned char *n,
			const unsigned char *k);

		int dencryption(unsigned char *m, const unsigned char *c,
			unsigned long long clen, const unsigned char *n,
			const unsigned char *k);

		/// Returns siganture of message hash.
		bool sign(secret_key const& _k, dev::bytesConstRef _hash, mcp::signature& sig);
		// this is curious for purpose, right now (commented by michael)
		bool sign(private_key const& _k, public_key const& _pk, dev::bytesConstRef _hash, mcp::signature& sig);

		/// Verify signature.
		// updated by michael at 1/10
		bool verify(public_key const& _k, dev::bytesConstRef const& _s);
		bool verify(public_key const& _k, mcp::signature const& _s, dev::bytesConstRef const& _o);
		//added by michael
		bool verify(public_key_comp const& _k, mcp::signature const& _s, dev::bytesConstRef const& _o);

		// added by michael at 1/10
		public_key recover(mcp::signature const& _s, dev::bytesConstRef const& _o);

		//ed25519 secret key to curve25519 secret key
		bool get_encry_secret_key_from_sign_key(secret_encry & curve, secret_key const & ed25519);
		//ed25519 public key to curve25519 public key
		bool get_encry_public_key_from_sign_key(public_key_comp & curve, public_key_comp const & ed25519);
		bool get_encry_public_key_from_sign_key(public_key_comp & curve, dev::bytesConstRef ed25519);

		// commented by michael at 1/5
		// ed25519 get public key from secret key
		// bool generate_public_from_secret(mcp::uint256_union const& _sk, mcp::uint256_union& _pk);
		
		// added by michael at 1/5
		bool generate_public_from_secret(secret_key const& _sk, public_key& _pk);
		bool generate_public_from_secret(secret_key const& _sk, public_key& _pk, public_key_comp& _pk_comp);
		secp256k1_context const* get_secp256k1_ctx();
	}
}
