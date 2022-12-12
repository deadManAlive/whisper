"""
    spiffs generator + uploader tools
"""

import esptool
import spiffsgen
import sys
                
BIN = "public.spiffs.bin"

if __name__ == "__main__":
    if len(sys.argv) <= 1:
        raise Exception("Port unknown!")
    
    # generate
    spiffsgen_args = [
        "0x170000",
        "public",
        BIN
    ]
    
    spiffsgen.main(spiffsgen_args)

    # upload
    esptool_args = [
        "--chip",
        "esp32",
        "--baud",
        "921600",
        "--port",
        sys.argv[1],
        "--before",
        "default_reset",
        "--after",
        "hard_reset",
        "write_flash",
        "-z",
        "--flash_mode",
        "dio",
        "--flash_freq",
        "80m",
        "--flash_size",
        "detect",
        "0x290000",
        BIN
    ]

    print(sys.argv)

    esptool.main(esptool_args)
