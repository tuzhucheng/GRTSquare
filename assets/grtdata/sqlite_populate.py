# This script is used to build a SQLite database out of the GRT GTFS files.
# The generated database can then be directly used by the application.

import os
import sqlite3

CREATE_ROUTES_TABLE_SQL = """CREATE TABLE routes
    (_id INTEGER PRIMARY KEY AUTOINCREMENT,
    routeNumber INTEGER,
    routeName VARCHAR);"""

CREATE_STOPS_TABLE_SQL = """CREATE TABLE stops
    (_id INTEGER PRIMARY KEY AUTOINCREMENT,
    stopNumber INTEGER,
    stopName VARCHAR,
    stopLatitude REAL,
    stopLongitude REAL);"""

CREATE_CALENDAR_TABLE_SQL = """CREATE TABLE calendar
    (_id INTEGER PRIMARY KEY AUTOINCREMENT,
    serviceId VARCHAR,
    startDate VARCHAR,
    endDate VARCHAR,
    monday INTEGER,
    tuesday INTEGER,
    wednesday INTEGER,
    thursday INTEGER,
    friday INTEGER,
    saturday INTEGER,
    sunday INTEGER);"""

CREATE_SHAPES_TABLE_SQL = """CREATE TABLE shapes
    (_id INTEGER PRIMARY KEY AUTOINCREMENT,
    shapeId INTEGER,
    shapePtLatitude REAL,
    shapePtLongitude REAL,
    shapePtSequence INTEGER);"""

CREATE_TRIPS_TABLE_SQL = """CREATE TABLE trips
    (_id INTEGER PRIMARY KEY AUTOINCREMENT,
    blockId VARCHAR,
    tripId VARCHAR,
    directionId INTEGER,
    tripHeadSign VARCHAR,
    routeId INTEGER,
    shapeId INTEGER,
    serviceId VARCHAR,
    FOREIGN KEY(routeId) REFERENCES routes(routeNumber),
    FOREIGN KEY(shapeId) REFERENCES shapes(shapeId),
    FOREIGN KEY(serviceId) REFERENCES calendar(serviceId));"""

CREATE_STOP_TIMES_TABLE_SQL = """CREATE TABLE stop_times
    (_id INTEGER PRIMARY KEY AUTOINCREMENT,
    tripId VARCHAR,
    stopId INTEGER,
    arrivalTime VARCHAR,
    departureTime VARCHAR,
    stopSequence INTEGER,
    FOREIGN KEY(tripId) REFERENCES trips(tripId),
    FOREIGN KEY(stopId) REFERENCES stops(stopNumber));"""

INSERT_INTO_ROUTES_SQL = """INSERT INTO routes
    (routeNumber, routeName) VALUES (?, ?);"""

INSERT_INTO_STOPS_SQL = """INSERT INTO stops
    (stopNumber, stopName, stopLatitude, stopLongitude) VALUES (?, ?, ?, ?);"""

INSERT_INTO_CALENDAR_SQL = """INSERT INTO calendar
    (serviceId, startDate, endDate, monday, tuesday, wednesday, thursday, friday, saturday, sunday)
    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"""

INSERT_INTO_SHAPES_SQL = """INSERT INTO shapes
    (shapeId, shapePtLatitude, shapePtLongitude, shapePtSequence)
    VALUES (?, ?, ?, ?);"""

INSERT_INTO_TRIPS_SQL = """INSERT INTO trips
    (blockId, tripId, directionId, tripHeadSign, routeId, shapeId, serviceId)
    VALUES (?, ?, ?, ?, ?, ?, ?);"""

INSERT_INTO_STOP_TIMES_SQL = """INSERT INTO stop_times
    (tripId, stopId, arrivalTime, departureTime, stopSequence)
    VALUES (?, ?, ?, ?, ?);"""

# remove the generated SQLite database if it exists so tables can be re-created
try:
    os.remove('grt.db')
except OSError:
    pass

connection = sqlite3.connect('grt.db')

cursor = connection.cursor()

# create tables
cursor.execute(CREATE_ROUTES_TABLE_SQL)
cursor.execute(CREATE_STOPS_TABLE_SQL)
cursor.execute(CREATE_CALENDAR_TABLE_SQL)
cursor.execute(CREATE_SHAPES_TABLE_SQL)
cursor.execute(CREATE_TRIPS_TABLE_SQL)
cursor.execute(CREATE_STOP_TIMES_TABLE_SQL)

# populate data in routes table
path = os.path.join("gtfs", "routes.csv")
with open(path) as csv:
    next(csv)
    for line in csv:
        line = line[:-1]
        fields = line.split(sep=",")
        cursor.execute(INSERT_INTO_ROUTES_SQL, (fields[1], fields[0]))

# populate data in stops table
path = os.path.join("gtfs", "stops.csv")
with open(path) as csv:
    next(csv)
    for line in csv:
        line = line[:-1]
        fields = line.split(sep=",")
        if fields[8] == "0":
            cursor.execute(INSERT_INTO_STOPS_SQL, (fields[3], fields[7], fields[0], fields[2]))

# populate data in calendar table
path = os.path.join("gtfs", "calendar.csv")
with open(path) as csv:
    next(csv)
    for line in csv:
        line = line[:-1]
        fields = line.split(sep=",")
        args = (fields[0], fields[1], fields[2], fields[3], fields[4], fields[5], fields[6], \
                fields[7], fields[8], fields[9])
        cursor.execute(INSERT_INTO_CALENDAR_SQL, args)

# populate data in shapes table
path = os.path.join("gtfs", "shapes.csv")
with open(path) as csv:
    next(csv)
    for line in csv:
        line = line[:-1]
        fields = line.split(sep=",")
        cursor.execute(INSERT_INTO_SHAPES_SQL, (fields[0], fields[1], fields[2], fields[3]))

# populate data in trips table
path = os.path.join("gtfs", "trips.csv")
with open(path) as csv:
    next(csv)
    for line in csv:
        line = line[:-1]
        fields = line.split(sep=",")
        args = (fields[0], fields[6], fields[2], fields[3], fields[1], fields[4], fields[5])
        cursor.execute(INSERT_INTO_TRIPS_SQL, args)

# populate data in stops table
path = os.path.join("gtfs", "stop_times.csv")
with open(path) as csv:
    next(csv)
    for line in csv:
        line = line[:-1]
        fields = line.split(sep=",")
        args = (fields[0], fields[3], fields[1], fields[2], fields[4])
        cursor.execute(INSERT_INTO_STOP_TIMES_SQL, args)

connection.commit()

connection.close()
