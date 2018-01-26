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

#ifndef cbc_SERVER_ROLE_H_INCLUDED
#define cbc_SERVER_ROLE_H_INCLUDED

#include <cbc/server/Port.h>
#include <cbc/json/json_value.h>
#include <cbc/resource/ResourceManager.h>
#include <cbc/beast/net/IPEndpoint.h>
#include <vector>

namespace cbc {

/** Indicates the level of administrative permission to grant.
 * IDENTIFIED role has unlimited resources but cannot perform some
 *            RPC commands.
 * ADMIN role has unlimited resources and is able to perform all RPC
 *            commands.
 */
enum class Role
{
    GUEST,
    USER,
    IDENTIFIED,
    ADMIN,
    FORBID
};

/** Return the allowed privilege role.
    jsonRPC must meet the requirements of the JSON-RPC
    specification. It must be of type Object, containing the key params
    which is an array with at least one object. Inside this object
    are the optional keys 'admin_user' and 'admin_password' used to
    validate the credentials.
*/
Role
requestRole (Role const& required, Port const& port,
    Json::Value const& jsonRPC, beast::IP::Endpoint const& remoteIp,
    std::string const& user);

Resource::Consumer
requestInboundEndpoint (Resource::Manager& manager,
    beast::IP::Endpoint const& remoteAddress,
        Port const& port, std::string const& user);

/**
 * Check if the role entitles the user to unlimited resources.
 */
bool
isUnlimited (Role const& role);

/**
 * If the HTTP header X-User exists with a non-empty value was passed by an IP
 * configured as secure_gateway, then the user can be positively identified.
 */
bool
isIdentified (Port const& port, beast::IP::Address const& remoteIp,
        std::string const& user);

} // cbc

#endif
