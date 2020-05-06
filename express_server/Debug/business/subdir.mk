################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../business/http_parser.cpp \
../business/main_thread.cpp \
../business/udp_manager.cpp 

OBJS += \
./business/http_parser.o \
./business/main_thread.o \
./business/udp_manager.o 

CPP_DEPS += \
./business/http_parser.d \
./business/main_thread.d \
./business/udp_manager.d 


# Each subdirectory must supply rules for building sources it contributes
business/http_parser.o: ../business/http_parser.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -lstdc++ -lm -std=c++0x -I/mnt/hgfs/public/files -I/mnt/hgfs/public/header -I/mnt/hgfs/projects/Cross_Live/file_express/header -I../business -I/mnt/hgfs/public/rudp/server/rudp -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"business/http_parser.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

business/main_thread.o: ../business/main_thread.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -lstdc++ -lm -std=c++0x -I/mnt/hgfs/public/files -I/mnt/hgfs/public/rudp/header/speed -I/mnt/hgfs/linux/file_express/header -I/mnt/hgfs/public/multicast -I/mnt/hgfs/public/rudp/header/thread -I/mnt/hgfs/public/rudp/header/channel -I/mnt/hgfs/public/rudp/header/group -I/mnt/hgfs/public/rudp/header/fec -I/mnt/hgfs/projects/Cross_Live/file_express/header -I/mnt/hgfs/public/rudp/header/include -I/mnt/hgfs/public/rudp/server/rudp -I/mnt/hgfs/public/header -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"business/main_thread.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

business/udp_manager.o: ../business/udp_manager.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -lstdc++ -lm -std=c++0x -I/mnt/hgfs/public/files -I/mnt/hgfs/public/rudp/header/speed -I/mnt/hgfs/public/multicast -I/mnt/hgfs/public/rudp/header/thread -I/mnt/hgfs/public/rudp/header/channel -I/mnt/hgfs/public/rudp/header/group -I/mnt/hgfs/public/rudp/header/fec -I/mnt/hgfs/projects/Cross_Live/file_express/header -I/mnt/hgfs/public/rudp/header/include -I/mnt/hgfs/public/rudp/server/rudp -I/mnt/hgfs/public/header -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"business/udp_manager.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


