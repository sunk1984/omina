################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Driver/ADC.c \
../Driver/Audio.c \
../Driver/BarCode.c \
../Driver/ExtIO.c \
../Driver/HMI.c \
../Driver/Power.c \
../Driver/RS485.c \
../Driver/Relay.c \
../Driver/bcm2835.c \
../Driver/serial.c 

OBJS += \
./Driver/ADC.o \
./Driver/Audio.o \
./Driver/BarCode.o \
./Driver/ExtIO.o \
./Driver/HMI.o \
./Driver/Power.o \
./Driver/RS485.o \
./Driver/Relay.o \
./Driver/bcm2835.o \
./Driver/serial.o 

C_DEPS += \
./Driver/ADC.d \
./Driver/Audio.d \
./Driver/BarCode.d \
./Driver/ExtIO.d \
./Driver/HMI.d \
./Driver/Power.d \
./Driver/RS485.d \
./Driver/Relay.d \
./Driver/bcm2835.d \
./Driver/serial.d 


# Each subdirectory must supply rules for building sources it contributes
Driver/%.o: ../Driver/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I"/home/test/workspace/omnia/app" -I"/home/test/workspace/omnia/Driver" -I"/home/test/workspace/omnia/FrameWork" -O1 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


