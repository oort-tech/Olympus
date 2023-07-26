#include "rpc_ws.hpp"

mcp::rpc_ws_config::rpc_ws_config() :
	address(boost::asio::ip::address_v4::loopback()),
	port(mcp::rpc_ws::rpc_ws_port),
    rpc_ws_enable(false)
{
}

void mcp::rpc_ws_config::serialize_json(mcp::json & json_a) const
{
    json_a["ws"] = rpc_ws_enable ? "true" : "false";
    json_a["ws_addr"] =  address.to_string();
    json_a["ws_port"] = port;
}

bool mcp::rpc_ws_config::deserialize_json(mcp::json const & json_a)
{ 
    auto error(false);
    try
    {
        if (!error)
        {
            if (json_a.count("ws") && json_a["ws"].is_string())
            {
                rpc_ws_enable = (json_a["ws"].get<std::string>() == "true" ? true : false);
            }
            else
            {
                error = true;
            }

            if (json_a.count("ws_addr") && json_a["ws_addr"].is_string())
            {
                std::string address_text = json_a["ws_addr"].get<std::string>();
                boost::system::error_code ec;
                address = boost::asio::ip::address::from_string(address_text, ec);
                if (ec)
                {
                    error = true;
                }
            }
            else
            {
                error = true;
            }

            if (json_a.count("ws_port") && json_a["ws_port"].is_number_unsigned())
            {
                uint64_t port_l = json_a["ws_port"].get<uint64_t>();
                if (port_l <= std::numeric_limits<uint16_t>::max())
                {
                    port = port_l;
                }
                else
                {
                    error = true;
                }
            }
            else
            {
                error = true;
            }
        }
    }
    catch (std::runtime_error const &)
    {
        error = true;
    }
    return error;
}

bool mcp::rpc_ws_config::parse_old_version_data(mcp::json const & json_a, uint64_t const& version)
{
    auto error(false);
    try
    {
		/// parse json used low version
		switch (version)
		{
			//case 0:
			//{
			//	/// parse
			//	break;
			//}
			//case 1:
			//{
			//	/// parse
			//	break;
			//}
		default:
			error |= deserialize_json(json_a);
			break;
		}
    }
    catch (std::runtime_error const &)
    {
        error = true;
    }
    return error;
}

/*	input: subscribe:Subscribe message describe; indexno:assign message index
	return: -1:indexno is exist; other:message index
 */
int mcp::subscribe::add(std::string subscribe,int indexno)
{
	int index = is_subscribe_exist(subscribe);
	if (index > 0)//exist
	{
		return index;
	}
	else//insert
	{
		if (indexno < 1)
		{
			index = get_max_index() + 1;
		}
		else
		{
			if (index_is_exist(indexno))
				return -1;
			index = indexno;
		}
		subcribe_list.insert(std::pair<int, std::string>(index, subscribe));
	}
	return index;
}

/*input: index:subscribe index; conn:socket
  return: 0:ok; other:eroor code
 */
mcp::rpc_ws_error mcp::subscribe::subscription(std::string message, mcp::rpc_ws_connection & conn)
{
	int index = 0;
	rpc_ws_error ret = rpc_ws_error::success;
	index = get_index_by_message(message);
	if (index == 0)//not exist
	{
		ret = rpc_ws_error::message_not_exist;
		return ret;
	}

	wLock lock(mutex);
	std::map<int, std::list<mcp::rpc_ws_connection* > >::iterator it = mes_subscribe.find(index);
	if (it != mes_subscribe.end())
	{
		bool exist = false;
		std::list<mcp::rpc_ws_connection* >::iterator itli = it->second.begin();
		for (; itli != it->second.end(); itli++)
		{
			if (&conn == (*itli))
			{
				exist = true;
				break;
			}
		}
		if (exist)
		{
			ret = rpc_ws_error::account_is_subscribed;
		}
		else
		{
			it->second.push_back(&conn);
		}
	}
	else
	{
		std::list<mcp::rpc_ws_connection*> li;
		li.push_back(&conn);
		mes_subscribe.insert(std::pair<int, std::list<mcp::rpc_ws_connection*> >(index, li));
	}
	return ret;
}

