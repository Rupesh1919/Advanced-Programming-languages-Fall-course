fn main() {
    let mut data = Box::new(vec![1, 2, 3, 4, 5]);
    println!("Sum: {}", data.iter().sum::<i32>()); // borrow
    data.push(6);                                   // mutable borrow
    consume(data);                                  // ownership moved
    // println!("{:?}", data);  // won't compile: used after move
}

fn consume(v: Box<Vec<i32>>) {
    println!("{:?}", v);
} // freed automatically here