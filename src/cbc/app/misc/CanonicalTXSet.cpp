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
#include <cbc/app/misc/CanonicalTXSet.h>
#include <boost/range/adaptor/transformed.hpp>

namespace cbc {

bool CanonicalTXSet::Key::operator< (Key const& rhs) const
{
    if (mAccount < rhs.mAccount) return true;

    if (mAccount > rhs.mAccount) return false;

    if (mSeq < rhs.mSeq) return true;

    if (mSeq > rhs.mSeq) return false;

    return mTXid < rhs.mTXid;
}

bool CanonicalTXSet::Key::operator> (Key const& rhs) const
{
    if (mAccount > rhs.mAccount) return true;

    if (mAccount < rhs.mAccount) return false;

    if (mSeq > rhs.mSeq) return true;

    if (mSeq < rhs.mSeq) return false;

    return mTXid > rhs.mTXid;
}

bool CanonicalTXSet::Key::operator<= (Key const& rhs) const
{
    if (mAccount < rhs.mAccount) return true;

    if (mAccount > rhs.mAccount) return false;

    if (mSeq < rhs.mSeq) return true;

    if (mSeq > rhs.mSeq) return false;

    return mTXid <= rhs.mTXid;
}

bool CanonicalTXSet::Key::operator>= (Key const& rhs)const
{
    if (mAccount > rhs.mAccount) return true;

    if (mAccount < rhs.mAccount) return false;

    if (mSeq > rhs.mSeq) return true;

    if (mSeq < rhs.mSeq) return false;

    return mTXid >= rhs.mTXid;
}

uint256 CanonicalTXSet::accountKey (AccountID const& account)
{
    uint256 ret = beast::zero;
    memcpy (
        ret.begin (),
        account.begin (),
        account.size ());
    ret ^= mSetHash;
    return ret;
}

void CanonicalTXSet::insert (std::shared_ptr<STTx const> const& txn)
{
    mMap.insert (
        std::make_pair (
            Key (
                accountKey (txn->getAccountID(sfAccount)),
                txn->getSequence (),
                txn->getTransactionID ()),
            txn));
}

std::vector<std::shared_ptr<STTx const>>
CanonicalTXSet::prune(AccountID const& account,
    std::uint32_t const seq)
{
    auto effectiveAccount = accountKey (account);

    Key keyLow(effectiveAccount, seq, zero);
    Key keyHigh(effectiveAccount, seq+1, zero);

    auto range = boost::make_iterator_range(
        mMap.lower_bound(keyLow),
            mMap.lower_bound(keyHigh));
    auto txRange = boost::adaptors::transform(
        range,
    [](auto const& p)
    {
        return p.second;
    });

    std::vector<std::shared_ptr<STTx const>> result(
        txRange.begin(), txRange.end());

    mMap.erase(range.begin(), range.end());

    return result;
}

CanonicalTXSet::iterator CanonicalTXSet::erase (iterator const& it)
{
    iterator tmp = it;
    ++tmp;
    mMap.erase (it);
    return tmp;
}

} // cbc
