#pragma once

#include <array>
#include <atomic>
#include <condition_variable>
#include <type_traits>

#include <boost/optional/optional_io.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/format.hpp>

#include <mcp/common/numbers.hpp>
#include <mcp/common/mcp_json.hpp>
namespace mcp
{
	using bufferstream = boost::iostreams::stream_buffer<boost::iostreams::basic_array_source<uint8_t>>;
	using vectorstream = boost::iostreams::stream_buffer<boost::iostreams::back_insert_device<std::vector<uint8_t>>>;
	// C++ stream are absolutely horrible so I need this helper function to do the most basic operation of creating a file if it doesn't exist or truncating it.
	void open_or_create(std::fstream &, std::string const &);
	// Reads a json object from the stream and if was changed, write the object back to the stream
	template <typename T>
	bool fetch_object(T & object, boost::filesystem::path const & path_a, bool const & file_exists)
	{
        bool error(false);
		std::fstream stream_a;
		if(file_exists)
			stream_a.open(path_a.string(), std::ios_base::in);
		else
			mcp::open_or_create(stream_a, path_a.string());
		error = stream_a.fail();
        if (!error)
        {
            mcp::json json_l;
            try
            {
                //file empty will parse error,so add this check.
                stream_a.seekg(0, std::ios_base::end);
                std::streampos pos = stream_a.tellg();
				if (pos != std::streampos(0) && pos != std::streampos(-1))
				{
					stream_a.seekg(std::ios_base::beg);
					stream_a >> json_l;
				}
				else
				{
					if (file_exists)
						error = true;
				}
            }
            catch (const std::exception & e)
            {
                std::cout << e.what() << std::endl;
                error = true;
            }

            if (!error)
            {
                auto updated(false);
				error = object.deserialize_json(updated, json_l);

                if (!error && updated)
                {
                    stream_a.close();
                    stream_a.open(path_a.string(), std::ios_base::out | std::ios_base::trunc);
                    try
                    {                
                        stream_a << json_l.dump(4);
                    }
                    catch (std::runtime_error const & e)
                    {
						std::cerr << "Update config file fail, message: " << e.what() << std::endl;
                        error = true;
                    }
                }
            }

			if (!error)
				stream_a.close();
		}
        return error;
	}
}