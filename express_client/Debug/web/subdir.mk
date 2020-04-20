################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../web/web_protocol_manager.cpp 

OBJS += \
./web/web_protocol_manager.o 

CPP_DEPS += \
./web/web_protocol_manager.d 


# Each subdirectory must supply rules for building sources it contributes
web/%.o: ../web/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -lstdc++ -std=c++0x -I/mnt/hgfs/public/files -I/mnt/hgfs/public/vendor/openssl/include -I/mnt/hgfs/public/rudp/client/rudp -I/mnt/hgfs/public/rudp/header/include -I/usr/include/event2 -I/mnt/hgfs/public/header -I/mnt/hgfs/projects/Cross_Live/file_express/header -I../business -I../web -I../json -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


