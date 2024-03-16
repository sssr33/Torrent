
// order in which boost sees the correct value of _WIN32_WINNT
// https://stackoverflow.com/questions/9750344/boostasio-winsock-and-winsock-2-compatibility-issue
// https://github.com/boostorg/beast/issues/1895
#include <SDKDDKVer.h>
#include <boost/asio.hpp>
#include <Windows.h>

#include <cstdlib>
#include <libtorrent/libtorrent.hpp>

int main() {
	lt::session s;

	return 0;
}
