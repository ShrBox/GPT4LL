#include <openai/openai.hpp>
#include <llapi/EventAPI.h>
#include <llapi/ScheduleAPI.h>
#include <llapi/Global.h>
#include <llapi/mc/Level.hpp>
#include <llapi/DynamicCommandAPI.h>

extern Logger logger;

namespace ChatModule {
    bool hasError;
    nlohmann::basic_json init_request_header;
    nlohmann::basic_json request_header;
    std::vector<std::string> enabledPlayers;
    std::string reply_format;

    bool processChatMessage(Event::PlayerChatEvent ev) {
        if (std::find(enabledPlayers.begin(), enabledPlayers.end(), ev.mPlayer->getXuid()) != enabledPlayers.end()) {
            std::string &messageReceived = ev.mMessage;
            std::string playerName = ev.mPlayer->getRealName();
            std::thread([messageReceived, playerName] {
                try {
                    if (request_header.empty()) {
                        request_header = init_request_header;
                    }
                    request_header.at("messages").push_back({{"role",    "user"},
                                                             {"name",    playerName},
                                                             {"content", messageReceived}});
                    nlohmann::basic_json reply = openai::chat().create(request_header);
                    std::string msg;
                    if (reply.find("choices") != reply.end()) {
                        nlohmann::json &choices = reply.at("choices")[0];
                        if (choices.find("message") != choices.end()) {
                            nlohmann::json &message = choices.at("message");
                            msg = message.value("content", "");
                            request_header.at("messages").push_back(message);
                        }
                    }
                    logger.info("{}{}", reply_format, msg);
                    Schedule::nextTick([msg] {
                        Global<Level>->broadcastText(reply_format + msg, TextType::RAW);
                    });
                } catch (std::exception &e) {
                    hasError = true;
                    logger.error(e.what());
                }
            }).detach();
        }
        return true;
    }

    void registerCommands() {
        using ParamType = DynamicCommand::ParameterType;
        using Param = DynamicCommand::ParameterData;
        DynamicCommand::setup("chatgpt", "GPT for LiteLoaderBDS", {{"mode", {"chat", "renew"}}},
                              {Param("mode_enum", ParamType::Enum, false, "mode")},
                              {{"mode"}},
                              [](DynamicCommand const &command, CommandOrigin const &origin, CommandOutput &output,
                                 std::unordered_map<std::string, DynamicCommand::Result> &results) {
                                  auto action = results["mode_enum"].get<std::string>();
                                  switch (do_hash(action.c_str())) {
                                      case do_hash("chat"): {
                                          if (origin.getOriginType() != (CommandOriginType) OriginType::Player) {
                                              output.error("You are not player");
                                              return;
                                          }
                                          std::string xuid = origin.getPlayer()->getXuid();
                                          auto it = std::find(enabledPlayers.begin(), enabledPlayers.end(), xuid);
                                          if (it != enabledPlayers.end()) {
                                              enabledPlayers.erase(it);
                                              output.success("Chat disabled");
                                          } else {
                                              enabledPlayers.push_back(xuid);
                                              output.success("Chat enabled");
                                          }
                                          break;
                                      }
                                      case do_hash("renew"): {
                                          if (origin.getPermissionsLevel() >= CommandPermissionLevel::GameMasters ||
                                              ChatModule::hasError) {
                                              request_header.clear();
                                              ChatModule::hasError = false;
                                              output.success("New conversation started");
                                          } else {
                                              output.error("No permission");
                                          }
                                          break;
                                      }
                                      default:
                                          return;
                                  }
                              }, CommandPermissionLevel::Any);
    }

    void Init(std::string &prompt, std::string &model, std::string &format) {
        registerCommands();
        init_request_header = nlohmann::basic_json({
                                                           {"model",    model},
                                                           {"messages", {{{"role", "system"}, {"content", prompt}}}}
                                                   });
        reply_format = format;
        Event::PlayerChatEvent::subscribe(processChatMessage);
    }
}