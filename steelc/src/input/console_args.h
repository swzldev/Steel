#pragma once

#include <string>
#include <vector>

class console_args {
public:
	class arg_iterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::string;
		using difference_type = std::ptrdiff_t;
		using pointer = const std::string*;
		using reference = const std::string&;

		arg_iterator(std::vector<std::string>::const_iterator it)
			: iter(it) {
		}

		reference operator*() const { return *iter; }
		pointer operator->() const { return &(*iter); }

		arg_iterator& operator++() { ++iter; return *this; }

		arg_iterator operator++(int) {
			arg_iterator tmp = *this;
			++iter;
			return tmp;
		}

		bool operator==(const arg_iterator& other) const {
			return iter == other.iter;
		}

		bool operator!=(const arg_iterator& other) const {
			return iter != other.iter;
		}

	private:
		std::vector<std::string>::const_iterator iter;
	};

public:
	console_args(int argc, char** argv);

	bool has_arg(const std::string& arg) const;
	int find_arg(const std::string& arg) const;

	inline const std::string* get_arg(size_t index) const {
		if (index < args.size()) {
			return &args[index];
		}
		return nullptr;
	}

	inline size_t count() const { return args.size(); }

	// iterators
	inline arg_iterator begin() const { return arg_iterator(args.begin()); }
	inline arg_iterator end()   const { return arg_iterator(args.end()); }
	inline arg_iterator cbegin() const { return arg_iterator(args.cbegin()); }
	inline arg_iterator cend()   const { return arg_iterator(args.cend()); }

private:
	std::vector<std::string> args;
};