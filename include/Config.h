#pragma once
#include <any>
#include <map>
#include <functional>
#include <unordered_map>

class Config {
public:
   static Config* get();

   enum class Option : int {
      ImGuiEnabled = 0x0 
   };

   using cb_function = std::function<void(bool)>;
   
   void add_listener(Option option, cb_function cb);
   void remove_listener(Option option);

   bool is_option_enabled(const Option& option) const { return m_config.at(option); }
   void set_option_enabled(Option option, bool enabled);
private:
   Config();
   ~Config();
   std::map<Option, bool> m_config;
   std::map<Option, cb_function> m_listeners;
};
