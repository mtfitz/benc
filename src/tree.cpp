#include "benc.hpp"

using namespace benc;
using namespace std;

BencType BencTree::type() {
    return m_type;
}

BencInt& BencTree::get_int() {
    return get<BencInt>(m_data.value());
}

BencString& BencTree::get_byte_string() {
    return get<BencString>(m_data.value());
}

BencList& BencTree::get_list() {
    return get<BencList>(m_data.value());
}

BencDict& BencTree::get_dict() {
    return get<BencDict>(m_data.value());
}

std::string BencTree::to_string(size_t level, bool local_override) {
    string indent('\t', level);

    switch (m_type) {
    case BencType::INT:
    {
        return (local_override ? "" : indent) + std::to_string(get<BencInt>(m_data.value()));
    }
    case BencType::STRING:
    {
        auto& byte_vec = get<BencString>(m_data.value());
        return (local_override ? "" : indent) + '"' + string(byte_vec.begin(), byte_vec.end()) + '"';
    }
    case BencType::LIST:
    {
        auto& this_list = get<BencList>(m_data.value());
        string items = "[\n";
        bool after_first = false;
        for(auto& s : this_list) {
            if (after_first)
                items += ",\n";

            items += indent + s.to_string(level + 1);

            after_first = true;
        }
        items += "\n]";
        return items;
    }
    case BencType::DICT:
    {
        auto& this_dict = get<BencDict>(m_data.value());
        string items = "{\n";
        bool after_first = false;
        for(auto& s : this_dict) {
            if (after_first)
                items += ",\n";

            auto& key_byte_string = s.first;
            string key_string = string(key_byte_string.begin(), key_byte_string.end());
            items += indent + '"' + key_string + '"' + ": " + s.second.to_string(level + 1, true);

            after_first = true;
        }
        items += indent + "\n}";
        return items;
    }
    default:
        return "";
    }
}