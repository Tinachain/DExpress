################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../public/rudp_public.cpp 

OBJS += \
./public/rudp_public.o 

CPP_DEPS += \
./public/rudp_public.d 


# Each subdirectory must supply rules for building sources it contributes
public/%.o: ../public/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/mnt/hgfs/linux/project/public/header -I/mnt/hgfs/linux/project/public/rudp/include -I/mnt/hgfs/linux/project/public/rudp/fec -I/mnt/hgfs/linux/project/public/rudp/group -I/mnt/hgfs/linux/project/public/rudp/header -I/mnt/hgfs/linux/project/rudp_server/include -I/mnt/hgfs/linux/project/rudp_server/public -I/mnt/hgfs/linux/project/rudp_server/group -I/mnt/hgfs/linux/project/rudp_server/rudp -I/mnt/hgfs/linux/project/public/rudp -I/mnt/hgfs/linux/project/rudp_server/fec -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


