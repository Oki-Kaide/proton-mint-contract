#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>

using namespace eosio;
using namespace std;

namespace swap {
  static constexpr name PROTONSWAP_ACCOUNT = name("proton.swaps");
	static constexpr uint16_t MEMO_MAX       = 1024;
	static constexpr uint128_t FEE_PRECISION = 100;
	static constexpr uint128_t FEE_FACTOR    = FEE_PRECISION * 100;
  
	struct pool_fee
	{
		uint64_t	exchange_fee;
		uint64_t	add_liquidity_fee;
		uint64_t	remove_liquidity_fee;
	};

  struct pool_base {
    symbol lt_symbol;
    name creator;
    string memo;
    extended_asset pool1;
    extended_asset pool2;
    checksum256 hash;
    pool_fee fee;

    void set_pool1(extended_asset new_pool1)
    {
      check(!(new_pool1.quantity.amount <= 0), " pool1 must be positive value");
      pool1 = new_pool1;
    }

    void set_pool2(extended_asset new_pool2)
    {
      check(!(new_pool2.quantity.amount <= 0), " pool2 must be positive value");
      pool2 = new_pool2;
    }

    void set_memo(const string& new_memo)
    {
      check(!(new_memo.size() > MEMO_MAX), "Memo size is " + to_string(new_memo.size()) + " which is bigger then maximum allowed " + to_string(MEMO_MAX));
      memo = new_memo;
    }
  };

  struct pool : public pool_base{
    bool active = true;
    uint16_t  reserved;

    uint64_t primary_key()const { return lt_symbol.code().raw(); }

    checksum256 by_hash() const {
      return hash;
    }

    EOSLIB_SERIALIZE(pool, (lt_symbol)(creator)(memo)(pool1)(pool2)(hash)(fee)(active)(reserved))
  };
  typedef multi_index <name("pools"), pool> pools_t;

  pools_t pools = pools_t(PROTONSWAP_ACCOUNT, PROTONSWAP_ACCOUNT.value);

  // Func
  uint64_t compute_transfer(uint128_t token1, uint128_t token2, uint128_t quantity, uint128_t exchange_fee)
  {
    const uint128_t coefficient = (token1 * token2) / (token1 + quantity);
    uint128_t transfer = token2 - coefficient;
    transfer -= (transfer * exchange_fee) / FEE_FACTOR;
    return static_cast<uint64_t>(transfer);
  }
};