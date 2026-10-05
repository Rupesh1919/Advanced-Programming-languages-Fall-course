// Single-file version for online compilers (e.g. onlinegdb.com). Paste all of it and click Run.
// Ride.h - Base class Ride and its derived classes.
// Demonstrates INHERITANCE (StandardRide/PremiumRide/SharedRide derive from Ride)
// and POLYMORPHISM (virtual fare(), rideDetails(), rideType()).
#ifndef RIDE_H
#define RIDE_H

#include <string>

class Ride {
protected:
    // Protected: hidden from outside code, but visible to subclasses.
    int rideID;
    std::string pickupLocation;
    std::string dropoffLocation;
    double distance;    // miles
    double fareAmount;  // the "fare" attribute (named so it doesn't clash with fare())

public:
    Ride(int id, const std::string& pickup, const std::string& dropoff, double distance);
    virtual ~Ride() = default;

    // Calculates the fare from the distance, stores it, and returns it.
    // Subclasses override this with their own pricing.
    virtual double fare();

    // Prints the ride's information. Subclasses may extend it.
    virtual void rideDetails();

    virtual std::string rideType() const;

    // One-line description used by Driver and Rider listings.
    std::string summary();

    // Read-only accessors (no setters: a ride's route cannot be changed after creation).
    int getRideID() const { return rideID; }
    std::string getPickupLocation() const { return pickupLocation; }
    std::string getDropoffLocation() const { return dropoffLocation; }
    double getDistance() const { return distance; }

    static std::string money(double amount);  // formats 12.5 as "$12.50"
    static double roundToCents(double amount);  // 24.375 -> 24.38
};

// Everyday ride: booking fee plus a moderate per-mile rate.
class StandardRide : public Ride {
public:
    static constexpr double BOOKING_FEE = 2.50;
    static constexpr double RATE_PER_MILE = 1.75;

    StandardRide(int id, const std::string& pickup, const std::string& dropoff, double distance);
    double fare() override;
    std::string rideType() const override;
};

// Luxury ride: higher booking fee and per-mile rate, with a minimum fare.
class PremiumRide : public Ride {
private:
    std::string vehicleModel;

public:
    static constexpr double BOOKING_FEE = 5.00;
    static constexpr double RATE_PER_MILE = 3.25;
    static constexpr double MINIMUM_FARE = 15.00;

    PremiumRide(int id, const std::string& pickup, const std::string& dropoff, double distance,
                const std::string& vehicleModel = "Luxury Sedan");
    double fare() override;
    void rideDetails() override;  // adds the vehicle model to the base details
    std::string rideType() const override;
    std::string getVehicleModel() const { return vehicleModel; }
};

// Pooled ride: priced like a StandardRide, then discounted.
// Two levels of inheritance: Ride -> StandardRide -> SharedRide.
class SharedRide : public StandardRide {
public:
    static constexpr double POOL_DISCOUNT = 0.30;  // 30% off

    SharedRide(int id, const std::string& pickup, const std::string& dropoff, double distance);
    double fare() override;
    std::string rideType() const override;
};

#endif
// People.h - Driver and Rider classes.
// Demonstrates ENCAPSULATION: the ride lists are private and can only be
// changed or read through the public methods below.
#ifndef PEOPLE_H
#define PEOPLE_H

#include <memory>
#include <string>
#include <vector>


class Driver {
private:
    int driverID;
    std::string name;
    double rating;    // average of all ratings received (1.0 - 5.0)
    int ratingCount;
    std::vector<std::shared_ptr<Ride>> assignedRides;  // PRIVATE - no direct access

public:
    Driver(int id, const std::string& name, double rating = 5.0);

    // Adds a completed ride. Returns false if the ride is null or already assigned.
    bool addRide(const std::shared_ptr<Ride>& ride);

    // Records a new 1-5 star rating and updates the running average.
    void rateDriver(int stars);

    double totalEarnings() const;
    void getDriverInfo() const;

    int getDriverID() const { return driverID; }
    std::string getName() const { return name; }
    double getRating() const { return rating; }
    std::size_t getRideCount() const { return assignedRides.size(); }
};

class Rider {
private:
    int riderID;
    std::string name;
    std::vector<std::shared_ptr<Ride>> requestedRides;  // PRIVATE - no direct access

public:
    Rider(int id, const std::string& name);

    // Adds a ride to the rider's history. Returns false if null or a duplicate.
    bool requestRide(const std::shared_ptr<Ride>& ride);

    double totalSpent() const;
    void viewRides() const;

    int getRiderID() const { return riderID; }
    std::string getName() const { return name; }
    std::size_t getRideCount() const { return requestedRides.size(); }
};

#endif
// Ride.cpp - Implementation of Ride and its subclasses.

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

// ---------------- Ride (base class) ----------------

