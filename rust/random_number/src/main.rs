use rand::{self, Rng};

fn main() {
    println!("Guess Number");
    let random_number: i32 = rand::thread_rng().gen_range(1..101);
    loop {
        println!("Input your number:");
        let mut input = String::new();
        std::io::stdin()
            .read_line(&mut input)
            .expect("Failed to read line");
        println!("your number is {}", input);
        let input_number: i32 = match input.trim().parse() {
            Ok(num) => num,
            Err(_) => continue,
        };
        match input_number.cmp(&random_number) {
            std::cmp::Ordering::Less => println!("Too small!"),
            std::cmp::Ordering::Greater => println!("Too big!"),
            std::cmp::Ordering::Equal => {
                println!("You win!");
                break;
            }
        }
    }
}
