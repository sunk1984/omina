################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app/TestApp.c \
../app/test_led.c 

OBJS += \
./app/TestApp.o \
./app/test_led.o 

C_DEPS += \
./app/TestApp.d \
./app/test_led.d 


# Each subdirectory must supply rules for building sources it contributes
app/%.o: ../app/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I"/home/test/workspace/omnia/app" -I"/home/test/workspace/omnia/Driver" -I"/home/test/workspace/omnia/FrameWork" -O1 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


