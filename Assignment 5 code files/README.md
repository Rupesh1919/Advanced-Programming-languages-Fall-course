# Ride Sharing System (C++ and Smalltalk)

A class-based ride sharing system demonstrating encapsulation, inheritance and
polymorphism in C++ and Smalltalk.

| File | Purpose |
|------|---------|
| `cpp/cpp_demo_online.cpp` | C++ program: all classes + demo |
| `cpp/cpp_tests_online.cpp` | C++ unit tests (22 tests) |
| `smalltalk/smalltalk_demo_online.st` | Smalltalk program: all classes + demo |
| `smalltalk/smalltalk_tests_online.st` | Smalltalk unit tests (22 tests) |

## Classes

- `Ride` (base class): rideID, pickupLocation, dropoffLocation, distance, fare; `fare()`, `rideDetails()`
- `StandardRide` (inherits Ride): $2.50 + $1.75 per mile
- `PremiumRide` (inherits Ride): $5.00 + $3.25 per mile, minimum $15.00
- `SharedRide` (inherits StandardRide): standard price with 30% discount
- `Driver`: private `assignedRides`; `addRide()`, `getDriverInfo()`, `rateDriver()`
- `Rider`: private `requestedRides`; `requestRide()`, `viewRides()`

## How to run

Each file is self-contained. Paste it into an online compiler such as
https://www.onlinegdb.com and click Run:

- C++ files: select language **C++17**
- Smalltalk files: select language **Smalltalk** (GNU Smalltalk)

Locally:

```bash
g++ -std=c++17 cpp/cpp_demo_online.cpp -o demo && ./demo
g++ -std=c++17 cpp/cpp_tests_online.cpp -o tests && ./tests
gst smalltalk/smalltalk_demo_online.st
gst smalltalk/smalltalk_tests_online.st
```

## Sample C++ output

The following excerpt is from a successful run of the C++ demo. Calling `fare()`
through base-class pointers produces different prices for the same distance:

```text
========== Same 10-mile trip, different ride types ==========
  Basic: $15.00
  Standard: $20.00
  Shared: $14.00
  Premium: $37.50

========== Drivers ==========
Adding ride #101 to Alice a second time accepted? no
Driver #1: Alice Johnson
  Rating: 4.90 (2 ratings)
  Completed rides: 3
    - #101 Standard: Downtown -> Airport ($24.38)
    - #103 Shared: University -> Mall ($11.55)
    - #105 Standard: Mall -> Stadium ($11.60)
  Total earnings: $47.53
```

The C++ test program also completed successfully: **22 passed, 0 failed**.
