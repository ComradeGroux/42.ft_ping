# 42.ft_ping
A little reimplementation of the ping command in C for learning purpose

## Warning
This project use a RAW socket. That means it must run in root. To avoid this, I could use a DGRAM socket but it is implementation depending so I choose to modify executable capabilities just after the link part so we need to root priviliege only one time during the compilation and then we can run the application in user space.