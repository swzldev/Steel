import Core.Math;

interface IPrintable {
	func Prnt() -> void;
}

class vec2 : IPrintable {
	constructor() {
		x = 0;
		y = 0;
	}
	constructor(i32 initX, i32 initY) {
		x = initX;
		y = initY;
	}

	override Prnt() {
		Print(x);
		Print(", ");
		Print(y);
		Print("\n");
	}

	x: i32;
	y: i32;
}

class vec3 : IPrintable {
	constructor() {
		x = 0;
		y = 0;
		z = 0;
	}
	constructor(i32 initX, i32 initY, i32 initZ) {
		x = initX;
		y = initY;
		z = initZ;
	}

	override Prnt() {
		Print(x);
		Print(", ");
		Print(y);
		Print(", ");
		Print(z);
		Print("\n");
	}

	x: i32;
	y: i32;
	z: i32;
}

func Main() -> i32 {
	return 5;
}