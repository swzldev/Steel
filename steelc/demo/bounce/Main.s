import Core.Math;
import Game;

func Main() -> i32 {
	const player = Player();
	player.position.x = 50;
	player.position.y = 5;

	while (true) {
		player.Tick();
		Render(&player);
		Wait(0.1);
	}
	
	return 0;
}

func Render(Player* player) {
	const mapWidth = 100;
	const mapHeight = 20;
	const groundLevel = 15;

	SetConsolePos(0, 0);
	for (let y = 0; y < mapHeight; y++) {
		if (player.position.y != y) {
			if (y < groundLevel) {
				Print(' ' * mapWidth);
			}
			else if (y == groundLevel) {
				Print('-' * mapWidth);
			}
			else {
				Print('#' * mapWidth);
			}
		}
		else {
			for (let x = 0; x < mapWidth; x++) {
				if (y < groundLevel) {
					if (player.position.x == x and player.position.y == y) {
						Print('@');
					}
					else {
						Print(' ');
					}
				}
				else if (y == groundLevel) {
					Print('-');
				}
				else {
					Print('#');
				}
			}
		}
		Print("\n");
	}
}