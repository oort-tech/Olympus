#include <mcp/common/working.hpp>
#include <mcp/common/assert.hpp>
#include <shlobj.h>

namespace mcp
{
boost::filesystem::path app_path ()
{
	boost::filesystem::path result;
	WCHAR path[MAX_PATH];
	if (SUCCEEDED (SHGetFolderPathW (NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		result = boost::filesystem::path (path);
	}
	else
	{
		assert_x(false);
	}
	return result;
}
}