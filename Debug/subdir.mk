################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Client.cpp \
../Counter.cpp \
../FBF.cpp \
../Message.cpp \
../Node.cpp \
../NodeId.cpp \
../Shard.cpp \
../tcp.cpp 

OBJS += \
./Client.o \
./Counter.o \
./FBF.o \
./Message.o \
./Node.o \
./NodeId.o \
./Shard.o \
./tcp.o 

CPP_DEPS += \
./Client.d \
./Counter.d \
./FBF.d \
./Message.d \
./Node.d \
./NodeId.d \
./Shard.d \
./tcp.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


