set IPADDR=192.168.1.39


::enable all channel
::..\thrift2.exe %IPADDR% 1 tp_monitor 19   0xAA 0x71 0x7 0x13 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0xF 0x18 

::allow woofer only
::..\thrift2.exe %IPADDR% 1 tp_monitor 19     0xAA 0x71 0x7 0x13 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x7 0x0 0x0 0x0 0x22 0x49 

::allow mid-A only
::..\thrift2.exe %IPADDR% 1 tp_monitor 19     0xAA 0x71 0x7 0x13 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0xB 0x0 0x0 0x0 0x10 0x6 

::allow mid-B only
::..\thrift2.exe %IPADDR% 1 tp_monitor 19     0xAA 0x71 0x7 0x13 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0xD 0x0 0x0 0x0 0x89 0x21

::allow tweeter only
::..\thrift2.exe %IPADDR% 1 tp_monitor 19     0xAA 0x71 0x7 0x13 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0xE 0x0 0x0 0x0 0x55 0xBA



pause