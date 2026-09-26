#pragma once
#include <cstdint>
#include <vector>
#include <cstdio>
#include <iterator>
#include <string>
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
inline void JsonEscapeInto(std::string& out, const std::string& value) { for (const char c : value) { switch(c) { case '"': out += "\\\""; break; case '\\': out += "\\\\"; break; case '\n': out += "\\n"; break; case '\r': out += "\\r"; break; case '\t': out += "\\t"; break; default: if (static_cast<unsigned char>(c) < 0x20) { char b[8]; snprintf(b,sizeof(b),"\\u%04x",c); out += b; } else out += c; } } }
template <typename A> void AppendSkillBitfield(std::string& out,const A& a) { out+='['; bool first=true; if(a.valid()) for(uint32_t wi=0;wi<a.size();wi++){uint32_t w=a[wi];for(uint32_t bit=0;bit<32;bit++)if(w&(1u<<bit)){if(!first)out+=',';out+=std::to_string(wi*32+bit);first=false;}} out+=']'; }
struct HeroSnapshot { uint32_t id,level,primary_profession_id,secondary_profession_id; };
struct TitleSnapshot { uint32_t id,current_points,current_title_tier_index,points_needed_current_rank,next_title_tier_index,points_needed_next_rank,max_title_rank,max_title_tier_index; bool percentage_based,has_tiers; };
struct AccountSnapshot { std::string character_name_utf8; uint32_t primary_profession_id,secondary_profession_id,level,map_id; std::vector<HeroSnapshot> heroes; std::vector<TitleSnapshot> titles; std::vector<uint32_t> unlocked_account_skills,learned_character_skills; };
namespace detail { struct VectorBitfield { const std::vector<uint32_t>& words; bool valid()const{return true;} uint32_t size()const{return static_cast<uint32_t>(words.size());} uint32_t operator[](uint32_t i)const{return words[i];} }; }
inline std::string BuildAccountJson(const AccountSnapshot& s) {
 std::string j; j.reserve(24*1024); j+="{\"type\":\"gwst-title-export\",\"version\":2";
 j+=",\"character\":{\"name\":\""; JsonEscapeInto(j,s.character_name_utf8); j+="\",\"primaryProfessionId\":"+std::to_string(s.primary_profession_id)+",\"secondaryProfessionId\":"+std::to_string(s.secondary_profession_id)+",\"level\":"+std::to_string(s.level)+",\"mapId\":"+std::to_string(s.map_id)+"}";
 j+=",\"titles\":["; bool ft=true; for(const auto&t:s.titles){if(!ft)j+=',';j+="{\"id\":"+std::to_string(t.id)+",\"name\":\"";JsonEscapeInto(j,TitleName(t.id));j+="\",\"currentPoints\":"+std::to_string(t.current_points)+",\"currentTitleTierIndex\":"+std::to_string(t.current_title_tier_index)+",\"pointsNeededCurrentRank\":"+std::to_string(t.points_needed_current_rank)+",\"nextTitleTierIndex\":"+std::to_string(t.next_title_tier_index)+",\"pointsNeededNextRank\":"+std::to_string(t.points_needed_next_rank)+",\"maxTitleRank\":"+std::to_string(t.max_title_rank)+",\"maxTitleTierIndex\":"+std::to_string(t.max_title_tier_index)+",\"percentageBased\":"+(t.percentage_based?"true":"false")+",\"hasTiers\":"+(t.has_tiers?"true":"false")+"}";ft=false;} j+="]";
 j+=",\"heroes\":["; bool fh=true; for(const auto&h:s.heroes){if(!fh)j+=',';j+="{\"id\":"+std::to_string(h.id)+",\"name\":\"";JsonEscapeInto(j,HeroName(h.id));j+="\",\"level\":"+std::to_string(h.level)+",\"primaryProfessionId\":"+std::to_string(h.primary_profession_id)+",\"secondaryProfessionId\":"+std::to_string(h.secondary_profession_id)+"}";fh=false;} j+="]";
 j+=",\"unlockedAccountSkills\":";AppendSkillBitfield(j,detail::VectorBitfield{s.unlocked_account_skills});j+=",\"learnedCharacterSkills\":";AppendSkillBitfield(j,detail::VectorBitfield{s.learned_character_skills});j+="}";return j;
}
} // namespace account_export
