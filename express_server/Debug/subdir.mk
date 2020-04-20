################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 

C_SRCS += \
/mnt/hgfs/public/header/cJSON.c 

C_DEPS += \
/mnt/hgfs/public/header/cJSON.d 

CPP_SRCS += \
/mnt/hgfs/public/header/ini.cpp \
/mnt/hgfs/public/header/path.cpp \
/mnt/hgfs/public/header/public.cpp \
/mnt/hgfs/public/header/string_list.cpp \
/mnt/hgfs/public/header/write_log.cpp \
/mnt/hgfs/public/header/json.cpp \
/mnt/hgfs/public/rudp/header/include/rudp_timer.cpp \
/mnt/hgfs/public/rudp/header/include/rudp_public.cpp \
/mnt/hgfs/public/rudp/header/fec/matrix.cpp \
/mnt/hgfs/public/rudp/header/fec/galois8bit.cpp \
/mnt/hgfs/public/rudp/header/fec/fec_coder.cpp \
/mnt/hgfs/public/rudp/header/group/rudp_group.cpp \
/mnt/hgfs/public/rudp/header/encrypt/aes/rudp_aes.cpp \
/mnt/hgfs/public/rudp/header/channel/send_channel.cpp \
/mnt/hgfs/public/rudp/header/channel/recv_channel.cpp \
/mnt/hgfs/public/rudp/header/thread/recv_thread.cpp \
/mnt/hgfs/public/rudp/header/thread/send_thread.cpp \
/mnt/hgfs/public/rudp/header/frames/rudp_frames.cpp \
/mnt/hgfs/public/rudp/server/rudp/rudp_socket.cpp \
/mnt/hgfs/public/files/files.cpp \
/mnt/hgfs/public/multicast/multicast_server.cpp 


OBJS += \
/mnt/hgfs/public/header/ini.o \
/mnt/hgfs/public/header/path.o \
/mnt/hgfs/public/header/public.o \
/mnt/hgfs/public/header/string_list.o \
/mnt/hgfs/public/header/write_log.o \
/mnt/hgfs/public/header/cJSON.o \
/mnt/hgfs/public/header/json.o \
/mnt/hgfs/public/rudp/header/include/rudp_timer.o \
/mnt/hgfs/public/rudp/header/include/rudp_public.o \
/mnt/hgfs/public/rudp/header/fec/matrix.o \
/mnt/hgfs/public/rudp/header/fec/galois8bit.o \
/mnt/hgfs/public/rudp/header/fec/fec_coder.o \
/mnt/hgfs/public/rudp/header/group/rudp_group.o \
/mnt/hgfs/public/rudp/header/encrypt/aes/rudp_aes.o \
/mnt/hgfs/public/rudp/header/channel/send_channel.o \
/mnt/hgfs/public/rudp/header/channel/recv_channel.o \
/mnt/hgfs/public/rudp/header/thread/recv_thread.o \
/mnt/hgfs/public/rudp/header/thread/send_thread.o \
/mnt/hgfs/public/rudp/header/frames/rudp_frames.o \
/mnt/hgfs/public/rudp/server/rudp/rudp_socket.o \
/mnt/hgfs/public/files/files.o \
/mnt/hgfs/public/multicast/multicast_server.o

CPP_DEPS += \
/mnt/hgfs/public/header/ini.d \
/mnt/hgfs/public/header/path.d \
/mnt/hgfs/public/header/public.d \
/mnt/hgfs/public/header/string_list.d \
/mnt/hgfs/public/header/write_log.d \
/mnt/hgfs/public/header/json.d \
/mnt/hgfs/public/rudp/include/include/rudp_timer.d \
/mnt/hgfs/public/rudp/header/include/rudp_public.d \
/mnt/hgfs/public/rudp/header/fec/matrix.d \
/mnt/hgfs/public/rudp/header/fec/galois8bit.d \
/mnt/hgfs/public/rudp/header/fec/fec_coder.d \
/mnt/hgfs/public/rudp/header/group/rudp_group.d \
/mnt/hgfs/public/rudp/header/channel/send_channel.d \
/mnt/hgfs/public/rudp/header/channel/recv_channel.d \
/mnt/hgfs/public/rudp/header/encrypt/aes/rudp_aes.d \
/mnt/hgfs/public/rudp/header/thread/recv_thread.d \
/mnt/hgfs/public/rudp/header/thread/send_thread.d \
/mnt/hgfs/public/rudp/header/frames/rudp_frames.d \
/mnt/hgfs/public/rudp/server/rudp/rudp_socket.d \
/mnt/hgfs/public/files/files.d \
/mnt/hgfs/public/multicast/multicast_server.d

