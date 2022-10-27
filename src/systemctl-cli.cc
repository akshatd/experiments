#include <string>
#include <vector>
#include <tuple>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <sdbus-c++/sdbus-c++.h>
#include <CLI/App.hpp>
#include <CLI/Formatter.hpp>
#include <CLI/Config.hpp>

const std::string kDestination   = "org.freedesktop.systemd1";
const std::string kObjectPath    = "/org/freedesktop/systemd1";
const std::string kInterfaceMngr = "org.freedesktop.systemd1.Manager";
const std::string kInterfaceUnit = "org.freedesktop.systemd1.Unit";

const std::string kMthdStart   = "StartUnit";
const std::string kMthdStop    = "StopUnit";
const std::string kMthdRestart = "RestartUnit";
const std::string kMthdEnable  = "EnableUnitFiles";
const std::string kMthdDisable = "DisableUnitFiles";
const std::string kMthdReload  = "Reload";
const std::string kMthdGetUnit = "GetUnit";

const std::string kPropActiveState = "ActiveState";
const std::string kPropFileState   = "UnitFileState";

int main(int argc, char* argv[]) {
	CLI::App app{"A simple program to start/stop/etc systemd services"};

	std::string unit    = "default";
	std::string cmd_str = "default";
	app.add_option("-u,--unit", unit, "Unit to do stuff to");
	app.add_option("-c,--cmd", cmd_str, "What to do to the unit(start/stop/restart/enable/disable/status)");
	try {
		(app).parse((argc), (argv));
	} catch (const CLI::ParseError& e) {
		return (app).exit(e);
	}
	unit += ".service";

	std::unique_ptr<sdbus::IProxy> proxy = sdbus::createProxy(kDestination, kObjectPath);
	if (cmd_str == "start") {
		sdbus::ObjectPath job;
		proxy->callMethod(kMthdStart).onInterface(kInterfaceMngr).withArguments(unit, "replace").storeResultsTo(job);
		fmt::print("Job: {}\n", job);
	} else if (cmd_str == "stop") {
		sdbus::ObjectPath job;
		proxy->callMethod(kMthdStop).onInterface(kInterfaceMngr).withArguments(unit, "replace").storeResultsTo(job);
		fmt::print("Job: {}\n", job);
	} else if (cmd_str == "restart") {
		sdbus::ObjectPath job;
		proxy->callMethod(kMthdRestart).onInterface(kInterfaceMngr).withArguments(unit, "replace").storeResultsTo(job);
		fmt::print("Job: {}\n", job);
	} else if (cmd_str == "enable") {
		bool                                                              carries_install_info;
		std::vector<sdbus::Struct<std::string, std::string, std::string>> changes;

		proxy->callMethod(kMthdEnable)
			.onInterface(kInterfaceMngr)
			.withArguments(std::vector<std::string>{unit}, false, true)
			.storeResultsTo(carries_install_info, changes);
		proxy->callMethod(kMthdReload).onInterface(kInterfaceMngr);

		for (const std::tuple<std::string, std::string, std::string>& change : changes) {
			fmt::print("Change: {}\n", change);
		}
	} else if (cmd_str == "disable") {
		std::vector<sdbus::Struct<std::string, std::string, std::string>> changes;

		proxy->callMethod(kMthdDisable)
			.onInterface(kInterfaceMngr)
			.withArguments(std::vector<std::string>{unit}, false)
			.storeResultsTo(changes);
		proxy->callMethod(kMthdReload).onInterface(kInterfaceMngr);

		for (const std::tuple<std::string, std::string, std::string>& change : changes) {
			fmt::print("Change: {}\n", change);
		}
	} else if (cmd_str == "status") {
		sdbus::ObjectPath unit_object_path;
		proxy->callMethod(kMthdGetUnit).onInterface(kInterfaceMngr).withArguments(unit).storeResultsTo(unit_object_path);
		std::unique_ptr<sdbus::IProxy> unit_proxy = sdbus::createProxy(kDestination, unit_object_path);

		std::string active_state = unit_proxy->getProperty(kPropActiveState).onInterface(kInterfaceUnit);
		std::string file_state   = unit_proxy->getProperty(kPropFileState).onInterface(kInterfaceUnit);
		fmt::print(
			"State of {} at {}:\nActive state: {}\nFile state: {}\n", unit, unit_object_path, active_state, file_state);
	} else {
		fmt::print("your command is bullshit\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}