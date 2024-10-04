
// order in which boost sees the correct value of _WIN32_WINNT
// https://stackoverflow.com/questions/9750344/boostasio-winsock-and-winsock-2-compatibility-issue
// https://github.com/boostorg/beast/issues/1895
#include <SDKDDKVer.h>
#include <boost/asio.hpp>
#include <Windows.h>
#include <iphlpapi.h>

#include <cstdlib>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <semaphore>
#include <libtorrent/libtorrent.hpp>

#include <boost/cobalt.hpp>

#include "UI/Dx11Window.h"

#pragma comment(lib, "Iphlpapi.lib")

/*
max_queued_disk_bytes
hashing_threads
aio_threads

NEED TO COPY PDB for all libs
*/

namespace TEST_HELPERS {
	enum class ConsoleColor {
		Default, // light gray for text, black for background
		Red,
		Green,
		Blue,
		Yellow, // red + green
		Magenta, // red + blue
		Cyan, // green + blue
		LightGray, // red + green + blue
		Black,

		// same colors + intensify:

		LightRed,
		LightGreen,
		LightBlue,
		LightYellow,
		LightMagenta,
		LightCyan,
		White, // LightGray + intensify
		DarkGray, // 0 + intensify
	};

	class Helpers {
	public:
		static void SetConsoleColor(ConsoleColor textFgColor, ConsoleColor textBgColor = ConsoleColor::Default) {
			WORD fgColorAttr = ColorToColorAttr<FOREGROUND_RED, FOREGROUND_GREEN, FOREGROUND_BLUE, FOREGROUND_INTENSITY>(textFgColor, ConsoleColor::LightGray);
			WORD bgColorAttr = ColorToColorAttr<BACKGROUND_RED, BACKGROUND_GREEN, BACKGROUND_BLUE, BACKGROUND_INTENSITY>(textBgColor, ConsoleColor::Black);

			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hConsole, fgColorAttr | bgColorAttr);
		}

	private:
		template<WORD RedAttr, WORD GreenAttr, WORD BlueAttr, WORD IntensifyAttr>
		static WORD ColorToColorAttr(ConsoleColor color, ConsoleColor defaultColor = ConsoleColor::Red) {
			if (defaultColor == ConsoleColor::Default) {
				// will cause infinite recursion
				// defaultColor must be set to specific color
				assert(false);
				defaultColor = ConsoleColor::Red;
			}

			auto thisFn = [](ConsoleColor color, ConsoleColor defaultColor = ConsoleColor::Red)
			{
				return ColorToColorAttr<RedAttr, GreenAttr, BlueAttr, IntensifyAttr>(color, defaultColor);
			};

			switch (color) {
			case ConsoleColor::Default: return thisFn(defaultColor, defaultColor);
			case ConsoleColor::Red: return RedAttr;
			case ConsoleColor::Green: return GreenAttr;
			case ConsoleColor::Blue: return BlueAttr;
			case ConsoleColor::Yellow: return RedAttr | GreenAttr;
			case ConsoleColor::Magenta: return RedAttr | BlueAttr;
			case ConsoleColor::Cyan: return GreenAttr | BlueAttr;
			case ConsoleColor::LightGray: return RedAttr | GreenAttr | BlueAttr;
			case ConsoleColor::Black: return 0;
			case ConsoleColor::LightRed: return thisFn(ConsoleColor::Red) | IntensifyAttr;
			case ConsoleColor::LightGreen: return thisFn(ConsoleColor::Green) | IntensifyAttr;
			case ConsoleColor::LightBlue: return thisFn(ConsoleColor::Blue) | IntensifyAttr;
			case ConsoleColor::LightYellow: return thisFn(ConsoleColor::Yellow) | IntensifyAttr;
			case ConsoleColor::LightMagenta: return thisFn(ConsoleColor::Magenta) | IntensifyAttr;
			case ConsoleColor::LightCyan: return thisFn(ConsoleColor::Cyan) | IntensifyAttr;
			case ConsoleColor::White: return thisFn(ConsoleColor::LightGray) | IntensifyAttr;
			case ConsoleColor::DarkGray: return thisFn(ConsoleColor::Black) | IntensifyAttr;
			}

			// must not happen
			assert(false);
			return thisFn(ConsoleColor::Red);
		}
	};
}