# Each subdirectory must supply rules for building sources it contributes

/mnt/hgfs/public/header/%.o: /mnt/hgfs/public/header/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -g -std=c11 -I/mnt/hgfs/public/header -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

/mnt/hgfs/public/header/%.o: /mnt/hgfs/public/header/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -std=c++0x -I/mnt/hgfs/public/header -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
	
# RUDP ENCRYPT
/mnt/hgfs/public/rudp/header/encrypt/aes/%.o: /mnt/hgfs/public/rudp/header/encrypt/aes/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/mnt/hgfs/public/header \
						-I/mnt/hgfs/public/vendor/openssl/include \
						-I/mnt/hgfs/public/rudp/header/include \
						-L/mnt/hgfs/public/vendor/openssl -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
	
# FILES
/mnt/hgfs/public/files/%.o: /mnt/hgfs/public/files/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -std=c++0x -I/mnt/hgfs/public/header -I/mnt/hgfs/public/files -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
	
# RUDP Include
/mnt/hgfs/public/rudp/header/include/%.o: /mnt/hgfs/public/rudp/header/include/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -std=c++0x -I/mnt/hgfs/public/rudp/header/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
	
# RUDP FEC
/mnt/hgfs/public/rudp/header/fec/%.o: /mnt/hgfs/public/rudp/header/fec/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -std=c++0x -I/mnt/hgfs/public/rudp/header/include -I/mnt/hgfs/public/rudp/header/fec -I/mnt/hgfs/public/rudp/header -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
	
# RUDP GROUP
/mnt/hgfs/public/rudp/header/group/%.o: /mnt/hgfs/public/rudp/header/group/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -std=c++0x -I/mnt/hgfs/public/header -I/mnt/hgfs/public/rudp/header/include -I/mnt/hgfs/public/rudp/header/fec -I/mnt/hgfs/public/rudp/header -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
	
# RUDP SPEED
/mnt/hgfs/public/rudp/header/speed/%.o: /mnt/hgfs/public/rudp/header/speed/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -std=c++0x -I/mnt/hgfs/public/header -I/mnt/hgfs/public/rudp/header/include -I/mnt/hgfs/public/rudp/header/fec -I/mnt/hgfs/public/rudp/header/channel -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '	
	
# RUDP CHANNEL
/mnt/hgfs/public/rudp/header/channel/%.o: /mnt/hgfs/public/rudp/header/channel/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -std=c++0x -I/mnt/hgfs/public/header -I/mnt/hgfs/public/rudp/header/include -I/mnt/hgfs/public/rudp/header/fec -I/mnt/hgfs/public/rudp/header/speed -I/mnt/hgfs/public/rudp/header/channel -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '	
	
# RUDP THREAD
/mnt/hgfs/public/rudp/header/thread/%.o: /mnt/hgfs/public/rudp/header/thread/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -std=c++0x -I/mnt/hgfs/public/header -I/mnt/hgfs/public/rudp/header/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '	
	
# RUDP FRAMES
/mnt/hgfs/public/rudp/header/frames/%.o: /mnt/hgfs/public/rudp/header/frames/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -std=c++0x -I/mnt/hgfs/public/header -I/mnt/hgfs/public/rudp/header/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
	
# RUDP Server Socket
/mnt/hgfs/public/rudp/server/rudp/%.o: /mnt/hgfs/public/rudp/server/rudp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -std=c++0x -I/mnt/hgfs/public/header -I/mnt/hgfs/public/rudp/header/include -I/mnt/hgfs/public/rudp/header/fec -I/mnt/hgfs/public/rudp/header/group -I/mnt/hgfs/public/rudp/header/speed -I/mnt/hgfs/public/rudp/header/channel -I/mnt/hgfs/public/rudp/header/thread -I/mnt/hgfs/public/rudp/header/frames -I/mnt/hgfs/public/rudp/header -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
	
# Multicast
/mnt/hgfs/public/multicast/%.o: /mnt/hgfs/public/multicast/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/mnt/hgfs/public/rudp/header/include -I/mnt/hgfs/public/header -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
