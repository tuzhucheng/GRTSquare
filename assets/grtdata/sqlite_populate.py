# This script is used to build a SQLite database out of the GRT GTFS files.
# The generated database can then be directly used by the application.

import os
import sqlite3
import re

CREATE_ROUTES_TABLE_SQL = """CREATE TABLE routes
    (_id INTEGER PRIMARY KEY AUTOINCREMENT,
    routeNumber INTEGER,
    routeName VARCHAR);"""

CREATE_STOPS_TABLE_SQL = """CREATE TABLE stops
    (_id INTEGER PRIMARY KEY AUTOINCREMENT,
    stopNumber INTEGER,
    stopName VARCHAR);"""

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

CREATE_CALENDAR_DATES_TABLE_SQL = """CREATE TABLE calendar_dates
    (_id INTEGER PRIMARY KEY AUTOINCREMENT,
    serviceId VARCHAR,
    date VARCHAR,
    exceptionType INTEGER);"""

CREATE_TRIPS_TABLE_SQL = """CREATE TABLE trips
    (_id INTEGER PRIMARY KEY AUTOINCREMENT,
    tripId VARCHAR,
    directionId INTEGER,
    tripHeadSign VARCHAR,
    routeId INTEGER,
    serviceId VARCHAR,
    FOREIGN KEY(routeId) REFERENCES routes(routeNumber),
    FOREIGN KEY(serviceId) REFERENCES calendar(serviceId));"""

CREATE_STOP_TIMES_TABLE_SQL = """CREATE TABLE stop_times
    (_id INTEGER PRIMARY KEY AUTOINCREMENT,
    tripId VARCHAR,
    stopId INTEGER,
    arrivalTime VARCHAR,
    departureTime VARCHAR,
    FOREIGN KEY(tripId) REFERENCES trips(tripId),
    FOREIGN KEY(stopId) REFERENCES stops(stopNumber));"""

CREATE_FAVORITE_STOPS_TABLE_SQL = """CREATE TABLE favorite_stops
    (_id INTEGER PRIMARY KEY AUTOINCREMENT,
    stopId INTEGER,
    routeId INTEGER);"""

CREATE_GLOBAL_CONFIGS_TABLE_SQL = """CREATE TABLE global_configs
    (_id INTEGER PRIMARY KEY AUTOINCREMENT,
    configName VARCHAR,
    configValue VARCHAR);"""

# Column Ids
# Stops table
STOPS_ID_COL_ID = -1
STOPS_NAME_COL_ID = -1
STOPS_LOC_TYPE_COL_ID = -1

surround_quotes = re.compile(r"""\"*\"""")

def createTables(cursor):
    cursor.execute(CREATE_ROUTES_TABLE_SQL)
    cursor.execute(CREATE_STOPS_TABLE_SQL)
    cursor.execute(CREATE_CALENDAR_TABLE_SQL)
    cursor.execute(CREATE_CALENDAR_DATES_TABLE_SQL)
    cursor.execute(CREATE_TRIPS_TABLE_SQL)
    cursor.execute(CREATE_STOP_TIMES_TABLE_SQL)
    cursor.execute(CREATE_FAVORITE_STOPS_TABLE_SQL)
    cursor.execute(CREATE_GLOBAL_CONFIGS_TABLE_SQL)

def processHeader(csv):
    header = csv.readline()
    header = header[:-1]
    header = header.split(sep=",")
    return header

def mapColumnNameToId(header, col_names):
    mapping = {}
    for c in col_names:
        for i in range(0, len(header)):
            if header[i] == c:
                mapping[c] = i
                break
    return mapping

