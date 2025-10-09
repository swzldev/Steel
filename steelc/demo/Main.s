func Add<Fty, Tty>(Fty a, Tty b) -> Fty {
	return a + b;
}

func Main() -> i32 {
	const fl = Add<i32, i32>(3.5, 3);

	Print(fl);

	return 0;
}