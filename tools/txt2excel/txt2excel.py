from genericpath import isfile
import math
from os import listdir

from openpyxl import Workbook
from openpyxl.styles import Alignment, PatternFill

COLUMNS = []
MODES = ["Regular", "Manual", "Container"]
for i in range(0, 52):
    prefix = 'A' if i > 25 else ''
    ascii_code = i if i <= 25 else i - 26
    COLUMNS.append("{}{}".format(prefix, chr(ascii_code + 65)))


OPERATION_CASES_COUNTS = {
    "Open": 16,
    "Read": 16,
    "Write": 6,
    "Create": 1,
    "Delete": 6
}

CASE_LABELS = {
    "Open": range(0, 16),
    "Read": range(0, 16),
    "Write": ["1", "128", "256", "512", "1024", "2048"],
    "Delete": ["1", "128", "256", "512", "1024", "2048"]
}

benchmarks = {}

def get_columns_for_mode(operation, mode):
    mode_index = MODES.index(mode)
    multiplier = 2 if operation == "Write" else 1
    start_index = (mode_index * OPERATION_CASES_COUNTS[operation] * multiplier + (1 * mode_index))
    end_index = start_index + OPERATION_CASES_COUNTS[operation] * multiplier
    return COLUMNS[start_index:end_index]

def get_case_label_for_index(operation, index):
    if operation == "Write":
        case = str(CASE_LABELS[operation][math.ceil(index // 2)])
        return f"{case} (2)" if index % 2 == 1 else case
    else:
        return str(CASE_LABELS[operation][index])
    
def get_values(operation, mode, case_index):
    if operation == "Create":
        return benchmarks["Filesystem"][operation]["C"][mode]
    else:
        case = get_case_label_for_index(operation, case_index)
        if case.endswith(")"):
            case = case[:-4]
        return benchmarks["Filesystem"][operation]["C"][mode][case]


def add_spreadsheet_headers(workbook):
    sheets = ["Open", "Read", "Write", "Create", "Delete"]
    for sheet in sheets:
        add_spreadsheet_header(workbook[sheet], sheet)

def add_spreadsheet_header(sheet, operation):
    for i in range(0, 3):
        multiplier = 2 if operation == "Write" else 1
        range_start = (i * OPERATION_CASES_COUNTS[operation] * multiplier + (1 * i))
        range_end = range_start + OPERATION_CASES_COUNTS[operation] * multiplier - 1
        
        sheet.merge_cells(start_row=1, start_column=range_start + 1, end_row=1, end_column=range_end + 1)
        column_code = f"{COLUMNS[range_start]}1" 
        sheet[column_code] = MODES[i]
        sheet[column_code].alignment = Alignment(horizontal="center")
        sheet[column_code].fill = PatternFill(fgColor="cccccc", fill_type="solid")
        
        if operation == "Create":
            continue

        for index, column in enumerate(range(range_start, range_end + 1)):
            column_code = f"{COLUMNS[column]}2"
            label = get_case_label_for_index(operation, index) 
            sheet[column_code] = str(int(label) + 1) if operation in ["Read", "Open"] else label 
            sheet[column_code].alignment = Alignment(horizontal="center")
            sheet[column_code].fill = PatternFill(fgColor="cccccc", fill_type="solid")


def add_data_to_spreadsheet(workbook):
    for operation in list(benchmarks["Filesystem"].keys()):
        print("\n", operation)
        sheet = workbook[operation]

        # Regular
        start_row = 2 if operation == "Create" else 3
        for mode in MODES:                          
            for index, column in enumerate(get_columns_for_mode(operation, mode)):
                values = get_values(operation, mode, index)
                for row in range(0, 100):
                    cell = f"{column}{start_row + row}"
                    sheet[cell] = values[row][index % 2] if operation == "Write" else values[row]
        
        # Add formulas
        MATH_OPERATIONS = ["MIN", "MAX", "SUM"]
        DIVIDERS = ["", "", "/100"]
        ALL_COLUMNS = [get_columns_for_mode(operation, mode) for mode in MODES]
        for index, mode in enumerate(MODES):
            for list_index, column_list in enumerate(ALL_COLUMNS):
                for column_index, value_column in enumerate(column_list):
                    formula = f"={MATH_OPERATIONS[index]}({value_column}{start_row}:{value_column}{start_row + 99}){DIVIDERS[index]}"
                    start_row_offset = 101 + list_index
                    sheet[f"{ALL_COLUMNS[index][column_index]}{start_row + start_row_offset}"] = formula

def save_spreadsheet():
    workbook = Workbook()

    workbook.active.title = "Open"
    workbook.create_sheet("Read", 2)
    workbook.create_sheet("Write", 3)
    workbook.create_sheet("Create", 4)
    workbook.create_sheet("Delete", 5)

    add_spreadsheet_headers(workbook)
    add_data_to_spreadsheet(workbook)
    workbook.save(filename="../Filesystem.xlsx")

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

def process_file(path, type, operation, language, mode, case):
    file = open(path, "r")
    list = get_target_list(type, operation, language, mode, case)
    for line in file.readlines():
        if operation == "Write":
            measurements = [int(x) for x in line.split(", ")]
            list.append((measurements[0], measurements[1]))
        else:
            list.append(int(line))
        
def read_measurement_files(folder):
    files = [file for file in listdir(folder) if isfile(f"{folder}/{file}")]
    for file in files:

        # Build relative path and strip file extension if present
        path = f"{folder}/{file}"

        file_parts = path[0:-4].split("_")
        file_parts[0] = file_parts[0][11:]

        if file_parts[1] == "Create": # There is just one case in create operation
            process_file(path, file_parts[0], file_parts[1], file_parts[2], file_parts[3], None)
        else:
            process_file(path, file_parts[0], file_parts[1], file_parts[2], file_parts[3], file_parts[4])

if __name__ == "__main__":
    read_measurement_files("../Results")
    save_spreadsheet()