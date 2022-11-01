#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <variant>
#include <vector>
#include <map>

namespace benc {
    using byte = uint8_t;

    class BencTree;
    using BencInt = int64_t;
    using BencString = std::vector<byte>;
    using BencList = std::vector<BencTree>;
    using BencDict = std::map<BencString, BencTree>;

    ///
    /// An algebraic type for Bencode data types.
    ///
    enum BencType {
        INT,
        STRING,
        LIST,
        DICT,
        NONE
    };

    ///
    /// A parse tree for representing decoded Bencode structures.
    ///
    class BencTree {
    private:
        BencType m_type;
        std::optional<
            std::variant<BencInt,       // integer 
                        BencString,    // byte string
                        BencList,      // list
                        BencDict>>     // dictionary
            m_data;

    public:
        ///
        /// Constructs an empty tree.
        ///
        BencTree() : m_type(NONE), m_data({}) {}
        ///
        /// Constructs a tree storing an integer.
        ///
        /// @param i decoded integer
        ///
        BencTree(BencInt i) : m_type(INT), m_data(i) {}
        ///
        /// Constructs a tree storing an byte string.
        ///
        /// @param s decoded byte string
        ///
        BencTree(BencString s) : m_type(STRING), m_data(s) {}
        ///
        /// Constructs a tree storing a list of other nodes.
        ///
        /// @param v decoded list
        ///
        BencTree(BencList v) : m_type(LIST), m_data(v) {}
        ///
        /// Constructs a tree storing a dictionary mapping strings to other nodes.
        ///
        /// @param d decoded dictionary
        ///
        BencTree(BencDict d) : m_type(DICT), m_data(d) {}
        /*///
        /// Constructs a tree by copying contents.
        ///
        /// @param tree the tree to be copied
        ///
        BencTree(const BencTree& tree) : m_type(tree.m_type), m_data(tree.m_data) {}*/

        ///
        /// Gets the type of the tree's stored data.
        ///
        /// @return type of content (integer, byte string, list or dictionary)
        ///
        BencType type();

        ///
        /// Gets the tree's stored integer (throws if does not exist).
        ///
        /// @return stored integer
        ///
        BencInt& get_int();
        ///
        /// Gets the tree's stored byte string (throws if does not exist).
        ///
        /// @return stored byte string
        ///
        BencString& get_byte_string();
        ///
        /// Gets the tree's stored list (throws if does not exist).
        ///
        /// @return stored list
        ///
        BencList& get_list();
        ///
        /// Gets the tree's stored dictionary (throws if does not exist).
        ///
        /// @return stored dictionary
        ///
        BencDict& get_dict();

        ///
        /// Converts the tree to a string with JSON-like syntax (warning: not sanitized).
        ///
        /// @return string representation of tree
        ///
        std::string to_string(size_t level = 0, bool local_override = false);
    };

    ///
    /// Decodes Bencode data into a tree.
    ///
    /// @param src input Bencode source bytes
    ///
    /// @return decoded tree (on success)
    ///
    std::optional<BencTree> decode(std::vector<byte>& src);
    ///
    /// Decodes Bencode data into a tree.
    ///
    /// @param src input Bencode source string
    ///
    /// @return decoded tree (on success)
    ///
    std::optional<BencTree> decode(std::string& src);
    
    ///
    /// Encodes a tree into bytes of Bencode source.
    ///
    /// @param tree input tree
    ///
    /// @return a vector of bytes containing Bencode data (on success)
    ///
    std::optional<std::vector<byte>> encode(BencTree& tree);
    ///
    /// Encodes a tree into a Bencode source string (warning: may contain invalid Unicode).
    ///
    /// @param tree input tree
    ///
    /// @return a string containing Bencode data (on success)
    ///
    std::optional<std::string> encode_string(BencTree& tree);
}