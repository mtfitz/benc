#include <optional>
#include <variant>
#include <iostream>
#include <string>
#include <vector>

#include "benc.hpp"

using namespace benc;

using byte = benc::byte;

std::optional<std::vector<byte>> benc::encode(BencTree& tree);
std::optional<std::string> benc::encode_string(BencTree& tree);

std::optional<std::vector<byte>> encode_integer(BencTree& tree)
{
    if (tree.type() != BencType::INT) {
        return {};
    }

    std::string result_str = "i" + std::to_string(tree.get_int()) + "e";
    std::vector<byte> result(result_str.begin(), result_str.end());

    return result;
}

std::optional<std::vector<byte>> encode_byte_string(BencTree& tree)
{
    if (tree.type() != BencType::STRING) {
        return {};
    }

    BencString& str_vec = tree.get_byte_string();
    std::string prefix = std::to_string(str_vec.size()) + ":";
    std::vector<byte> result(prefix.begin(), prefix.end());
    result.insert(result.end(), str_vec.begin(), str_vec.end());

    return result;
}

std::optional<std::vector<byte>> encode_list(BencTree& tree)
{
    if (tree.type() != BencType::LIST) {
        return {};
    }

    std::vector<byte> result;
    result.push_back('l');
    BencList& this_list = tree.get_list();
    for (auto& x : this_list) {
        auto x_enc = encode(x);
        if (!x_enc) {
            return {};
        }
        auto x_enc_unwrapped = x_enc.value();
        result.insert(result.end(), x_enc_unwrapped.begin(), x_enc_unwrapped.end());
    }
    result.push_back('e');

    return result;
}

std::optional<std::vector<byte>> encode_dict(BencTree& tree)
{
    if (tree.type() != BencType::DICT) {
        return {};
    }

    std::vector<byte> result;
    result.push_back('d');
    BencDict& this_dict = tree.get_dict();
    for (auto x : this_dict) {
        BencTree x1 = BencTree(x.first);
        BencTree x2 = x.second;

        auto x1_enc = encode(x1);
        if (!x1_enc) {
            return {};
        }

        auto x2_enc = encode(x2);
        if (!x2_enc) {
            return {};
        }

        auto x1_enc_unwrapped = x1_enc.value();
        auto x2_enc_unwrapped = x2_enc.value();
        result.insert(result.end(), x1_enc_unwrapped.begin(), x1_enc_unwrapped.end());
        result.insert(result.end(), x2_enc_unwrapped.begin(), x2_enc_unwrapped.end());
    }
    result.push_back('e');

    return result;
}

std::optional<std::vector<byte>> benc::encode(BencTree& tree)
{
    auto encoded_result = encode_integer(tree);
    if (encoded_result) {
        return encoded_result;
    }
    encoded_result = encode_byte_string(tree);
    if (encoded_result) {
        return encoded_result;
    }
    encoded_result = encode_list(tree);
    if (encoded_result) {
        return encoded_result;
    }
    encoded_result = encode_dict(tree);
    if (encoded_result) {
        return encoded_result;
    }

    return {};
}

std::optional<std::string> benc::encode_string(BencTree& tree)
{
    auto result = encode(tree);
    if (!result) {
        return {};
    }
    auto result_unwrapped = result.value();
    return std::string(result_unwrapped.begin(), result_unwrapped.end());
}