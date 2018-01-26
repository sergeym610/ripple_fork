//------------------------------------------------------------------------------
/*
    This file is part of cbcd: https://github.com/cbc/cbcd
    Copyright (c) 2012, 2013 cbc Labs Inc.

    Permission to use, copy, modify, and/or distribute this software for any
    purpose  with  or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
    MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
//==============================================================================

#include <BeastConfig.h>
#include <cbc/app/paths/cbcLineCache.h>
#include <cbc/ledger/OpenView.h>

namespace cbc {

cbcLineCache::cbcLineCache(
    std::shared_ptr <ReadView const> const& ledger)
{
    // We want the caching that OpenView provides
    // And we need to own a shared_ptr to the input view
    // VFALCO TODO This should be a CachedLedger
    mLedger = std::make_shared<OpenView>(&*ledger, ledger);
}

std::vector<cbcState::pointer> const&
cbcLineCache::getcbcLines (AccountID const& accountID)
{
    AccountKey key (accountID, hasher_ (accountID));

    std::lock_guard <std::mutex> sl (mLock);

    auto it = lines_.emplace (key,
        std::vector<cbcState::pointer>());

    if (it.second)
        it.first->second = getcbcStateItems (
            accountID, *mLedger);

    return it.first->second;
}

} // cbc
