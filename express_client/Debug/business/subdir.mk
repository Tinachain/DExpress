################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../business/main_thread.cpp \
../business/multicast_manager.cpp \
../business/udp_manager.cpp 

OBJS += \
./business/main_thread.o \
./business/multicast_manager.o \
./business/udp_manager.o 

CPP_DEPS += \
./business/main_thread.d \
./business/multicast_manager.d \
./business/udp_manager.d 


# Each subdirectory must supply rules for building sources it contributes
business/main_thread.o: ../business/main_thread.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -lstdc++ -std=c++0x -I/mnt/hgfs/public/multicast -I/mnt/hgfs/public/files -I/mnt/hgfs/public/rudp/client/rudp -I/mnt/hgfs/projects/Cross_Live/file_express/header -I/mnt/hgfs/public/rudp/header/include -I/usr/include/event2 -I/mnt/hgfs/public/header -I../business -I../web -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"business/main_thread.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

business/multicast_manager.o: ../business/multicast_manager.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -lstdc++ -std=c++0x -I/mnt/hgfs/public/files -I/mnt/hgfs/public/multicast -I/mnt/hgfs/public/rudp/client/rudp -I/mnt/hgfs/public/multicast -I/mnt/hgfs/public/rudp/header/include -I/mnt/hgfs/public/rudp/client/rudp -I/mnt/hgfs/projects/Cross_Live/file_express/header -I/mnt/hgfs/public/header -I/mnt/hgfs/public/rudp/header/include -I/usr/include/event2 -I/mnt/hgfs/public/header -I../business -I../web -include/mnt/hgfs/public/multicast/multicast_client.cpp -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"business/multicast_manager.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

business/udp_manager.o: ../business/udp_manager.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -lstdc++ -std=c++0x -I/mnt/hgfs/public/multicast -I/mnt/hgfs/public/files -I/mnt/hgfs/public/rudp/client/rudp -I/mnt/hgfs/projects/Cross_Live/file_express/header -I/mnt/hgfs/public/rudp/header/include -I/usr/include/event2 -I/mnt/hgfs/public/header -I../business -I../web -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"business/udp_manager.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


