import Core.Math;

interface IOtherInterface {
	func OtherImplement() -> i32;
}

interface IInterface {
	func Implement() -> i32;
}

class MyClass : IInterface, IOtherInterface {
	override Implement() {
		return 42;
	}
	override OtherImplement() {
		return 24;
	}
}

func Main() -> i32 {
	const [MyClass] obj = MyClass();
	return obj.OtherImplement();
}