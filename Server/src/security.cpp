#include "security.h"

#include <boost/uuid/detail/md5.hpp>
#include <boost/algorithm/hex.hpp>
#include <algorithm>
#include <iterator>

using boost::uuids::detail::md5;
#define BOOST_UUID_COMPAT_PRE_1_71_MD5 ;


std::string toString(const md5::digest_type &digest)
{
    const auto charDigest = reinterpret_cast<const char *>(&digest);
    std::string result;
    boost::algorithm::hex(charDigest, charDigest + sizeof(md5::digest_type), std::back_inserter(result));
    return result;
}

std::string getHash(const std::string& source)
{
    using boost::uuids::detail::md5;

    md5 hash;
    md5::digest_type digest;

    hash.process_bytes(source.data(), source.size());
    hash.get_digest(digest);

    return toString(digest);
}

