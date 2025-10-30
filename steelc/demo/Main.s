/*class SmartPointer<T> {
	constructor(T* ptr) {
		this.ptr = ptr;
	}

	func Get() -> T* {
		return ptr;
	}

	ptr: T*;
}*/

enum Color {
	Red,
	Green,
	Blue
}

func Main() -> int {
	/*const val = 42;
	const ptr = SmartPointer<int>(&val);

	Print(ptr.Get());*/

	const color = Color.Green;
	if (color == Color.Red) {
		Print("The color is Red");
	} else {
		Print("The color is not Red");
	}

	return 0;
}