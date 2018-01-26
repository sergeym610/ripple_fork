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
#include <cbc/app/main/Application.h>
#include <cbc/app/paths/cbcState.h>
#include <cbc/protocol/STAmount.h>
#include <cstdint>
#include <memory>

namespace cbc {

cbcState::pointer
cbcState::makeItem (
    AccountID const& accountID,
        std::shared_ptr<SLE const> sle)
{
    // VFALCO Does this ever happen in practice?
    if (! sle || sle->getType () != ltcbc_STATE)
        return {};
    return std::make_shared<cbcState>(
        std::move(sle), accountID);
}

cbcState::cbcState (
    std::shared_ptr<SLE const>&& sle,
        AccountID const& viewAccount)
    : sle_ (std::move(sle))
    , mFlags (sle_->getFieldU32 (sfFlags))
    , mLowLimit (sle_->getFieldAmount (sfLowLimit))
    , mHighLimit (sle_->getFieldAmount (sfHighLimit))
    , mLowID (mLowLimit.getIssuer ())
    , mHighID (mHighLimit.getIssuer ())
    , lowQualityIn_ (sle_->getFieldU32 (sfLowQualityIn))
    , lowQualityOut_ (sle_->getFieldU32 (sfLowQualityOut))
    , highQualityIn_ (sle_->getFieldU32 (sfHighQualityIn))
    , highQualityOut_ (sle_->getFieldU32 (sfHighQualityOut))
    , mBalance (sle_->getFieldAmount (sfBalance))
{
    mViewLowest = (mLowID == viewAccount);

    if (!mViewLowest)
        mBalance.negate ();
}

Json::Value cbcState::getJson (int)
{
    Json::Value ret (Json::objectValue);
    ret["low_id"] = to_string (mLowID);
    ret["high_id"] = to_string (mHighID);
    return ret;
}

std::vector <cbcState::pointer>
getcbcStateItems (AccountID const& accountID,
    ReadView const& view)
{
    std::vector <cbcState::pointer> items;
    forEachItem(view, accountID,
        [&items,&accountID](
        std::shared_ptr<SLE const> const&sleCur)
        {
             auto ret = cbcState::makeItem (accountID, sleCur);
             if (ret)
                items.push_back (ret);
        });

    return items;
}

} // cbc
