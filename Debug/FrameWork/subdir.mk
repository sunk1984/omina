################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FrameWork/LogFile.c \
../FrameWork/main.c 

OBJS += \
./FrameWork/LogFile.o \
./FrameWork/main.o 

C_DEPS += \
./FrameWork/LogFile.d \
./FrameWork/main.d 


# Each subdirectory must supply rules for building sources it contributes
FrameWork/%.o: ../FrameWork/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I"/home/test/workspace/omnia/app" -I"/home/test/workspace/omnia/Driver" -I"/home/test/workspace/omnia/FrameWork" -O1 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


