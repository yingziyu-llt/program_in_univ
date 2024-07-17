fn main() {
    let mut input = String::new();
    std::io::stdin()
        .read_line(&mut input)
        .expect("Error reading input!");
    let farin: i64 = input.trim().parse().expect("Not an integer!");
    println!("{}C", (farin - 32) * 5 / 9);
}
