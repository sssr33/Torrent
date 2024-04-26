
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

#pragma comment(lib, "Iphlpapi.lib")

/*
max_queued_disk_bytes
hashing_threads
aio_threads

NEED TO COPY PDB for all libs
*/

int main() {
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

	while (true) {
		alertNotifyCounter.wait(alertCounter);
		alertCounter = alertNotifyCounter.load();

		std::vector<lt::alert*> alerts;

		session.pop_alerts(&alerts);

		for (auto& alert : alerts) {
			int alertType = alert->type();
			auto name = lt::alert_name(alertType);

			std::cout << name << ": ";
			std::cout << alert->message();

			switch (alertType) {
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
			}

			std::cout << std::endl;
		}

	}

	return 0;
}
