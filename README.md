# Firgelli (Actuonix) LAC Control
This code controls the Firgelli LAC from Linux.
It is provided as-is with no warranty.
* Name: Richard Treffers
* e-mail: info@StarmanSystems.com
* Date: August 4, 2012

## Requirements:
* `libusb-1.0`
  * To get libusb-1.0: [ ](# "Written by EmergenTek")
    * Type command `sudo apt edit-sources`
    * then choose `2` to open the nano text editor. 
    * Add a line `deb http://ftp.us.debian.org/debian stretch main`
    * replace `stretch` with `jessie` if running a different version of Raspbian.
      * How to tell? Type command `cat /etc/os-release` to display release info text.
    * run `sudo apt-get update` to update the package lists from the new debian.org repo.
    * run `sudo apt-get install libusb-1.0`
* g++ compiler

## Installation:
* to generate main program lac:
```
make
```
* make sure that lac is accessible from the current PATH	

* copy the appropriate .rules file into `/etc/udev/rules` and follow instructions within the file.
* It has been tested under Ubuntu 12:04 and SUSE 12.1
* Make sure lac and setall programs are in /bin/ [ ](# "Line written by EmergenTek")

## Program usage:
```
lac [debug] [rank=1] [write=location,value]
```

* debug - puts more printout as to operation
* rank=n - picks the nth LAC card
* write=location,value transmits the value to the register location
  * locations should be prefaced by 0x if in hexadecimal.	
* setall - a script to set sample values

## Troubleshooting and Remarks: [ ](# "Written by EmergenTek")
* If after you write an lac command, it echoes:
```
lac: firgelli.C:82: void Firgelli::Open(int): Assertion `rval==0' failed.
```
* This means that there is a loose USB connection. 

	
## Examples:
```
setall   #to configure
lac write=0x20,200    # to move to position 200
lac write=0x20,800    # to move to position 800
lac write=0x10        # to read current position 
```
See `Lac_Advanced_Configuration.pdf`  for `0x##` register descriptions.

## LIMITATIONS
	Multiple lacs (using the rank option above) is untested.
