################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../business/main_thread.cpp \
../business/udp_manager.cpp 

OBJS += \
./business/main_thread.o \
./business/udp_manager.o 

CPP_DEPS += \
./business/main_thread.d \
./business/udp_manager.d 


# Each subdirectory must supply rules for building sources it contributes
business/%.o: ../business/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


