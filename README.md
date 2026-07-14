# fpga-pcie-xdma-hardware-software
A hardware-software co-design project implementing a PCIe XDMA subsystem on an FPGA to read, write, and validate Block RAM (BRAM) using Linux kernel drivers and custom C applications

In this project we use Vivado, a U50 FPGA and the goal is to finally make an application (software bridge to hardware) in order to write and read on the memory.

## Vivado Instructions
Instructions for creating the Vivado file

### Constraints
We add the constraints mentioned below: 
```
set_property PACKAGE_PIN J18 [get_ports hbm_cattrip_tri_o]
set_property IOSTANDARD LVCMOS18 [get_ports hbm_cattrip_tri_o]
```
If you do not include these lines, Vivado’s Design Rule Checker (DRC) will fail during the implementation stage and refuse to generate your bitstream.
More details (Gemini explanation seems solid) : 

FPGAs containing High Bandwidth Memory (HBM) run extremely hot when performing intensive memory transactions. Because HBM can be destroyed by excessive heat in fractions of a second, AMD/Xilinx hardware features a built-in safety cutoff called HBM Catastrophic Over-Temperature Trip (CATTRIP).

If the HBM temperature reaches critical physical limits, the hardware must instantly shut down or trip to protect the card from permanent physical damage (which would require a costly RMA).

To enforce this, Vivado has a hardcoded Design Rule Check (DRC) safety constraint on the bitstream compiler:

Rule: If HBM interfaces are present in your project, the safety signal hbm_cattrip must be explicitly assigned to its dedicated physical protection pin (J18) and configured with the correct voltage.

If you leave this unconstrained, Vivado blocks bitstream generation because running the design without the hardware over-temp safety wire mapped risks melting the physical board.

### Block design Implementation
We use Vivado. We create a new block design. Here we add a XDMA (DMA/Bridge subsystem for PCI express). We connect it into an AXI Interconnect (works as a multiplexer for memory) and then we simulate memories by adding for each memory that we want to create : 
* AXI BRAM Controller
* Block Memory Generator

We can add as many memories as we want. Each memory is made out of these two above. Depending on the number of memories we want, we configure in this manner the interconnect in order to create the correct number of Master ports.
Clock will be automatically created by Vivado. We also connect every clock and reset_n together because clock and reset_n are the same for everybody in this system. The final design should look like this: ( In this scenario, I have created 2 memories)
<img width="1037" height="311" alt="image" src="https://github.com/user-attachments/assets/a16b7db1-b44f-44c8-a5ff-cc966f0151fd" />

Don't forget to edit the addresses of memories in the Address Editor tab in Vivado! For reference I have used these addresses:
<img width="1046" height="397" alt="image" src="https://github.com/user-attachments/assets/d1d2ddf0-7612-43d4-86cc-6732029d6f05" />


A TCL file is uploaded in this repo under the name `my_xdma_design.tcl`. It contains the above block design.

## Bitstream Generation
After the above, our design is ready and we can do an ANALYSIS, SYNTHESIS and then generate our bitstream and load it in our FPGA. To load the bitstream in the FPGA:
1. Open hw_manager in your PC
2. Load the bitstream via hardware manager on Vivado
3. Close hardware manager on Vivado
4. Close hardware manager on your PC
5. ` sudo reboot now ` the system

## Drivers
Now it is time to download the drivers in order to use the memory via software. We use the ready drivers from Xilinx. The repo is: https://github.com/Xilinx/dma_ip_drivers/tree/master/XDMA/linux-kernel . 
By loading the drivers in, some files will be generated: Some of them are the c2h files and some are h2c files. Through these files, we can write and read in our memory respectively. A much more detailed explanation about drivers is in this repo: https://github.com/mwrnd/notes/tree/main/XDMA_Communication

## Making the application
The applications which we will use to write and read are basically C-scripts. 
All the scripts that I have created are in the folder under the name `scripts-application`
Explanation for every script:
`Custom_Reading.c` : A file that outputs what an address contains. You can choose whichever address you want.
`Reading.c` : A file that outputs what a certain address contains. Let's say address X.
`01Script_for_write_and_read.c` : This file just writes and reads from address X.
`02Script_for_write_and_read.c` : This file writes and reads from address X, something different from the above file. This is made to showcase the problem that data written by one .exe execution can be overwritten by another execution without any warning.
My fix:
We will map every address available in a map that has false and true values. Every time we want to write, we just check the value in the table.
`Array_editing.c` : Script to create the txt file which will be the table with false and true values. It creates a file that contains 16384 addresses. Each line is an address. This is because in Vivado, in the address editor window we selected that the first and second memory block will have addresses respectively going from 
* 0xC000_0000 up to 0xC000_1FFF
* 0xC000_2000 up to 0xC000_3FFF
In total 16384 addresses.
`Global_array` : This is the txt file that will be created
`Writing_when_mem_available.c` : This is the file that writes only when memory available. Specifically in the beginning, it asks you to write an address in hex. The address that you input must:
* Be inside the range of available memory
* Must not be written beforehand by another execution
Then and only then, the script will write something in this address that is defined inside the file. (A pattern of numbers. You can change it of course)

## Available Enhancements
The way the `Global_array` txt file is used is not efficient. In order to update it in case the address is valid:
1. We copy it in a temp file line by line
2. When we reach the line of the address where we are writing we update it to False
3. We continue the copying of the file
4. We delete the global array file and we rename the temp file as global array
This is done because True and False have different number of letters and we can't delete True and add False because the last letter 'e' will go to the next line. This issue can be avoided by replacing "True " and "False" by 'T' and 'F' or just '1' and '0'. Maybe there exists some other lines of code that can be used to replace "True" with "False" and not destroy the arrangement of the txt file.


