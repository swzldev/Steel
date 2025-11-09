module Core {
	module Math {
		class Vector2 {
			constructor() {
				this.x = 0;
				this.y = 0;
			}
			constructor(int x, int y) {
				this.x = x;
				this.y = y;
			}

			func CompoundAdd(Vector2 other) {
				this.x = this.x + other.x;
				this.y = this.y + other.y;
			}

			x: int;
			y: int;
		}
	}
}