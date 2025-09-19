import Core.Math;

interface IInterface {
	func GetNumber() -> i32;
}

class MyBase {
	
}

class MyClass : IInterface {

	override GetNumber() {
		return 10;
	}
}

func MakeObject() -> MyClass {
	return MyClass();
}

func Main() -> i32 {
	const obj = MyClass();
	return obj.GetNumber();
}