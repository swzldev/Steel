module Core {
	module Math {
		class Vector2 {
			constructor() {
				this.x = 0;
				this.y = 0;
			}
			constructor(i32 initX, i32 initY) {
				x = initX;
				y = initY;
			}

			func CompoundAdd(Vector2 other) {
				this.x = this.x + other.x;
				this.y = this.y + other.y;
			}

			x: i32;
			y: i32;
		}
	}
}