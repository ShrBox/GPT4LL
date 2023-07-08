/**
 * @file plugin.cpp
 * @brief The main file of the plugin
 */

#include <llapi/LoggerAPI.h>

#include "version.h"
#include <openai/openai.hpp>
#include "module/chat.h"
#include "settings.h"

// We recommend using the global logger.
extern Logger logger;

/**
 * @brief The entrypoint of the plugin. DO NOT remove or rename this function.
 *        
 */

void InitConfig() {
    if (!std::filesystem::exists("plugins/GPT4LL"))
        std::filesystem::create_directories("plugins/GPT4LL");
    if (std::filesystem::exists("plugins/GPT4LL/config.json")) {
        try {
            Settings::LoadConfigFromJson("plugins/GPT4LL/config.json");
        }
        catch (std::exception &e) {
            logger.error("Configuration file is Invalid, Error: {}", e.what());
        }
        catch (...) {
            logger.error("Configuration file is Invalid");
        }
    } else {
        Settings::WriteDefaultConfig("plugins/GPT4LL/config.json");
    }
}

void InitOpenAI() {
    auto &ChatAI = openai::start(Settings::apikey);
    if (!Settings::proxy.empty()) ChatAI.setProxy(Settings::proxy);
}

void PluginInit() {
    InitConfig();
    InitOpenAI();
    ChatModule::Init(Settings::prompt, Settings::model, Settings::format);
}