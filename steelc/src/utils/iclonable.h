#pragma once

#include <memory>

template<typename T>
class iclonable {
public:
	virtual std::shared_ptr<T> clone() const = 0;
};