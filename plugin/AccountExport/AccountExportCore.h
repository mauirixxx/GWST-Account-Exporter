#pragma once
#include <cstdint>
#include <vector>
#include <cstdio>
#include <iterator>
#include <string>
#include <algorithm>
#include "hero-names.generated.h"
namespace account_export {
inline const char* HeroName(const uint32_t hero_id) { return hero_id < std::size(kHeroNames) ? kHeroNames[hero_id] : "Unknown"; }
inline const char* TitleName(const uint32_t id) {
    static constexpr const char* names[] = {
        "Hero", "Tyrian Cartographer", "Canthan Cartographer", "Gladiator", "Champion", "Kurzick",
        "Luxon", "Drunkard", "Deprecated Skill Hunter", "Survivor", "Kind of a Big Deal",
        "Deprecated Treasure Hunter", "Deprecated Wisdom", "Protector of Tyria", "Protector of Cantha",
        "Lucky", "Unlucky", "Sunspear", "Elonian Cartographer", "Protector of Elona", "Lightbringer",
        "Legendary Defender of Ascalon", "Commander", "Gamer", "Tyrian Skill Hunter", "Tyrian Vanquisher",
        "Canthan Skill Hunter", "Canthan Vanquisher", "Elonian Skill Hunter", "Elonian Vanquisher",
        "Legendary Cartographer", "Legendary Guardian", "Legendary Skill Hunter", "Legendary Vanquisher",
        "Sweet Tooth", "Guardian of Tyria", "Guardian of Cantha", "Guardian of Elona", "Asura", "Deldrimor",
        "Ebon Vanguard", "Norn", "Master of the North", "Party Animal", "Zaishen", "Treasure Hunter", "Wisdom", "Codex"
    };
    return id < std::size(names) ? names[id] : "Unknown";
}
inline uint32_t ToolboxTitleMax(const uint32_t id) {
    switch (id) {
        case 7: case 43: case 34: case 4: case 45: case 46: return 10000; // Drunkard, Party, Sweets, Champion, Treasure, Wisdom
        case 9: return 1337500; // Survivor
        case 10: return 30; // KoaBD
        case 17: case 20: return 50000; // Sunspear, Lightbringer
        case 38: case 39: case 40: case 41: return 160000; // EotN reputation
        case 42: return 1000; // Master of the North
        case 47: case 3: return 200000; // Codex, Gladiator
        case 23: return 135000; // Gamer
        case 44: case 0: return 100000; // Zaishen, Hero
        case 15: return 2500000; // Lucky
        case 16: return 500000; // Unlucky
        case 5: case 6: return 10000000; // Kurzick, Luxon
        default: return 0;
    }
}
inline void JsonEscapeInto(std::string& out, const std::string& value) { for (const char c : value) { switch(c) { case '"': out += "\\\""; break; case '\\': out += "\\\\"; break; case '\n': out += "\\n"; break; case '\r': out += "\\r"; break; case '\t': out += "\\t"; break; default: if (static_cast<unsigned char>(c) < 0x20) { char b[8]; snprintf(b,sizeof(b),"\\u%04x",c); out += b; } else out += c; } } }
template <typename A> void AppendSkillBitfield(std::string& out,const A& a) { out+='['; bool first=true; if(a.valid()) for(uint32_t wi=0;wi<a.size();wi++){uint32_t w=a[wi];for(uint32_t bit=0;bit<32;bit++)if(w&(1u<<bit)){if(!first)out+=',';out+=std::to_string(wi*32+bit);first=false;}} out+=']'; }
struct HeroSnapshot { uint32_t id,level,primary_profession_id,secondary_profession_id; };
struct TitleSnapshot { uint32_t id,current_points,current_title_tier_index,points_needed_current_rank,next_title_tier_index,points_needed_next_rank,max_title_rank,max_title_tier_index,current_rank,next_rank; bool percentage_based,has_tiers; };
struct AccountSnapshot { std::string character_name_utf8; uint32_t primary_profession_id,secondary_profession_id,level,map_id; std::vector<HeroSnapshot> heroes; std::vector<TitleSnapshot> titles; std::vector<uint32_t> unlocked_account_skills,learned_character_skills; };
inline double ToolboxProgress(const TitleSnapshot& t) {
    if (t.percentage_based) return static_cast<double>(std::min(t.current_points,1000u))/1000.0;
    if (t.points_needed_next_rank == 0xFFFFFFFFu) return 1.0;
    if (!t.current_points) return 0.0;
    uint32_t target=ToolboxTitleMax(t.id); if(!target) target=t.points_needed_next_rank;
    if(!target || target==0xFFFFFFFFu) return 1.0;
    return std::min(1.0,static_cast<double>(t.current_points)/static_cast<double>(target));
}
namespace detail { struct VectorBitfield { const std::vector<uint32_t>& words; bool valid()const{return true;} uint32_t size()const{return static_cast<uint32_t>(words.size());} uint32_t operator[](uint32_t i)const{return words[i];} }; }
inline std::string BuildAccountJson(const AccountSnapshot& s) {
 std::vector<uint32_t> sort_order(s.titles.size()); for(uint32_t i=0;i<sort_order.size();++i)sort_order[i]=i;
 std::stable_sort(sort_order.begin(),sort_order.end(),[&](uint32_t a,uint32_t b){const auto&x=s.titles[a];const auto&y=s.titles[b];const bool ux=x.points_needed_next_rank==0xFFFFFFFFu,uy=y.points_needed_next_rank==0xFFFFFFFFu;if(ux!=uy)return !ux;const double px=ToolboxProgress(x),py=ToolboxProgress(y);if(px!=py)return px>py;return x.id<y.id;});
 std::vector<uint32_t> toolbox_position(s.titles.size());for(uint32_t pos=0;pos<sort_order.size();++pos)toolbox_position[sort_order[pos]]=pos;
 std::string j; j.reserve(24*1024); j+="{\"type\":\"gwst-title-export\",\"version\":3";
 j+=",\"character\":{\"name\":\""; JsonEscapeInto(j,s.character_name_utf8); j+="\",\"primaryProfessionId\":"+std::to_string(s.primary_profession_id)+",\"secondaryProfessionId\":"+std::to_string(s.secondary_profession_id)+",\"level\":"+std::to_string(s.level)+",\"mapId\":"+std::to_string(s.map_id)+"}";
 j+=",\"titles\":["; bool ft=true; uint32_t ti=0; for(const auto&t:s.titles){if(!ft)j+=',';j+="{\"id\":"+std::to_string(t.id)+",\"name\":\"";JsonEscapeInto(j,TitleName(t.id));char pbuf[32];snprintf(pbuf,sizeof(pbuf),"%.6f",ToolboxProgress(t));j+="\",\"currentPoints\":"+std::to_string(t.current_points)+",\"rank\":"+std::to_string(t.current_rank)+",\"nextRank\":"+std::to_string(t.next_rank)+",\"toolboxProgress\":"+pbuf+",\"toolboxSortOrder\":"+std::to_string(toolbox_position[ti++])+",\"currentTitleTierIndex\":"+std::to_string(t.current_title_tier_index)+",\"pointsNeededCurrentRank\":"+std::to_string(t.points_needed_current_rank)+",\"nextTitleTierIndex\":"+std::to_string(t.next_title_tier_index)+",\"pointsNeededNextRank\":"+std::to_string(t.points_needed_next_rank)+",\"maxTitleRank\":"+std::to_string(t.max_title_rank)+",\"maxTitleTierIndex\":"+std::to_string(t.max_title_tier_index)+",\"percentageBased\":"+(t.percentage_based?"true":"false")+",\"hasTiers\":"+(t.has_tiers?"true":"false")+"}";ft=false;} j+="]";
 j+=",\"heroes\":["; bool fh=true; for(const auto&h:s.heroes){if(!fh)j+=',';j+="{\"id\":"+std::to_string(h.id)+",\"name\":\"";JsonEscapeInto(j,HeroName(h.id));j+="\",\"level\":"+std::to_string(h.level)+",\"primaryProfessionId\":"+std::to_string(h.primary_profession_id)+",\"secondaryProfessionId\":"+std::to_string(h.secondary_profession_id)+"}";fh=false;} j+="]";
 j+=",\"unlockedAccountSkills\":";AppendSkillBitfield(j,detail::VectorBitfield{s.unlocked_account_skills});j+=",\"learnedCharacterSkills\":";AppendSkillBitfield(j,detail::VectorBitfield{s.learned_character_skills});j+="}";return j;
}
} // namespace account_export