Ride::Ride(int id, const std::string& pickup, const std::string& dropoff, double distance)
    : rideID(id), pickupLocation(pickup), dropoffLocation(dropoff),
      distance(distance), fareAmount(0.0) {
    if (distance <= 0) {
        throw std::invalid_argument("Ride distance must be positive");
    }
}

double Ride::fare() {
    const double BASE_RATE_PER_MILE = 1.50;
    fareAmount = roundToCents(distance * BASE_RATE_PER_MILE);
    return fareAmount;
}

void Ride::rideDetails() {
    // fare() is a virtual call, so each subclass's own pricing is used here.
    std::cout << "[" << rideType() << " Ride #" << rideID << "]\n"
              << "  From: " << pickupLocation << "  ->  To: " << dropoffLocation << "\n"
              << "  Distance: " << distance << " miles\n"
              << "  Fare: " << money(fare()) << "\n";
}

std::string Ride::rideType() const { return "Basic"; }

std::string Ride::summary() {
    return "#" + std::to_string(rideID) + " " + rideType() + ": " + pickupLocation +
           " -> " + dropoffLocation + " (" + money(fare()) + ")";
}

std::string Ride::money(double amount) {
    std::ostringstream out;
    out << "$" << std::fixed << std::setprecision(2) << amount;
    return out.str();
}

double Ride::roundToCents(double amount) {
    return std::round(amount * 100.0) / 100.0;
}

// ---------------- StandardRide ----------------

StandardRide::StandardRide(int id, const std::string& pickup, const std::string& dropoff,
                           double distance)
    : Ride(id, pickup, dropoff, distance) {}

double StandardRide::fare() {
    fareAmount = roundToCents(BOOKING_FEE + distance * RATE_PER_MILE);
    return fareAmount;
}

std::string StandardRide::rideType() const { return "Standard"; }

// ---------------- PremiumRide ----------------

PremiumRide::PremiumRide(int id, const std::string& pickup, const std::string& dropoff,
                         double distance, const std::string& vehicleModel)
    : Ride(id, pickup, dropoff, distance), vehicleModel(vehicleModel) {}

double PremiumRide::fare() {
    fareAmount = roundToCents(std::max(MINIMUM_FARE, BOOKING_FEE + distance * RATE_PER_MILE));
    return fareAmount;
}

void PremiumRide::rideDetails() {
    Ride::rideDetails();  // reuse the base class output, then extend it
    std::cout << "  Vehicle: " << vehicleModel << "\n";
}

std::string PremiumRide::rideType() const { return "Premium"; }

// ---------------- SharedRide ----------------

SharedRide::SharedRide(int id, const std::string& pickup, const std::string& dropoff,
                       double distance)
    : StandardRide(id, pickup, dropoff, distance) {}

double SharedRide::fare() {
    // Start from the parent's (StandardRide's) price, then apply the pool discount.
    fareAmount = roundToCents(StandardRide::fare() * (1.0 - POOL_DISCOUNT));
    return fareAmount;
}

std::string SharedRide::rideType() const { return "Shared"; }
// People.cpp - Implementation of Driver and Rider.

#include <iomanip>
#include <iostream>
#include <stdexcept>

namespace {
// Shared helper: true if a ride with the same ID is already in the list.
bool containsRide(const std::vector<std::shared_ptr<Ride>>& rides, int rideID) {
    for (const auto& r : rides) {
        if (r->getRideID() == rideID) return true;
    }
    return false;
}
}  // namespace

// ---------------- Driver ----------------

Driver::Driver(int id, const std::string& name, double rating)
    : driverID(id), name(name), rating(rating), ratingCount(1) {
    if (rating < 1.0 || rating > 5.0) {
        throw std::invalid_argument("Driver rating must be between 1 and 5");
    }
}

bool Driver::addRide(const std::shared_ptr<Ride>& ride) {
    if (!ride || containsRide(assignedRides, ride->getRideID())) return false;
    assignedRides.push_back(ride);
    return true;
}

void Driver::rateDriver(int stars) {
    if (stars < 1 || stars > 5) {
        throw std::invalid_argument("Star rating must be between 1 and 5");
    }
    rating = (rating * ratingCount + stars) / (ratingCount + 1);
    ++ratingCount;
}

double Driver::totalEarnings() const {
    double total = 0.0;
    for (const auto& ride : assignedRides) total += ride->fare();  // polymorphic call
    return total;
}

void Driver::getDriverInfo() const {
    std::cout << "Driver #" << driverID << ": " << name << "\n"
              << "  Rating: " << std::fixed << std::setprecision(2) << rating
              << " (" << ratingCount << " ratings)\n"
              << "  Completed rides: " << assignedRides.size() << "\n";
    std::cout.unsetf(std::ios::fixed);
    for (const auto& ride : assignedRides) std::cout << "    - " << ride->summary() << "\n";
    std::cout << "  Total earnings: " << Ride::money(totalEarnings()) << "\n";
}

// ---------------- Rider ----------------

Rider::Rider(int id, const std::string& name) : riderID(id), name(name) {}

