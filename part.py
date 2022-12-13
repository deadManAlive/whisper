import csv
from typing import Tuple


def get_spiffs_parameter(partition_table: str ="default.csv") -> Tuple[str | None, str | None]:
    """
    Get SPIFFS partition offset and size from partition table

    Takes one optional argument `partition_table` as a csv partition table, \\ 
    defaults to `default.csv` at current working directory.

    format: name, type, subtype, offset, size, flags 
        
    """
    spiffs_offset = None
    spiffs_size = None

    with open(partition_table, mode='r') as csv_file:
        csv_dict = csv.DictReader(csv_file, fieldnames=["name", "type", "subtype", "offset", "size", "flags"])
        line_count = 0
        for row in csv_dict:
            if row["name"] == "spiffs":
                spiffs_offset = row['offset'].strip()
                spiffs_size = row['size'].strip()
                
        return spiffs_offset, spiffs_size
if __name__ == "__main__":
    get_spiffs_parameter("readme.md")