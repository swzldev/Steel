class Ref<T> {
	constructor(T* x) {
		this.value = x;
	}

	value: T*;
}

func Add<T>(T a, T b) -> T {
	return a + b;
}

func Main() -> i32 {
	return Add<i32>(3, 4);
}