# STM32F429_Communication
This project is made for training purposes, everything was written from scratch besides CMSIS.   
It's simplicity can be used to perceive a concept of a few protocols (UDS or TCP for example) and use it in own project.

It supports protocols:
* HTTP - for now it only allows to load hardcoded page into browser, which contains some text
* UDS (Unified Diagnostic Services) - works via Ethernet and CAN
* TCP
* IP
* Ethernet
* CAN

Project also contains a python module and script used for testing UDS requests via Ethernet.
The script sends specified request and then checks whether the answer received from board matches the expected value. After all tests are done from given test case, it prints out summary in a googletest fashion.  
See pictures [Test1.png](scripts/Test1.png) [Test2.png](scripts/Test2.png) for more details.

Future plans:
* Use C++ to write modules such as GPIO
* Add UDP protocol
* Add ARP protocol
