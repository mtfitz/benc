#include <optional>
#include <variant>
#include <iostream>
#include <string>
#include <vector>

#include "benc/benc.hpp"

using namespace benc;

using byte = benc::byte;

std::optional<BencTree> benc::decode(std::vector<byte>& src);
std::optional<BencTree> benc::decode(std::string& src);

std::optional<BencInt> parse_integer(std::vector<byte>& in)
{
    // check if minimum size of integer representation (iXe)
    if (in.size() < 3)
        return {};

    std::vector<byte> working_bytes;
    if (in[0] != 'i')
        return {};
    working_bytes.push_back(in[0]);

    for (auto i = in.begin() + 1; i != in.end(); i++) {
        byte this_byte = *i;
        working_bytes.push_back(this_byte);
        // break if not valid int (or bounds)
        if ( (this_byte < 0x30) || (this_byte > 0x39) && this_byte != '-')
            break;
    }

    if (working_bytes.back() != 'e')
        return {};

    int64_t this_int = 0;
    char sign = false;
    if (working_bytes[1] == '-') {
        sign = true;
    }

    int64_t ten_pow = 1;
    bool got_sign = false;
    for (auto i = working_bytes.rbegin() + 1; i != working_bytes.rend() - 1; i++) {
        // if we already read '-', the loop should have ended already
        if (got_sign)
            return {};
        
        byte this_digit = *i - 0x30;
        
        // if not a valid digit, check if '-'
        if (this_digit > 0x9) {
            if (!got_sign) {
                got_sign = true;
                continue;
            } else {
                return {};
            }
        }

        // push digit to integer
        this_int += this_digit * ten_pow;
        ten_pow *= 10;
    }

    // apply sign change, if necessary
    if (sign)
        this_int = -this_int;

    in.erase(in.begin(), in.begin() + working_bytes.size());

    return this_int;
}

std::optional<BencString> parse_byte_string(std::vector<byte>& in)
{
    // ensure minimum string size (a:b)
    if (in.size() < 3) {
        return {};
    }

    // get string size from first bytes
    std::vector<byte> count_working_bytes;
    for (auto i = in.begin(); i != in.end(); i++) {
        if (*i < '0' || *i > '9') { // if not a digit
            if (*i == ':') // stop at ':'
                break;
            else // error, out of bounds
                return {};
        }

        count_working_bytes.push_back(*i);
    }

    // ensure the count actually exists
    if (count_working_bytes.size() == 0)
        return {};

    // convert count bytes to integer
    size_t count = 0;
    size_t ten_pow = 1;
    for (auto i = count_working_bytes.rbegin(); i != count_working_bytes.rend(); i++) {
        byte this_digit = *i - 0x30;
        count += this_digit * ten_pow;
        ten_pow *= 10;
    }

    // ensure we can safely proceed, and next byte is ':'
    if (in.size() < count_working_bytes.size() + 2)
        return {};
    if (in[count_working_bytes.size()] != ':')
        return {};
    
    // get byte string
    std::vector<byte> byte_string;
    byte_string.reserve(count);
    size_t bytes_remaining = count;
    for (auto i = in.begin() + count_working_bytes.size() + 1; i != in.end(); i++) {
        if (bytes_remaining == 0)
            break;
        byte_string.push_back(*i);
        bytes_remaining--;
    }

    // ensure given count was correct (i.e. no EOF)
    if (byte_string.size() != count)
        return {};

    in.erase(in.begin(), in.begin() + count_working_bytes.size() + 1 + count);

    return byte_string;
}

std::optional<BencList> parse_list(std::vector<byte>& in)
{
    // ensure minimum list size (lXe)
    if (in.size() < 3) {
        return {};
    }

    // check if list
    if (in[0] != 'l')
        return {};

    // remove 'l'
    in.erase(in.begin(), in.begin() + 1);
    
    // parse until list end
    std::vector<BencTree> list_nodes;
    while (true) {
        // first check if list is done
        if (in.size() > 0 && in[0] == 'e') {
            // remove 'e'
            in.erase(in.begin(), in.begin() + 1);
            break;
        }

        auto node_wrapped = benc::decode(in);
        if (node_wrapped.has_value()) {
            list_nodes.push_back(node_wrapped.value());
        } else {
            return {};
        }
    }

    return list_nodes;
}

std::optional<BencDict> parse_dict(std::vector<byte>& in)
{
    // ensure minimum dictionary size (dXe)
    if (in.size() < 3) {
        return {};
    }

    // check if dictionary
    if (in[0] != 'd')
        return {};

    // remove 'd'
    in.erase(in.begin(), in.begin() + 1);

    std::map<BencString, BencTree> dict;
    while (true) {
        // first check if dictionary is done
        if (in.size() > 0 && in[0] == 'e') {
            // remove 'e'
            in.erase(in.begin(), in.begin() + 1);
            break;
        }

        auto string_wrapped = parse_byte_string(in);
        auto node_wrapped = benc::decode(in);
        if (string_wrapped.has_value() && node_wrapped.has_value()) {
            dict.insert({string_wrapped.value(), node_wrapped.value()});
        } else {
            return {};
        }
    }

    return dict;
}

std::optional<BencTree> benc::decode(std::vector<byte>& src)
{
    // try to parse integer
    auto parsed_integer = parse_integer(src);
    if (parsed_integer) {
        //bt = parsed_integer.value();
        return BencTree(parsed_integer.value());
    }
    
    // try to parse byte string
    auto parsed_byte_string = parse_byte_string(src);
    if (parsed_byte_string) {
        return BencTree(parsed_byte_string.value());
    }

    // try to parse list
    auto parsed_list = parse_list(src);
    if (parsed_list) {
        return BencTree(parsed_list.value());
    }

    // try to parse dictionary
    auto parsed_dict = parse_dict(src);
    if (parsed_dict) {
        return BencTree(parsed_dict.value());
    }

    // default: nothing to parse
    return {};
}

std::optional<BencTree> benc::decode(std::string& src)
{
    std::vector<byte> src_vec(src.begin(), src.end());
    std::optional<BencTree> result(benc::decode(src_vec));
    src = std::string(src_vec.begin(), src_vec.end());
    return result;
}