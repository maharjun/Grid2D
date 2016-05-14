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

##  Major Algorithm:

###   Top Level pseudocode:

We have the following data:

1.  **FullAttractorBasin** -
    
    This is a Region that represents the entire attractor basin

2.  **attBasinPartition** -
    
    This is a vector of regions which forms a partition of FullAttractor-
    Basin.  The partition basically splits  FullAttractorBasin  into Sub-
    regions based on how many  time steps they take to get attracted into
    region 0. attBasinPartition[j] <==> j+1 steps to get attracted.

3.  **outOfBoundsRegion** - 
    
    This is the region that consist of points that end up outside the grid
    boundary upon simulation. This is maintained so that these points are
    not tested repeatedly.

        for each point on the outer boundary:
            simulate one timestep, and get NextTimePoint
            if NextTimePoint in attBasinPartition[j] for any existing j:
                add point to attBasinPartition[j+1] (create if it doesnt exist)
            elif NextTimePoint in outOfBoundsRegion or NextTimePoint out of grid:

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
