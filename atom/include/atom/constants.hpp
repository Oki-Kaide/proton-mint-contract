#pragma once

using namespace eosio;
using namespace std;

#define SYSTEM_CONTRACT name("eosio")
#define XTOKENS_CONTRACT name("xtokens")
#define SYSTEM_TOKEN_CONTRACT name("eosio.token")
#define FEE_ACCOUNT name("a.resources")

#define XUSDC_SYMBOL symbol("XUSDC", 6)
#define XPRXUSDC_SYMBOL symbol("XPRUSDC", 8)
#define XPR_SYMBOL symbol("XPR", 4)

namespace proton
{
  static constexpr auto SECONDS_IN_MINUTE = 60;
  static constexpr auto SECONDS_IN_HOUR   = SECONDS_IN_MINUTE * 60;
  static constexpr auto SECONDS_IN_DAY    = SECONDS_IN_HOUR * 24;
  static constexpr auto HOURS_IN_DAY      = 24;
  static constexpr auto MAX_TERM_MONTHS   = 3;
  static constexpr auto MAX_PROCESS       = 5;
  static constexpr auto ASSET_RAM_COST_BYTES = 151;
  static constexpr auto FREE_NFTS = 10;
  static constexpr auto FREE_RAM = ASSET_RAM_COST_BYTES * FREE_NFTS;
} // namespace proton