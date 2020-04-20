################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../web/web_protocol_client.cpp \
../web/web_protocol_manager.cpp 

OBJS += \
./web/web_protocol_client.o \
./web/web_protocol_manager.o 

CPP_DEPS += \
./web/web_protocol_client.d \
./web/web_protocol_manager.d 


# Each subdirectory must supply rules for building sources it contributes
web/%.o: ../web/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -lstdc++ -lm -std=c++0x -I/mnt/hgfs/linux/project/rudp_live/web -I/home/fxh7622/桌面/include/jsoncpp -I/mnt/hgfs/linux/project/public/rudp/server/group -I/usr/local/include -I/mnt/hgfs/linux/project/public/header -I/mnt/hgfs/linux/project/public/rudp/include -I/mnt/hgfs/linux/project/public/rudp/header -I/mnt/hgfs/linux/project/public/rudp/server/rudp -I/mnt/hgfs/linux/project/public/rudp/header/fec -I/mnt/hgfs/linux/project/public/rudp/header/include -I/mnt/hgfs/linux/project/rudp_live/include/linux -I/mnt/hgfs/linux/project/public/rudp -include/mnt/hgfs/linux/project/rudp_live/web/web_protocol_manager.h -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


