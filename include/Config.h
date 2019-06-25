#pragma once
#include <any>
#include <map>
#include <functional>
#include <unordered_map>
#include <variant>
#include <string>
#include <glm/glm.hpp>
#include "physbody.h"

class Config {
public:
   static Config* get();
   using config_value = std::variant<int, bool, vec3d>;
   enum class Option : int {
      ImGuiEnabled = 0x0,
      ShipPosition = 0x1
   };

   using cb_function = std::function<void(std::any)>;
   
   void add_listener(Option option, cb_function cb);
   void remove_listener(Option option);

   bool is_option_available(Option option) const { return m_config.at(option).index() != std::variant_npos; }
   template <typename T>
   T get_option(Option option) const { return std::get<T>(m_config.at(option)); }
   template <typename T>
   void set_option_value(Option option, const T& value) {
       m_config.insert_or_assign(option, value);
       if (m_listeners.find(option) != m_listeners.end()) {
          auto f = m_listeners[option];
          f(value);
       }
   }
private:
   Config();
   ~Config();
   std::map<Option, config_value> m_config;
   std::map<Option, cb_function> m_listeners;
};