/*input: index:subscribe index; conn:socket
return: 0:ok; other:eroor code
*/
mcp::rpc_ws_error mcp::subscribe::unsubscription(std::string message, mcp::rpc_ws_connection & conn)
{
	int index = 0;
	rpc_ws_error ret = rpc_ws_error::success;
	index = get_index_by_message(message);
	if (index == 0)//not exist
	{
		ret = rpc_ws_error::message_not_exist;
		return ret;
	}

	wLock lock(mutex);
	std::map<int, std::list<mcp::rpc_ws_connection* > >::iterator it = mes_subscribe.find(index);
	if (it != mes_subscribe.end())
	{
		bool exist = false;
		std::list<mcp::rpc_ws_connection* >::iterator itli = it->second.begin();
		for (; itli != it->second.end(); itli++)
		{
			if (&conn == (*itli))
			{
				it->second.erase(itli++);
				break;
			}
		}
	}
	else
	{
		ret = rpc_ws_error::message_not_exist;
	}
	return ret;
}

void mcp::subscribe::trigger(int index, std::string data)
{
	rLock lock(mutex);
	std::map<int, std::list<mcp::rpc_ws_connection *> >::iterator it = mes_subscribe.find(index);
	if (it != mes_subscribe.end())
	{
		std::list<mcp::rpc_ws_connection *>::iterator itli = it->second.begin();
		for (; itli != it->second.end(); itli++)
		{
			(*itli)->do_send(data);
		}
	}
}

/*delete socket from subsribe message list*/
void mcp::subscribe::close_websocket(mcp::rpc_ws_connection &conn)
{
	wLock lock(mutex);
	std::map<int, std::list<mcp::rpc_ws_connection *> >::iterator it = mes_subscribe.begin();
	for (;it != mes_subscribe.end(); it++)
	{
		std::list<mcp::rpc_ws_connection *>::iterator itli = it->second.begin();
		for (; itli != it->second.end(); itli++)
		{
			if (&conn == (*itli))
			{
				it->second.erase(itli++);
				break;
			}
		}
	}
}

int mcp::subscribe::is_subscribe_exist(std::string subscribe)
{
	int ret = 0;

	rLock lock(mutex);
	std::map<int, std::string>::iterator it = subcribe_list.begin();
	for (; it != subcribe_list.end(); it++)
	{
		if (strcmp(subscribe.c_str(), it->second.c_str()) == 0)
		{
			ret = it->first;
			break;
		}
	}
	return ret;
}

int mcp::subscribe::get_max_index()
{
	rLock lock(mutex);
	if (subcribe_list.size() > 0)
	{
		std::map<int, std::string>::iterator itsubList = subcribe_list.end();
		itsubList--;
		return itsubList->first;
	}
	else
	{
		return 0;
	}
}

int mcp::subscribe::get_index_by_message(std::string message)
{
	int ret = 0;
	rLock lock(mutex);

	std::map<int, std::string>::iterator itsub = subcribe_list.begin();
	for (; itsub != subcribe_list.end(); itsub++)
	{
		if (strcmp(itsub->second.c_str(), message.c_str()) == 0)
		{
			ret = itsub->first;
		}
	}
	return ret;
}

bool mcp::subscribe::index_is_exist(int indexno)
{
	rLock lock(mutex);
	std::map<int, std::string>::iterator itsubList = subcribe_list.find(indexno);
	if (itsubList != subcribe_list.end())
	{
		return true;
	}
	return false;
}


/*socket*/
mcp::rpc_ws::rpc_ws(boost::asio::io_service & service_a, std::shared_ptr<mcp::async_task> background_a, mcp::rpc_ws_config const & config_a) :
	acceptor(service_a),
	background(background_a),
	sock(service_a),
	config(config_a)
{
}

void mcp::rpc_ws::start()
{
	auto endpoint(bi::tcp::endpoint(config.address, config.port));

	boost::system::error_code ec;
	acceptor.open(endpoint.protocol(), ec);
	if (ec)
	{
        LOG(m_log.error) << boost::str(boost::format("Error while open protocol for WebSocket RPC "));
		throw std::runtime_error(ec.message());
	}
	acceptor.set_option(bi::tcp::acceptor::reuse_address(true));

	acceptor.bind(endpoint, ec);
	if (ec)
	{
        LOG(m_log.error) << boost::str(boost::format("Error while binding for WebSocket RPC on port %1%: %2%") % endpoint.port() % ec.message());
		throw std::runtime_error(ec.message());
	}
	acceptor.listen();

    LOG(m_log.info) << "WebSocket RPC started, http://" << endpoint;
	
	accept();
}
void mcp::rpc_ws::accept()
{
	acceptor.async_accept(
		sock,
		std::bind(
			&rpc_ws::on_accept,
			shared_from_this(),
			std::placeholders::_1));
}

