// KT header-only library
// Requirements: C++17

#pragma once
#include <cassert>
#include <string>
#include <vector>

namespace kt {
///
/// \brief Root node for a trie of T per node
///
template <typename T = char>
struct string_trie {
	using type = T;
	using string_t = std::basic_string<type>;
	using string_view_t = std::basic_string_view<type>;

	///
	/// \brief Forward declaration of node
	///
	struct node_t;

	///
	/// \brief Array of child nodes
	///
	std::vector<node_t> nodes;

	///
	/// \brief Add nodes corresponding to a new word, starting at index
	///
	node_t& add(string_view_t word, std::size_t index = 0);
	///
	/// \brief Find a node corresponding to a substring, starting at index
	///
	node_t* find(string_view_t str, std::size_t index = 0);
	///
	/// \brief Find a node corresponding to a substring, starting at index
	///
	node_t const* find(string_view_t str, std::size_t index = 0) const;

	///
	/// \brief Find a node corresponding to substring str and add all words to container out
	///
	template <typename C>
	void words(C& out, string_view_t str) const;
	///
	/// \brief Add all words in child nodes to container out
	///
	template <typename C>
	void add_word_nodes(C& out) const;

  private:
	///
	/// \brief const helper for find
	///
	template <typename Ret, typename Self>
	static Ret* find(Self& self, std::string_view str, std::size_t index);
};

///
/// \brief Node for the trie (has a string_trie parent)
///
template <typename T>
struct string_trie<T>::node_t : string_trie<T> {
	using typename string_trie<T>::type;
	using typename string_trie<T>::string_t;

	///
	/// \brief Cached string value up to this node
	///
	string_t value;
	///
	/// \brief Stored character
	///
	type c = {};
	///
	/// \brief Whether this node designates a terminating word
	///
	bool word = false;
};

template <typename T>
typename string_trie<T>::node_t& string_trie<T>::add(string_view_t word, std::size_t index) {
	assert(index < word.size() && "Invalid index");
	for (node_t& node : this->nodes) {
		if (node.c == word[index]) {
			if (index + 1 == word.size()) {
				node.word = true;
				return node;
			}
			return node.add(word, index + 1);
		}
	}
	node_t node;
	node.c = word[index];
	node.value = word.substr(0, index + 1);
	if (index + 1 == word.size()) {
		node.word = true;
		nodes.push_back(std::move(node));
		return nodes.back();
	}
	nodes.push_back(std::move(node));
	return nodes.back().add(word, index + 1);
}

template <typename T>
typename string_trie<T>::node_t* string_trie<T>::find(string_view_t str, std::size_t index) {
	return find<node_t>(*this, str, index);
}

template <typename T>
typename string_trie<T>::node_t const* string_trie<T>::find(string_view_t str, std::size_t index) const {
	return find<node_t const>(*this, str, index);
}

template <typename T>
template <typename C>
void string_trie<T>::words(C& out, string_view_t str) const {
	using value_type = typename C::value_type;
	if (node_t const* node = find(str)) {
		if (node->word) {
			out.insert(out.end(), value_type{node->value});
		}
		node->add_word_nodes(out);
	}
}

template <typename T>
template <typename C>
void string_trie<T>::add_word_nodes(C& out) const {
	using value_type = typename C::value_type;
	for (node_t const& node : this->nodes) {
		if (node.word) {
			out.insert(out.end(), value_type{node.value});
		}
		node.add_word_nodes(out);
	}
}

template <typename T>
template <typename Ret, typename Self>
Ret* string_trie<T>::find(Self& self, std::string_view str, std::size_t index) {
	if (index >= str.size()) {
		return nullptr;
	}
	for (auto& node : self.nodes) {
		if (node.c == str[index]) {
			if (index == str.size() - 1) {
				return &node;
			} else {
				return node.find(str, index + 1);
			}
		}
	}
	return nullptr;
}
} // namespace kt
