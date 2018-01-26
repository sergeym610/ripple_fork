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

#ifndef cbc_TX_CHANGE_H_INCLUDED
#define cbc_TX_CHANGE_H_INCLUDED

#include <cbc/app/main/Application.h>
#include <cbc/app/misc/AmendmentTable.h>
#include <cbc/app/misc/NetworkOPs.h>
#include <cbc/app/tx/impl/Transactor.h>
#include <cbc/basics/Log.h>
#include <cbc/protocol/Indexes.h>

namespace cbc {

class Change
    : public Transactor
{
public:
    Change (ApplyContext& ctx)
        : Transactor(ctx)
    {
    }

    static
    TER
    preflight (PreflightContext const& ctx);

    TER doApply () override;
    void preCompute() override;

    static
    std::uint64_t
    calculateBaseFee (
        PreclaimContext const& ctx)
    {
        return 0;
    }

    static
    TER
    preclaim(PreclaimContext const &ctx);

private:
    TER applyAmendment ();

    TER applyFee ();
};

}

#endif
