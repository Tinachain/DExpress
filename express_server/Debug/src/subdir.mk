################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/express_server.cpp 

OBJS += \
./src/express_server.o 

CPP_DEPS += \
./src/express_server.d 


# Each subdirectory must supply rules for building sources it contributes
src/express_server.o: ../src/express_server.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -lstdc++ -lm -std=c++0x -I/mnt/hgfs/linux/public/header -I/mnt/hgfs/public/multicast -I/mnt/hgfs/public/files -I/mnt/hgfs/public/rudp/header/speed -I/mnt/hgfs/public/rudp/header/thread -I/mnt/hgfs/public/rudp/header/channel -I/mnt/hgfs/public/rudp/header/group -I/mnt/hgfs/public/rudp/header/fec -I/mnt/hgfs/projects/Cross_Live/file_express/header -I/mnt/hgfs/public/rudp/header/include -I/mnt/hgfs/public/rudp/server/rudp -I/mnt/hgfs/public/header -I../business -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/express_server.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


