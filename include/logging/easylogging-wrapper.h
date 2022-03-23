#ifndef EASYLOGGING_WRAPPER_H
#define EASYLOGGING_WRAPPER_H

#include <map>
#include <string>
#include <variant>
#include <easylogging++.h>

inline void InitLogging(const std::string &filename = "") {
	el::Configurations defaultConf;
	defaultConf.setToDefault();
	defaultConf.setGlobally(el::ConfigurationType::Format, R"({"datetime":"%datetime{%Y-%M-%dT%H:%m:%s.%g}", %msg})");
	// TODO: load configuration from a file so this can be changed on the fly
	defaultConf.set(el::Level::Debug, el::ConfigurationType::Enabled, "true");
	defaultConf.set(el::Level::Trace, el::ConfigurationType::Enabled, "true");
	defaultConf.set(
		el::Level::Trace, el::ConfigurationType::Format,
		R"({"datetime":"%datetime{%Y-%M-%dT%H:%m:%s.%g}", loc: "%loc", func: "%func", %msg})");
	if (filename.empty()) {
		defaultConf.setGlobally(el::ConfigurationType::ToFile, "false");
	} else {
		defaultConf.setGlobally(el::ConfigurationType::ToFile, "true");
		defaultConf.setGlobally(el::ConfigurationType::Filename, filename);
	}
	el::Loggers::reconfigureLogger("default", defaultConf);
}

inline std::string makeLog(
	const el::Level level, const int group, const int code,
	const std::map<std::string, std::variant<int, float, bool, std::string>> &event_map) {
	int         event_code = ((int)level << 8) | (group << 4) | (code << 1);
	std::string log        = "e: " + std::to_string(event_code) + ", ";
	for (const auto &[event_string, event_value] : event_map) {
		log += event_string + ": ";
		if (const int *intval = std::get_if<int>(&event_value)) {
			log += std::to_string(*intval);
		} else if (const float *floatval = std::get_if<float>(&event_value)) {
			log += std::to_string(*floatval);
		} else if (const bool *boolval = std::get_if<bool>(&event_value)) {
			log += std::to_string(*boolval);
		} else if (const std::string *stringval = std::get_if<std::string>(&event_value)) {
			log += "\"" + *stringval + "\"";
		} else {
			return "you chose the wrong house, fool!\n";
		}
		log += ", ";
	}
	return log;
}

inline void doLog(
	const el::Level level, const int group, const int code,
	const std::map<std::string, std::variant<int, float, bool, std::string>> &event_map) {
	std::string log = makeLog(level, group, code, event_map);
	switch (level) {
		case el::Level::Debug:
			LOG(DEBUG) << log;
			break;
		case el::Level::Error:
			LOG(ERROR) << log;
			break;
		case el::Level::Fatal:
			LOG(FATAL) << log;
			break;
		case el::Level::Info:
			LOG(INFO) << log;
			break;
		case el::Level::Trace:
			LOG(TRACE) << log;
			break;
		default:
			break;
	}
}

#endif
