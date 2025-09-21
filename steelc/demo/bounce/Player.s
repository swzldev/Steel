import Core.Math;

module Game {
    class Player {
        constructor() {
            this.position = Vector2();
            this.velocity = Vector2();

            this.gravity = Vector2(0, 1);
        }

        func Tick() {
            // left and right movement
            if (GetKey('a')) {
                this.velocity.x = -3;
            }
            else if (GetKey('d')) {
                this.velocity.x = 3;
            }
            else {
                this.velocity.x = 0;
            }

            // apply gravity and velocity
            this.velocity.CompoundAdd(gravity);

            if (this.position.y >= 15) {
                this.position.y = 15;
                this.velocity.y = -5;
            }

            this.position.CompoundAdd(this.velocity);
        }

        position: Vector2;
        velocity: Vector2;

        gravity: Vector2;
    }
}