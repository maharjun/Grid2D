##  Algorithm for A-B:

Implemented as state machine.

###   State variables:

`ACursor` - A cursor That points to a potential start of an interval in the difference
`BInterval` - The interval in B

This is a derived state
`AInterval` - The interval in A inside which ACursor is contained

    while BInterval and AInterval are valid:
        if BInterval < ACursor:
            increment BInterval
        else if BInterval cont ACursor:
            if BInterval cont AInterval:
                increment AInterval
                ACursor = AInterval.begin if AInterval is valid
            else:
                ACursor = BInterval.end
        else if BInterval > ACursor:
            if AInterval cont BInterval.begin():
                add [ACursor, BInterval.begin) to diff
                ACursor = BInterval.begin
            else:
                add [ACursor, AInterval.end) to diff
                increment AInterval
                ACursor = AInterval.begin if AInterval is valid

    if AInterval is valid:
        Add [ACursor, AInterval.end) to diff
        Add all subsequent AIntervals to diff

##  Algorithm for tracing the midway boundary 

    □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □
    □ □ □ □ □ □ □ □ □ ■ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □
    □ □ □ □ □ □ □ □ ■ ■ ■ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □
    □ □ □ □ □ □ □ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ □ □ □ □ □ □ □ □
    □ □ □ □ □ □ □ □ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ □ □ □ □ □ □ □
    □ □ □ □ □ □ □ □ □ □ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ □ □ □ □ □ □ □ □ □ □
    □ □ □ □ □ □ □ □ □ □ □ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ □ □ □ □ □ □ □ □ □ □ □ □ □ □
    □ □ □ □ □ □ □ □ □ □ □ □ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □
    □ □ □ □ □ □ □ □ □ □ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ □ □ □ □ □ □ □ □ □ □ □ □ □
    □ □ □ □ □ □ □ □ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ □ □ □ □ □ □ □ □ □ □ □ □
    □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □ □

create a EdgeMap which is an unordered map as follows (Python Dict
syntax)

{BoundaryPoint:NextBoundaryPoint}

####    Filling EdgeMap - Algo 1
    for each C in gridColumns:
        LeftDiff = C - column to left of C
        RightDiff = C - column to right of C
        for all points in LeftDiff:
            Add left edge in upward direction to EdgeMap
        for all points in RightDiff:
            Add right edge in downward direction ot EdgeMap
        for all segment boundaries in C:
            Add upper edge of upper boundary in right direction to EdgeMap
            Add lower edge of lower boundary in left direction to EdgeMap

####    Filling EdgeMap - Algo 2
    for each C in gridColumns:
        for each contained interval:
            Add upper edge of upper boundary in right direction to EdgeMap
            Add lower edge of lower boundary in left direction to EdgeMap
    for each R in gridRows:
        for each contained interval:
            Add left edge of left boundary in upward direction to EdgeMap
            Add right edge of right boundary in downward direction to EdgeMap

Find Least Point (constant time).

Trace boundary using The linked list represented by EdgeMap with the
starting as the least point with the starting and return vector

##  Example

Region:

    □ □ ■ ■ ■ □ □
    □ □ ■ ■ ■ □ □
    □ □ ■ ■ ■ □ □
    □ □ □ □ □ □ □
    ■ ■ ■ □ □ □ □
    ■ ■ ■ □ □ □ □
    ■ ■ ■ □ □ □ □

Inner Boundary

    □ □ ■ ■ ■ □ □
    □ □ ■ □ ■ □ □
    □ □ ■ ■ ■ □ □
    □ □ □ □ □ □ □
    ■ ■ ■ □ □ □ □
    ■ □ ■ □ □ □ □
    ■ ■ ■ □ □ □ □

Outer Boundary

    □ ■ □ □ □ ■ □
    □ ■ □ □ □ ■ □
    □ ■ □ □ □ ■ □
    ■ ■ ■ ■ ■ □ □
    □ □ □ ■ □ □ □
    □ □ □ ■ □ □ □
    □ □ □ ■ □ □ □

Midway Boundary

    □ □ ■ ■ ■ ■ □ □
    □ □ ■ □ □ ■ □ □
    □ □ ■ □ □ ■ □ □
    □ □ ■ ■ ■ ■ □ □
    ■ ■ ■ ■ □ □ □ □
    ■ □ □ ■ □ □ □ □
    ■ □ □ ■ □ □ □ □
    ■ ■ ■ ■ □ □ □ □

Actually returned boundary

    □ □ □ □ □ □ □ □
    □ □ □ □ □ □ □ □
    □ □ □ □ □ □ □ □
    □ □ □ □ □ □ □ □
    ■ ■ ■ ■ □ □ □ □
    ■ □ □ ■ □ □ □ □
    ■ □ □ ■ □ □ □ □
    ■ ■ ■ ■ □ □ □ □
