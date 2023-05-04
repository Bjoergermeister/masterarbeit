from genericpath import isfile
from os import listdir

benchmarks = {}

def get_target_list(type, operation, language, mode, case):
    if type not in benchmarks:
        benchmarks[type] = {}
    
    if operation not in benchmarks[type]:
        benchmarks[type][operation] = {}

    if language not in benchmarks[type][operation]:
        benchmarks[type][operation][language] = {}

    if mode not in benchmarks[type][operation][language]:
        if operation == "Create": # there a no cases in create operation, so we can stop neesting lists here
            benchmarks[type][operation][language][mode] = []
        else:
            benchmarks[type][operation][language][mode] = {}

    if operation == "Create":
        return benchmarks[type][operation][language][mode]

    if case not in benchmarks[type][operation][language][mode]:
        benchmarks[type][operation][language][mode][case] = []

    return benchmarks[type][operation][language][mode][case]

def import_data(folder, target):
    files = [file for file in listdir(folder) if isfile(f"{folder}/{file}")]
    for file in files:

        # Build relative path and strip file extension if present
        path = f"{folder}/{file}"

        file_parts = path[0:-4].split("_")
        file_parts[0] = file_parts[0][14:]

        if file_parts[1] == "Create": # There is just one case in create operation
            process_file(path, file_parts[0], file_parts[1], file_parts[2], file_parts[3], None)
        else:
            process_file(path, file_parts[0], file_parts[1], file_parts[2], file_parts[3], file_parts[4])

def process_file(path, type, operation, language, mode, case):
    file = open(path, "r")
    list = get_target_list(type, operation, language, mode, case)
    for line in file.readlines():
        if operation == "Write":
            measurements = [int(x) for x in line.split(", ")]
            list.append((measurements[0], measurements[1]))
        else:
            list.append(int(line))