def main():
    # remove the generated SQLite database if it exists so tables can be re-created
    try:
        os.remove('grt.db')
    except OSError:
        pass

    connection = sqlite3.connect('grt.db')

    cursor = connection.cursor()

    # create tables
    createTables(cursor)

    # populate data in routes table
    INSERT_INTO_ROUTES_SQL = """INSERT INTO routes
    (routeNumber, routeName) VALUES (?, ?);"""
    path = os.path.join("gtfs", "routes.txt")
    with open(path) as csv:
        header = processHeader(csv)
        col_names = ["route_long_name", "route_id"]
        mapping = mapColumnNameToId(header, col_names)
        print(mapping)

        for line in csv:
            line = line[:-1]
            fields = line.split(sep=",")
            route_name = fields[mapping["route_long_name"]]
            match = surround_quotes.search(route_name)
            if match:
                route_name = route_name[1:len(route_name)-1]
            cursor.execute(INSERT_INTO_ROUTES_SQL, (fields[mapping["route_id"]], route_name))

    # populate data in stops table
    INSERT_INTO_STOPS_SQL = """INSERT INTO stops
    (stopNumber, stopName) VALUES (?, ?);"""
    path = os.path.join("gtfs", "stops.txt")
    with open(path) as csv:
        header = processHeader(csv)
        col_names = ["stop_id", "stop_name", "location_type"]
        mapping = mapColumnNameToId(header, col_names)
        print(mapping)

        for line in csv:
            line = line[:-1]
            fields = line.split(sep=",")
            stop_name = fields[mapping["stop_name"]]
            match = surround_quotes.search(stop_name)
            if match:
                stop_name = stop_name[1:len(stop_name)-1]
            if fields[mapping["location_type"]] == "0":
                cursor.execute(INSERT_INTO_STOPS_SQL, (fields[mapping["stop_id"]], stop_name))

    # populate data in calendar table
    INSERT_INTO_CALENDAR_SQL = """INSERT INTO calendar
    (serviceId, startDate, endDate, monday, tuesday, wednesday, thursday, friday, saturday, sunday)
    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"""
    path = os.path.join("gtfs", "calendar.txt")
    with open(path) as csv:
        header = processHeader(csv)
        col_names = ["service_id", "start_date", "end_date", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday"]
        mapping = mapColumnNameToId(header, col_names)
        print(mapping)

        for line in csv:
            line = line[:-1]
            fields = line.split(sep=",")
            args = (fields[mapping["service_id"]], fields[mapping["start_date"]], fields[mapping["end_date"]], fields[mapping["monday"]], fields[mapping["tuesday"]], \
                    fields[mapping["wednesday"]], fields[mapping["thursday"]], fields[mapping["friday"]], fields[mapping["saturday"]], fields[mapping["sunday"]])
            cursor.execute(INSERT_INTO_CALENDAR_SQL, args)

    # populate data in calendar_dates table
    INSERT_INTO_CALENDAR_DATES_SQL = """INSERT INTO calendar_dates
    (serviceId, date, exceptionType) VALUES (?, ?, ?);"""
    path = os.path.join("gtfs", "calendar_dates.txt")
    with open(path) as csv:
        header = processHeader(csv)
        col_names = ["service_id", "date", "exception_type"]
        mapping = mapColumnNameToId(header, col_names)
        print(mapping)

        for line in csv:
            line = line[:-1]
            fields = line.split(sep=",")
            cursor.execute(INSERT_INTO_CALENDAR_DATES_SQL, (fields[mapping["service_id"]], fields[mapping["date"]], fields[mapping["exception_type"]]))

    # populate data in trips table
    INSERT_INTO_TRIPS_SQL = """INSERT INTO trips
    (tripId, routeId, serviceId, directionId, tripHeadSign)
    VALUES (?, ?, ?, ?, ?);"""
    path = os.path.join("gtfs", "trips.txt")
    with open(path) as csv:
        header = processHeader(csv)
        col_names = ["route_id", "service_id", "trip_id", "trip_headsign", "direction_id"]
        mapping = mapColumnNameToId(header, col_names)
        print(mapping)

        for line in csv:
            line = line[:-1]
            fields = line.split(sep=",")
            trip_headsign = fields[mapping["trip_headsign"]]
            match = surround_quotes.search(trip_headsign)
            if match:
                trip_headsign = trip_headsign[1:len(trip_headsign)-1]
            args = (fields[mapping["trip_id"]], fields[mapping["route_id"]], fields[mapping["service_id"]], fields[mapping["direction_id"]], trip_headsign)
            cursor.execute(INSERT_INTO_TRIPS_SQL, args)

    # populate data in stops table
    INSERT_INTO_STOP_TIMES_SQL = """INSERT INTO stop_times
    (tripId, stopId, arrivalTime, departureTime)
    VALUES (?, ?, ?, ?);"""
    path = os.path.join("gtfs", "stop_times.txt")
    with open(path) as csv:
        header = processHeader(csv)
        col_names = ["trip_id", "stop_id", "arrival_time", "departure_time"]
        mapping = mapColumnNameToId(header, col_names)
        print(mapping)

        for line in csv:
            line = line[:-1]
            fields = line.split(sep=",")
            args = (fields[mapping["trip_id"]], fields[mapping["stop_id"]], fields[mapping["arrival_time"]], fields[mapping["departure_time"]])
            cursor.execute(INSERT_INTO_STOP_TIMES_SQL, args)

    connection.commit()

    # populate data in global configuration table
    INSERT_INTO_GLOBAL_CONFIGS_SQL = """INSERT INTO global_configs (configName, configValue) VALUES (?, ?)"""
    result = cursor.execute("SELECT MAX(endDate) FROM calendar")
    args = ("endDate", result.fetchone()[0])
    cursor.execute(INSERT_INTO_GLOBAL_CONFIGS_SQL, args)
    result = cursor.execute("SELECT MIN(startDate) FROM calendar")
    args = ("startDate", result.fetchone()[0])
    cursor.execute(INSERT_INTO_GLOBAL_CONFIGS_SQL, args)

    connection.commit()
    connection.close()

if __name__ == "__main__":
    main()
