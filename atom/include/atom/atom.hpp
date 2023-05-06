#pragma once

// Standard.
#include <vector>

// EOS
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/transaction.hpp>
#include <eosio/singleton.hpp>

// Local
#include "atomicassets.hpp"
#include "protonswap.hpp"
#include "constants.hpp"
#include "tables.hpp"

using namespace eosio;
using namespace std;

namespace proton {
  CONTRACT atom : public contract {
  public:
    using contract::contract;

    atom( name receiver, name code, datastream<const char*> ds )
      : contract(receiver, code, ds),
        _resources(receiver, receiver.value) {}

    ACTION initstorage (
      name account
    );
    
    ACTION mintlasttemp (
      name creator,
      name collection_name,
      name schema_name,
      name new_asset_owner,
      atomicassets::ATTRIBUTE_MAP immutable_data,
      atomicassets::ATTRIBUTE_MAP mutable_data,
      uint64_t count
    );

    void mintasset(
        name authorized_minter,
        name collection_name,
        name schema_name,
        int32_t template_id,
        name new_asset_owner,
        atomicassets::ATTRIBUTE_MAP immutable_data,
        atomicassets::ATTRIBUTE_MAP mutable_data,
        vector <asset> tokens_to_back
    );

    void buyram  ( const name& payer,
                    const name& receiver,
                    const asset& quant );

    [[eosio::on_notify("*::transfer")]]
    void ontransfer   ( const name& from,
                        const name& to,
                        const asset& quantity,
                        const string& memo );

    // Action wrappers
    using mint_action     = action_wrapper<"mintasset"_n, &atom::mintasset>;
    using buyram_action   = action_wrapper<"buyram"_n,    &atom::buyram>;
    using transfer_action = action_wrapper<"transfer"_n,  &atom::ontransfer>;

    resources_table _resources;
  private:
  };
}