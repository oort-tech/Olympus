#pragma once
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <mcp/common/log.hpp>
#include <mcp/common/mcp_json.hpp>
#include <mcp/core/blocks.hpp>
#include <mcp/common/async_task.hpp>

namespace ba = boost::asio;
namespace bi = boost::asio::ip;

namespace mcp
{
	enum class rpc_ws_error
	{
		success = 0,
		action_not_exist = 1,
		unable_parse_JSON = 2,
		internal_server_error = 3,
		message_error = 4,
		account_is_subscribed = 100,
		message_not_exist = 101
	};

	class rpc_ws_config
	{
	public:
		rpc_ws_config();

		rpc_ws_config(bool enable_control_a);

        void serialize_json(mcp::json & json_a) const;

        bool deserialize_json(mcp::json const & json_a);

        bool parse_old_version_data(mcp::json const &, uint64_t const&);

		boost::asio::ip::address address;
		uint16_t port;
		bool enable_control;
        bool rpc_ws_enable;
	};

	class rpc_ws_connection;
	class subscribe : public std::enable_shared_from_this<subscribe>
	{
	public:
		int add(std::string subscribe, int indexno);

		mcp::rpc_ws_error subscription(std::string message, mcp::rpc_ws_connection & conn);

		mcp::rpc_ws_error unsubscription(std::string message, mcp::rpc_ws_connection & conn);

		void trigger(int index, std::string data);

		void close_websocket(mcp::rpc_ws_connection & conn);

		int get_index_by_message(std::string message);

	private:
		int get_max_index();

		int is_subscribe_exist(std::string subscribe);

		bool index_is_exist(int indexno);

		typedef boost::shared_lock<boost::shared_mutex> rLock;
		typedef boost::unique_lock<boost::shared_mutex> wLock;

		std::map<int, std::list<mcp::rpc_ws_connection* > > mes_subscribe;	/*Subscribe message list*/
		std::map<int, std::string> subcribe_list;									/*message list*/
		boost::shared_mutex mutex;
	};

	
	class rpc_ws : public std::enable_shared_from_this<rpc_ws>
	{
	public:
		rpc_ws(boost::asio::io_service & service_a, std::shared_ptr<mcp::async_task> background_a, mcp::rpc_ws_config const & config_a);

		void start();

		int register_subscribe(std::string data,int index=0);

		mcp::rpc_ws_error subscription(std::string message, mcp::rpc_ws_connection & conn);

		mcp::rpc_ws_error unsubscription(std::string message, mcp::rpc_ws_connection & conn);

		void trigger_subscribe(int index, std::string data);

		void trigger_subscribe(std::string message, mcp::json & pdata);

		void close_ws(mcp::rpc_ws_connection & conn);

		static uint16_t const rpc_ws_port = 8764;

		//register to chain
		void on_new_block(std::shared_ptr<mcp::block> block);
		void on_stable_block(std::shared_ptr<mcp::block> block);
		void on_stable_mci(uint64_t const & stable_mci);
	private:
		virtual void accept();

		void on_accept(boost::system::error_code ec);

		std::shared_ptr<mcp::async_task> background;
		bi::tcp::acceptor acceptor;
		bi::tcp::socket sock;
		mcp::rpc_ws_config config;
		mcp::subscribe subscribe;	/*subscribe message*/
        mcp::log m_log = { mcp::log("rpc") };
	};

	class rpc_ws_connection : public std::enable_shared_from_this<rpc_ws_connection>
	{
	public:
		rpc_ws_connection(bi::tcp::socket sock, mcp::rpc_ws & rpc_ws_a);

		virtual void runloop();

		void do_send(std::string res);

	private:
		void on_accept(boost::system::error_code ec);

		void do_read();

		void on_read(boost::system::error_code ec, std::size_t bytes_transferred);

		void on_write(boost::system::error_code ec, std::size_t bytes_transferred);


		boost::beast::websocket::stream<bi::tcp::socket> ws;
		ba::strand<ba::io_service::executor_type> strand;
		boost::beast::multi_buffer buffer;
		std::string data;
		mcp::rpc_ws & rpc_ws;
		template<class ConstBufferSequence>
		std::string to_string(ConstBufferSequence const & bs);
        mcp::log m_log = { mcp::log("rpc") };
	};

	class rpc_ws_handler : public std::enable_shared_from_this<mcp::rpc_ws_handler>
	{
	public:
		rpc_ws_handler(mcp::rpc_ws & rpc_ws_a, mcp::rpc_ws_connection & rpc_ws_connection_a, std::string body_a);
		void process_request();

		std::string response;
	private:
		void get_response();

		void deal_error(rpc_ws_error error_Code);

		void subscribe();

		void unsubscribe();

		mcp::json request_json;
		std::string body;
		mcp::json response_l;
		mcp::rpc_ws & rpc_ws;
		mcp::rpc_ws_connection & rpc_ws_connection;
	};


/** Returns the correct RPC WEBSOCKET implementation based on TLS configuration */
std::shared_ptr<mcp::rpc_ws> get_rpc_ws(
	boost::asio::io_service & service_a, 
	std::shared_ptr<mcp::async_task> background_a, 
	mcp::rpc_ws_config const & config_a
);

}



