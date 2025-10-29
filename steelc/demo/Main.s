
class SmartPointer<T> {
	constructor(T* ptr) {
		this.ptr = ptr;
	}

	func Get() -> T* {
		return ptr;
	}

	ptr: T*;
}

func Main() -> int {
	const val = 42;
	const ptr = SmartPointer<int>(&val);

	Print(ptr.Get());

	return 0;
}