void mcp::rpc_ws::on_accept(boost::system::error_code ec)
{
	if (ec)
	{
		LOG(m_log.info) << boost::str(boost::format("Error accepting WebSocket RPC connections: %1%") % ec);
	}
	else
	{
		std::make_shared<rpc_ws_connection>(std::move(sock), *this)->runloop();
	}
	accept();
}

int mcp::rpc_ws::register_subscribe(std::string data, int index)
{
	return subscribe.add(data,index);
}

void mcp::rpc_ws::on_new_block(std::shared_ptr<mcp::block> block)
{
	background->sync_async([this, block]
	{
		mcp::json p;
		p["hash"] = block->hash().hex();
		trigger_subscribe("new_block", p);
	});
}

void mcp::rpc_ws::on_stable_block(std::shared_ptr<mcp::block> block)
{
	background->sync_async([this, block]
	{
		mcp::json p;
		p["hash"] = block->hash().hex();
		trigger_subscribe("stable_block", p);
	});
}

void mcp::rpc_ws::on_stable_mci(uint64_t const & stable_mci)
{
}

mcp::rpc_ws_error mcp::rpc_ws::subscription(std::string message, mcp::rpc_ws_connection & conn)
{
	return subscribe.subscription(message,conn);
}

mcp::rpc_ws_error mcp::rpc_ws::unsubscription(std::string message, mcp::rpc_ws_connection & conn)
{
	return subscribe.unsubscription(message, conn);
}

void mcp::rpc_ws::trigger_subscribe(int index, std::string data)
{
	subscribe.trigger(index,data);
}

void mcp::rpc_ws::trigger_subscribe(std::string message, mcp::json & pdata)
{
	std::string data = pdata.dump();
	int index = subscribe.get_index_by_message(message);
	subscribe.trigger(index, data);
}

void mcp::rpc_ws::close_ws(mcp::rpc_ws_connection & conn)
{
	subscribe.close_websocket(conn);
}

/*deal websocket connection */
mcp::rpc_ws_connection::rpc_ws_connection(bi::tcp::socket sock, mcp::rpc_ws & rpc_ws_a) :
	ws(std::move(sock)),
	strand(ws.get_executor()),
	rpc_ws(rpc_ws_a)
{

}

template<class ConstBufferSequence>
std::string mcp::rpc_ws_connection::to_string(ConstBufferSequence const& bs)
{
	std::string s;
	s.reserve(buffer_size(bs));
	for (auto b : boost::beast::detail::buffers_range(bs))
		s.append(reinterpret_cast<char const*>(b.data()),
			b.size());
	return s;
}

void mcp::rpc_ws_connection::runloop()
{
	ws.async_accept(
		ba::bind_executor(
			strand,
			std::bind(
				&rpc_ws_connection::on_accept,
				shared_from_this(),
				std::placeholders::_1)));
}

void mcp::rpc_ws_connection::on_accept(boost::system::error_code ec)
{
	if (ec)
	{
		LOG(m_log.error) << boost::str(boost::format("Error accepting data WebSocket RPC connections: %1%") % ec);
		return;
	}

	// Read a message
	do_read();
}

void mcp::rpc_ws_connection::do_read()
{
	// Read a message into our buffer
	ws.async_read(
		buffer,
		boost::asio::bind_executor(
			strand,
			std::bind(
				&rpc_ws_connection::on_read,
				shared_from_this(),
				std::placeholders::_1,
				std::placeholders::_2)));
}

void mcp::rpc_ws_connection::on_read(
	boost::system::error_code ec,
	std::size_t bytes_transferred)
{
	boost::ignore_unused(bytes_transferred);

	// This indicates that the session was closed
	if (ec == boost::beast::websocket::error::closed)
	{
		rpc_ws.close_ws(*this);
		return;
	}

	if (ec)
	{
        LOG(m_log.error) << boost::str(boost::format("Error read data WebSocket RPC connections: %1%") % ec);
	}

	// deal the message
	auto handler(std::make_shared<mcp::rpc_ws_handler>(this->rpc_ws, *this, to_string(buffer.data())));
	handler->process_request();

	if (strlen(handler->response.c_str()) > 0)
	{
		buffer.consume(buffer.size());
		boost::beast::ostream(buffer) << handler->response;

		ws.async_write(
			buffer.data(),
			boost::asio::bind_executor(
				strand,
				std::bind(
					&rpc_ws_connection::on_write,
					shared_from_this(),
					std::placeholders::_1,
					std::placeholders::_2)));
	}
	else
	{
		buffer.consume(buffer.size());
		do_read();
	}
}

