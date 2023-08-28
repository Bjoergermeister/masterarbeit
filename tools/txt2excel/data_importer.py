from genericpath import isfile
from os import listdir

benchmarks = {}

TYPE_CONVERSIONS = {
    "Filesystem": lambda value: int(float(value)),
    "Memory": lambda value: float(value),
    "Network": lambda value: float(value),
    "ContainerVM": lambda value: int(float(value))
}

def get_target_list(type, operation, language, mode, case):
    if type not in benchmarks:
        benchmarks[type] = {}
    
    if operation not in benchmarks[type]:
        benchmarks[type][operation] = {}

    if language not in benchmarks[type][operation]:
        # ContainerVM has no language or no cases, so return here
        if type == "ContainerVM":
            benchmarks[type][operation][language] = []
            return benchmarks[type][operation][language]
        else:
            benchmarks[type][operation][language] = {}

    if mode not in benchmarks[type][operation][language]:
        if operation == "Create" or type == "Network": # there are no cases in create operation, so we can stop nesting lists here
            benchmarks[type][operation][language][mode] = []
        else:
            benchmarks[type][operation][language][mode] = {}

    if operation == "Create" or type == "Network":
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

        # Not all measurements have type, operation, language, mode and case.
        # Therefore, the file_parts array does not always have the length 5
        # To fix this, fill it up so that it always has the length 5 
        if len(file_parts) < 5:
            file_parts.extend([None for i in range(0, 5 - len(file_parts))])
        process_file(path, file_parts[0], file_parts[1], file_parts[2], file_parts[3], file_parts[4])
        '''
        try:
            if file_parts[0] == "ContainerVM": # ContainerVM measurements are only one language and one case
                process_file(path, file_parts[0], file_parts[1], file_parts[2], None, None)
            if file_parts[1] == "Create" or file_parts[0] == "Network": # There is just one case in create operation
                process_file(path, file_parts[0], file_parts[1], file_parts[2], file_parts[3], None)
            else:
                process_file(path, file_parts[0], file_parts[1], file_parts[2], file_parts[3], file_parts[4])
        except IndexError as error:
            print(error, path, file_parts)
        '''
def process_file(path, type, operation, language, mode, case):
    file = open(path, "r")
    list = get_target_list(type, operation, language, mode, case)
    for line in file.readlines():
        if (operation == "Write" and type == "Filesystem") or (operation == "ThroughputUDP" and type == "Network"):
            measurements = [TYPE_CONVERSIONS[type](x) for x in map(lambda string: string.replace(",", "."), line.split(", "))]
            list.append((measurements[0], measurements[1]))
        else:
            list.append(TYPE_CONVERSIONS[type](line))
