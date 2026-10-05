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
// tests.cpp - Simple self-contained unit tests (no external framework needed).
#include <cmath>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>


static int passed = 0;
static int failed = 0;

void check(const std::string& label, bool condition) {
    if (condition) {
        ++passed;
        std::cout << "PASS: " << label << "\n";
    } else {
        ++failed;
        std::cout << "FAIL: " << label << "\n";
    }
}

bool approx(double a, double b) { return std::fabs(a - b) < 0.001; }

int main() {
    // --- Fare calculations (inheritance + overriding) ---
    Ride basic(1, "A", "B", 10);
    StandardRide standard(2, "A", "B", 10);
    PremiumRide premium(3, "A", "B", 10);
    PremiumRide shortPremium(4, "A", "B", 1);
    SharedRide shared(5, "A", "B", 10);

    check("Basic ride: 10 mi x $1.50 = $15.00", approx(basic.fare(), 15.00));
    check("Standard ride: $2.50 + 10 x $1.75 = $20.00", approx(standard.fare(), 20.00));
    check("Premium ride: $5.00 + 10 x $3.25 = $37.50", approx(premium.fare(), 37.50));
    check("Premium ride minimum fare is $15.00", approx(shortPremium.fare(), 15.00));
    check("Shared ride: 30% off standard = $14.00", approx(shared.fare(), 14.00));

    // --- Polymorphism through base-class pointers ---
    std::vector<std::shared_ptr<Ride>> list = {
        std::make_shared<StandardRide>(10, "A", "B", 10),
        std::make_shared<PremiumRide>(11, "A", "B", 10),
        std::make_shared<SharedRide>(12, "A", "B", 10)};
    check("Base pointer calls StandardRide::fare()", approx(list[0]->fare(), 20.00));
    check("Base pointer calls PremiumRide::fare()", approx(list[1]->fare(), 37.50));
    check("Base pointer calls SharedRide::fare()", approx(list[2]->fare(), 14.00));
    check("rideType() is polymorphic", list[0]->rideType() == "Standard" &&
                                           list[1]->rideType() == "Premium" &&
                                           list[2]->rideType() == "Shared");

    // --- Validation ---
    bool threw = false;
    try { StandardRide bad(99, "A", "B", 0); } catch (const std::invalid_argument&) { threw = true; }
    check("Zero distance throws invalid_argument", threw);

    // --- Driver (encapsulation) ---
    Driver d(1, "Test Driver", 4.0);
    check("New driver has no rides", d.getRideCount() == 0);
    check("addRide accepts a new ride", d.addRide(list[0]));
    check("addRide rejects a duplicate ride", !d.addRide(list[0]));
    check("addRide rejects a null ride", !d.addRide(nullptr));
    d.addRide(list[1]);
    check("Driver ride count is 2", d.getRideCount() == 2);
    check("Driver earnings = $57.50", approx(d.totalEarnings(), 57.50));
    d.rateDriver(5);
    check("Rating average (4 + 5) / 2 = 4.5", approx(d.getRating(), 4.5));
    threw = false;
    try { d.rateDriver(7); } catch (const std::invalid_argument&) { threw = true; }
    check("Rating of 7 stars is rejected", threw);

    // --- Rider ---
    Rider r(1, "Test Rider");
    check("New rider has no rides", r.getRideCount() == 0);
    check("requestRide accepts a new ride", r.requestRide(list[2]));
    check("requestRide rejects a duplicate", !r.requestRide(list[2]));
    r.requestRide(list[0]);
    check("Rider total spent = $34.00", approx(r.totalSpent(), 34.00));

    std::cout << "\n" << passed << " passed, " << failed << " failed\n";
    return failed == 0 ? 0 : 1;
}
