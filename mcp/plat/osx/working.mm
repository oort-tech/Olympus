#include <mcp/common/working.hpp>

#include <Foundation/Foundation.h>

namespace mcp
{
boost::filesystem::path app_path ()
{
	NSString * dir_string = [NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES) lastObject];
	char const * dir_chars = [dir_string UTF8String];
	boost::filesystem::path result (dir_chars);
	[dir_string release];
	return result;
}
}