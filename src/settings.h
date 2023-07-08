//
// Created by ShrBox on 2023/2/4.
//
#include <llapi/LoggerAPI.h>
#include <Nlohmann/json.hpp>

#define INITJSON(key, val)                           \
	if (json.find(key) != json.end())             \
	{                                             \
		const nlohmann::json &out = json.at(key); \
		out.get_to(val);                          \
	}

extern Logger logger;

namespace Settings {
    std::string prompt = "You are a AI helper in the Minecraft Bedrock Edition server";
    std::string proxy = "";
    std::string apikey = "";
    std::string model = "gpt-3.5-turbo";
    nlohmann::json globaljson()
    {
        nlohmann::json json;
        json["prompt"] = prompt;
        json["proxy"] = proxy;
        json["apikey"] = apikey;
        json["model"] = model;
        return json;
    }

    void initjson(nlohmann::json json)
    {
        INITJSON("prompt", prompt);
        INITJSON("proxy", proxy);
        INITJSON("apikey", apikey)
        INITJSON("model", model);
    }
    void WriteDefaultConfig(const std::string &fileName)
    {
        std::ofstream file(fileName);
        if (!file.is_open())
        {
            logger.error("Can't open file {}", fileName);
            return;
        }
        auto json = globaljson();
        file << json.dump(4);
        file.close();
    }
    void LoadConfigFromJson(const std::string &fileName)
    {
        std::ifstream file(fileName);
        if (!file.is_open())
        {
            logger.error("Can't open file {}", fileName);
            return;
        }
        nlohmann::json json;
        file >> json;
        file.close();
        initjson(json);
        WriteDefaultConfig(fileName);
    }
    void reloadJson(const std::string &fileName)
    {
        std::ofstream file(fileName);
        if (file)
        {
            file << globaljson().dump(4);
        }
        else
        {
            logger.error("Configuration File Creation failed!");
        }
        file.close();
    }
}