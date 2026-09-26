#include "AccountExport.h"
#include "AccountExportCore.h"
#include <GWCA/Constants/Constants.h>
#include <GWCA/Context/AccountContext.h>
#include <GWCA/Context/CharContext.h>
#include <GWCA/Context/WorldContext.h>
#include <GWCA/GameContainers/Array.h>
#include <GWCA/GameEntities/Agent.h>
#include <GWCA/GameEntities/Hero.h>
#include <GWCA/GameEntities/Title.h>
#include <GWCA/Managers/AgentMgr.h>
#include <GWCA/Managers/ChatMgr.h>
#include <GWCA/Managers/MapMgr.h>
#include <GWCA/Managers/PlayerMgr.h>
#include <GWCA/Utilities/Hook.h>
#include <imgui.h>
#include <string>
#include <vector>
namespace {
GW::HookEntry ChatCmd_HookEntry;
std::string WStringToUtf8(const wchar_t* wstr){if(!wstr||!*wstr)return{};const int n=WideCharToMultiByte(CP_UTF8,0,wstr,-1,nullptr,0,nullptr,nullptr);if(n<=1)return{};std::string out(static_cast<size_t>(n),'\0');const int written=WideCharToMultiByte(CP_UTF8,0,wstr,-1,out.data(),n,nullptr,nullptr);if(written<=0)return{};out.resize(static_cast<size_t>(written)-1);return out;}
template<typename A>std::vector<uint32_t> CopyWords(const A&a){std::vector<uint32_t>w;if(a.valid()){w.reserve(a.size());for(uint32_t i=0;i<a.size();i++)w.push_back(a[i]);}return w;}
void ExportTitles(GW::HookStatus*,const wchar_t*,int,const LPWSTR*){
 const auto* world=GW::GetWorldContext();const auto* account=GW::GetAccountContext();const auto* character=GW::GetCharContext();const auto* player=GW::Agents::GetControlledCharacter();
 if(!world||!account||!character||!player){GW::Chat::WriteChat(GW::Chat::Channel::CHANNEL_WARNING,L"[GWST Export] Not in game yet - load a character first.",nullptr,true);return;}
 account_export::AccountSnapshot s;s.character_name_utf8=WStringToUtf8(character->player_name);s.primary_profession_id=static_cast<uint32_t>(player->primary);s.secondary_profession_id=static_cast<uint32_t>(player->secondary);s.level=player->level;s.map_id=static_cast<uint32_t>(GW::Map::GetMapID());
 for(uint32_t i=0;i<static_cast<uint32_t>(GW::Constants::TitleID::None);i++){const auto id=static_cast<GW::Constants::TitleID>(i);GW::Title* t=GW::PlayerMgr::GetTitleTrack(id);if(!t)continue;s.titles.push_back({i,t->current_points,t->current_title_tier_index,t->points_needed_current_rank,t->next_title_tier_index,t->points_needed_next_rank,t->max_title_rank,t->max_title_tier_index,t->is_percentage_based(),t->has_tiers()});}
 const auto& heroes=world->hero_info;if(heroes.valid()){s.heroes.reserve(heroes.size());for(uint32_t i=0;i<heroes.size();i++){const GW::HeroInfo&h=heroes[i];s.heroes.push_back({static_cast<uint32_t>(h.hero_id),h.level,static_cast<uint32_t>(h.primary),static_cast<uint32_t>(h.secondary)});}}
 s.unlocked_account_skills=CopyWords(account->unlocked_account_skills);s.learned_character_skills=CopyWords(world->unlocked_character_skills);
 const std::string json=account_export::BuildAccountJson(s);ImGui::SetClipboardText(json.c_str());wchar_t message[160];swprintf(message,_countof(message),L"[GWST Export] Title export copied to clipboard (%zu title tracks).",s.titles.size());GW::Chat::WriteChat(GW::Chat::Channel::CHANNEL_GLOBAL,message,nullptr,true);
}}
DLLAPI ToolboxPlugin* ToolboxPluginInstance(){static AccountExport instance;return &instance;}
void AccountExport::Initialize(ImGuiContext*ctx,const ImGuiAllocFns allocator_fns,const HMODULE toolbox_dll){ToolboxPlugin::Initialize(ctx,allocator_fns,toolbox_dll);GW::Chat::CreateCommand(&ChatCmd_HookEntry,L"exporttitles",ExportTitles);}
void AccountExport::SignalTerminate(){ToolboxPlugin::SignalTerminate();GW::Chat::DeleteCommand(&ChatCmd_HookEntry);}
