
#include "Config.h"

Config* Config::get() {
   static Config config;
   return &config;
}

Config::Config() {
   // Set default values here:
   m_config[Option::ImGuiEnabled] = true;
   m_config[Option::ShipPosition] = vec3d({ 0., 0., 0. });
}

Config::~Config() {
   m_config.clear();
   m_listeners.clear();
}

void Config::add_listener(Option option, cb_function cb) {
   if (m_listeners.find(option) == m_listeners.end()) {
      m_listeners.insert_or_assign(option, cb);
   }
}

void Config::remove_listener(Option option) {
   if (m_listeners.find(option) != m_listeners.end()) {
      m_listeners.erase(option);
   }
}