bool Rider::requestRide(const std::shared_ptr<Ride>& ride) {
    if (!ride || containsRide(requestedRides, ride->getRideID())) return false;
    requestedRides.push_back(ride);
    return true;
}

double Rider::totalSpent() const {
    double total = 0.0;
    for (const auto& ride : requestedRides) total += ride->fare();  // polymorphic call
    return total;
}

void Rider::viewRides() const {
    std::cout << "Rider #" << riderID << ": " << name << " - ride history ("
              << requestedRides.size() << " rides)\n";
    if (requestedRides.empty()) {
        std::cout << "    (no rides yet)\n";
    }
    for (const auto& ride : requestedRides) std::cout << "    - " << ride->summary() << "\n";
    std::cout << "  Total spent: " << Ride::money(totalSpent()) << "\n";
}
// main.cpp - Demonstrates the Ride Sharing System.
#include <iostream>
#include <memory>
#include <vector>


void banner(const std::string& title) {
    std::cout << "\n========== " << title << " ==========\n";
}

int main() {
    std::cout << "RIDE SHARING SYSTEM (C++)\n";

    // ---- POLYMORPHISM: one list holds different ride types ----
    // Each element is a pointer to the base class Ride, but the object it
    // points to may be a StandardRide, PremiumRide, or SharedRide.
    std::vector<std::shared_ptr<Ride>> rides = {
        std::make_shared<StandardRide>(101, "Downtown", "Airport", 12.5),
        std::make_shared<PremiumRide>(102, "Hotel Plaza", "Convention Center", 3.0, "Tesla Model S"),
        std::make_shared<SharedRide>(103, "University", "Mall", 8.0),
        std::make_shared<PremiumRide>(104, "Airport", "Uptown", 18.0, "BMW 7 Series"),
        std::make_shared<StandardRide>(105, "Mall", "Stadium", 5.2),
        std::make_shared<Ride>(106, "Library", "Park", 2.0)};

    banner("All rides (polymorphic rideDetails())");
    for (const auto& ride : rides) {
        ride->rideDetails();  // the correct override runs for each object
    }

    banner("Fares (polymorphic fare())");
    double total = 0.0;
    for (const auto& ride : rides) {
        double f = ride->fare();
        total += f;
        std::cout << "  Ride #" << ride->getRideID() << " (" << ride->rideType()
                  << "): " << Ride::money(f) << "\n";
    }
    std::cout << "  Total of all fares: " << Ride::money(total) << "\n";

    // Same distance, different types -> different fares
    banner("Same 10-mile trip, different ride types");
    std::vector<std::shared_ptr<Ride>> sameTrip = {
        std::make_shared<Ride>(201, "A", "B", 10),
        std::make_shared<StandardRide>(202, "A", "B", 10),
        std::make_shared<SharedRide>(203, "A", "B", 10),
        std::make_shared<PremiumRide>(204, "A", "B", 10)};
    for (const auto& ride : sameTrip) {
        std::cout << "  " << ride->rideType() << ": " << Ride::money(ride->fare()) << "\n";
    }

    // ---- DRIVERS ----
    banner("Drivers");
    Driver alice(1, "Alice Johnson", 4.8);
    Driver bob(2, "Bob Smith", 4.5);

    alice.addRide(rides[0]);
    alice.addRide(rides[2]);
    alice.addRide(rides[4]);
    bob.addRide(rides[1]);
    bob.addRide(rides[3]);
    bob.addRide(rides[5]);

    bool added = alice.addRide(rides[0]);  // duplicate - rejected
    std::cout << "Adding ride #101 to Alice a second time accepted? "
              << (added ? "yes" : "no") << "\n";

    alice.rateDriver(5);
    bob.rateDriver(3);

    alice.getDriverInfo();
    std::cout << "\n";
    bob.getDriverInfo();

    // ---- RIDERS ----
    banner("Riders");
    Rider carol(501, "Carol White");
    Rider dave(502, "Dave Brown");
    Rider erin(503, "Erin Green");

    carol.requestRide(rides[0]);
    carol.requestRide(rides[1]);
    dave.requestRide(rides[2]);
    dave.requestRide(rides[3]);
    dave.requestRide(rides[4]);

    carol.viewRides();
    std::cout << "\n";
    dave.viewRides();
    std::cout << "\n";
    erin.viewRides();

    // ---- ENCAPSULATION ----
    banner("Encapsulation");
    std::cout << "assignedRides is private. This line would NOT compile:\n"
              << "    alice.assignedRides.clear();\n"
              << "The list can only be used through addRide(), getRideCount(), etc.\n"
              << "Alice's ride count via getRideCount(): " << alice.getRideCount() << "\n";

    try {
        StandardRide bad(999, "X", "Y", -4);
    } catch (const std::invalid_argument& e) {
        std::cout << "Creating a ride with distance -4 was rejected: " << e.what() << "\n";
    }

    return 0;
}