struct test_op final : boost::cobalt::op<int> {
	test_op(boost::asio::io_context& io, int res, int waitMs)
		: res(res)
		, waitMs(waitMs)
		, io(io)
	{}

	void ready(boost::cobalt::handler<int> h) override {
		//h(123);
	}

	void initiate(boost::cobalt::completion_handler<int> complete) override {
		std::thread(
			[completeTmp = std::move(complete), this] () mutable
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(this->waitMs));

				boost::asio::post(this->io,
					[complete = std::move(completeTmp), this] () mutable
					{
						complete(this->res);
					});
			}).detach();
	}

	int res = 123;
	int waitMs = 1000;
	boost::asio::io_context& io;
};

boost::cobalt::task<void> my_task_wait1(boost::asio::io_context& io) {
	co_await test_op(io, 100, 30000000);
	co_await test_op(io, 100, 1000);
	co_await test_op(io, 100, 1000);
	co_return;
}

boost::cobalt::task<void> my_task_wait2(boost::asio::io_context& io) {
	co_await test_op(io, 100, 1000);
	co_return;
}

boost::cobalt::task<void> my_task(boost::asio::io_context& io) {
	{
		auto t1 = my_task_wait1(io);
		auto t2 = my_task_wait2(io);

		auto v = co_await boost::cobalt::race(t1, t2);

		int stop = 234;
	}

	co_await test_op(io, 100, 1000000000);

	co_return;
}

// https://www.crazygaze.com/blog/2016/03/17/how-strands-work-and-why-you-should-use-them/

