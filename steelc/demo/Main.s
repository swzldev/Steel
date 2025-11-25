func print(message: string) {
    printf(message);
}

func main() -> int {
    for (let i = 0; i < 20; i++) {
        if (i % 2 == 0) {
            print("Hello, SteelC!\n");
        }
        else {
            print("Goodbye, SteelC!\n");
        }
    }

    return 0;
}