# fpga-pcie-xdma-hardware-software
A hardware-software co-design project implementing a PCIe XDMA subsystem on an FPGA to read, write, and validate Block RAM (BRAM) using Linux kernel drivers and custom C applications

In this project we use Vivado and the goal is to finally make an application (software bridge to hardware) in order to write and read on the memory.

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
Don't forget to edit the addresses of memories in the Address Editor tab in Vivado!

A TCL file is uploaded in this repo under the name `my_xdma_design.tcl`. It contains the above block design.

