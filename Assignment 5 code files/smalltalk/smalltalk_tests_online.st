"Single-file version for online compilers (e.g. onlinegdb.com). Paste all of it and click Run."
"RideSharing.st - Class definitions for the Ride Sharing System (GNU Smalltalk).

 ENCAPSULATION: in Smalltalk every instance variable is private to its object.
   Outside code can only reach state through the messages (methods) a class defines.
 INHERITANCE:  StandardRide, PremiumRide and SharedRide are subclasses of Ride
   (SharedRide is a subclass of StandardRide).
 POLYMORPHISM: every ride class answers #fare, #rideType and #rideDetails in its
   own way, so the same message sent to different rides gives different results."

Object subclass: Ride [
    | rideID pickupLocation dropoffLocation distance fare |

    <comment: 'Base class for every kind of ride.'>

    "---------- class-side (constructor and helpers) ----------"

    Ride class >> id: anId from: aPickup to: aDropoff distance: miles [
        miles <= 0 ifTrue: [^self error: 'Ride distance must be positive'].
        ^self new setId: anId from: aPickup to: aDropoff distance: miles
    ]

    Ride class >> roundToCents: aNumber [
        "24.375 -> 24.38 (kept as an exact fraction, so totals add up exactly)"
        ^(aNumber * 100) rounded / 100
    ]

    Ride class >> money: aNumber [
        "Formats 12.5 as '$12.50'"
        | cents |
        cents := (aNumber * 100) rounded.
        ^'$', (cents // 100) printString, '.', ((cents \\ 100) printPaddedWith: $0 to: 2)
    ]

    Ride class >> formatNumber: aNumber [
        "Formats 12.5 as '12.5', 3 as '3', 5.25 as '5.25'.
         (Avoids Float>>printString, which has a bug in GNU Smalltalk 3.2.5.)"
        | cents whole frac |
        cents := (aNumber * 100) rounded.
        whole := (cents // 100) printString.
        frac := cents \\ 100.
        frac = 0 ifTrue: [^whole].
        frac \\ 10 = 0 ifTrue: [^whole, '.', (frac // 10) printString].
        ^whole, '.', (frac printPaddedWith: $0 to: 2)
    ]

    "---------- initialization (private) ----------"

    setId: anId from: aPickup to: aDropoff distance: miles [
        rideID := anId.
        pickupLocation := aPickup.
        dropoffLocation := aDropoff.
        distance := miles.
        fare := 0
    ]

    "---------- read-only accessors (no setters) ----------"

    rideID [ ^rideID ]
    pickupLocation [ ^pickupLocation ]
    dropoffLocation [ ^dropoffLocation ]
    distance [ ^distance ]

    "---------- behaviour that subclasses override ----------"

    rideType [ ^'Basic' ]

    fare [
        "Base pricing: $1.50 per mile."
        fare := Ride roundToCents: distance * (3/2).
        ^fare
    ]

    rideDetails [
        "self fare is sent polymorphically, so each subclass's pricing is used."
        Transcript
            showCr: '[', self rideType, ' Ride #', rideID printString, ']';
            showCr: '  From: ', pickupLocation, '  ->  To: ', dropoffLocation;
            showCr: '  Distance: ', (Ride formatNumber: distance), ' miles';
            showCr: '  Fare: ', (Ride money: self fare)
    ]

    summary [
        ^'#', rideID printString, ' ', self rideType, ': ', pickupLocation,
            ' -> ', dropoffLocation, ' (', (Ride money: self fare), ')'
    ]
]

Ride subclass: StandardRide [
    <comment: 'Everyday ride: $2.50 booking fee + $1.75 per mile.'>

    rideType [ ^'Standard' ]

    fare [
        fare := Ride roundToCents: (5/2) + (distance * (7/4)).
        ^fare
    ]
]

Ride subclass: PremiumRide [
    | vehicleModel |

    <comment: 'Luxury ride: $5.00 booking fee + $3.25 per mile, minimum $15.00.'>

    PremiumRide class >> id: anId from: aPickup to: aDropoff distance: miles vehicle: aModel [
        ^(self id: anId from: aPickup to: aDropoff distance: miles) setVehicle: aModel
    ]

    setVehicle: aModel [ vehicleModel := aModel ]

    vehicleModel [ ^vehicleModel ifNil: ['Luxury Sedan'] ]

    rideType [ ^'Premium' ]

    fare [
        fare := Ride roundToCents: (5 + (distance * (13/4)) max: 15).
        ^fare
    ]

    rideDetails [
        "Reuse the inherited output, then extend it."
        super rideDetails.
        Transcript showCr: '  Vehicle: ', self vehicleModel
    ]
]

StandardRide subclass: SharedRide [
    <comment: 'Pooled ride: StandardRide price with a 30% discount.'>

    rideType [ ^'Shared' ]

    fare [
        "Start from the parent's (StandardRide's) price, then apply the discount."
        fare := Ride roundToCents: super fare * (7/10).
        ^fare
    ]
]

Object subclass: Driver [
    | driverID name rating ratingCount assignedRides |

    <comment: 'A driver. assignedRides is private and only reachable through methods.'>

    Driver class >> id: anId name: aName rating: aRating [
        (aRating between: 1 and: 5) ifFalse: [^self error: 'Driver rating must be between 1 and 5'].
        ^self new setId: anId name: aName rating: aRating
    ]

    setId: anId name: aName rating: aRating [
        driverID := anId.
        name := aName.
        rating := aRating.
        ratingCount := 1.
        assignedRides := OrderedCollection new
    ]

    driverID [ ^driverID ]
    name [ ^name ]
    rating [ ^rating ]
    rideCount [ ^assignedRides size ]

    "Note: there is deliberately NO #assignedRides accessor."

    addRide: aRide [
        "Answers true if added, false if nil or already assigned."
        aRide isNil ifTrue: [^false].
        (assignedRides detect: [:r | r rideID = aRide rideID] ifNone: [nil]) notNil
            ifTrue: [^false].
        assignedRides add: aRide.
        ^true
    ]

    rateDriver: stars [
        (stars between: 1 and: 5) ifFalse: [^self error: 'Star rating must be between 1 and 5'].
        rating := ((rating * ratingCount) + stars) / (ratingCount + 1).
        ratingCount := ratingCount + 1
    ]

    totalEarnings [
        ^assignedRides inject: 0 into: [:sum :ride | sum + ride fare]
    ]

    getDriverInfo [
        Transcript
            showCr: 'Driver #', driverID printString, ': ', name;
            showCr: '  Rating: ', ((Ride money: rating) copyFrom: 2 to: (Ride money: rating) size),
                ' (', ratingCount printString, ' ratings)';
            showCr: '  Completed rides: ', assignedRides size printString.
        assignedRides do: [:ride | Transcript showCr: '    - ', ride summary].
        Transcript showCr: '  Total earnings: ', (Ride money: self totalEarnings)
    ]
]

Object subclass: Rider [
    | riderID name requestedRides |

    <comment: 'A rider. requestedRides is private and only reachable through methods.'>

    Rider class >> id: anId name: aName [
        ^self new setId: anId name: aName
    ]

    setId: anId name: aName [
        riderID := anId.
        name := aName.
        requestedRides := OrderedCollection new
    ]

    riderID [ ^riderID ]
    name [ ^name ]
    rideCount [ ^requestedRides size ]

    requestRide: aRide [
        "Answers true if added, false if nil or a duplicate."
        aRide isNil ifTrue: [^false].
        (requestedRides detect: [:r | r rideID = aRide rideID] ifNone: [nil]) notNil
            ifTrue: [^false].
        requestedRides add: aRide.
        ^true
    ]

    totalSpent [
        ^requestedRides inject: 0 into: [:sum :ride | sum + ride fare]
    ]

    viewRides [
        Transcript showCr: 'Rider #', riderID printString, ': ', name,
            ' - ride history (', requestedRides size printString, ' rides)'.
        requestedRides isEmpty ifTrue: [Transcript showCr: '    (no rides yet)'].
        requestedRides do: [:ride | Transcript showCr: '    - ', ride summary].
        Transcript showCr: '  Total spent: ', (Ride money: self totalSpent)
    ]
]
"tests.st - Simple self-contained unit tests.
 Run with:  gst RideSharing.st tests.st"

| passed failed check approx threw list d r |

passed := 0.
failed := 0.
check := [:label :condition |
    condition
        ifTrue: [passed := passed + 1. Transcript showCr: 'PASS: ', label]
        ifFalse: [failed := failed + 1. Transcript showCr: 'FAIL: ', label]].
approx := [:a :b | (a - b) abs < (1/1000)].

"--- Fare calculations (inheritance + overriding) ---"
check value: 'Basic ride: 10 mi x $1.50 = $15.00'
      value: (approx value: (Ride id: 1 from: 'A' to: 'B' distance: 10) fare value: 15).
check value: 'Standard ride: $2.50 + 10 x $1.75 = $20.00'
      value: (approx value: (StandardRide id: 2 from: 'A' to: 'B' distance: 10) fare value: 20).
check value: 'Premium ride: $5.00 + 10 x $3.25 = $37.50'
      value: (approx value: (PremiumRide id: 3 from: 'A' to: 'B' distance: 10) fare value: 37.5).
check value: 'Premium ride minimum fare is $15.00'
      value: (approx value: (PremiumRide id: 4 from: 'A' to: 'B' distance: 1) fare value: 15).
check value: 'Shared ride: 30% off standard = $14.00'
      value: (approx value: (SharedRide id: 5 from: 'A' to: 'B' distance: 10) fare value: 14).

"--- Polymorphism: same message, different receivers ---"
list := OrderedCollection new.
list add: (StandardRide id: 10 from: 'A' to: 'B' distance: 10);
     add: (PremiumRide id: 11 from: 'A' to: 'B' distance: 10);
     add: (SharedRide id: 12 from: 'A' to: 'B' distance: 10).
check value: 'fare sent to a collection gives each type''s own price'
      value: ((list collect: [:ride | ride fare]) asArray = (Array with: 20 with: 75/2 with: 14)).
check value: 'rideType is polymorphic'
      value: ((list collect: [:ride | ride rideType]) asArray = #('Standard' 'Premium' 'Shared')).
check value: 'SharedRide inherits from StandardRide and Ride'
      value: ((list at: 3) isKindOf: StandardRide) & ((list at: 3) isKindOf: Ride).

"--- Validation ---"
threw := false.
[StandardRide id: 99 from: 'A' to: 'B' distance: 0] on: Error do: [:e | threw := true. e return: nil].
check value: 'Zero distance is rejected' value: threw.

"--- Driver (encapsulation) ---"
d := Driver id: 1 name: 'Test Driver' rating: 4.
check value: 'New driver has no rides' value: d rideCount = 0.
check value: 'addRide: accepts a new ride' value: (d addRide: (list at: 1)).
check value: 'addRide: rejects a duplicate ride' value: (d addRide: (list at: 1)) not.
check value: 'addRide: rejects nil' value: (d addRide: nil) not.
d addRide: (list at: 2).
check value: 'Driver ride count is 2' value: d rideCount = 2.
check value: 'Driver earnings = $57.50' value: (approx value: d totalEarnings value: 57.5).
d rateDriver: 5.
check value: 'Rating average (4 + 5) / 2 = 4.5' value: (approx value: d rating value: 4.5).
threw := false.
[d rateDriver: 7] on: Error do: [:e | threw := true. e return: nil].
check value: 'Rating of 7 stars is rejected' value: threw.
check value: 'assignedRides is not accessible from outside'
      value: (d respondsTo: #assignedRides) not.

"--- Rider ---"
r := Rider id: 1 name: 'Test Rider'.
check value: 'New rider has no rides' value: r rideCount = 0.
check value: 'requestRide: accepts a new ride' value: (r requestRide: (list at: 3)).
check value: 'requestRide: rejects a duplicate' value: (r requestRide: (list at: 3)) not.
r requestRide: (list at: 1).
check value: 'Rider total spent = $34.00' value: (approx value: r totalSpent value: 34).

Transcript cr; showCr: passed printString, ' passed, ', failed printString, ' failed'.