void mcp::rpc_ws_connection::on_write(
	boost::system::error_code ec,
	std::size_t bytes_transferred)
{
	boost::ignore_unused(bytes_transferred);

	if (ec)
	{
        LOG(m_log.error) << boost::str(boost::format("Error write data WebSocket RPC connections: %1%") % ec);
		return;
	}

	// Clear the buffer
	buffer.consume(buffer.size());

	// Do another read
	do_read();
}

void mcp::rpc_ws_connection::do_send(std::string res)
{
	if(ws.is_open())
		ws.write(boost::asio::buffer(std::string(res)));
}

mcp::rpc_ws_handler::rpc_ws_handler(mcp::rpc_ws & rpc_ws_a, mcp::rpc_ws_connection & rpc_ws_connection_a, std::string body_a) :
	body(body_a),
	rpc_ws(rpc_ws_a),
	rpc_ws_connection(rpc_ws_connection_a)
{

}

void mcp::rpc_ws_handler::process_request()
{
	try
	{
		request_json = mcp::json::parse(body);
		std::string action = request_json["action"];

		bool handled = false;
		if (action == "subscribe")
		{
			subscribe();
		}
		else if (action == "unsubscribe")
		{
			unsubscribe();
		}
		else if (action == "confirm")
		{
			//todo :confirm
		}
		else
		{
			deal_error(rpc_ws_error::action_not_exist);
		}
	}
	catch (std::exception const & err)
	{
		deal_error(rpc_ws_error::unable_parse_JSON);
	}
	catch (...)
	{
		deal_error(rpc_ws_error::internal_server_error);
	}

	get_response();
}

void mcp::rpc_ws_handler::get_response()
{
	if (response_l.size() > 0)
	{
		response = response_l.dump();
	}
}

void mcp::rpc_ws_handler::deal_error(rpc_ws_error error_Code)
{
	response_l["code"] = (int)error_Code;
	switch (error_Code)
	{
	case rpc_ws_error::success:
	{
		break;
	}
	case rpc_ws_error::action_not_exist:
	{
		response_l["error"] = "Action not exist!";
		break;
	}
	case rpc_ws_error::unable_parse_JSON:
	{
		response_l["error"] = "Unable to parse JSON";
		break;
	}
	case rpc_ws_error::internal_server_error:
	{
		response_l["error"] = "Internal server error in WebSocket RPC";
		break;
	}
	case rpc_ws_error::message_error:
	{
		response_l["error"] = "Message must be string";
		break;
	}
	case rpc_ws_error::account_is_subscribed:
	{
		response_l["error"] = "The account is already subscribed this message";
		break;
	}
	case rpc_ws_error::message_not_exist:
	{
		response_l["error"] = "Subscribed message is not exist";
		break;
	}
	default:
		break;
	}
}

void mcp::rpc_ws_handler::subscribe()
{
	mcp::json messages = request_json["message"];

	try
	{
		int num = 0;
		rpc_ws_error ret;
		for (auto mes : messages)
		{
			std::string message = mes.get<std::string>();
			ret = rpc_ws.subscription(message, rpc_ws_connection);
			num++;
		}
		if (num == 1)
		{
			deal_error(ret);
		}
		else
		{
			deal_error(rpc_ws_error::success);
		}
	}
	catch (...)
	{
		deal_error(rpc_ws_error::message_error);
	}

}

void mcp::rpc_ws_handler::unsubscribe()
{
	mcp::json messages = request_json["message"];

	try
	{
		int num = 0;
		rpc_ws_error ret;
		for (auto mes : messages)
		{
			std::string message = mes.get<std::string>();
			ret = rpc_ws.unsubscription(message, rpc_ws_connection);
			num++;
		}
		if (num == 1)
			deal_error(ret);
		else
			deal_error(rpc_ws_error::success);
	}
	catch (...)
	{
		deal_error(rpc_ws_error::message_error);
	}
}


std::shared_ptr<mcp::rpc_ws> mcp::get_rpc_ws(
	boost::asio::io_service & service_a, 
	std::shared_ptr<mcp::async_task> background_a, 
	mcp::rpc_ws_config const & config_a
)
{
	std::shared_ptr<rpc_ws> impl(new rpc_ws(service_a, background_a, config_a));
	return impl;
}

