import csv

from data_importer import import_data
from data_importer import benchmarks

HEADERS = {
    "Other": ["level", "regular", "manual", "container"],
    "Write": ["level", "regular", "regular2", "manual", "manual2", "container", "container2"],
}

CASES = {
    "Delete": ["1", "128", "256", "512", "1024", "2048"],
    "Other": ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15",]
}

first = lambda list: [element[0] for element in list]
second = lambda list: [element[1] for element in list]


def calculate_average(times):
    if len(times) < 20:
        return ""
    return round(sum(sorted(times)[5:-5]) / (len(times) - 10), 2)

def write_filesystem_create(writer, operation, language):
    regular_value = calculate_average(benchmarks["Filesystem"]["Create"][language]["Regular"])
    manual_value = calculate_average(benchmarks["Filesystem"]["Create"][language]["Manual"])
    container_value = calculate_average(benchmarks["Filesystem"]["Create"][language]["Container"])
    writer.writerow([0, regular_value, manual_value, container_value])    

def write_filesystem_write(writer, operation, language):
    for size in ["1", "128", "256", "512", "1024", "2048"]:
        regular_value = calculate_average(first(benchmarks["Filesystem"]["Write"][language]["Regular"][size]))
        regular_value2 = calculate_average(second(benchmarks["Filesystem"]["Write"][language]["Regular"][size]))
        manual_value = calculate_average(first(benchmarks["Filesystem"]["Write"][language]["Manual"][size]))
        manual_value2 = calculate_average(second(benchmarks["Filesystem"]["Write"][language]["Manual"][size]))
        container_value = calculate_average(first(benchmarks["Filesystem"]["Write"][language]["Container"][size]))
        container_value2 = calculate_average(second(benchmarks["Filesystem"]["Write"][language]["Container"][size]))
        writer.writerow([size, regular_value, regular_value2, manual_value, manual_value2, container_value, container_value2])

def write_filesystem_open_read_delete(writer, operation, language):
    cases = CASES["Delete"] if operation == "Delete" else CASES["Other"]
    for index in cases:
        write_row(writer, "Filesystem", operation, language, index)

def write_memory(writer, operation, language):
    try:
        for index in [str(x) for x in range(1, 129)]:
            write_row(writer, "Memory", operation, language, index)
    except KeyError as error:
        print(error, operation, language, index)

def write_network(writer, operation, language):
    write_row(writer, "Network", operation, language, 1)

def write_row(writer, type, operation, language, index):
    regular_value = get_value(type, operation, language, "Regular", index)
    manual_value = get_value(type, operation, language, "Manual", index)
    container_value = get_value(type, operation, language, "Container", index)
    writer.writerow([index, regular_value, manual_value, container_value]) 

def get_value(type, operation, language, mode, index):
    if type == "Network":
        return calculate_average(benchmarks[type][operation][language][mode])

    if index in benchmarks[type][operation][language][mode]:
        return calculate_average(benchmarks[type][operation][language][mode][index])
    else:
        return ""

def write_csv(filename, func, operation, language):
    
    with open(filename, "w", newline='') as file:
        writer = csv.writer(file)

        field = HEADERS["Write"] if operation == "Write" and "Filesystem" in filename else HEADERS["Other"]
        writer.writerow(field)
        func(writer, operation, language)

if __name__ == "__main__": 
    import_data(f"../../Results", benchmarks)

    for language in ["C", "Java"]:
        # Filesystem
        write_csv(f"../../Filesystem_Open_{language}.csv", write_filesystem_open_read_delete, "Open", language)
        write_csv(f"../../Filesystem_Read_{language}.csv", write_filesystem_open_read_delete, "Read", language)
        write_csv(f"../../Filesystem_Write_{language}.csv", write_filesystem_write, "Write", language)
        write_csv(f"../../Filesystem_Delete_{language}.csv", write_filesystem_open_read_delete, "Delete", language)
        write_csv(f"../../Filesystem_Create_{language}.csv", write_filesystem_create, "Create", language)

        # Memory
        for operation in ["Allocation", "Write", "Read"]:
            write_csv(f"../../Memory_{operation}_{language}.csv", write_memory, operation, language)

        for operation in ["Latency", "Throughput"]:
            write_csv(f"../../Network_{operation}_{language}.csv", write_network, operation, language)