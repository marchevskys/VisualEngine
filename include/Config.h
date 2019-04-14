#pragma once
#include <any>
#include <map>
#include <functional>
#include <unordered_map>

class Config {
public:
   static Config* get();

   enum class Option : int {
      ImGuiEnabled = 0x0,
      ShipPosition = 0x1
   };

   using cb_function = std::function<void(std::any)>;
   
   void add_listener(Option option, cb_function cb);
   void remove_listener(Option option);

   bool is_option_available(Option option) const { return m_config.at(option).has_value(); }
   std::any get_option(Option option) const { return m_config.at(option); }
   void set_option_value(Option option, const std::any& value);
private:
   Config();
   ~Config();
   std::map<Option, std::any> m_config;
   std::map<Option, cb_function> m_listeners;
};