int main() {
	TEST_HELPERS::Helpers::SetConsoleColor(TEST_HELPERS::ConsoleColor::Default);

	//{
	//	//boost::cobalt::run(my_task()); // sync
	//	boost::asio::io_context ctx;
	//	boost::cobalt::spawn(ctx, my_task(ctx), boost::asio::detached);
	//	ctx.run();
	//	int stop = 234;
	//}

	{
		Dx11Window wnd(L"TorrentWndClass", L"Torrent");

		WindowsThreadMessageQueue wndMsgQueue;

		while (true) {
			wndMsgQueue.WaitAndProcessQueuedMessages();
		}
	}

	{
		ULONG buf_size = 10000;
		std::vector<char> buffer(buf_size);
		PIP_ADAPTER_ADDRESSES adapter_addresses
			= reinterpret_cast<IP_ADAPTER_ADDRESSES*>(&buffer[0]);

		//ULONG flags = GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_ANYCAST;

		ULONG flags = 0;// GAA_FLAG_INCLUDE_PREFIX;

		DWORD res = GetAdaptersAddresses(AF_UNSPEC, flags, nullptr, adapter_addresses, &buf_size);
		if (res == ERROR_BUFFER_OVERFLOW)
		{
			buffer.resize(buf_size);
			adapter_addresses = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(&buffer[0]);
			res = GetAdaptersAddresses(AF_UNSPEC, flags, nullptr, adapter_addresses, &buf_size);
		}
		if (res != NO_ERROR)
		{
			assert(false);
			/*ec = error_code(WSAGetLastError(), system_category());
			return std::vector<ip_interface>();*/
		}

		for (PIP_ADAPTER_ADDRESSES adapter = adapter_addresses;
			adapter != nullptr; adapter = adapter->Next)
		{
			int stop = 234;
		}

	}

	lt::settings_pack settingsPack;

	settingsPack.set_str(lt::settings_pack::string_types::outgoing_interfaces, "{0695B14B-DBF7-0AD7-D2D7-EF9E329450D6}");
	settingsPack.set_str(lt::settings_pack::string_types::listen_interfaces, "{0695B14B-DBF7-0AD7-D2D7-EF9E329450D6}:55651");

	settingsPack.set_bool(lt::settings_pack::bool_types::enable_dht, true);
	settingsPack.set_bool(lt::settings_pack::bool_types::enable_lsd, true);
	settingsPack.set_bool(lt::settings_pack::bool_types::enable_natpmp, true);
	settingsPack.set_bool(lt::settings_pack::bool_types::enable_upnp, true);

	settingsPack.set_int(lt::settings_pack::int_types::alert_mask, lt::alert_category::all);

	lt::session_params sessionParams(std::move(settingsPack));

	lt::session::paused;

	lt::session session(std::move(sessionParams), lt::session::paused);

	auto torrent = session.find_torrent({});

	//torrent.get_peer_info

	if (session.is_paused()) {
		int stop = 234;
	}

	std::condition_variable alertCv;
	std::mutex alertMtx;

	int alertCounter = 0;
	std::atomic<int> alertNotifyCounter = 0;

	session.set_alert_notify([&]
		{
			++alertNotifyCounter;
			alertNotifyCounter.notify_one();
		});

	session.resume();

	TEST_HELPERS::ConsoleColor colors[] =
	{
		TEST_HELPERS::ConsoleColor::Green,
		TEST_HELPERS::ConsoleColor::LightBlue,
		TEST_HELPERS::ConsoleColor::LightMagenta,
		TEST_HELPERS::ConsoleColor::LightRed,
		TEST_HELPERS::ConsoleColor::Default,
	};
	int colorsIdx = 0;

	while (true) {
		alertNotifyCounter.wait(alertCounter);
		alertCounter = alertNotifyCounter.load();

		std::vector<lt::alert*> alerts;

		session.pop_alerts(&alerts);

		for (auto& alert : alerts) {
			int alertType = alert->type();
			auto name = lt::alert_name(alertType);

			/*switch (alertType) {
			case lt::portmap_log_alert::alert_type: {
				auto concreteAlert = static_cast<lt::portmap_log_alert*>(alert);
				break;
			}
			case lt::log_alert::alert_type: {
				auto concreteAlert = static_cast<lt::log_alert*>(alert);
				break;
			}
			case lt::dht_log_alert::alert_type: {
				auto concreteAlert = static_cast<lt::dht_log_alert*>(alert);
				break;
			}
			case lt::peer_log_alert::alert_type: {
				auto concreteAlert = static_cast<lt::peer_log_alert*>(alert);
				break;
			}
			case lt::picker_log_alert::alert_type: {
				auto concreteAlert = static_cast<lt::picker_log_alert*>(alert);
				break;
			}
			case lt::torrent_log_alert::alert_type: {
				auto concreteAlert = static_cast<lt::torrent_log_alert*>(alert);
				break;
			}
			default:
				break;
			}*/

			/*switch (alertType) {
			case lt::portmap_log_alert::alert_type: {
				break;
			}
			case lt::portmap_alert::alert_type: {
				break;
			}
			case lt::portmap_error_alert::alert_type: {
				break;
			}
			default:
				continue;
			}*/

			switch (alertType) {
			case lt::dht_log_alert::alert_type:
			case lt::dht_pkt_alert::alert_type:
			case lt::dht_outgoing_get_peers_alert::alert_type:
				continue;
			default: break;
			}

			TEST_HELPERS::Helpers::SetConsoleColor(TEST_HELPERS::ConsoleColor::Black, TEST_HELPERS::ConsoleColor::LightYellow);
			std::cout << name << ": ";
			//TEST_HELPERS::Helpers::SetConsoleColor(colors[colorsIdx]);
			TEST_HELPERS::Helpers::SetConsoleColor(colors[alertType % (int)std::size(colors)]);
			std::cout << alert->message();
			TEST_HELPERS::Helpers::SetConsoleColor(TEST_HELPERS::ConsoleColor::Default);

			std::cout << std::endl;
			++colorsIdx;
			colorsIdx %= (int)std::size(colors);
		}

	}

	return 0;
}
