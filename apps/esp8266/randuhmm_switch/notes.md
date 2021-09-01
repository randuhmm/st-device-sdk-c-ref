
Install UART drivers depending on the type of USB connection:

Node MCU - https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers

FTDI - https://ftdichip.com/drivers/

To flash from windows:

python %APPDATA%\Python\Python39\site-packages\esptool.py ^
--chip esp8266 --port COM6 --baud 115200 --before default_reset ^
--after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size 2MB 0x11000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp8266\randuhmm_dimmer\build\ota_data_initial.bin 0x0000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp8266\randuhmm_dimmer\build\bootloader\bootloader.bin 0x14000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp8266\randuhmm_dimmer\build\randuhmm_dimmer.bin 0x8000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp8266\randuhmm_dimmer\build\partitions.2MB.bin

python %APPDATA%\Python\Python39\site-packages\esptool.py ^
--chip esp8266 --port COM6 --baud 115200 --before default_reset ^
--after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size 2MB 0x11000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp8266\rad_dimmer_x2\build\ota_data_initial.bin 0x0000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp8266\rad_dimmer_x2\build\bootloader\bootloader.bin 0x14000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp8266\rad_dimmer_x2\build\rad_dimmer_x2.bin 0x8000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp8266\rad_dimmer_x2\build\partitions.2MB.bin

python %APPDATA%\Python\Python39\site-packages\esptool.py ^
--chip esp8266 --port COM7 --baud 115200 --before default_reset ^
--after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size 2MB 0x11000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp8266\randuhmm_switch\build\ota_data_initial.bin 0x0000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp8266\randuhmm_switch\build\bootloader\bootloader.bin 0x14000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp8266\randuhmm_switch\build\randuhmm_switch.bin 0x8000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp8266\randuhmm_switch\build\partitions.2MB.bin

python %APPDATA%\Python\Python39\site-packages\esptool.py ^
--chip esp8266 --port COM6 --baud 115200 --before default_reset ^
--after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size 2MB 0x11000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp8266\switch_example\build\ota_data_initial.bin 0x0000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp8266\switch_example\build\bootloader\bootloader.bin 0x14000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp8266\switch_example\build\switch_example.bin 0x8000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp8266\switch_example\build\partitions.2MB.bin


python %APPDATA%\Python\Python39\site-packages\esptool.py ^
--port COM6 -b 460800 --before default_reset --after hard_reset write_flash ^
--flash_mode dio --flash_size detect --flash_freq 40m 0x1000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp32\randuhmm_dimmer\build\bootloader\bootloader.bin 0x8000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp32\randuhmm_dimmer\build\partition_table\partition-table.bin 0x11000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp32\randuhmm_dimmer\build\ota_data_initial.bin 0x20000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp32\randuhmm_dimmer\build\randuhmm_dimmer.bin


python %APPDATA%\Python\Python39\site-packages\esptool.py ^
--port COM6 -b 460800 --before default_reset --after hard_reset write_flash ^
--flash_mode dio --flash_size detect --flash_freq 40m 0x1000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp32\switch_example\build\bootloader\bootloader.bin 0x8000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp32\switch_example\build\partition_table\partition-table.bin 0x11000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp32\switch_example\build\ota_data_initial.bin 0x20000 ^
W:\home\jonny\Projects\st-device-sdk-c-ref\apps\esp32\switch_example\build\switch_example.bin
