# ZephyrProject-Bluetooth-Starter
The folder will be a buildable Bluetooth seed including

   a) ZephyrProject with a Bluetooth 128-bit based attribute (Bluetooth-Host)
   
   b) iOS Project (Bluetooth-Peripheral)
   
   c) Android Project (Bluetooth-Peripheral)
   
   Install (after cloning) 
   
   1) Use ZephyrProject Getting Started
   2) source zephyr*.sh
   3) pushd zephyr/src
      a) MAC: cmake -Bbuild -H.
      b) Linux: cmake ....
      c) Windows: (?)
   4) cd build
   5) make 
   6) make flash
   7) popd
   
