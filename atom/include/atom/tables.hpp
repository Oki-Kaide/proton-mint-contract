#pragma once

using namespace eosio;
using namespace std;
namespace proton {
  struct [[eosio::table, eosio::contract("atom")]] Resource {
    name account;
    uint64_t ram_bytes;

    uint64_t primary_key() const { return account.value; };
  };
  typedef multi_index<"resources"_n, Resource> resources_table;

  // PROTON RAM EXTERNAL EOSIO
  struct [[eosio::table("globalram"), eosio::contract("eosio.system")]] eosio_global_stateram {
      eosio_global_stateram() { }
      asset     ram_price_per_byte     = { 200, symbol{"XPR", 4} };   // 0.02 XPR per byte
      uint64_t  max_per_user_bytes     = 3 * 1024 * 1024;             // 3 MB per user
      uint16_t  ram_fee_percent        = 10'00;                       // ram fee percent . 10'00 is 10 %
      uint64_t  total_ram              = 0;                           // total ram purchased with XPR
      uint64_t  total_xpr              = 0;                           // total ram purchased with XPR


      EOSLIB_SERIALIZE(eosio_global_stateram, (ram_price_per_byte)(max_per_user_bytes)(ram_fee_percent)(total_ram)(total_xpr))
  };
  typedef eosio::singleton< "globalram"_n, eosio_global_stateram > global_stateram_singleton;
}