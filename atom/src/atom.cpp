#include <atom/atom.hpp>

namespace proton
{
  void atom::ontransfer (const name& from, const name& to, const asset& quantity, const string& memo) {
    // Skip if outgoing
    if (from == get_self()) {
      return;
    }

    // Skip if deposit from system accounts
    if (from == "eosio.stake"_n || from == "eosio.ram"_n || from == "eosio"_n) {
      return;
    }

    // Skip if not xtoken and USDC
    name token_contract = get_first_receiver();
    if (!(token_contract == XTOKENS_CONTRACT && quantity.symbol == XUSDC_SYMBOL)) {
      return;
    }

    // Validate transfer
    check(to == get_self(), "Invalid Deposit");

    // Find pool
    auto pool = swap::pools.find(XPRXUSDC_SYMBOL.code().raw());
    check(pool != swap::pools.end(), "pool not found");

    // Find price
    uint64_t protocol_fee = 10;
    uint64_t fee = pool->fee.exchange_fee + protocol_fee;
    const uint64_t swap_out_amount = swap::compute_transfer(pool->pool2.quantity.amount, pool->pool1.quantity.amount, quantity.amount, fee);
    auto swap_out = asset(swap_out_amount, pool->pool1.quantity.symbol);

    // Swap XUSDC -> XPR
    transfer_action t_action( XTOKENS_CONTRACT, {get_self(), "active"_n} );
    t_action.send(get_self(), swap::PROTONSWAP_ACCOUNT, quantity, XPRXUSDC_SYMBOL.code().to_string() + "," + to_string(swap_out.amount * 99 / 100)); // 1% slippage max

    // Calculate RAM bytes
    global_stateram_singleton globalram("eosio"_n, "eosio"_n.value);
    eosio_global_stateram _gstateram = globalram.exists() ? globalram.get() : eosio_global_stateram{};
    
    asset ram_cost = _gstateram.ram_price_per_byte;
    uint64_t ram_bytes = swap_out.amount / ram_cost.amount;

    // Determine RAM receiver
    check(memo == "account" || memo == "contract", "memo must be 'account' or 'contract'");
    name ram_receiver = memo == "account"
      ? from
      : get_self();

    // Buy the RAM
    buyram_action br_action( SYSTEM_CONTRACT, {get_self(), "active"_n} );
    br_action.send(get_self(), ram_receiver, swap_out);

    // Add bytes if contract balance
    if (memo == "contract") {
      auto resource = _resources.find(from.value);
      check(resource != _resources.end(), "resource does not exist, use initstorage first");
      _resources.modify(resource, same_payer, [&](auto& r) {
          r.ram_bytes += ram_bytes;
      });
    }
  }

  void atom::initstorage (
    name account
  ) {
    require_auth(account);

    auto resource = _resources.find(account.value);

    if (resource == _resources.end()) {
      _resources.emplace(get_self(), [&](auto& r) {
        r.account = account;
        r.ram_bytes = FREE_RAM;
      });
    }
  }

  void atom::mintlasttemp (
    name creator,
    name collection_name,
    name schema_name,
    name new_asset_owner,
    atomicassets::ATTRIBUTE_MAP immutable_data,
    atomicassets::ATTRIBUTE_MAP mutable_data,
    uint64_t count
  ) {
    // Verify enough RAM
    require_auth(creator);

    auto resource = _resources.find(creator.value);
    check(resource != _resources.end(), "resource balance not found");
    uint64_t ram_cost = count * ASSET_RAM_COST_BYTES;
    check(resource->ram_bytes >= ram_cost, "Need more blockchain storage. Current: " + to_string(resource->ram_bytes) + " bytes, Required: " + to_string(ram_cost) + " bytes");

    _resources.modify(resource, same_payer, [&](auto& r) {
      r.ram_bytes -= ram_cost;
    });

    if (resource->ram_bytes == 0) {
      _resources.erase(resource);
    }

    // Further validation
    auto collection = atomicassets::collections.find(collection_name.value);
    check(collection != atomicassets::collections.end(), "no collection found with name");

    auto creator_authorized = find(collection->authorized_accounts.begin(), collection->authorized_accounts.end(), creator);
    check(creator_authorized != collection->authorized_accounts.end(), "creator is not authorized");

    auto self_authorized = find(collection->authorized_accounts.begin(), collection->authorized_accounts.end(), get_self());
    check(self_authorized != collection->authorized_accounts.end(), "contract is not authorized");

    atomicassets::templates_t templates = atomicassets::get_templates(collection_name);
    check(templates.begin() != templates.end(), "no templates found for collection");

    auto relevant_template = templates.end();
    relevant_template--;

    for (int i = 0; i < count; ++i) {
      atom::mint_action m_action( atomicassets::ATOMICASSETS_ACCOUNT, {get_self(), "active"_n} );
      m_action.send(
        get_self(),
        collection_name,
        schema_name,
        relevant_template->template_id,
        new_asset_owner,
        immutable_data,
        mutable_data,
        vector<asset>{} // token to back
      );
    }
  }
} // namepsace contract