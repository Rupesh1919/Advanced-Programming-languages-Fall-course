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
"main.st - Demo of the Ride Sharing System.
 Run with:  gst RideSharing.st main.st"

| banner rides sameTrip total alice bob carol dave erin added |

banner := [:title | Transcript cr; showCr: '========== ', title, ' =========='].

Transcript showCr: 'RIDE SHARING SYSTEM (Smalltalk)'.

"---- POLYMORPHISM: one collection holds different ride types ----"
rides := OrderedCollection new.
rides
    add: (StandardRide id: 101 from: 'Downtown' to: 'Airport' distance: 12.5);
    add: (PremiumRide id: 102 from: 'Hotel Plaza' to: 'Convention Center' distance: 3 vehicle: 'Tesla Model S');
    add: (SharedRide id: 103 from: 'University' to: 'Mall' distance: 8);
    add: (PremiumRide id: 104 from: 'Airport' to: 'Uptown' distance: 18 vehicle: 'BMW 7 Series');
    add: (StandardRide id: 105 from: 'Mall' to: 'Stadium' distance: 5.2);
    add: (Ride id: 106 from: 'Library' to: 'Park' distance: 2).

banner value: 'All rides (polymorphic rideDetails)'.
rides do: [:ride | ride rideDetails].

banner value: 'Fares (polymorphic fare)'.
total := 0.
rides do: [:ride |
    total := total + ride fare.
    Transcript showCr: '  Ride #', ride rideID printString, ' (', ride rideType, '): ',
        (Ride money: ride fare)].
Transcript showCr: '  Total of all fares: ', (Ride money: total).

banner value: 'Same 10-mile trip, different ride types'.
sameTrip := OrderedCollection new.
sameTrip
    add: (Ride id: 201 from: 'A' to: 'B' distance: 10);
    add: (StandardRide id: 202 from: 'A' to: 'B' distance: 10);
    add: (SharedRide id: 203 from: 'A' to: 'B' distance: 10);
    add: (PremiumRide id: 204 from: 'A' to: 'B' distance: 10).
sameTrip do: [:ride | Transcript showCr: '  ', ride rideType, ': ', (Ride money: ride fare)].

"---- DRIVERS ----"
banner value: 'Drivers'.
alice := Driver id: 1 name: 'Alice Johnson' rating: 4.8.
bob := Driver id: 2 name: 'Bob Smith' rating: 4.5.

alice addRide: (rides at: 1); addRide: (rides at: 3); addRide: (rides at: 5).
bob addRide: (rides at: 2); addRide: (rides at: 4); addRide: (rides at: 6).

added := alice addRide: (rides at: 1).    "duplicate - rejected"
Transcript showCr: 'Adding ride #101 to Alice a second time accepted? ',
    (added ifTrue: ['yes'] ifFalse: ['no']).

alice rateDriver: 5.
bob rateDriver: 3.

alice getDriverInfo.
Transcript cr.
bob getDriverInfo.

"---- RIDERS ----"
banner value: 'Riders'.
carol := Rider id: 501 name: 'Carol White'.
dave := Rider id: 502 name: 'Dave Brown'.
erin := Rider id: 503 name: 'Erin Green'.

carol requestRide: (rides at: 1); requestRide: (rides at: 2).
dave requestRide: (rides at: 3); requestRide: (rides at: 4); requestRide: (rides at: 5).

carol viewRides.
Transcript cr.
dave viewRides.
Transcript cr.
erin viewRides.

"---- ENCAPSULATION ----"
banner value: 'Encapsulation'.
[alice assignedRides]
    on: MessageNotUnderstood
    do: [:e |
        Transcript showCr: 'Asking alice for #assignedRides fails: Driver does not understand it.'.
        e return: nil].
Transcript showCr: 'The list can only be used through addRide:, rideCount, etc.'.
Transcript showCr: 'Alice''s ride count via rideCount: ', alice rideCount printString.

[StandardRide id: 999 from: 'X' to: 'Y' distance: -4]
    on: Error
    do: [:e |
        Transcript showCr: 'Creating a ride with distance -4 was rejected: ', e messageText.
        e return: nil].
