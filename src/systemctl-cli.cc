#include <string>
#include <vector>
#include <tuple>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <sdbus-c++/sdbus-c++.h>
#include <CLI/App.hpp>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>

const std::string kDestination = "org.freedesktop.systemd1";
const std::string kObjectPath  = "/org/freedesktop/systemd1";
const std::string kInterface   = "org.freedesktop.systemd1.Manager";

const std::string kMethodStart   = "StartUnit";
const std::string kMethodStop    = "StopUnit";
const std::string kMethodRestart = "RestartUnit";
const std::string kMethodEnable  = "EnableUnitFiles";
const std::string kMethodDisable = "DisableUnitFiles";
const std::string kMethodReload  = "Reload";

int main(int argc, char* argv[]) {
	CLI::App app{"A simple program to start/stop/etc systemd services"};

	std::string unit    = "default";
	std::string cmd_str = "default";
	app.add_option("-u,--unit", unit, "Unit to do stuff to");
	app.add_option("-c,--cmd", cmd_str, "What to do to the unit(start/stop/restart/enable/disable)");
	CLI11_PARSE(app, argc, argv);
	unit += ".service";

	// Create proxy
	std::unique_ptr<sdbus::IProxy> proxy = sdbus::createProxy(kDestination, kObjectPath);

	// Invoke a function call

	if (cmd_str == "start") {
		sdbus::ObjectPath job;
		proxy->callMethod(kMethodStart).onInterface(kInterface).withArguments(unit, "replace").storeResultsTo(job);
		fmt::print("Job: {}\n", job);
	} else if (cmd_str == "stop") {
		sdbus::ObjectPath job;
		proxy->callMethod(kMethodStop).onInterface(kInterface).withArguments(unit, "replace").storeResultsTo(job);
		fmt::print("Job: {}\n", job);
	} else if (cmd_str == "restart") {
		sdbus::ObjectPath job;
		proxy->callMethod(kMethodRestart).onInterface(kInterface).withArguments(unit, "replace").storeResultsTo(job);
		fmt::print("Job: {}\n", job);
	} else if (cmd_str == "enable") {
		bool                                                              carries_install_info;
		std::vector<sdbus::Struct<std::string, std::string, std::string>> changes;

		proxy->callMethod(kMethodEnable)
			.onInterface(kInterface)
			.withArguments(std::vector<std::string>{unit}, false, true)
			.storeResultsTo(carries_install_info, changes);
		proxy->callMethod(kMethodReload).onInterface(kInterface);

		for (const std::tuple<std::string, std::string, std::string>& change : changes) {
			fmt::print("Change: {}\n", change);
		}
	} else if (cmd_str == "disable") {
		std::vector<sdbus::Struct<std::string, std::string, std::string>> changes;

		proxy->callMethod(kMethodDisable)
			.onInterface(kInterface)
			.withArguments(std::vector<std::string>{unit}, false)
			.storeResultsTo(changes);
		proxy->callMethod(kMethodReload).onInterface(kInterface);

		for (const std::tuple<std::string, std::string, std::string>& change : changes) {
			fmt::print("Change: {}\n", change);
		}
	} else {
		fmt::print("your command is bullshit\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}