################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/express_client.cpp 

OBJS += \
./src/express_client.o 

CPP_DEPS += \
./src/express_client.d 


# Each subdirectory must supply rules for building sources it contributes
src/express_client.o: ../src/express_client.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -lstdc++ -std=c++0x -I../web -I/mnt/hgfs/public/multicast -I/mnt/hgfs/public/files -I/mnt/hgfs/public/rudp/client/rudp -I/mnt/hgfs/projects/Cross_Live/file_express/header -I/mnt/hgfs/public/rudp/header/include -I/usr/include/event2 -I/mnt/hgfs/public/header -I../business -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/express_client.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


