# Guide: Uploading Code to the ESP32-S3 Using PlatformIO and VSCode
## Step 1: Install Visual Studio Code (VSCode)
Download and Install:

Visit the official VSCode website and download the latest version for your operating system (Windows, macOS, or Linux).
Follow the installation instructions provided by the installer.
Launch Visual Studio Code:

After installation, open VSCode.
## Step 2: Install PlatformIO
Install PlatformIO Extension:

In VSCode, click on the Extensions icon on the left sidebar (it looks like a square).
In the search box, type "PlatformIO IDE" and click on "Install" to add the PlatformIO IDE extension.
Restart VSCode:

After installing PlatformIO, you will be prompted to restart VSCode. Click "Restart Now".
## Step 3: Setting Up a New PlatformIO Project
Create a New Project:

After restarting, click on the PlatformIO Home Icon (a house with an arrow) on the left sidebar, or open the Command Palette (Ctrl + Shift + P), type PlatformIO: Home, and select it.
Click on New Project.
Enter Project Details:

Project Name: Enter a name for your project (e.g., "Hacker123").
Board: In the search field, type esp32-s3-devkitc-1" and select it from the list.
Framework: Choose "Arduino"
Location: Choose where you want to save the project.
Click Finish to create the project.
Open the Project Directory:

After the project is created, the project directory will open in VSCode. You should see folders like src, include, and lib.
## Step 4: Integrate the Provided Code
Add Header and Cpp Files:

Copy the provided .h and .cpp files into the src folder of your project.
If there are specific header files that should go into a separate include folder, add them there.
Check the platformio.ini:

Open the platformio.ini file in the root directory of the project.
Make sure it looks something like this:

; PlatformIO Project Configuration File
;
;   Build options: build flags, source filter
;   Upload options: custom upload port, speed and extra flags
;   Library options: dependencies, extra library storages
;   Advanced options: extra scripting
;
; Please visit documentation for the other options and examples
; https://docs.platformio.org/page/projectconf.html

[env]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino
build_src_filter = +<*> -<.git/> -<programs/*> +<programs/${PIOENV}.cpp>
monitor_speed = 115200
lib_deps = 
	bodmer/TFT_eSPI@^2.5.43
	fbiego/CST816S@^1.1.1
	knolleary/PubSubClient@^2.8
	adafruit/Adafruit ADS1X15@^2.5.0
	johannesschramm/ArduinoStopwatch@^1.0.2
	SPI
	bblanchon/ArduinoJson@^7.0.3

[env:iot-coffeemaker-main]
build_flags = 
	-DLOG_LOCAL_LEVEL=ESP_LOG_VERBOSE
	-DCORE_DEBUG_LEVEL=4
	-std=c++2a

[env:display-tests-main]
build_flags = 
	-DLOG_LOCAL_LEVEL=ESP_LOG_VERBOSE
	-DCORE_DEBUG_LEVEL=4
	-std=c++2a
	-DUSER_SETUP_LOADED=1
	-DGC9A01_DRIVER=1
	-DTFT_WIDTH=240
	-DTFT_HEIGHT=240
	-DTFT_MISO=12
	-DTFT_MOSI=11
	-DTFT_SCLK=10
	-DTFT_CS=9
	-DTFT_DC=8
	-DTFT_RST=14
	-DTFT_BL=2
	-DTFT_BACKLIGHT_ON=1
	-DLOAD_GLCD=1
	-DLOAD_FONT2=1
	-DLOAD_FONT4=1
	-DLOAD_FONT6=1
	-DLOAD_FONT7=1
	-DLOAD_FONT8=1
	-DLOAD_GFXFF=1
	-DSMOOTH_FONT=1
	-DSPI_FREQUENCY=80000000

[env:inputs-test-main]
build_flags = 
	-DLOG_LOCAL_LEVEL=ESP_LOG_VERBOSE
	-DCORE_DEBUG_LEVEL=4
	-std=c++2a

This configuration ensures the correct board and settings are used. Adjust the board entry if a different board name is required.
## Step 5: Compile and Upload the Code to the ESP32-S3 Board
Connect the ESP32-S3:

Connect the ESP32-S3 board to your computer using a USB cable.
Build (Compile) the Code:

Click the checkmark icon at the bottom left of VSCode, or open the Command Palette (Ctrl + Shift + P), type PlatformIO: Build, and select it.
PlatformIO will start compiling the code. If there are any errors, review the code and fix any issues.
Upload the Code:

Once the build process completes successfully, click the right arrow icon (next to the checkmark), or open the Command Palette (Ctrl + Shift + P), type PlatformIO: Upload, and select it.
PlatformIO will upload the code to the ESP32-S3 board. You should see a success message when the upload is complete.
Open the Serial Monitor (Optional):

To monitor the output from the ESP32-S3, you can open the Serial Monitor by clicking on the plug icon in the bottom bar or selecting PlatformIO: Monitor from the Command Palette.
The Serial Monitor will display any debug or status messages from the board.
## Step 6: Customize the Code
Make Adjustments:

Your students can now customize the provided code to fit their specific needs. For example, they might want to read sensor values, control the display, or handle touch events.
Use the Control Topic:

To manage data recording, students can implement the Control topic. They can use tools like Node-RED to send start and stop commands over MQTT to the ESP32-S3 board. This was already explained in a previous guide.
Upload New Code:

After making any changes, students should compile and upload the code again to test the modifications on the board.


Use Chat-GPT or Gemini to unterstand the Code and how to use it.
